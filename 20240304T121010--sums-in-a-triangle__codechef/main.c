
// This is free and unencumbered software released into the public domain.
// anton@hakanssn.com

#include <stdint.h>
#include <stddef.h>
typedef unsigned char Byte;
typedef uint8_t   U8;
typedef int32_t   I32;
typedef int64_t   I64;
typedef uint32_t  U32;
typedef uint64_t  U64;
typedef ptrdiff_t Size;
typedef size_t    USize;
typedef I32    B32;

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define size_of(s)  (Size)sizeof(s)
#define align_of(s) (Size)_Alignof(s)
#define count_of(s) (size_of((s)) / size_of(*(s)))

#ifndef assert
# define assert(c)  while((!(c))) __builtin_unreachable()
#endif


////////////////////////////////////////////////////////////////////////////////
//- Arena Allocator

#define new(a, t, n) (t *) arena_alloc(a, size_of(t), align_of(t), (n))

typedef struct Arena Arena;
struct Arena {
  Size capacity;
  Size at;
  Byte *backing;
};

__attribute((malloc, alloc_size(2,4), alloc_align(3)))
static Byte *arena_alloc(Arena *a, Size objsize, Size align, Size count)
{
  Size avail = a->capacity - a->at;
  Size padding = -(uintptr_t)(a->backing + a->at) & (align - 1);
  Size total   = padding + objsize * count;
  assert(total < avail);
  Byte *p = (a->backing + a->at) + padding;
  memset(p, 0, objsize * count);
  a->at += total;
  return p;
}


////////////////////////////////////////////////////////////////////////////////
//- Program

#include <unistd.h>

typedef struct Test Test;
struct Test {
  I64 *items;
  Size count;
};

void solve(Arena arena, Test test)
{
  Size result = 0;

  I64 *dp = new(&arena, I64, (test.count + 2) * test.count);
  dp += 2 * test.count; // safe mem. access previous row

  for (Size row = 0; row < test.count; row++) {
    for (Size col = 0; col < row + 1; col++) {
      I64 v = test.items[(row * test.count) + col];
      I64 a = dp[(row - 1) * test.count + col - 0] + v;
      I64 b = dp[(row - 1) * test.count + col - 1] + v;
      dp[(row * test.count) + col] = a > b ? a : b;
    }
  }

  for (Size col = 0; col < test.count; col++) {
    I64 d = dp[(test.count - 1) * test.count +  col];
    result = result > d ? result : d;
  }

  printf("%ld\n",  result);
}

void solve_top_down(Arena arena, Test test)
{
  I64 *dp = new (&arena, typeof(*dp), 1 + test.count * test.count);
  for (Size col = 0; col < test.count; col++) {
    Size last_row = (test.count - 1);
    dp[last_row * test.count + col] = test.items[last_row * test.count + col];
  }
  for (Size row = test.count - 2; row >= 0; row--) {
    for (Size col = 0; col < row + 1; col++) {
      I64 v = test.items[row * test.count + col];
      I64 a = dp[(row + 1) * test.count + col + 0] + v;
      I64 b = dp[(row + 1) * test.count + col + 1] + v;
      dp[row * test.count + col] = a > b ? a : b;
    }
  }
  printf("%ld\n",  dp[0]);
}


int main(int argc, char **argv)
{
  Arena arena[1] = {0};
  arena->capacity = 1 << 30;
  arena->backing = malloc(arena->capacity);

  Size T = 0;
  scanf("%ld\n", &T);

  for (Size t = 0; t < T; t++) {
    Arena temp = *arena;
    Test test = {0};
    scanf("%ld\n", &test.count);
    test.items = new(&temp, I64, test.count * test.count);
    for (Size row = 0; row < test.count; row++) {
      for (Size col = 0; col < row + 1; col++) {
        scanf("%ld ", &test.items[(row * test.count) + col]);
      }
    }
    solve_top_down(temp, test);
  }

  free(arena->backing);
  return 0;
}


/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
