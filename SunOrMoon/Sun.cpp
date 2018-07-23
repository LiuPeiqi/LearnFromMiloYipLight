//https://zhuanlan.zhihu.com/p/30745861 by Milo Yip

#include <string>
#include <iostream>
#include <cmath>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>
#include "../PNG/pnglib.h"

const float PI = 3.1415926535;
const float TOW_PI = PI * 2.0f;

float Trace(float x, float y, float dx, float dy) {
	const size_t MAX_STEP = 10;
	const float MAX_DISTANCE = 2;
	const float EPSILON = 1e-6;
	auto CircleSDF = [](auto x, auto y, auto cx, auto cy, auto r) {
		auto ux = x - cx;
		auto uy = y - cy;
		return std::sqrt(ux*ux + uy * uy) - r;
	};
	float t = 0;
	for (size_t i = 0; i < MAX_STEP && t < MAX_DISTANCE; ++i) {
		float sd = CircleSDF(x + dx * t, y + dy * t, 0.5f, 0.5f, 0.1f);
		if (sd < EPSILON) {
			return 2;
		}
		t += sd;
	}
	return 0;
}

auto engine = std::default_random_engine();
template<size_t N>
float Sample(float x, float y) {
	float sum = 0;
	auto dist = std::uniform_real_distribution<float>(0, 1);
	for (size_t i = 0; i < N; ++i) {
		//auto a = TOW_PI * (i + dist(engine)) / N;
		//auto a = TOW_PI * i / N;
		auto a = TOW_PI * dist(engine);
		sum += Trace(x, y, std::cos(a), std::sin(a));
	}
	return sum / N;
}

int main(int argc, char* argv[])
{
	if (argc < 4) {
		std::cout << "Command arguments: width height png_path" << std::endl;
		return 0;
	}
	size_t w = std::stoi(argv[1]);
	size_t h = std::stoi(argv[2]);
	lpq::Image<lpq::RGB> img(w, h);
	auto start = std::chrono::high_resolution_clock::now();

	auto interval_line = [&](auto interval, auto offset) {
		return [&]() {
			for (size_t y = offset; y < h; y += interval) {
				for (size_t x = 0; x < w; ++x) {
					auto& pic = img.At(x, y);
					auto light = static_cast<unsigned char>(std::min(Sample<64>((float)x / w, (float)y / h) * 255.0f, 255.0f));
					pic.r = light;
					pic.g = light;
					pic.b = light;
					//pic.a = 255;
				}
			}
		};
	};
	std::vector<std::thread> threads;
	for (int i = 0; i < 8; ++i) {
		threads.emplace_back(interval_line(8, i));
	}
	for (int i = 0; i < 8; ++i) {
		threads[i].join();
	}

	//for (size_t y = 0; y < h; ++y) {
	//	for (size_t x = 0; x < w; ++x) {
	//		auto& pic = img.At(x, y);
	//		auto light = static_cast<unsigned char>(std::min(Sample<64>((float)x / w, (float)y / h) * 255.0f, 255.0f));
	//		pic.r = light;
	//		pic.g = light;
	//		pic.b = light;
	//		pic.a = 255;
	//	}
	//}

	auto finish = std::chrono::high_resolution_clock::now();
	std::cout << "Cost:" << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() / 1000.0 << "s." << std::endl;
	lpq::SaveToPNG(argv[3], img);
	return 0;
}