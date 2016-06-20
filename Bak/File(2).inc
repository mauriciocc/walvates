.const

msg_open_file_error_title   db "Atenção!",0
msg_open_file_error_message db "Não foi possível estabelecer comunicação!",0

.data

.code

OPEN_FILE_MACRO MACRO path, hWin
	
	invoke CreateFile, path, GENERIC_READ or GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL
	
	.if eax == INVALID_HANDLE_VALUE
		invoke MessageBox, hWin, addr msg_open_file_error_message, addr msg_open_file_error_title, MB_OK or MB_ICONERROR
		ret		
	.endif
		 
ENDM

readAllBytes proto :UINT

readAllBytes proc filePtr:UINT
	local fileSize :UINT
	local allocatedHeap :UINT
	local bytes_read :UINT	
	
	invoke GetFileSize, filePtr, NULL
	mov fileSize, eax
	
	invoke GetProcessHeap
	invoke HeapAlloc, eax, HEAP_ZERO_MEMORY, fileSize
	mov allocatedHeap, eax
	
	invoke ReadFile, filePtr, allocatedHeap, fileSize, addr bytes_read, NULL
	
	mov eax, allocatedHeap
	ret	
readAllBytes endp