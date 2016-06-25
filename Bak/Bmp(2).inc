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
	biSize dword
	biWidth dword
	biHeight dword
	biPlanes word
	biBitCount word
	biCompress dword
	biSizeImag dword
	biXPPMeter dword
	biYPPMeter dword
	biClrUsed dword
	biClrImpor dword
BmpBitMap ends
PBmpBitMap TYPEDEF PTR BmpBitMap

Bmp STRUCT
	header BmpHeader <>
	bitMap BmpBitMap <>	
Bmp ends
PBmp TYPEDEF PTR Bmp

.code