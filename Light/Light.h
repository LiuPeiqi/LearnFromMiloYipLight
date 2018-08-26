#ifndef LEARN_LIGHT_LIGHT_LIB_H
#define LEARN_LIGHT_LIGHT_LIB_H
#include <functional>
#include <random>
#include <cmath>
#include <iostream>
namespace lpq {

	const double PI = 3.1415926535;
	const double TOW_PI = PI * 2.0f;

	template<typename T>
	using FSampleType = std::function<T(T, T)>;

	template<typename T>
	using FTraceType = std::function<T(T, T, T, T)>;

	template<typename T, typename DistributeType>
	auto FFSampleWhich(const DistributeType& distribute) {
		return [&](const size_t N, const FTraceType<T>& trace) {
			return [=](T x, T y) {
				auto sum = T{};
				auto dist = std::uniform_real_distribution<T>(0, 1);
				for (size_t i = 0; i < N; ++i) {
					T a = static_cast<T>(TOW_PI) * distribute(static_cast<T>(i), static_cast<T>(N));
					sum += trace(x, y, std::cos(a), std::sin(a));
				}
				return sum / N;
			};
		};
	}

	template<typename T, typename EngineType>
	auto FUniformDistribution(EngineType &engine) {
		return [&](T ignore_1, T ignore_2) {
			auto r = std::uniform_real_distribution<T>(0, 1)(engine);
			return r;
		};
	}

	template<typename T, typename EngineType>
	auto FJitteredDistribution(EngineType &engine) {
		return [&](T i, T N) {
			auto r = (i + std::uniform_real_distribution<T>(0, 1)(engine)) / N;
			return r;
		};
	}

	template<typename T>
	T StratifiedDistribution(T i, T N) {
		return i / N;
	}

	template<typename T>
	auto CircleSDF(T cx, T cy, T r) {
		return [=](T x, T y) -> T{
			auto ux = x - cx;
			auto uy = y - cy;
			auto sd = std::sqrt(ux*ux + uy * uy) - r;
			return sd;
		};
	}

	template<typename T>
	bool InRange(T x, T min, T max) {
		return x >= min && x <= max;
	}

	template<typename T>
	auto RectSDF(T lx, T by, T width, T height) {
		auto rx = lx + width;
		auto ty = by + height;
		return [=](T x, T y) -> T {
			return InRange(x, lx, rx) && InRange(y, by, ty) ? -1 : 1;
		};
	}

	template<typename T> 
	FTraceType<T> Trace(std::function<T(T, T)> SDF, size_t MAX_STEP = 10, T MAX_DISTANCE = 2, T EPSILON = 1e-6) {
		return [=](T x, T y, T dx, T dy) {
			T t = 0;
			for (size_t i = 0; i < MAX_STEP; ++i) {
				T sd = SDF(x + dx * t, y + dy * t);
				if (sd < EPSILON) {
					return (T)2;
				}
				t += sd;
			}
			return (T)0;
		};
	}

	auto engine = std::mt19937();
	auto UniformDistribution = FUniformDistribution<float>(engine);
	auto FUniformSample = FFSampleWhich<float>(UniformDistribution);
	auto JitteredDistribution = FJitteredDistribution<float>(engine);
	auto FJitteredSample = FFSampleWhich<float>(JitteredDistribution);
	auto FStratifiedSample = FFSampleWhich<float>(StratifiedDistribution<float>);
}
#endif LEARN_LIGHT_LIGHT_LIB_H
