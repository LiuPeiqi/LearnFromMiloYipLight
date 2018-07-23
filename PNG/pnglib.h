//https://zhuanlan.zhihu.com/p/26525083 by Milo Yip
#ifndef LEARN_LIGHT_PNG_LIB_H
#define LEARN_LIGHT_PNG_LIB_H

#include <cassert>
#include <fstream>
#include <memory>
#include <filesystem>
namespace lpq {
	struct RGB {
		unsigned char r, g, b;
	};
	struct RGBA
	{
		unsigned char r, g, b, a;
	};

	//template<typename ChannelType>
	//void SaveToPNG(std::filesystem::path, Image<ChannelType>);

	template<typename ChannelType>
	class Image {
		template<typename ChannelType>
		friend void SaveToPNG(const std::filesystem::path&, const Image<ChannelType>&);
	public:
		const size_t channel_size = sizeof(ChannelType);
		const size_t deepth = channel_size * 8;
		Image(size_t width, size_t height)
			:width(width), height(height)
		{
			auto size = width * height * channel_size;
			data = std::make_unique<unsigned char[]>(size);
		}
		ChannelType& At(size_t x, size_t y) {
			assert(x < width && y < height);
			auto pos = y * width * channel_size + x * channel_size;
			return reinterpret_cast<ChannelType&>(data[pos]);
		}

		size_t GetWidth() const { return width; }
		size_t GetHeight() const { return height; }
	private:
		std::unique_ptr<unsigned char[]> data;
		size_t width, height;
	};

	template<typename ChannelType>
	void SaveToPNG(const std::filesystem::path& file_path, const Image<ChannelType>& image) {
		auto w = image.GetWidth();
		auto h = image.GetHeight();
		auto alpha = image.channel_size == 4;
		const auto* img = image.data.get();
		auto out = std::ofstream(file_path, std::ios::binary);
		auto SVPNG_PUT = [&](unsigned char c) {out << c; };
		static const unsigned t[] = { 0, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
			/* CRC32 Table */    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c };
		unsigned a = 1, b = 0, c, p = w * image.channel_size + 1, x, y, i;   /* ADLER-a, ADLER-b, CRC, pitch */
		auto SVPNG_U32C = [&](auto u) {
			SVPNG_PUT(u >> 24);
			SVPNG_PUT((u >> 16) & 255);
			SVPNG_PUT((u >> 8) & 255);
			SVPNG_PUT(u & 255);
		};
		auto SVPNG_U8C = [&](auto u) {
			SVPNG_PUT(u);
			c ^= u;
			c = (c >> 4) ^ t[c & 15];
			c = (c >> 4) ^ t[c & 15];
		};
		auto SVPNG_U8AC = [&](auto ua, auto l) {
			for (int i = 0; i < l; ++i) {
				SVPNG_U8C(ua[i]);
			}
		};
		auto SVPNG_U16LC = [&](auto u) {
			SVPNG_U8C(u & 255);
			SVPNG_U8C((u >> 8) & 255);
		};
		auto SVPNG_U32U = [&](auto u) {
			SVPNG_U8C(u >> 24);
			SVPNG_U8C((u >> 16) & 255);
			SVPNG_U8C((u >> 8) & 255);
			SVPNG_U8C(u & 255);
		};
		auto SVPNG_U8ADLER = [&](auto u){
			SVPNG_U8C(u);
			a = (a + u) % 65521;
			b = (b + a) % 65521;
		};
		auto SVPNG_BEGIN = [&](auto s, auto l) {
			SVPNG_U32C(l);
			c = ~0U;
			SVPNG_U8AC(s, 4);
		};
		auto SVPNG_END = [&]() {
			SVPNG_U32C(~c);
		};
		out << "\x89PNG\r\n\32\n";	                /* Magic */
		SVPNG_BEGIN("IHDR", 13);                    /* IHDR chunk { */
		SVPNG_U32C(w); SVPNG_U32C(h);               /*   Width & Height (8 bytes) */
		SVPNG_U8C(8); SVPNG_U8C(alpha ? 6 : 2);     /*   Depth=8, Color=True color with/without alpha (2 bytes) */
		SVPNG_U8AC("\0\0\0", 3);                    /*   Compression=Deflate, Filter=No, Interlace=No (3 bytes) */
		SVPNG_END();                                /* } */
		SVPNG_BEGIN("IDAT", 2 + h * (5 + p) + 4);   /* IDAT chunk { */
		SVPNG_U8AC("\x78\1", 2);                    /*   Deflate block begin (2 bytes) */
		for (y = 0; y < h; y++) {                   /*   Each horizontal line makes a block for simplicity */
			SVPNG_U8C(y == h - 1);                  /*   1 for the last block, 0 for others (1 byte) */
			SVPNG_U16LC(p); SVPNG_U16LC(~p);        /*   Size of block in little endian and its 1's complement (4 bytes) */
			SVPNG_U8ADLER(0);                       /*   No filter prefix (1 byte) */
			for (x = 0; x < p - 1; x++, img++)
				SVPNG_U8ADLER(*img);                /*   Image pixel data */
		}
		SVPNG_U32C((b << 16) | a);                  /*   Deflate block end with adler (4 bytes) */
		SVPNG_END();                                /* } */
		SVPNG_BEGIN("IEND", 0); SVPNG_END();        /* IEND chunk {} */
	}
}

#endif // !LEARN_LIGHT_PNG_LIB_H
