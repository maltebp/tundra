#pragma once

template<typename T>
class Vec2 {
public:
	T x;
	T y;
};

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4324)
#endif

template<typename T>
class alignas(4) Vec3 {
public: 
	
	T x;
	T y;
	T z;

	bool operator==(const Vec3<T>& other) const {
		return x == other.x && y == other.y && z == other.z;
	}

	bool operator!=(const Vec3<T>& other) const {
		return !(*this == other);
	}
};

#ifdef _MSC_VER
#pragma warning(pop)
#endif