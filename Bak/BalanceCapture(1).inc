.const

serial_capture_file db "serial_captured_input.txt",0
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

.code

readUranoValue proto :HWND
readUranoValue proc hWin:HWND	
	
	local serial_ptr :dword
	
	local bytes_read :dword
	local result :dword
		
	OPEN_FILE_MACRO addr serial_capture_file, hWin
	.if eax == INVALID_HANDLE_VALUE
		ret
	.endif					
	mov serial_ptr, eax
					
		invoke PurgeComm, serial_ptr, PURGE_RXCLEAR or PURGE_TXCLEAR
		
		invoke mem_alloc, type BalanceCapture
		mov result, eax
		
		invoke ReadFile, serial_ptr, result, type BalanceCapture, addr bytes_read, NULL
						
	invoke CloseHandle, serial_ptr
	
	mov eax, result
	ret
readUranoValue endp