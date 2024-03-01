#pragma once

template<typename T>
class Vec2 {
public:
	T x;
	T y;
	T z;
};

template<typename T>
class Vec3 {
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