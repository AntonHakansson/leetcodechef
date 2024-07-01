#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define MIN(a, b)  ((a) < (b) ? (a) : b);
#define COUNTOF(A) (sizeof(A)/sizeof(A[0]))

int findMin(int* nums, int numsSize)
{
  int best = INT_MAX;

  int l = 0;
  int r = numsSize - 1;
  while (l <= r) {
    int m = (l + r) / 2;
    best = MIN(best, nums[m]);
    if (nums[l] < nums[m] || nums[r] < nums[m]) {
      if (nums[l] < nums[r]) {
        r = m - 1;
      }
      else {
        l = m + 1;
      }
    }
    else {
      r = m - 1;
    }
  }

  return best;
}

int main(int argc, char **argv)
{
  {
    int test1[] = {3,4,5,1,2};
    int r = findMin(test1, COUNTOF(test1));
    printf("%d\n", r);
  }
  {
    int test1[] = {4,5,6,7,0,1,2};
    int r = findMin(test1, COUNTOF(test1));
    printf("%d\n", r);
  }
  {
    int test1[] = {5,1,2,3,4};
    int r = findMin(test1, COUNTOF(test1));
    printf("%d\n", r);
  }
  return 0;
}
