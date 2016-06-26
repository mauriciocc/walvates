.const

.data

BmpHeader STRUCT
	bfType 			word 	?
	bfSize 			dword 	?
	bfReser1 		word 	?	
	bfReser2 		word 	?	
	bfOffSetBits 	dword 	?
BmpHeader ends
PBmpHeader TYPEDEF PTR BmpHeader

BmpBitMap STRUCT
	biSize 		dword	?
	biWidth 	dword	?
	biHeight 	dword	?
	biPlanes 	word	?
	biBitCount 	word	?
	biCompress 	dword	?
	biSizeImag 	dword	?
	biXPPMeter 	dword	?
	biYPPMeter 	dword	?
	biClrUsed 	dword	?
	biClrImpor 	dword	?
BmpBitMap ends
PBmpBitMap TYPEDEF PTR BmpBitMap

Bmp STRUCT
	header BmpHeader <>
	bitMap BmpBitMap <>	
Bmp ends
PBmp TYPEDEF PTR Bmp

.code

CalculateWidthAlignment proc bmpWidth:dword
	local alignmentFill :dword
	mov alignmentFill, -1
	.repeat
		inc alignmentFill
		
		mov eax, bmpWidth
		add eax, alignmentFill
						
		mov ecx, 4
		
		mov edx, 0
		div ecx		
	.until (edx == 0)
	
	mov eax, alignmentFill	
	ret	
CalculateWidthAlignment endp

ReadBmpImageDataLine proc uses esi edi bmpPtr:dword, lineNumber:dword
	local widthAlign :dword
	local lineContent :dword
	
	mov esi, bmpPtr
	
	invoke CalculateWidthAlignment, [esi].Bmp.bitMap.biWidth
	mov widthAlign, eax
	
	invoke mem_alloc, [esi].Bmp.bitMap.biWidth
	mov lineContent, eax

	; Compute line position in image data (considering bmp 4 byte align)
	mov eax, lineNumber
	mov ecx, [esi].Bmp.bitMap.biWidth
	add ecx, widthAlign
	mul ecx
	add eax, [esi].Bmp.header.bfOffSetBits
	mov edi, lineContent
	
	mov edx, 0
	mov ecx, [esi].Bmp.bitMap.biWidth	
	.while (ecx > 0)		
			
		mov dl, byte ptr [esi+eax]
		
		.if (edx == 255)
			mov [edi], byte ptr "0"
		.elseif (edx == 0)
			mov [edi], byte ptr "1"
		.else
			mov [edi], byte ptr "9"
		.endif
		
		inc edi
		inc eax		
		dec ecx		
	.endw
	
	mov eax, lineContent
	ret
ReadBmpImageDataLine endp