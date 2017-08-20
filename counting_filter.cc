/*
 * Author: Tony Allen
 *
 */

#include <algorithm>
#include <array>
#include <cassert>
#include <limits>

#include "counting_filter.h"
#include "smhasher/src/MurmurHash3.h"

using namespace std;

// ----------------------------------------------------------------------------

template <typename T, int64_t kSize, int32_t kNumHashPairs>
CountingFilter<T, kSize, kNumHashPairs>::CountingFilter() :
  num_insertions_(0) {

  counters_.fill(0);
}

// ----------------------------------------------------------------------------

template <typename T, int64_t kSize, int32_t kNumHashPairs>
void CountingFilter<T, kSize, kNumHashPairs>::Add(
  const T *key, const int size) {

  for (int32_t xx = 0; xx < kNumHashPairs; ++xx) {
    int64_t idx1, idx2;
    IdxFromKey(key, size, xx, &idx1, &idx2);
    // It's possible that the count can exceed the maximum uint8 value, so
    // we'll just leave it be. After many removals, this could result in a
    // false negative, but this is very unlikely. Let's just assert for this
    // case.
    assert(counters_[idx1] <= numeric_limits<uint8_t>::max());
    assert(counters_[idx2] <= numeric_limits<uint8_t>::max());
    counters_[idx1] += 1;
    counters_[idx2] += 1;
  }
  ++num_insertions_;
}

// ----------------------------------------------------------------------------

template <typename T, int64_t kSize, int32_t kNumHashPairs>
bool CountingFilter<T, kSize, kNumHashPairs>::MaybeContains(
  const T *key, const int size) const {

  for (int32_t xx = 0; xx < kNumHashPairs; ++xx) {
    int64_t idx1, idx2;
    IdxFromKey(key, size, xx, &idx1, &idx2);
    if (counters_[idx1] == 0 || counters_[idx2] == 0) {
      return false;
    }
  }
  return true;
}

// ----------------------------------------------------------------------------

template <typename T, int64_t kSize, int32_t kNumHashPairs>
int CountingFilter<T, kSize, kNumHashPairs>::CountUpperBound(
  const T *key, const int size) const {

  int count_ub = numeric_limits<uint8_t>::max() + 1;

  for (int32_t xx = 0; xx < kNumHashPairs; ++xx) {
    int64_t idx1, idx2;
    IdxFromKey(key, size, xx, &idx1, &idx2);
    count_ub = min(static_cast<int>(counters_[idx1]), count_ub);
    count_ub = min(static_cast<int>(counters_[idx2]), count_ub);
  }
  return count_ub;
}

// ----------------------------------------------------------------------------

template <typename T, int64_t kSize, int32_t kNumHashPairs>
void CountingFilter<T, kSize, kNumHashPairs>::Remove(
  const T *key, const int size) {

  for (int32_t xx = 0; xx < kNumHashPairs; ++xx) {
    int64_t idx1, idx2;
    IdxFromKey(key, size, xx, &idx1, &idx2);
    assert(counters_[idx1] > 0);
    assert(counters_[idx2] > 0);
    counters_[idx1] -= 1;
    counters_[idx2] -= 1;
  }
  --num_insertions_;
}

// ----------------------------------------------------------------------------

template <typename T, int64_t kSize, int32_t kNumHashPairs>
void CountingFilter<T, kSize, kNumHashPairs>::IdxFromKey(
  const T *key,
  const int size,
  const uint32_t seed,
  int64_t *idx1,
  int64_t *idx2) const {

  array<uint64_t, 2> results;
  MurmurHash3_x64_128(key, size, seed, results.data());
  *idx1 = results[0] % counters_.size();
  *idx2 = results[1] % counters_.size();
  assert(*idx1 < counters_.size());
  assert(*idx2 < counters_.size());
}

// ----------------------------------------------------------------------------

// Explicit instantiation.
// TODO: remove this since it's only needed for the test.
template class CountingFilter<int, 6000, 2>;
