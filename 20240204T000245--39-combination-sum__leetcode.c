#include <stdio.h>
#include <string.h>
#include <stdint.h>

/** https://leetcode.com/problems/combination-sum/description/
 *
 * Return an array of arrays of size *returnSize.
 * The sizes of the arrays are returned as *returnColumnSizes array.
 * Note: Both returned array and *columnSizes array must be malloced, assume caller calls free().
 */
int** combinationSum(int* candidates, int candidatesSize, int target, int* returnSize, int** returnColumnSizes)
{
  int **r = malloc(32 * 1024 * 1024);
  int r_count = 0;
  *returnColumnSizes = malloc(150 * sizeof(int));
  *returnSize = 0;

  // assume candidates are sorted (not stated in problem)

  const uint32_t RA_POP = 1 << 0;

  typedef struct Stack_Frame { uint32_t flags; int idx; int sum; } Stack_Frame;
  struct { Stack_Frame items[1 << 14]; int count; } stack = { 0, .count = 1 };
  struct { int    *items;          int count; } writer = { .items = r + 150, 0};
  while (stack_count > 0) {
    Stack_Frame *frame = &stack.items[stack.count - 1];

    if (frame->flags & RA_POP) { goto ra_pop; }

    if (frame->sum == target) {
      returnColumnSizes[r_count] = writer.count;
      r[r_count] = writer.items;
      r_count += 1;

      memcpy(writer.items + writer.count, writer.items, writer.count);
      writer.items = writer.items + writer.count;
    }
    else {
      writer.items[writer.count++] = candidates[frame->idx];

      for (int i = frame->idx; i < candidatesSize; i++) {
        int new_sum = frame->sum + candidates[i];
        stack[stack_count++] = (Stack_Frame){ 0, .idx = i, .sum = new_sum };
      }

      frame->flags |= RA_POP:
      goto keep_frame;
    ra_pop:
      writer.items[--writer.count] = candidates[frame->idx];
    }

    stack.count -= 1;
  keep_frame: ;
  }

  *returnSize = r_count;
  return r;
}

#define array_count(arr) (sizeof(arr) / sizeof(arr[0]))
int main(int argc, char **argv)
{
  {
    int candidates[] = { 2, 3, 5, 6 };
    int target = 7;
    int r_count = 0;
    int *r_column_counts = 0;
    int **r = combinationSum(candidates, target, array_count(candidates), &r_count, &r_column_counts);
    free(r);
    free(r_column_counts);
  }
  return 0;
}



int foo()
{
  int result = 0;

  typedef struct { uint32_t flags; int i; } Frame_Stack;
  struct { Frame_Stack items[1 << 14]; int count; } stack = {0};
  stack.items[stack.count++] = (Frame_Stack){ .i = 0 };
  while (stack.count > 0) {
    Frame_Stack *frame = &stack.items[stack.count - 1];
    /* if (frame->flags & ) {  } */
    if (frame->i) {
      stack.items[stack.count++] = (Frame_Stack){};
    }
    stack.count--;
  }

  return result;
}
