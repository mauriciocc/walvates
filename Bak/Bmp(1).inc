BMP STRUCT
	
	
BMP ends

BmpHeader STRUCT
	bfType word
	bfSize dword 
	bfReser1 word	
	bfReser2 word
	bfOffSetBits dword
BmpHeader ends

BmpBitMap STRUCT
	BiSize dword
	BiWidth dword
	BiHeight dword
	BiPlanes word
	BiBitCount word
	BiCompress dword
	BiSizeImag dword
	BiXPPMeter dword
	BiYPPMeter dword
	BiClrUsed dword
	BiClrImpor dword
BmpBitMap ends

.const

.data

.code