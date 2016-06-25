include basic_includes.inc
include classes\File.inc
include classes\BalanceCapture.inc
include classes\Product.inc
include classes\Number.inc
include classes\ViewCtrl.inc

.data


CaptionText        db   "Open File",0



szFilter db "All files *.*",0,"*.*",0,
            "Text files *.txt",0,"*.txt",0,
            "Assembly files *.asm",0,"*.asm",0,
            "Resource files *.rc",0,"*.rc",0,0





.code

ProcEvento proc uses edi esi hWin:HWND,uMsg:UINT,wParam:WPARAM,lParam:LPARAM
	mov eax,uMsg	
	; WM_INITDIALOG -> pode ser utilizado para inicializar recursos, como abrir portas
	; WM_CLOSE -> indica fechamento do programa, pode liberar recursos também
	.if eax==WM_INITDIALOG
		invoke InitializeProductDatabase, addr productsFile, hWin
		mov productsDatabase, eax	
	.elseif eax==WM_COMMAND

		invoke decode_wparam,wParam				
		
		.if edx==BN_CLICKED
			
			; Item Added		
			.if eax == AddItemBtn
				invoke ReadDialogField, hWin, FEanInput, EAN_BUFFER
					push eax					
						invoke AddItemBtnClicked, eax, hWin
					pop eax
				invoke mem_free, eax	
			.endif
			
			; Close Sale
			.if eax == CloseSaleBtn
				invoke CloseSaleBtnClicked, hWin								
			.endif
			
			; Read Balance
			.if eax == ReadBalanceBtn
				invoke ReadBalanceBtnClicked, hWin								
			.endif
			
			; Select Product 
			.if eax == SelectProductBtn
				invoke SelectProductBtnClicked, hWin
				invoke SetDlgItemText,hWin, FEanInput, eax
			.endif			

			
		.endif
				
	;Evento de fechamento de janela
	.elseif eax==WM_CLOSE
		invoke mem_free, productsDatabase
		invoke EndDialog,hWin,0
		
	.else ; Sinaliza que evento não foi tratado com sucesso		
		mov eax,FALSE
		ret		
	.endif
	
	; Sinaliza que evento foi tratado com sucesso
	mov eax,TRUE
	ret

ProcEvento endp

start:
	
	invoke GetModuleHandle,NULL	
	mov Inst_principal,eax
	
	invoke DialogBoxParam, Inst_principal, ID_DLG1, NULL, addr ProcEvento, NULL
	invoke ExitProcess,0
	;fim do código
end start


		;chr$("text")
		;invoke MessageBox, hWin, chr$("text"), chr$("text"), MB_OK	
		
					;mov ponteiro_serial, OpenFileDlg(hWin, 0, addr CaptionText, addr szFilter)
			
			; Lê balanca
			; invoke readUranoValue,addr balanceRead, hWin						
			;invoke SetDlgItemText, hWin, 1002, addr balanceRead.vlrTotal
			;mov esi, products_pointer
			;invoke MessageBox, hWin, addr [esi+(37*type Product)].Product.product_name, NULL, MB_OK
		
		;			invoke findProductByBarcode, eax, products_pointer
;			.if eax == -1
;				mov eax, chr$("Item não encontrado")
;				MSG_BOX eax, eax				
;				ret
;			.endif
;			mov esi, eax
;			
;			invoke toDouble,addr [esi].Product.price
;			SUM_FP realValue
			;mov eax, real8$(realValue)
			;invoke SetDlgItemText, hWin, FTotalValue, eax		