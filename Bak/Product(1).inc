Product struct
	barcode 		db 16  dup (0)	
	product_name 	db 128 dup (0) 
	company 		db 64  dup (0)
	price			db 8   dup (0)	
Product ends
PProduct TYPEDEF PTR Product

.code

parseProductList proto :UINT
parseProductList proc uses esi edi listString:UINT	
	local strSize :UINT
	local result :UINT
	
	invoke mem_size, listString	
	mov strSize, eax
	
	; 128 Aqui representa o numero de itens na lista, pode se tornar dinamico fazendo uso do realloc
	; Colocado numero alto para permitir expansão da lista
	invoke mem_alloc, (type Product * 128)
	mov result, eax
	
	
	mov esi, listString
	mov edi, result
	mov ecx, esi
	add ecx, strSize
	.while esi < ecx		
		mov al, [esi]		
		; Se identificar digito, indica novo registro
		.if (al >= '0') && (al <= '9')
			
			push ecx
			
			mov edx, 0
			mov ecx, 0
			mov al, [esi]
			.repeat
				
				.if al == 9
						
						;novo campo
						inc edx
						mov ecx, 0
					
					.else

						.if edx == 0
							mov [edi].Product.barcode[ecx], al
						.elseif edx == 1							
							mov [edi].Product.product_name[ecx], al
						.elseif edx == 2
							mov [edi].Product.company[ecx], al							
						.elseif edx == 3
							mov [edi].Product.price[ecx], al
						.endif
						
						inc ecx
				.endif
				
															
				inc esi
				mov al, [esi]
			.until al == 13
				
			add edi, type Product
			pop ecx
			
		.else
		
			inc esi
						
		.endif
	.endw	
	
	mov eax, result
	ret	
parseProductList endp


InitializeProductDatabase proto :dword,:HWND  
InitializeProductDatabase proc path:dword, hWin:HWND
	local filePtr:dword
	local bytesPtr:dword
	local result:dword
	
	OPEN_FILE_MACRO path, hWin
		mov filePtr, eax		
				
		invoke readAllBytes, filePtr
		mov bytesPtr, eax
																	
			invoke parseProductList, bytesPtr
			mov result, eax
		
		invoke mem_free, bytesPtr
	invoke CloseHandle, filePtr
	
	mov eax, result
	
	ret	
InitializeProductDatabase endp

findProductByBarcode proto :dword, :dword
findProductByBarcode proc codeToSearch:dword, productList:dword	
	
	invoke mem_array_length, productList, type Product
	mov ecx, eax
	push ecx
	mov esi, productList
	.while ecx > 0		
		
		mov edi, codeToSearch
				
		push esi
		push ecx			
			mov ecx, 14
			cld
			repe cmpsb
			mov eax, ecx
		pop ecx
		pop esi
		
		.break .if (eax == 0)

		add esi, type Product		
		dec ecx
	.endw
	
	.if ecx == 0
		mov eax, -1
		pop ecx	
	.else
		mov eax, ecx
		pop ecx
		sub ecx, eax
		mov eax, type Product
		mul ecx
		add eax, productList		
	.endif
	
	ret
		
findProductByBarcode endp