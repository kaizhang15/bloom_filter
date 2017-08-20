/*
 * Author: Tony Allen
 *
 */

#include <iostream>

#include "../counting_filter.h"

using namespace std;

int main() {
  // For a set cardinality of 6000, filter size of 1000, and 4 hashes, we know
  // what the expected error should be.
  const int set_cardinality = 1000;
  const double expected_error = 0.0561;

  CountingFilter<int, 6000, 2> filter;

  int expected_upper_bound = 0;

  for (int iteration = 0; iteration < 20; ++iteration) {
    // Fill the filter increasing the counters for each element.
    for (int xx = 0; xx < set_cardinality; ++xx) {
      filter.Add(&xx);
    }
    ++expected_upper_bound;

    // Verify what we just added shows up in the filter and has a count upper
    // bound of 1.
    for (int xx = 0; xx < set_cardinality; ++xx) {
      if (!filter.MaybeContains(&xx)) {
        cout << "Did not observe " << xx << " in the counting filter\n";
        return -1;
      }
      const int ub = filter.CountUpperBound(&xx);
      if (ub < expected_upper_bound) {
        cout << "Did not observe upper bound less than expected "
             << expected_upper_bound << " for element " << xx
             << ", observed " << ub << endl;
        return -1;
      }
    }
  }

  // Verify false positive rate for numbers we definitely didn't insert.
  double false_pos_rate = 0;
  int n = 0;
  for (int ii = set_cardinality; ii < 100 * set_cardinality; ++ii) {
    if (filter.MaybeContains(&ii)) {
      ++false_pos_rate;
    }
    ++n;
  }
  false_pos_rate = 100 * false_pos_rate / n;

  if (abs(expected_error - false_pos_rate) > .01) {
    cout << "Observed bogus false positive rate (" << false_pos_rate << ")\n";
    return -1;
  }

  cout << "Test finished\n";
  return 0;
}
