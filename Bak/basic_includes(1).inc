include \masm32\include\masm32rt.inc

; Protótipo de funcoes
ProcEvento PROTO :HWND,:UINT,:WPARAM,:LPARAM

; Identificadores de elementos de janela
.const

.data	
	Inst_principal dd ?
.code

MSG_BOX MACRO text, title
	pusha
	invoke MessageBox, NULL, text, title, MB_OK
	popa		 
ENDM



decode_wparam PROTO :WPARAM
decode_wparam proc wParam:WPARAM
	;wParam informa tipo (parte alta) e ID (parte baixa)
	mov eax,wParam ; tipo_numero -> tipo do evento + id do elemento
	mov edx,eax
	shr edx,16 ; -> desloca 16 bits para a direita, mantendo so o tipo
	and eax,0FFFFh ; -> elimina os 16 bits superiores, mantento so o numero
	ret
decode_wparam endp

mem_alloc proto :UINT

mem_alloc proc numberOfBytes:UINT
	invoke GetProcessHeap
	invoke HeapAlloc, eax, HEAP_ZERO_MEMORY, numberOfBytes
	ret
mem_alloc endp

mem_free proto :UINT

mem_free proc dataPtr:UINT
	invoke GetProcessHeap
	invoke HeapFree, eax, 0, dataPtr
	ret
mem_free endp

mem_size proto :UINT

mem_size proc dataPtr:UINT
	invoke GetProcessHeap
	invoke HeapSize, eax, 0, dataPtr
	ret
mem_size endp

mem_array_length proto :dword, :dword

mem_array_length proc dataPtr:dword, itemSize:dword	
	invoke mem_size, dataPtr	 
	mov edx, 0
	div itemSize	
	ret
mem_array_length endp