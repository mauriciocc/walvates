.const
	MainDialog 			equ 1000
	FTotalValue  		equ 1008
	FEanInput 	 		equ 1003
	AddItemBtn	 		equ 1001
	FProductList 		equ 1002
	CloseSaleBtn 		equ 1005
	ReadBalanceBtn  	equ 1004
	SelectProductBtn  	equ 1006
	
	
	STRING_BUFFER	equ 1024*8
	EAN_BUFFER		equ	16
	
	EMPTY_STRING db 0
	
	fileSelectorCaption db "Selecione um Código de Barras...", 0
	fileSelectorFilter  db "Arquivos BMP 256 Cores",0,"*.bmp",0,0
	
	createFileCaption db "Escolha um nome para o log de venda...", 0
	createFilePattern db "Arquivo Texto (.txt)",0,"*txt",0,0

	         
			
	productsFile db "lista_barra.txt",0
	
	msg_attention db "Atenção!",0
	msg_product_not_found_message db "Item não encontrado",0
	msg_add_at_least_one_product db "Adicione pelo menos um item para poder fechar a venda!",0
	msg_weight db "Peso: ",0
	msg_price_kg db "Preço/Kg: ",0
	msg_kg db " Kg",0
	msg_total_value db "Valor Total: ", 0
	msg_rs db "R$ ", 0
	
	LINE_BREAK db 13,10,0
	TAB_STR db 9,0	

.data
	
	totalValue REAL8 0.00
	productsDatabase dd ?
		
.code

UpdateTotalValue macro hWin
	mov eax, real8$(totalValue)
	invoke SetDlgItemText, hWin, FTotalValue, eax	
endm

ReadDialogField proc hWin:HWND, element:dword, bufferSize:dword
	invoke mem_alloc, bufferSize
	push eax	
	invoke GetDlgItemText, hWin, element, eax, bufferSize
	pop eax
	ret 
ReadDialogField endp

UpdateProductList proc hWin:HWND, productPtr:dword
	local buffer :dword
	invoke ReadDialogField,hWin, FProductList, STRING_BUFFER	
	mov buffer, eax 
		mov buffer, cat$(buffer, addr [esi].Product.barcode, addr TAB_STR, addr [esi].Product.product_name, addr TAB_STR, addr [esi].Product.company, addr TAB_STR, addr msg_rs ,addr [esi].Product.price, addr LINE_BREAK)
		invoke SetDlgItemText, hWin, FProductList, buffer
	invoke mem_free, buffer
	invoke SetDlgItemText, hWin, FEanInput, NULL
	ret	
UpdateProductList endp

AddItemBtnClicked proc uses esi ean:dword, hWin:HWND
	invoke findProductByBarcode, ean, productsDatabase
	.if eax == -1		
		MSG_BOX addr msg_product_not_found_message, addr msg_attention		
	.else
		mov esi, eax
		
		invoke toDouble, addr [esi].Product.price
		SUM_FP totalValue
		UpdateTotalValue hWin
 		invoke UpdateProductList, hWin, esi
	.endif
	ret	
AddItemBtnClicked endp

CloseSaleBtnClicked proc uses ecx hWin:HWND
	local filePtr :dword
	local strBuffer :dword
	local bytesWritten :dword
	
	fld totalValue
	ftst
	fstsw ax
	fwait
	sahf
	
	.if ZERO?
		MSG_BOX addr msg_add_at_least_one_product, addr msg_attention
		ret
	.endif
	
	fstp totalValue
	
	mov eax, SaveFileDlg(hWin, hInstance, addr createFileCaption, addr createFilePattern)	
	mov cl, byte ptr [eax]	
	
	; User cancelou
	.if cl == 0
		ret
	.endif
		
	mov ecx, chr$(".txt")
	mov eax, add$(eax, ecx)
	
	OPEN_OR_CREATE_FILE_MACRO eax, hWin
	mov filePtr, eax
	
	invoke mem_alloc, STRING_BUFFER
	mov strBuffer, eax
	
	invoke GetDlgItemText, hWin, FProductList, strBuffer, STRING_BUFFER
	mov strBuffer, cat$(strBuffer, addr LINE_BREAK, addr msg_total_value, addr msg_rs, real8$(totalValue))
	
	mov ecx, len(strBuffer)	
	invoke WriteFile, filePtr, strBuffer, ecx, addr bytesWritten, NULL
	
	 
	invoke SetDlgItemText, hWin, FEanInput, addr EMPTY_STRING
	invoke SetDlgItemText, hWin, FTotalValue, addr EMPTY_STRING
	invoke SetDlgItemText, hWin, FProductList, addr EMPTY_STRING
	
	fldz
	fstp totalValue
		
	invoke mem_free, strBuffer
	invoke CloseHandle, filePtr
	ret
CloseSaleBtnClicked endp

ReadBalanceBtnClicked proc uses esi hWin:HWND
	local buffer :dword
	local realVal :real8
	

		
	invoke readUranoValue, hWin	
	.if eax == INVALID_HANDLE_VALUE
		ret
	.endif
	mov esi, eax
	
	invoke toDouble, addr [esi].BalanceCapture.peso
	fstp realVal
	mov eax, real8$(realVal)
	
	pusha 	
		invoke ReadDialogField,hWin, FProductList, STRING_BUFFER	
		mov buffer, eax
	popa
	 
	mov buffer, cat$(buffer, addr msg_weight, eax, addr msg_kg, chr$(9,9))
	
	invoke toDouble, addr [esi].BalanceCapture.precoKg
	fstp realVal
	mov eax, real8$(realVal)
	mov buffer, cat$(buffer, addr msg_price_kg, eax, chr$(9))
	
	invoke toDouble, addr [esi].BalanceCapture.vlrTotal
	fstp realVal
	mov eax, real8$(realVal)
	mov buffer, cat$(buffer, addr msg_total_value, addr msg_rs, eax, chr$(13,10))
	
	invoke SetDlgItemText, hWin, FProductList, buffer	
	
	invoke toDouble, addr [esi].BalanceCapture.vlrTotal
	SUM_FP totalValue
	UpdateTotalValue hWin
	
	invoke mem_free, esi
	invoke mem_free, buffer
	
	ret
ReadBalanceBtnClicked endp


SelectProductBtnClicked proc uses esi hWin:HWND
	local bmpPtr :dword	
	local bytesRead :dword
	local widthAlign :dword
	
	
	mov eax, OpenFileDlg(hWin, hInstance, addr fileSelectorCaption, addr fileSelectorFilter)	
	mov cl, byte ptr [eax]
	
	; User cancelou
	.if cl == 0
		ret
	.endif
		
	OPEN_FILE_MACRO eax, hWin
	push eax
		invoke readAllBytes, eax
		mov bmpPtr, eax
	pop eax 
	invoke CloseHandle, eax	

	mov esi, bmpPtr
	
	; Computa 'meio' da imagem
	mov eax, [esi].Bmp.bitMap.biHeight
	mov edx, 0
	mov ecx, 2
	div ecx	
	invoke ReadBmpImageDataLine, esi, eax
	invoke ReduceBmpLine, eax
	push eax
		invoke DecodeBarcode, eax		
		push eax
			invoke AddItemBtnClicked, eax, hWin
		pop eax					
		invoke mem_free, eax
	pop eax	
	invoke mem_free, eax	
	invoke mem_free, bmpPtr
	
	ret
SelectProductBtnClicked endp
