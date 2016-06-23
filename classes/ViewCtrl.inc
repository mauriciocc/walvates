.const
	FTotalValue  equ 1008
	FEanInput 	 equ 1003
	AddItemBtn	 equ 1001
	FProductList equ 1002
	CloseSaleBtn equ 1005
	
	productsFile db  "lista_barra.txt",0
	
	msg_attention db "Atenção!",0
	msg_product_not_found_message db "Item não encontrado",0

.data
	
	totalValue REAL8 0.00
	productsDatabase dd ?
		
.code

UpdateTotalValue macro hWin
	mov eax, real8$(totalValue)
	invoke SetDlgItemText, hWin, FTotalValue, eax	
endm

ReadEanInput proc hWin:HWND
	invoke mem_alloc, 16
	push eax	
	invoke GetDlgItemText, hWin, FEanInput, eax, 16
	pop eax
	ret 
ReadEanInput endp

UpdateProductList proc hWin:HWND, productPtr:dword
	local buffer :dword
	invoke mem_alloc, 1024*8
	mov buffer, eax	
		invoke GetDlgItemText, hWin, FProductList, buffer, 1024*8 
		mov buffer, cat$(buffer, addr [esi].Product.barcode, chr$(9), addr [esi].Product.product_name, chr$(9), addr [esi].Product.company, chr$(9), addr [esi].Product.price, chr$(13,10))
		invoke SetDlgItemText, hWin, FProductList, buffer
	invoke mem_free, buffer
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