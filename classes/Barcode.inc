.const

	GROUP6_LENGTH equ 42
	GROUP6_SEPARATOR_LENGTH equ 5
	BAR_CODE_DATA_LENGTH equ (GROUP6_LENGTH * 2) + GROUP6_SEPARATOR_LENGTH
	
	

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