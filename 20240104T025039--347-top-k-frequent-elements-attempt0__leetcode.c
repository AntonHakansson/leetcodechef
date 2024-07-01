#include <stdint.h>
#include <stddef.h>
typedef unsigned char Byte;
typedef uint8_t   U8;
typedef int32_t   I32;
typedef uint32_t  U32;
typedef ptrdiff_t Size;

#include <stdlib.h>
#include <string.h>
#include <stdio.h>  // stderr

#define size_of(s) (Size)sizeof(s)
#define align_of(s) (Size)_Alignof(s)
#ifndef assert
# define assert(c)  while((!(c))) __builtin_unreachable()
#endif

#define count_of(s) (size_of(s) / size_of(*(s)))
#define swap(Type, a, b) do { Type tmp = (a); (a) = (b); (b) = tmp; } while (0)



////////////////////////////////////////////////////////////////////////////////
//- Arena Allocator

#define new(a, t, n) (t *) arena_alloc(a, size_of(t), align_of(t), (n))

typedef struct Arena {
  Size capacity;
  Size at;
  Byte *backing;
} Arena;

static Arena *arena_make(Size capacity)
{
  Arena *a = malloc(capacity);
  Size backing_offset = size_of(*a);
  assert(capacity > backing_offset);
  a->backing = (Byte*)a + backing_offset;
  a->capacity = capacity - backing_offset;
  a->at = 0;
  return a;
}

__attribute((malloc, alloc_size(2,4), alloc_align(3)))
static Byte *arena_alloc(Arena *a, Size objsize, Size align, Size count)
{
  Size avail = a->capacity - a->at;
  Size padding = -(uintptr_t)(a->backing + a->at) & (align - 1);
  Size total   = padding + objsize * count;
  assert(total < avail);

  Byte *p = (a->backing + a->at) + padding;
  a->at += total;

  memset(p, 0, objsize * count);

  return p;
}

////////////////////////////////////////////////////////////////////////////////
//- Program

typedef struct {
  I32 value; // key
  I32 freq;  // value
} Item;

typedef struct Map {
  struct Map* child[4];
  Item item;
} Map;

U32 hash_u32(U32 x)
{
  x ^= x >> 16;
  x *= 0x7feb352dU;
  x ^= x >> 15;
  x *= 0x846ca68bU;
  x ^= x >> 16;
  return x;
}

I32 *upsert(Arena *arena, Map **map, I32 value)
{
  for (U32 hash = hash_u32((U32)value); *map; hash = hash << 2) {
    if ((*map)->item.value == value) { return &(*map)->item.freq; }
    map = &(*map)->child[hash >> 30];
  }
  *map = new(arena, Map, 1);
  (*map)->item.value = value;
  return &(*map)->item.freq;
}

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* topKFrequent(int* nums, int nums_count, int k, int* result_count)
{
  Arena *arena = arena_make(k * 1024 * 1024);
  new(arena, I32, k); // padding for result

  Map *m = 0;
  for (Size i = 0; i < nums_count; i++) {
    I32 *freq = upsert(arena, &m, nums[i]);
    *freq = *freq + 1;
  }

  Item *heap = new(arena, Item, nums_count);
  Size heap_count = 0;

  Map **dfs_stack = new(arena, Map*, nums_count);
  Size dfs_stack_count = 0;

  dfs_stack[dfs_stack_count++] = m;
  while (dfs_stack_count > 0) {
    Map *cur = dfs_stack[--dfs_stack_count];
    for (Size i = 0; i < 4; i++) {
      if (cur->child[i]) {
        dfs_stack[dfs_stack_count++] = cur->child[i];
      }
    }

    { // heap_push(heap, cur)
      I32 i = heap_count;
      heap[heap_count++] = cur->item;
      for (I32 parent = (i - 1) / 2; i > 0 && heap[i].freq > heap[parent].freq; parent = (i - 1) / 2) {
        swap(Item, heap[i], heap[parent]);
        i = parent;
      }
    }
  }

  // thrash arena with result
  *result_count = k;
  I32 *result = (I32 *)arena;

  for (I32 i = 0; i < k; i++)
  { // result[i] = heap_pop(heap)
    memcpy(result + i, &heap[0].value, sizeof(heap[i].value)); // avoid strict alias UB
    heap[0] = heap[--heap_count];

    // repair down
    int parent = 0;
    for (int child_left = parent * 2 + 1; child_left < heap_count; child_left = parent * 2 + 1) {
      int child_right = child_left + 1;
      int child = heap[child_left].freq > heap[child_right].freq ? child_left : child_right;
      if (child < heap_count && heap[child].freq > heap[parent].freq) {
        swap(Item, heap[parent], heap[child]);
        parent = child;
      }
      else {
        break;
      }
    }
  }

  return result;
}

#ifdef EXAMPLE
int main(int argc, char **argv)
{
  /*
    Example 1:
    Input: nums = [1,1,1,2,2,3], k = 2
    Output: [1,2]

    Example 2:
    Input: nums = [1], k = 1
    Output: [1]
   */
  printf("Example 1\n");
  {
    int nums[] = {1, 1, 1, 2, 2, 3};
    int k = 2;
    int result_count = 0;
    int *result = topKFrequent(nums, count_of(nums), k, &result_count);
    printf("result: ");
    for (int i = 0; i < result_count; i++) {
      if (i != 0) printf(" ");
      printf("%d", result[i]);
    }
    printf("\n");
  }
  printf("Example 2\n");
  {
    int nums[] = {1};
    int k = 1;
    int result_count = 0;
    int *result = topKFrequent(nums, count_of(nums), k, &result_count);
    printf("result: ");
    for (int i = 0; i < result_count; i++) {
      if (i != 0) printf(" ");
      printf("%d", result[i]);
    }
    printf("\n");
  }
  printf("Example 3\n");
  {
    int nums[] = {5, 3, 1, 1, 1, 3, 73, 1};
    int k = 2;
    int result_count = 0;
    int *result = topKFrequent(nums, count_of(nums), k, &result_count);
    printf("result: ");
    for (int i = 0; i < result_count; i++) {
      if (i != 0) printf(" ");
      printf("%d", result[i]);
    }
    printf("\n");
  }
  return 0;
}
#endif // EXAMPLE
