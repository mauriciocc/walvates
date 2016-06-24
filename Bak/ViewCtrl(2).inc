.const
	FTotalValue  	equ 1008
	FEanInput 	 	equ 1003
	AddItemBtn	 	equ 1001
	FProductList 	equ 1002
	CloseSaleBtn 	equ 1005
	ReadBalanceBtn  equ 1004
	SelectProductBtn  equ 1006
	
	STRING_BUFFER	equ 1024*8
	EAN_BUFFER		equ	16
	
	fileSelectorCaption db "Selecione um Código de Barras...", 0
	fileSelectorFilter db "Arquivos BMP",0,"*.bmp",0,0
            
			
	productsFile db  "lista_barra.txt",0
	
	msg_attention db "Atenção!",0
	msg_product_not_found_message db "Item não encontrado",0
	msg_weight db "Peso: ",0
	msg_price_kg db "Preço/Kg: ",0
	msg_kg db " Kg",0
	msg_total_value db "Valor Total: ", 0
	msg_rs db "R$ ", 0

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
		mov buffer, cat$(buffer, addr [esi].Product.barcode, chr$(9), addr [esi].Product.product_name, chr$(9), addr [esi].Product.company, chr$(9), addr msg_rs ,addr [esi].Product.price, chr$(13,10))
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

CloseSaleBtnClicked proc hWin:HWND
	local emptyString :dword
	mov emptyString, chr$(0)
	invoke SetDlgItemText, hWin, FEanInput, emptyString
	invoke SetDlgItemText, hWin, FTotalValue, emptyString
	invoke SetDlgItemText, hWin, FProductList, emptyString
	fldz
	fstp totalValue	
	ret
CloseSaleBtnClicked endp

ReadBalanceBtnClicked proc uses esi hWin:HWND
	local buffer :dword
	local realVal :real8
	
	invoke ReadDialogField,hWin, FProductList, STRING_BUFFER	
	mov buffer, eax 
		
	invoke readUranoValue, hWin	
	mov esi, eax
	
	invoke toDouble, addr [esi].BalanceCapture.peso
	fstp realVal
	mov eax, real8$(realVal)
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


SelectProductBtnClicked proc hWin:HWND
	mov ponteiro_serial, OpenFileDlg(hWin, 0, addr fileSelectorCaption, addr fileSelectorFilter)
	ret
SelectProductBtnClicked endp
