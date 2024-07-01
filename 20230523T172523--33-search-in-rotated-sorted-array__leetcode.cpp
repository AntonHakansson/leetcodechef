#include <cstdio>
#include <iostream>
#include <stack>
#include <string.h>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

using namespace std;

class Solution {
public:
  int search(vector<int> &nums, int target) {
    int l = 0;
    int r = nums.size() - 1;

    while (l <= r) {
      int m = (l + r) / 2;
      if (nums[m] == target) {
        return m;
      } else if (nums[l] <= nums[m]) {
        if ((nums[l] <= target) && (nums[m] >= target)) {
          r = m - 1;
        } else {
          l = m + 1;
        }
      } else {
        if ((nums[m] <= target) && (nums[r] >= target)) {
          l = m + 1;
        } else {
          r = m - 1;
        }
      }
    }

    return -1;
  }
};

int main(int argc, char **argv) {
  Solution sol;
  {
    vector<int> test{4, 5, 6, 7, 0, 1, 2};
    auto res2 = sol.search(test, 0);
  }
}
