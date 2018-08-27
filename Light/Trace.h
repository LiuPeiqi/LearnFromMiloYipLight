#ifndef LEARN_LIGHT_TRACE_LIB_H
#define LEARN_LIGHT_TRACE_LIB_H
#include <functional>

#include "SDF.h"
#include "Scene.h"

namespace lpq {
	template<typename T> 
	auto Trace(SceneType<T> scene, size_t MAX_STEP = 10, T MAX_DISTANCE = 2, T EPSILON = 1e-6) {
		return [=](T x, T y, T dx, T dy) {
			T t = 0;
			for (size_t i = 0; i < MAX_STEP; ++i) {
				EmissiveType r = scene(x + dx * t, y + dy * t);
				if (r.sd < EPSILON) {
					return r.emissive;
				}
				t += r.sd;
			}
			return (T)0;
		};
	}
}
#endif //LEARN_LIGHT_TRACE_LIB_H
