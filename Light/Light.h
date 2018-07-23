#ifndef LEARN_LIGHT_PNG_LIB_H
#define LEARN_LIGHT_PNG_LIB_H
#include <functional>
#include <random>
#include <cmath>
namespace lpq {

	const double PI = 3.1415926535;
	const double TOW_PI = PI * 2.0f;

	template<typename T>
	using FSampleType = std::function<T(T, T)>;

	template<typename T>
	using FTraceType = std::function<T(T, T, T, T)>;

	template<typename T, typename DistributeType>
	auto FFSampleWhich(const DistributeType& distribute) {
		return [&](size_t N, const FTraceType& trace) {
			return [&](T x, T y) {
				auto sum = T{};
				auto dist = std::uniform_real_distribution<T>(0, 1);
				for (size_t i = 0; i < N; ++i) {
					auto a = TOW_PI * distribute(i, N);
					sum += trace(x, y, std::cos(a), std::sin(a));
				}
				return sum / N;
			};
		};
	}

	template<typename T, typename EngineType>
	auto FUniformDistribution(EngineType &engine) {
		return [&](T ignore_1, T ignore_2) {
			return std::uniform_real_distribution<T>(0, 1)(engine);
		};
	}

	template<typename T, typename EngineType>
	auto FJitteredDistribution(EngineType &engine) {
		return [&](T i, T N) {
			return (i + std::uniform_real_distribution<T>(0, 1)(engine)) / N;
		};
	}

	template<typename T>
	T StratifiedDistribution(T i, T N) {
		return i / N;
	}

	auto engine = std::mt19937();
	auto UniformDistribution = FUniformDistribution<float>(engine);
	auto FUniformSample = FFSampleWhich<float>(UniformDistribution);
	FTraceType<float> trace;
	auto UniformSampe = FUniformSample(64, trace);
	auto JitteredDistribution = FUniformDistribution<float>(engine);
	auto FJitteredSample = FFSampleWhich<float>(JitteredDistribution);
	auto JitteredSampe = FJitteredSample(64, trace);
	auto FStratifiedSample = FFSampleWhich<float>(StratifiedDistribution<float>);
	auto StratifiedSample = FStratifiedSample(64, trace);
}
#endif LEARN_LIGHT_PNG_LIB_H
