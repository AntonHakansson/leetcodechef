#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define MIN(a, b)  ((a) < (b) ? (a) : b);
#define COUNTOF(A) (sizeof(A)/sizeof(A[0]))

int jump(int* nums, int numsSize)
{
  int dp[10000 + 1];
  for (int i = 0; i < numsSize; i += 1) {
    dp[i] = INT_MAX;
  }
  dp[0] = 0; // base case

  for (int i = 0; i < numsSize; i += 1) {
    for (int j = 1; j <= nums[i]; j += 1) {
      if (i + j < numsSize) {
        dp[i + j] = MIN(dp[i + j], 1 + dp[i]);
      }
    }
  }

  return dp[numsSize - 1];
}


int main(int argc, char **argv)
{
  int test1[] = {2, 3, 1, 1, 4};
  int r = jump(test1, COUNTOF(test1));
  return 0;
}



int integerBreak(int n)
{
  int result = 0;

  for (int k = 2; k <= n; k++) {
    int q = n / k;
    int r = n % k;

    int product = 1;
    for (int i = 0; i < k - r; i++) {
      product *= q;
    }
    for (int i = 0; i < r; i++) {
      product *= (q + 1);
    }
    result = product > result ? product : result;
  }

  return result;
}

/**
 * Return an array of arrays of size *returnSize.
 * The sizes of the arrays are returned as *returnColumnSizes array.
 * Note: Both returned array and *columnSizes array must be malloced, assume caller calls free().
 */
int** combinationSum(int* candidates, int candidatesSize, int target, int* returnSize, int** returnColumnSizes) {
  int **result = malloc(1024 * 1024 * 1024);
  *returnSize = 4;

  return result;
}
