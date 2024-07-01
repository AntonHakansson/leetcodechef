#include <algorithm>
#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

using namespace std;

class Solution {
public:
  vector<vector<int>> fourSum(vector<int> &nums, int target) {
    vector<vector<int>> result;

    sort(nums.begin(), nums.end());

    if (nums.size() < 4)
      return result;

    for (int i = 0; i < nums.size() - 3; i++) {
      if (i > 0 && nums[i] == nums[i - 1])
        continue;

      for (int j = i + 1; j < nums.size() - 2; j++) {
        if (j > (i + 1) && nums[j] == nums[j - 1])
          continue;

        auto l = j + 1;
        auto r = (int)nums.size() - 1;

        while (l < r) {
          auto sum = (long long)nums[i] + nums[j] + nums[l] + nums[r] - target;
          if (sum < 0) {
            l += 1;
          } else if (sum > 0) {
            r -= 1;
          } else /* (sum == 0) */ {
            // printf("Found sol(%d %d %d %d) %d %d %d %d\n", i, j, l, r,
            // nums[i], nums[j], nums[l], nums[r]);
            result.push_back({nums[i], nums[j], nums[l], nums[r]});
            do {
              l += 1;
            } while (l < r && nums[l] == nums[l - 1]);
            do {
              r -= 1;
            } while (l < r && nums[r] == nums[r + 1]);
          }
        }
      }
    }

    return result;
  }

  // https://leetcode.com/problems/4sum-ii/
  int fourSumCount(vector<int> &nums1, vector<int> &nums2, vector<int> &nums3,
                   vector<int> &nums4) {
    int N = nums1.size();
    int result = 0;
    unordered_map<int, int> map;

    for (int i = 0; i < N; i += 1) {
      for (int j = 0; j < N; j += 1) {
        map[nums1[i] + nums2[j]] += 1;
      }
    }

    for (int i = 0; i < N; i += 1) {
      for (int j = 0; j < N; j += 1) {
        result += map[-(nums3[i] + nums4[j])];
      }
    }

    return result;
  }
};

int main(int argc, char **argv) {
  Solution sol;
  vector<int> test{1, 0, -1, 0, -2, 2};
  vector<int> test2{2, 2, 2, 2, 2};
  vector<int> testf{-2, -1, -1, 1, 1, 2, 2};
  auto res = sol.fourSum(testf, 0);

  return 0;
}
