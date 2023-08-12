/*
Copyright @ 2023 Bernie Chen
Permission is hereby granted, free of charge, to any persion obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit person whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS, OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PURTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include<stdio.h>
#include<stdlib.h>
#include<error.h>
#include<string.h>
#define FORE_BYTES_TO_BITS	(32)
#define FORE_BYTES_ALIGN	(4)
#define BITS_PER_BYTE		(8)
//BMP file header structure
#pragma pack(1)
typedef struct {		//header size 14 byte
    unsigned short signature;	//BMP format, 0x4D42('BM')
    unsigned int fileSize;	//all of BMP file size
    unsigned int reserved;	//usuall 0
    unsigned int dataOffset;	//image data offset from header start
} BMPFileHeader;
#pragma pack()

//BMP image information header structure
#pragma pack(1)
typedef struct {		//header size 40 byte
    unsigned int headerSize;	//header size: 40 byte
    int width;			//image width
    int height;			//image height
    unsigned short planes;	//fix to 1
    unsigned short bitsPerPixel;
    unsigned int compression;	//if 0, represent no compression
    unsigned int imageSize;	//image data length
    int xPixelsPerMeter;
    int yPixelsPerMeter;
    unsigned int colorsUsed;
    unsigned int importantColors;
} BMPInfoHeader;
#pragma pack()
static void usage()
{
	printf("usage:\n");
	printf("bmp2bin <src.bmp>\n");
	return;
}
int main(int argc, char *argv[])
{
	FILE *file = NULL;
	unsigned char *imgData = NULL;
	int bytesPerRow = 0, bytesPerPixel = 0, i = 0, j = 0, k = 0;
	BMPFileHeader fileHeader;
	BMPInfoHeader infoHeader;

	if(argc < 2)
		goto usage;

	file = fopen(argv[1], "rb");
	if (file == NULL) {
		perror("failed:");
		return -1;
	}

	fread(&fileHeader, sizeof(BMPFileHeader), 1, file);
	if(fileHeader.signature != 0x4D42) {
		printf("This is not BMP file\n");
		return -1;
	}
	printf("header size:14 bytes\n");
	printf("BMP header:\n");
	printf("signature:	0x%04X\n", fileHeader.signature);
	printf("BMP file size:	  %d\n", fileHeader.fileSize);
	printf("reserved:	  %d\n", fileHeader.reserved);
	printf("dataOffset:	0x%04X\n", fileHeader.dataOffset);

	fread(&infoHeader, sizeof(infoHeader), 1, file);

	printf("info size:40 bytes\n");
	printf("\nInfo:\n");
	printf("width:		  %d\n", infoHeader.width);
	printf("height:		  %d\n", infoHeader.height);
	printf("planes:		  %d\n", infoHeader.planes);
	printf("bitsPerPixel:     %d\n", infoHeader.bitsPerPixel);
	printf("compression:	  %d\n", infoHeader.compression);
	printf("imageSize:	  %d\n", infoHeader.imageSize);
	printf("xPixelsPerMeter:  %d\n", infoHeader.xPixelsPerMeter);
	printf("yPixelsPerMeter:  %d\n", infoHeader.yPixelsPerMeter);
	printf("colorUsed:	  %d\n", infoHeader.colorsUsed);
	printf("importantColors:  %d\n", infoHeader.importantColors);

	if(infoHeader.bitsPerPixel == 1) {
		bytesPerRow = ((infoHeader.bitsPerPixel * infoHeader.width + 31) / FORE_BYTES_TO_BITS) * FORE_BYTES_ALIGN;
	} else if(infoHeader.bitsPerPixel == 24 || infoHeader.bitsPerPixel == 32) {
		bytesPerPixel = infoHeader.bitsPerPixel / BITS_PER_BYTE;
		bytesPerRow = (bytesPerPixel * infoHeader.width) + \
				((bytesPerPixel * infoHeader.width) % FORE_BYTES_ALIGN); //+ align byte
	}

	printf("\n%d bytes needed per one row\n\n", bytesPerRow);

	imgData = (unsigned char *)malloc(infoHeader.imageSize);

	fseek(file, fileHeader.dataOffset, SEEK_SET);

	fread(imgData, sizeof(unsigned char), infoHeader.imageSize, file);

	printf("\n%d raw bytes(little endian):\n", infoHeader.imageSize);
	for(i = 0 ; i < infoHeader.imageSize ; i++) {
		if(i > 0 && (i % bytesPerRow == 0))
			printf("\n");
		printf("0x%02X ", imgData[i]);
	}

	printf("\n\n1bit layout:\n");

	if(infoHeader.bitsPerPixel == 1) {
		unsigned int temp = 0;
		unsigned int mask = 1;
		unsigned int lastByte = infoHeader.width / BITS_PER_BYTE;
		//example: 6x8 bmp, width:6, it will be supplemented with 2 bits to make up 1 byte
		//so we will right offset 2 bit to get real bit
		unsigned int alignBits = BITS_PER_BYTE % infoHeader.width;
		for(i = 0 ; i < infoHeader.height ; i++) {
			for(j = 0 ; j <= lastByte; j += FORE_BYTES_ALIGN) {
				temp = (unsigned int)imgData[j]; //4 bytes align
				if(j == lastByte)
					temp >>= alignBits;
				for(k = (infoHeader.width - 1) ; k >= 0 ; k--) {
					temp >> k & mask ? printf("1 ") : printf("0 ");
				}
			}
			printf("\n");
		}
	} else if(infoHeader.bitsPerPixel == 24 || infoHeader.bitsPerPixel == 32) {
		for(i = 0 ; i < infoHeader.height ; i++) {
			for(j = 0 ; j < bytesPerPixel * infoHeader.width ; j += bytesPerPixel)
				imgData[j] == 0x00 ? printf("0 ") : printf("1 ");
			printf("\n");
		}
	}

	free(imgData);
	fclose(file);
	return 0;
usage:
	usage();
	return -1;
}
