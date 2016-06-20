.const

serial_port db "COM1",0

.data
BalanceCapture struct
	ignore1 db 18 dup(?)
	peso db 6 dup(0)
	ignore2 db 6 dup(?)
	precoKg db 7 dup(0)
	ignore3 db 6 dup(?)
	vlrTotal db 7 dup(0)
	ignore4 db 6 dup(?)
BalanceCapture ends
PBalanceCapture TYPEDEF PTR BalanceCapture

readUranoValue proto :UINT,:HWND


.code

readUranoValue proc balanceCapturePtr:UINT,hWin:HWND	
	
	local serial_ptr :UINT
	local read_bytes :UINT
	
	OPEN_FILE_MACRO addr serial_ptr, hWin			
					
	mov serial_ptr, eax
				
	invoke PurgeComm, serial_ptr, PURGE_RXCLEAR or PURGE_TXCLEAR
				
	invoke ReadFile, serial_ptr, balanceCapturePtr, type BalanceCapture, addr read_bytes, NULL
			
	invoke CloseHandle, serial_ptr

readUranoValue endp