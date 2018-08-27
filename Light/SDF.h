#ifndef LEARN_LIGHT_SDF_LIB_H
#define LEARN_LIGHT_SDF_LIB_H
#include <functional>
#include <algorithm>
#include <cmath>

namespace lpq {
	template<typename T>
	using SDFType = std::function<T(T, T)>;

	template<typename T>
	SDFType<T> CircleSDF(T cx, T cy, T r) {
		return [=](T x, T y) -> T{
			auto ux = x - cx;
			auto uy = y - cy;
			auto sd = std::sqrt(ux*ux + uy * uy) - r;
			return sd;
		};
	}

	template<typename T>
	bool InRange(T x, T min, T max) {
		return x > min && x < max;
	}

	template<typename T>
	bool InBoundary(T x, T min, T max) {
		return x == min || x == max;
	}

	template<typename T>
	SDFType<T> RectSDF(T lx, T by, T width, T height) {
		auto rx = lx + width;
		auto ty = by + height;
		return [=](T x, T y) -> T {
			if (InBoundary(x, lx, rx) && InBoundary(y, by, ty)) {
				return std::make_tuple(0, 2);
			}else if(InRange(x, lx, rx) && InRange(y, by, ty)) {
				auto sd = std::max(std::max(lx -x , x - rx), std::max(by - y, y - ty));
				return sd;
			}
			else {
				auto sd = std::min(std::min(lx - x, x - rx), std::min(by - y, y - ty));
				return sd;
			}
		};
	}

}
#endif //LEARN_LIGHT_SDF_LIB_H
