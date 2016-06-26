include basic_includes.inc
include classes\File.inc
include classes\BalanceCapture.inc
include classes\Product.inc
include classes\Number.inc
include classes\Bmp.inc
include classes\Barcode.inc
include classes\ViewCtrl.inc

.data

.code

ProcEvento proc uses edi esi hWin:HWND,uMsg:UINT,wParam:WPARAM,lParam:LPARAM
	mov eax,uMsg	

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
	mov hInstance,eax
	
	invoke DialogBoxParam, hInstance, MainDialog, NULL, addr ProcEvento, NULL
	invoke ExitProcess,0
	
end start