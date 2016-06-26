.const

	EVEN_CONST equ 0
	ODD_CONST equ 1
	DIGIT_LENGTH equ 7
	GROUP_LENGTH equ 6
	GROUP6_LENGTH equ DIGIT_LENGTH * GROUP_LENGTH
	GROUP6_SEPARATOR_LENGTH equ 5
	BAR_CODE_DATA_LENGTH equ (GROUP6_LENGTH * 2) + GROUP6_SEPARATOR_LENGTH
	BAR_CODE_LENGTH equ 14
	
	
	FIRST_DIGIT_0 db "111111",0
	FIRST_DIGIT_1 db "110100",0
	FIRST_DIGIT_2 db "110010",0
	FIRST_DIGIT_3 db "110001",0
	FIRST_DIGIT_4 db "101100",0
	FIRST_DIGIT_5 db "100110",0
	FIRST_DIGIT_6 db "100011",0
	FIRST_DIGIT_7 db "101010",0
	FIRST_DIGIT_8 db "101001",0
	FIRST_DIGIT_9 db "100101",0
	
	EVEN_0 db "1110010",0
	EVEN_1 db "1100110",0
	EVEN_2 db "1101100",0
	EVEN_3 db "1000010",0
	EVEN_4 db "1011100",0
	EVEN_5 db "1001110",0
	EVEN_6 db "1010000",0
	EVEN_7 db "1000100",0
	EVEN_8 db "1001000",0
	EVEN_9 db "1110100",0
	

.data

.code

ClearOuterBits proc uses esi edi linePtr:dword
	mov esi, linePtr
	mov edi, linePtr	
	mov eax, find$(1, linePtr, chr$("101"))
	add esi, eax
	add esi, 2
			
	mov ecx, GROUP6_LENGTH 
	rep movsb
	
	; Skip middle separator
	add esi, GROUP6_SEPARATOR_LENGTH
	
	mov ecx, GROUP6_LENGTH 
	rep movsb
	
	mov [edi], byte ptr 0
	
	mov eax, linePtr	
	ret
ClearOuterBits endp

ReduceBmpLine proc uses esi edi linePtr:dword
	mov esi, linePtr
	mov edi, linePtr
	
	invoke mem_size, linePtr
	mov ecx, eax
	
	mov edx, 0
	.while ecx > 0
		mov dl, byte ptr [esi]
		mov [edi], dl
		add esi, 2
		inc edi
		dec ecx
	.endw
	mov [edi], byte ptr 0
	
	invoke ClearOuterBits, linePtr
	mov eax, linePtr
	
	ret
ReduceBmpLine endp

IdentifyEvenOrOdd proc uses ecx esi digitPtr:dword
	mov esi, digitPtr
	mov ecx, 0
	mov eax, 0	
	.while ecx < DIGIT_LENGTH		
		.if byte ptr [esi+ecx] == '1'
			inc eax
		.endif 
		inc ecx		
	.endw
	mov edx, 0
	mov ecx, 2
	div ecx
	
	.if edx == 0
		mov eax, EVEN_CONST		
	.else
		mov eax, ODD_CONST
	.endif
ret	
IdentifyEvenOrOdd endp

CopyString proc uses esi edi stringPtr:dword
	local result :dword
		
	pusha		
		mov ecx, len(stringPtr)
		push ecx
			invoke mem_alloc, ecx
			mov result, eax
		pop ecx
		mov esi, stringPtr
		mov edi, result
		rep movsb	
	popa
	
	mov eax, result
	ret
CopyString endp

MirrorCode proc uses esi edi stringPtr:dword
	local result :dword
		
	pusha		
		invoke CopyString, stringPtr
		mov result, eax		
		mov result, rev$(result)		
	popa
	
	mov eax, result
	
	ret
MirrorCode endp

InverseCode proc uses esi edi stringPtr:dword
	local result :dword
		
	pusha		
		mov ecx, len(stringPtr)
		push ecx
			invoke mem_alloc, ecx
			mov result, eax
		pop ecx
		mov esi, stringPtr
		mov edi, result
		.while ecx > 0
			.if [esi] == '0'
				mov [edi], '1'
			.else
				mov [edi], '0'
			.endif
			inc esi
			inc edi
			dec ecx
		.endw	
	popa
	
	mov eax, result
	ret
InverseCode endp

DecodeDigit proc uses ecx esi edi digitPtr:dword, digitIdx:dword, parity:dword
	local tempBuffer :dword
	
	.if digitIdx == 0
		invoke mem_alloc, DIGIT_LENGTH
	.else
		invoke mem_alloc, DIGIT_LENGTH+1
	.endif
	
	mov tempBuffer, eax
	
	mov esi, digitPtr
	mov edi, tempBuffer
	
	.if digitIdx == 0
		mov ecx, DIGIT_LENGTH - 1
	.else
		mov ecx, DIGIT_LENGTH	
	.endif
	
	rep movsb
	
	mov ecx, 0
	.if digitIdx == 0
		mov edi, offset FIRST_DIGIT_0
	.else
		mov edi, offset EVEN_0
	.endif
	
	.while ecx < 9
	
		push ecx
		push edi
			.if  digitIdx > 0 && digitIdx <= 6
				.if parity == EVEN_CONST
					invoke MirrorCode, edi
					mov edi, eax
				.else
					invoke InverseCode, edi
					mov edi, eax
				.endif					
			.endif
			mov eax, cmp$(tempBuffer, edi)
			
			.if  digitIdx > 0 && digitIdx <= 6
				pusha
				invoke mem_free, edi
				popa
			.endif
		pop edi
		pop ecx
		
		.BREAK .IF (eax == 0)
		 		 
		inc ecx
		.if digitIdx == 0
			add edi, DIGIT_LENGTH
		.else
			add edi, DIGIT_LENGTH+1
		.endif
		
	.endw	
	
	pusha
		invoke mem_free, tempBuffer
	popa

	mov eax, ecx
	add eax, 48 ; To ASCII

	ret
DecodeDigit endp

DecodeBarcode proc uses esi edi ecx edx codePtr:dword
	local resultPtr :dword
	
	local firstDigitPtr :dword
	 
	invoke mem_alloc, DIGIT_LENGTH
	mov firstDigitPtr, eax
	
	invoke mem_alloc, BAR_CODE_LENGTH
	mov resultPtr, eax
	mov edi, resultPtr
	inc edi ; leave room for first digit
	
	
	mov esi, codePtr
	
	mov ecx, 0
	.while ecx < (GROUP_LENGTH*2)
		
		invoke IdentifyEvenOrOdd, esi
		
		.if ecx < 6
			pusha			
				mov edi,firstDigitPtr 	
				add eax, 48
				mov [edi+ecx], byte ptr al
			popa	
		.endif
		
		push ecx
			inc ecx
			invoke DecodeDigit, esi, ecx, eax
		pop ecx
		
		mov [edi], byte ptr al		
		add esi, DIGIT_LENGTH
		inc edi
		inc ecx
	.endw
	mov [edi], byte ptr 0
	
	MSG_BOX firstDigitPtr, NULL
	
	mov edi, resultPtr
	
	invoke DecodeDigit, firstDigitPtr, 0, 0
	mov [edi], byte ptr al	
	
	mov eax, resultPtr	
	ret
DecodeBarcode endp
