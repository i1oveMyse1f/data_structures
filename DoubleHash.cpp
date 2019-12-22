#include <iostream>

template<class T = unsigned>
struct DoubleHash {
	friend T pow(const T a, const T n, const T p) {
		if (n == 0)
			return 1;
		unsigned long long t = pow(a, n >> 1, p);
		t = t * t % p;
		if (n & 1)
			t = t * a % p;
		return t;
	}
	friend T inv(const T x, const T p) {
		return pow(x, p - 2, p);
	}

	T x1, x2;
	static const T MOD1 = 1e9 + 9;
	static const T MOD2 = 1e9 + 7;

	DoubleHash(T x = 0) : x1(x), x2(x) {}
	DoubleHash(T x1, T x2) : x1(x1), x2(x2) {}

	DoubleHash operator+(const DoubleHash other) const {
		T res1 = x1 + other.x1;
		T res2 = x2 + other.x2;

		if (res1 >= MOD1)
			res1 -= MOD1;
		if (res2 >= MOD2)
			res2 -= MOD2;
		return { res1, res2 };
	}

	DoubleHash operator-(const DoubleHash other) const {
		T res1, res2;
		if (x1 >= other.x1)
			res1 = x1 - other.x1;
		else
			res1 = MOD1 + x1 - other.x1;

		if (x2 >= other.x2)
			res2 = x2 - other.x2;
		else
			res2 = MOD2 + x2 - other.x2;

		return { res1, res2 };
	}

	DoubleHash operator*(const DoubleHash other) const {
		return { x1 * 1ULL * other.x1 % MOD1,
				 x2 * 1ULL * other.x2 % MOD2 };
	}

	DoubleHash operator+= (const DoubleHash other) {
		return *this = *this + other;
	}

	DoubleHash operator-= (const DoubleHash other) {
		return *this = *this - other;
	}

	DoubleHash operator*= (const DoubleHash other) {
		return *this = *this - other;
	}

	DoubleHash operator/(DoubleHash other) {
		return { x1 * 1ULL * inv(other.x1, MOD1) % MOD1,
				 x2 * 1ULL * inv(other.x2, MOD2) % MOD2 };
	}

	DoubleHash operator/=(const DoubleHash other) {
		return *this = *this / other;
	}

	friend std::istream& operator>>(std::istream& in, DoubleHash& a) {
		in >> a.x1;
		a.x2 = a.x1;
		return in;
	}

	friend std::ostream& operator<<(std::ostream& out, const DoubleHash a) {
		return out << "{ " << a.x1 << ", " << a.x2 << " } ";
	}

	bool operator==(const DoubleHash other) const {
		return x1 == other.x1 && x2 == other.x2;
	}

	bool operator!=(const DoubleHash other) const {
		return !(*this == other);
	}
};
