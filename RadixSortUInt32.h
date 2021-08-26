#pragma once

#include <algorithm>
#include <numeric>
#include <vector>

void RadixSort(std::vector<uint32_t> &arr) {
    size_t size = arr.size();
    std::vector<uint32_t> tmp(size);
    const uint32_t BUCKET_SIZE = 1 << 16;
    std::vector<uint32_t> ptr(BUCKET_SIZE);

    const uint32_t MASK = (1 << 16) - 1;
    for (size_t i = 0; i < size; ++i) {
        ++ptr[arr[i] & MASK];
    }
    std::partial_sum(ptr.begin(), ptr.end(), ptr.begin());
    for (int i = BUCKET_SIZE - 1; i > 0; --i) {
        ptr[i] = ptr[i - 1];
    }
    ptr[0] = 0;
    for (size_t i = 0; i < size; ++i) {
        tmp[ptr[arr[i] & MASK]++] = arr[i];
    }
    tmp.swap(arr);

    std::fill(ptr.begin(), ptr.end(), 0);

    for (size_t i = 0; i < size; ++i) {
        ++ptr[arr[i] >> 16];
    }
    std::partial_sum(ptr.begin(), ptr.end(), ptr.begin());
    for (int i = BUCKET_SIZE - 1; i > 0; --i) {
        ptr[i] = ptr[i - 1];
    }
    ptr[0] = 0;
    for (size_t i = 0; i < size; ++i) {
        tmp[ptr[arr[i] >> 16]++] = arr[i];
    }
    arr.swap(tmp);
}
