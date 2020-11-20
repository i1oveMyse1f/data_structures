template<class A, class B>
class MapForPoor {
public:
    bool contains(const A& key) const {
        for (auto& sorted : sets) {
            int left = 0, right = sorted.size();
            while (right - left > 1) {
                int mid = (left + right) / 2;
                if (key < sorted[mid].first)
                    right = mid;
                else 
                    left = mid;
            }
            if (sorted[left].first == key)
                return true;
        }
        return false;
    }

    B operator[](const A& key) const {
        for (auto& sorted : sets) {
            int left = 0, right = sorted.size();
            while (right - left > 1) {
                int mid = (left + right) / 2;
                if (key < sorted[mid].first)
                    right = mid;
                else
                    left = mid;
            }
            if (sorted[left].first == key)
                return sorted[left].second;
        }
        throw std::out_of_range("");
    }

    void insert(const std::pair<A, B> &keyValPair) {
        ++size_;
        sets.insert(sets.begin(), { keyValPair });
        if (sets.size() == 1)
            return;
        auto it_first = sets.begin();
        auto it_second = ++sets.begin();
        while (it_second != sets.end() && it_first->size() == it_second->size()) {
            auto new_first = sets.insert(it_first, std::vector<std::pair<A, B>>());
            std::merge(it_first->begin(), it_first->end(), 
                    it_second->begin(), it_second->end(),
                    std::back_inserter(*new_first));
            sets.erase(it_first);
            sets.erase(it_second);
            it_first = new_first;
            it_second = std::next(it_first);
        }
    }

    int size() const { return size_; }

private:
    std::list<std::vector<std::pair<A, B>>> sets;
    size_t size_;
};
