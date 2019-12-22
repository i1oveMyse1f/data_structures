#include <iostream>
#include <vector>

template<class A = unsigned, class B = int>
struct HashMap {
private:
	inline bool is_prime(int x) {
		for (int i = 2; i * i <= x; i++)
			if (x % i == 0)
				return false;
		return true;
	}

	inline int find_prime(int l) {
		while (!is_prime(l))
			++l;
		return l;
	}
	std::vector<std::pair<A, B>> hash_table; // { key, value }

public:
	inline HashMap(int size) : hash_table(find_prime(2 * size)) {}

	inline void insert(A x, B value = 1) {
		auto t = x % hash_table.size();
		while (hash_table[t].first && hash_table[t].first != x) {
			++t;
			if (t == hash_table.size())
				t = 0;
		}
		hash_table[t].first = x;
		hash_table[t].second += value;
	}

	inline bool contains(A x) const {
		auto t = x % hash_table.size();
		while (hash_table[t].first) {
			if (hash_table[t].first == x)
				return true;
			++t;
			if (t == hash_table.size())
				t = 0;
		}
		return false;
	}

	inline B operator[](A x) const {
		auto t = x % hash_table.size();
		while (hash_table[t].first) {
			if (hash_table[t].first == x)
				return hash_table[t].second;
			++t;
			if (t == hash_table.size())
				t = 0;
		}
		return 0;
	}

	inline B count(A x) const {
		return operator[](x);
	}
};
