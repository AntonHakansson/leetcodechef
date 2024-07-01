#include <algorithm>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

using namespace std;
class Solution {

  const int OP_SWAP = -69;
  struct rstate {
    int d, l, r;
  };

  size_t factorial(size_t n) {
    size_t result = 1;
    while (n > 1) {
      result *= n;
      n -= 1;
    }
    return result;
  }

public:
  // https://leetcode.com/problems/permutations/description/
  vector<vector<int>> permute(vector<int> &nums) {
    vector<vector<int>> result;
    vector<rstate> rstack;

    int N = nums.size();

    result.reserve(factorial(N));
    rstack.reserve(factorial(N));

    rstack.push_back({0, 0, 0});

    while (!rstack.empty()) {
      rstate &s = rstack.back();

      swap(nums[s.l], nums[s.r]);
      if (s.d == OP_SWAP) {
        continue;
      }
      rstack.push_back({OP_SWAP, s.r, s.l}); // queue swap revert.

      if (s.d >= N - 1) {
        result.push_back(nums);
      } else {
        for (int j = N - 1; j >= s.d; j--) {
          rstack.push_back({s.d + 1, s.d, j});
        }
      }

      rstack.pop_back();
    }

    return result;
  }

  // https://leetcode.com/problems/permutations-ii/description/
  vector<vector<int>> permuteUnique(vector<int> &nums) {
    int N = nums.size();

    vector<vector<int>> result;
    vector<rstate> rstack;
    char count[22] = {0};

    result.reserve(factorial(N));
    rstack.reserve(factorial(N));

    rstack.push_back({0, 0, 0});

    while (!rstack.empty()) {
      rstate s = rstack.back();
      rstack.pop_back();

      swap(nums[s.l], nums[s.r]);
      if (s.d == OP_SWAP) {
        continue;
      }
      rstack.push_back({OP_SWAP, s.r, s.l}); // queue swap revert.

      if (s.d >= N - 1) {
        result.push_back(nums);
      } else {
        memset(count, 0, 22);
        for (int j = N - 1; j >= s.d; j--) {
          auto candidate = nums[j] + 10;
          if (count[candidate] == 0) {
            count[candidate] += 1;
            rstack.push_back({s.d + 1, s.d, j});
          }
        }
      }
    }

    return result;
  }
};

int main(int argc, char **argv) {
  Solution sol;
  auto test = vector<int>{1, 1, 2};
  sol.permuteUnique(test);
}
