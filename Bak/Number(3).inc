.code

toDouble proc uses esi edi stringPtr:dword
	local strCopy :dword
	
	invoke mem_alloc, 16
	mov strCopy, eax
	
	mov esi, stringPtr	
	mov edi, strCopy
	
	mov al, [esi]	
	.while al != 0
		.if al == ','
			mov al, '.'
			mov [edi], al
		.else
			mov [edi], al		
		.endif		
		inc edi
		inc esi
		mov al, [esi]		
	.endw 
	
	invoke crt_atof, strCopy
	
	invoke mem_free, strCopy
	
	ret
toDouble endp