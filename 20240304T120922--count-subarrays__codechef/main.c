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
  if (test.count == 0) { puts("0"); return; }

  B32 *non_inc = new(&arena, typeof(*non_inc), test.count * test.count + test.count);
  I64 *dp = new(&arena, typeof(*dp), test.count * test.count + test.count);
  for (Size i = 0; i < test.count; i++) {
    I64 n = 1;
    dp[(n * test.count) + i] = 1;
    non_inc[(n * test.count) + i] = 1;
  }

  for (Size n = 2; n <= test.count; n++) {
    for (Size i = 0; i < test.count - n + 1; i++) {
      I64 l = dp[((n - 1) * test.count) + (i + 0)];
      I64 r = dp[((n - 1) * test.count) + (i + 1)];
      I64 intersection = dp[((n - 2) * test.count) + (i + 1)];
      I64 non_increasing = (test.items[i + n - 2] <= test.items[i + n - 1]) && (test.items[i] <= test.items[i + 1]) && non_inc[(n - 1) * test.count + i + 1];
      dp[(n * test.count) + i] = l + r - intersection + non_increasing;
      non_inc[(n * test.count) + i] = non_increasing;
    }
  }

  printf("%ld\n", dp[test.count * test.count + 0]);
}

void solve2(Arena arena, Test test)
{
  if (test.count == 0) {
    puts("0");
    return;
  }

  Size result = 0;
  Size l = 0, r = 1;
  while (r < test.count + 1) {
    B32 decreasing = test.items[r - 1] > test.items[r];
    B32 end = r >= test.count;
    if (decreasing || end) {
      Size n = r - l;
      result += (n * (1 + n)) / 2;
      l = r;
    }
    r += 1;
  }

  printf("%ld\n", result);
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
    test.items = new(&temp, I64, test.count);
    for (Size i = 0; i < test.count; i++) {
      scanf("%ld ", &test.items[i]);
    }
    solve2(temp, test);
  }

  free(arena->backing);
  return 0;
}


/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
