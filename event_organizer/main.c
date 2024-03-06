// This is free and unencumbered software released into the public domain.
// anton@hakanssn.com

#include <stdint.h>
#include <stddef.h>
typedef unsigned char Byte;
typedef uint8_t   U8;
typedef int32_t   I32;
typedef int64_t   I64;
typedef uint64_t  U64;
typedef ptrdiff_t Size;
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

typedef struct Event Event;
struct Event {
  Size start;
  Size end;
  U64 compensation;
};

typedef struct Test Test;
struct Test {
  Event *items;
  Size count;
};

int sort_event_end(const void *a, const void *b)
{
  const Event *A = (const Event *)a;
  const Event *B = (const Event *)b;
  return (A->end - B->end);
}

#define max(a, b) ((a) > (b)) ? (a) : (b)
void solve(Arena arena, Test test)
{
  qsort(test.items, test.count, sizeof(test.items[0]), &sort_event_end);

  U64 *dp = new (&arena, U64, test.count + 1);
  dp += 1; // dp[-1] = 0

  for (Size i = 0; i < test.count; i++) {
    Size latest_non_overlapping_event = -1; {
      Size k = i;
      while (--k >= 0) {
        if (test.items[k].end <= test.items[i].start) {
          latest_non_overlapping_event = k;
          break;
        }
      }
    }
    U64 pick      = dp[latest_non_overlapping_event] + test.items[i].compensation;
    U64 otherwise = dp[i - 1];
    dp[i] = max(pick, otherwise);
  }
  printf("%ld\n", dp[test.count - 1]);
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
    test.items = new(&temp, typeof(*test.items), test.count);
    for (Size i = 0; i < test.count; i++) {
      scanf("%ld ", &test.items[i].start);
      scanf("%ld ", &test.items[i].end);
      scanf("%ld\n", &test.items[i].compensation);
    }
    solve(temp, test);
  }

  free(arena->backing);
  return 0;
}


/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
