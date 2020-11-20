#include <chrono>
#include <numeric>
#include <optional>
#include <random>
#include <vector>

uint64_t SumSquares(const std::vector<uint64_t>& elements)
{
    return std::accumulate(elements.begin(), elements.end(), static_cast<uint64_t>(0),
                           [](uint64_t total, uint64_t current) {
                               return total + current * current;
                           });
}

class LinearHashFunction {
public:
    LinearHashFunction(uint64_t linear_coefficient = 1, uint64_t free_term = 0,
                       uint32_t prime_base = 3, uint32_t other_base = 2)
            : linear_coefficient_(linear_coefficient)
            , free_term_(free_term)
            , prime_base_(prime_base)
            , other_base_(other_base)
    {
    }

    uint32_t operator()(uint64_t point) const
    {
        uint32_t result = (point * linear_coefficient_ + free_term_) % prime_base_ % other_base_;
        if (result < 0) {
            result += prime_base_;
        }
        return result;
    }

private:
    uint64_t linear_coefficient_, free_term_;
    uint32_t prime_base_, other_base_;
};

LinearHashFunction GenerateRandomLinearHashFunction(uint32_t prime_base, uint32_t other_base)
{
    static const int kGeneratorSeed = 228;
    static std::mt19937 generator(kGeneratorSeed);
    std::uniform_int_distribution<uint64_t> rand_1_p(0, prime_base - 1);
    std::uniform_int_distribution<uint64_t> rand_0_p(0, prime_base - 1);
    auto linear_coefficient = rand_1_p(generator);
    auto free_term = rand_0_p(generator);
    return LinearHashFunction(linear_coefficient, free_term, prime_base, other_base);
}

class FixedSet {
public:
    void Initialize(const std::vector<int>& elements);
    bool Contains(int value) const;

private:
    class InnerHashTable {
    public:
        void Initialize(const std::vector<int>& elements);
        bool Contains(int value) const;

    private:
        LinearHashFunction hash_function_;
        std::vector<std::optional<int>> hash_table_;
        bool HaveCollisionAndFill(const std::vector<int>& elements);
        static const int kPrimeBase = 2'000'000'011;
    };

    LinearHashFunction hash_function_;
    std::vector<InnerHashTable> inner_hash_tables_;
    static const int kPrimeBase = 2'000'000'011;
};

void FixedSet::Initialize(const std::vector<int>& elements)
{
    uint32_t hash_table_size = elements.size();
    const int coefficient_hash_table_size = 8;
    auto max_sum_squares_buckets_size = coefficient_hash_table_size * hash_table_size;

    std::vector<uint64_t> buckets_size(elements.size());
    do {
        hash_function_ = GenerateRandomLinearHashFunction(kPrimeBase, elements.size());
        std::fill(buckets_size.begin(), buckets_size.end(), 0);
        for (int x : elements) {
            ++buckets_size[hash_function_(x)];
        }
    } while (SumSquares(buckets_size) > max_sum_squares_buckets_size);

    std::vector<std::vector<int>> buckets(hash_table_size);
    for (int number : elements) {
        buckets[hash_function_(number)].push_back(number);
    }

    inner_hash_tables_.assign(hash_table_size, InnerHashTable());
    for (size_t i = 0; i < inner_hash_tables_.size(); ++i) {
        inner_hash_tables_[i].Initialize(buckets[i]);
    }
}

bool FixedSet::Contains(int value) const
{
    if (inner_hash_tables_.empty()) {
        return false;
    }
    auto bucket_number = hash_function_(value);
    return inner_hash_tables_[bucket_number].Contains(value);
}

bool FixedSet::InnerHashTable::HaveCollisionAndFill(const std::vector<int>& elements)
{
    std::fill(hash_table_.begin(), hash_table_.end(), std::optional<int>());
    bool have_collisions = false;
    for (size_t i = 0; i < elements.size() && !have_collisions; ++i) {
        auto hash = InnerHashTable::hash_function_(elements[i]);
        if (hash_table_[hash].has_value() && hash_table_[hash] != elements[i]) {
            have_collisions = true;
        } else {
            hash_table_[hash] = std::make_optional(elements[i]);
        }
    }
    return have_collisions;
}

void FixedSet::InnerHashTable::Initialize(const std::vector<int>& elements)
{
    if (elements.empty()) {
        return;
    }
    auto hash_table_size = elements.size() * elements.size();
    hash_table_.resize(hash_table_size);
    do {
        InnerHashTable::hash_function_ =
                GenerateRandomLinearHashFunction(kPrimeBase, hash_table_size);
    } while (HaveCollisionAndFill(elements));
}

bool FixedSet::InnerHashTable::Contains(int value) const
{
    return !hash_table_.empty() && hash_table_[hash_function_(value)] == value;
}
