/*
 * Author: Tony Allen
 *
 */

#pragma once

#include <array>

template <typename T, int64_t kSize, int32_t kNumHashPairs>
class CountingFilter {
 public:
  CountingFilter();
  ~CountingFilter() = default;

  // Adds an element to the counting filter.
  void Add(const T *key, int size = sizeof(T));

  // Removes an element from the counting filter.
  void Remove(const T *key, int size = sizeof(T));

  // Test whether the element has been added. If false, the element is
  // definitely not in the set. If true, the element could be in the set or
  // it is a false positive as described above.
  bool MaybeContains(const T *key, int size = sizeof(T)) const;

  // Get the upper bound on the number of times an element could have been
  // inserted into the counting filter.
  int CountUpperBound(const T *key, int size = sizeof(T)) const;

  // Reset the filter to initial state.
  void Reset() {
    counters_.fill(0);
    num_insertions_ = 0;
  }

  // Accessors.
  int64_t num_insertions() const { return num_insertions_; }

 private:
  // Populate 'idx1' and 'idx2' with indexes into the 'counters_' array
  // calculated from the 128-bit hash.
  void IdxFromKey(const T *key,
                  int size,
                  uint32_t seed,
                  int64_t *idx1,
                  int64_t *idx2) const;

 private:
  // The number of elements this structure has inserted. This is decremented
  // upon removal of any element.
  int64_t num_insertions_;

  // Internal bit sets for the filter.
  std::array<uint8_t, kSize * kNumHashPairs * 2> counters_;
};
