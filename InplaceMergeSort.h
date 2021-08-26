#pragma once

#include <algorithm>
#include <functional>

template<class InputItFirst, class InputItSecond, class OutputIt, class Compare = std::less<>>
OutputIt InplaceMerge(InputItFirst first_begin, InputItFirst first_end,
                      InputItSecond second_begin, InputItSecond second_end,
                      OutputIt destination_begin, Compare compare = Compare()) {
    while (first_begin != first_end || second_begin != second_end) {
        if (second_begin == second_end ||
            (first_begin != first_end && compare(*first_begin, *second_begin))) {
            std::iter_swap(destination_begin++, first_begin++);
        } else {
            std::iter_swap(destination_begin++, second_begin++);
        }
    }
    return destination_begin;
}

template<class RandomIt>
RandomIt GetMiddleIterator(RandomIt first, RandomIt last) {
    return first + std::distance(first, last) / 2;
}

template<class RandomIt, typename Compare = std::less<>>
void InplaceMergeSort(RandomIt left, RandomIt right, Compare compare = Compare()) {
    int size = std::distance(left, right);
    if (size <= 1) {
        return;
    }

    auto middle = GetMiddleIterator(left, right);
    InplaceMergeSort(middle, right, compare);
    middle = GetMiddleIterator(left, right);
    while (middle != left) {
        int left_part_size = std::distance(left, middle);
        auto middle_left_part = GetMiddleIterator(left, middle);
        int left_sorted_part_size = std::distance(left, middle_left_part);

        InplaceMergeSort(left, middle_left_part, compare);
        left = middle_left_part - left_sorted_part_size;

        auto merged = InplaceMerge(left, middle_left_part,
                                   middle, right, middle_left_part, compare);
        left = merged + left_part_size % 2 - size;
        middle_left_part = left + left_sorted_part_size;
        if (left_part_size % 2 == 1) {
            for (auto iter = right - 2;
                 iter != middle_left_part && !compare(*iter, *(iter + 1));
                 --iter) {
                std::iter_swap(iter, iter + 1);
            }
            if (!compare(*middle_left_part, *(middle_left_part + 1))) {
                std::iter_swap(middle_left_part, middle_left_part + 1);
            }
        }

        middle = middle_left_part;
    }
}
