compile:
	gcc bmp2bin.c -o bmp2bin
usage:
	bmp2bin sample/1bit.bmp
	bmp2bin sample/24bit.bmp
	bmp2bin sample/32bit.bmp

description:
	covert image bmp file to bitmap then you can covert bitmap to hex and
	integrate these to header file.
