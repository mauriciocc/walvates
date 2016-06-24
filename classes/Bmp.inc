BMP STRUCT
	BmpHeader <>
	BmpBitMap <>	
BMP ends

BmpHeader STRUCT
	bfType word
	bfSize dword 
	bfReser1 word	
	bfReser2 word
	bfOffSetBits dword
BmpHeader ends

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

.const

.data

.code