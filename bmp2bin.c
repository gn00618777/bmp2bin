#include<stdio.h>
#include<error.h>

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
	printf("bmp2bin <src.bmp> <output.bin>\n");
	return;
}
int main(int argc, char *argv[])
{
	FILE *file;
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

	printf("BMP header:\n");
	printf("signature:	0x%04X\n", fileHeader.signature);
	printf("BMP file size:	0x%04X\n", fileHeader.fileSize);
	printf("reserved:	0x%04X\n", fileHeader.reserved);
	printf("dataOffset:	0x%04X\n", fileHeader.dataOffset);

	fread(&infoHeader, sizeof(infoHeader), 1, file);
unsigned int imageSize;     //image data length
    int xPixelsPerMeter;
    int yPixelsPerMeter;
    unsigned int colorsUsed;
    unsigned int importantColors;

	printf("\nInfo:\n");
	printf("width:		0x%04X\n", infoHeader.width);
	printf("height:		0x%04X\n", infoHeader.height);
	printf("planes:		0x%02X\n", infoHeader.planes);
	printf("bitsPerPixel:   0x%02X\n", infoHeader.bitsPerPixel);
	printf("compression:	0x%04X\n", infoHeader.compression);
	printf("imageSize:	0x%04X\n", infoHeader.imageSize);
	printf("xPixelsPerMeter:%d\n", infoHeader.xPixelsPerMeter);
	printf("yPixelsPerMeter:%d\n", infoHeader.yPixelsPerMeter);
	printf("colorUsed:	0x%04X\n", infoHeader.colorsUsed);
	printf("importantColors:0x%04X\n", infoHeader.importantColors);

	fclose(file);
	return 0;
usage:
	usage();
	return -1;
}
