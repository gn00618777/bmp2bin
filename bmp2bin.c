#include<stdio.h>
#include<stdlib.h>
#include<error.h>
#include<string.h>
#define FORE_BYTES_TO_BITS	(32)
#define FORE_BYTES_ALIGN	(4)
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
	FILE *file;
	unsigned char *imgData = NULL;
	int bytesPerRow = 0, i = 0, j = 0, k = 0;
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

	if(infoHeader.bitsPerPixel > 1) {
		printf("only support 1 bit color\n");
		return 0;
	}

	bytesPerRow = ((infoHeader.bitsPerPixel * infoHeader.width + 31) / FORE_BYTES_TO_BITS) * FORE_BYTES_ALIGN;

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

	printf("\n\nlayout:\n");
	unsigned int temp = 0;
	unsigned int mask = 1;
	for(i = 0 ; i < infoHeader.height ; i++) {
		for(j = 0 ; j < bytesPerRow; j += FORE_BYTES_ALIGN) {
			temp = (unsigned int)imgData[j];
			for(k = 31 ; k >= 0 ; k--) {
				temp >> k & mask ? printf("1 ") : printf("0 ");
			}
		}
		printf("\n");
	}

	free(imgData);
	fclose(file);
	return 0;
usage:
	usage();
	return -1;
}
