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

typedef struct TestCase TestCase;
struct TestCase {
  I64 *items;
  Size count;
};

U32 dp[100001] = {0};

int main(int argc, char **argv)
{
  Arena arena[1] = {0};
  arena->capacity = 1l << 30;
  arena->backing = malloc(arena->capacity);

  Size T;
  scanf("%ld\n", &T);
  for (Size t = 0; t < T; t++) {
    Arena temp = *arena;
    TestCase test = {0};
    scanf("%ld\n", &test.count);
    test.items = new(&temp, typeof(test.items[0]), test.count);
    for (int i = 0; i < test.count; i++) {
      scanf("%ld ", &test.items[i]);
    }

    { // solve
      memset(dp, 0, test.count * size_of(dp[0]));
      dp[test.count - 1] = 1;

      for (Size i = test.count - 2; i >= 0; i--) {
        B32 is_alternating = ((test.items[i + 1] >= 0) ^ (test.items[i] >= 0));
        if (is_alternating) {
          dp[i] = dp[i + 1] + 1;
        } else {
          dp[i] = 1;
        }
      }

      for (int x = 0; x < test.count; x++) {
        printf("%d ", dp[x]);
      }
      puts("");
    }
  }

  free(arena->backing);
  return 0;
}

