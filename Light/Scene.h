#ifndef LEARN_LIGHT_SCENE_LIB_H
#define LEARN_LIGHT_SCENE_LIB_H

#include <algorithm>
#include <functional>

#include "SDF.h"

namespace lpq {
	template<typename T>
	struct EmissiveType
	{
		T sd;
		T emissive;
	};

	template<typename T>
	using SceneType = std::function<EmissiveType<T>(T, T)>;

	template<typename T>
	SceneType<T> MakeSimpleScene(SDFType<T> SDF, T emissive) {
		return [=](T x, T y) {
			return { SDF(x, y), emissive };
		};
	}

	template<typename T>
	SceneType<T> UnionScene(SceneType<T> a, SceneType<T> b)
	{
		return [=](T x, T y) {
			EmissiveType<T> ra = a(x, y);
			EmissiveType<T> rb = b(x, y);
			return a.sd < b.sd ? ra : rb;
		}
	}

	template<typename T>
	SceneType<T> InterscetionScene(SceneType<T> a, SceneType<T> b)
	{
		return [=](T x, T y) {
			EmissiveType<T> ra = a(x, y);
			EmissiveType<T> rb = b(x, y);
			return { std::max(ra.sd, rb.sd), (ra.sd > rb.sd) ? rb.emissive: ra.emissive };
		};
	}

	template<typename T>
	SceneType<T> SubtractScene(SceneType<T> a, SceneType<T> b) {
		return [=](T x, T y) {
			EmissiveType<T> ra = a(x, y);
			EmissiveType<T> rb = b(x, y);
			return {std::max(ra.sd, -rb.sd), ra.emissive }:
		};
	}
}




#endif //LEARN_LIGHT_SCENE_LIB_H
