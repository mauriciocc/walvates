.const

	DIGIT_LENGTH equ 7
	GROUP_LENGTH equ 6
	GROUP6_LENGTH equ DIGIT_LENGTH * GROUP_LENGTH
	GROUP6_SEPARATOR_LENGTH equ 5
	BAR_CODE_DATA_LENGTH equ (GROUP6_LENGTH * 2) + GROUP6_SEPARATOR_LENGTH
	BAR_CODE_LENGTH equ 14
	
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
			
	mov ecx, BAR_CODE_DATA_LENGTH
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

DecodeDigit proc uses ecx esi edi digitPtr:dword
	local tempBuffer :dword
	
	invoke mem_alloc, DIGIT_LENGTH+1
	mov tempBuffer, eax
	
	mov esi, digitPtr
	mov edi, tempBuffer
	
	mov ecx, DIGIT_LENGTH
	rep movsb
	
	mov ecx, 0
	mov edi, offset EVEN_0
	.while ecx < 9
	
		push ecx
			mov eax, cmp$(tempBuffer, edi)
		pop ecx
		
		.BREAK .IF (eax == 0)
		 		 
		inc ecx
		add edi, DIGIT_LENGTH+1
	.endw	
	
	pusha
		invoke mem_free, tempBuffer
	popa

	mov eax, ecx
	add eax, 48 ; To ASCII

	ret
DecodeDigit endp

DecodeBarcode proc uses esi edi codePtr:dword
	local resultPtr :dword
		
	invoke mem_alloc, BAR_CODE_LENGTH
	mov resultPtr, eax
	mov edi, resultPtr
	
	
	mov esi, codePtr
	add esi, GROUP6_LENGTH
	add esi, GROUP6_SEPARATOR_LENGTH
	
	mov ecx, 0
	.while ecx < GROUP_LENGTH
		
		invoke DecodeDigit, esi
		mov [edi], byte ptr al		
		add esi, DIGIT_LENGTH
		inc edi
		inc ecx
	.endw
	mov [edi], byte ptr 0
	mov eax, resultPtr	
	ret
DecodeBarcode endp
