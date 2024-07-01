#include <algorithm>
#include <climits>
#include <cstdio>
#include <iostream>
#include <limits.h>
#include <stack>
#include <string.h>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

using namespace std;

class Solution {
public:
  // https://leetcode.com/problems/next-permutation/
  void nextPermutation(vector<int> &nums) {
    // 123 -> 132
    // 132 -> 213
    // 213 -> 231
    // 231 -> 312

    // Find the first number from the right that is smaller than its adjacent
    // element.
    int i = nums.size() - 2;
    while (i >= 0 && (nums[i] > nums[i + 1])) {
      i -= 1;
    }

    // If such a number exists, find number that is _just_ larger than it.
    if (i >= 0) {
      int j = nums.size() - 1;
      while (j > 0 && nums[i] >= nums[j]) {
        j -= 1;
      }
      swap(nums[i], nums[j]);
    }

    reverse(nums.begin() + i + 1, nums.end());
  }
};

int main(int argc, char **argv) {
  Solution sol;
  // auto test = vector<int>{1, 2};
  // auto test = vector<int>{1, 2, 3};
  auto test = vector<int>{1, 5, 1};
  sol.nextPermutation(test);
  for (auto x : test) {
    cout << x << ", ";
  }
  cout << endl;
}
