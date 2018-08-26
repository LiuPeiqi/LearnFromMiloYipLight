//https://zhuanlan.zhihu.com/p/30745861 by Milo Yip

#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <functional>

#include "../PNG/pnglib.h"
#include "../Light/Light.h"

int main(int argc, char* argv[])
{
	if (argc < 4) {
		std::cout << "Command arguments: width height png_path" << std::endl;
		return 0;
	}
	size_t w = std::stoi(argv[1]);
	size_t h = std::stoi(argv[2]);
	float w_r = 1.0f / w;
	float h_r = 1.0f / h;
	auto circle = lpq::CircleSDF(0.5f, 0.5f, 0.1f);
	auto trace = lpq::Trace<float>(circle);
	lpq::Image<lpq::RGB> img(w, h);
	//auto Sample = lpq::FStratifiedSample(64, trace);
	auto Sample = lpq::FJitteredSample(64, trace);
	//auto Sample = lpq::FUniformSample(64, trace);

	auto start = std::chrono::high_resolution_clock::now();
	auto last = start;
	for (size_t i = 0; i < img.GetHeight(); ++i) {
		auto step = std::chrono::high_resolution_clock::now();
		auto diff = step - last;
		auto ratio = static_cast<float>(i) * 100 * h_r;
		auto remain = diff * (h - i);
		std::cout <<"\r"<< ratio << "%\t" << "elapse:" << std::chrono::duration_cast<std::chrono::milliseconds>(step - start).count() * 1e-3
			<< "s\t\tremain:" << std::chrono::duration_cast<std::chrono::milliseconds>(remain).count() * 1e-3 << "s.";
		last = step;
		for (size_t j = 0; j < img.GetWidth(); ++j) {
			auto& pic = img.At(i, j);
			auto x = i * h_r;
			auto y = j * w_r;
			auto light = Sample(x, y) * 255;
			auto rgb = static_cast<unsigned char>(light);
			pic.r = rgb;
			pic.g = rgb;
			pic.b = rgb;
		}
	}
	auto finish = std::chrono::high_resolution_clock::now();

	std::cout << "Cost:" << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count() / 1000.0 << "s." << std::endl;
	lpq::SaveToPNG(argv[3], img);
	return 0;
}