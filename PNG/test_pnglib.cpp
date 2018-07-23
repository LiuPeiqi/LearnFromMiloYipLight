#include "pnglib.h"

#include <iostream>
using namespace lpq;

int main(){
	Image<RGB> image(256, 256);
	for (size_t y = 0; y < 256; y++) {
		for (size_t x = 0; x < 256; x++) {
			auto& rgb = image.At(x, y);
			rgb.r = (unsigned char)x;
			rgb.g = (unsigned char)y;
			rgb.b = 128;                 /* B */
		}
	}
	SaveToPNG("./test.png", image);
	return 0;
}

