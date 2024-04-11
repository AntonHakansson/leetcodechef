/*
#+title: Codechef: IPL: Zonal Computing Olympiad 2014, 30 Nov 2013
#+ref: https://www.codechef.com/practice/course/dynamic-programming/INTDP01/problems/ZCO14004
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public domain.

In IPL 2025, the amount that each player is paid varies from match to match. The match fee
depends on the quality of opposition, the venue etc.

The match fees for each match in the new season have been announced in advance. Each team
has to enforce a mandatory rotation policy so that no player ever plays three matches in a
row during the season.

Nikhil is the captain and chooses the team for each match. He wants to allocate a playing
schedule for himself to maximize his earnings through match fees during the season.

* Input format

- Line 1: A single integer /N/, the number of games in the IPL season.
- Line 2: /N/ non-negative integers, where the integer in position /i/ represents the fee
  for match /i/.

* Output format

The output consists of a single non-negative integer, the maximum amount of money that
Nikhil can earn during this IPL season.

* Sample 1:

#+begin_example
5
10 3 5 7 3
#+end_example

#+begin_example
23
#+end_example

Explanation: 10+3+7+3

* Sample 2:

#+begin_example
8
3 2 3 2 3 5 1 3
#+end_example

#+begin_example
17
#+end_example

Explanation: 3+3+3+5+3

 */

#define HEAP_CAP     (1ll << 30)

#include <stdint.h>
#include <stddef.h>
typedef uint8_t   U8;
typedef int64_t   I64;
typedef uint64_t  U64;
typedef ptrdiff_t Size;

#define size_of(s)   (Size)sizeof(s)
#define count_of(s)  (size_of((s)) / size_of(*(s)))
#define assert(c)    while((!(c))) __builtin_unreachable()
#define new(a, t, n) (t *) arena_alloc(a, size_of(t), (Size)_Alignof(t), (n))

typedef struct { U8 *beg, *end; } Arena;
typedef struct { Size cap; U8 *buf; U8 *at; } Stream;

__attribute((malloc, alloc_size(2,4), alloc_align(3)))
static U8 *arena_alloc(Arena *a, Size objsize, Size align, Size count) {
  Size padding = -(uintptr_t)(a->beg) & (align - 1);
  Size total   = padding + objsize * count;
  assert(total < (a->end - a->beg) && "out of memory");
  U8 *p = a->beg + padding;
  __builtin_memset(p, 0, objsize * count);
  a->beg += total;
  return p;
}
static I64 expect_i64(Stream *io) {
  U64 u = 0, s = 0;
  while(*io->at && *io->at <= 32) { ++io->at; }
  if (*io->at == '-') { s = ~s; ++io->at; } else if (*io->at == '+') { ++io->at; }
  while ((*io->at >= '0') && (*io->at <= '9')) { u = u*10 + (*(io->at++) - '0'); }
  return (u^s) + !!s;
}
static void append_i64(Stream *io, I64 x, char separator) {
  char tmpbuf[20];
  if (x < 0) { *io->at++ = '-'; x = -x; }
  int i = count_of(tmpbuf);
  do { tmpbuf[--i] = (x % 10) + '0'; } while (x /= 10);
  do { *io->at++ = tmpbuf[i++]; } while (i < count_of(tmpbuf));
  *(io->at++) = separator;
}

#define max(a, b) ((a) > (b) ? (a) : (b))
#define max3(a, b, c) (max((a), max((b), (c))))
static void run(Arena arena, Stream * restrict reader, Stream * restrict writer) {
  Size N = expect_i64(reader);
  I64 *a = new (&arena, typeof(*a), N);
  for (Size i = 0; i < N; i++) { a[i] = expect_i64(reader); }

  U64 *dp0 = new(&arena, U64, N + 4) + 3; // dp0[-3..N-1] safe
  U64 *dp1 = new(&arena, U64, N + 4) + 3; // dp1[-3..N-1] safe
  for (Size i = 0; i < N; i++) {
    dp1[i] = a[i] + dp0[i - 1];
    dp0[i] = a[i] + max3(dp0[i - 2], dp1[i - 2], dp1[i - 3]);
  }
  Size i = N - 1;
  U64 result = max(dp0[i - 0], dp1[i - 0]);
  result = max3(result, dp0[i - 1], dp1[i - 1]);
  result = max3(result, dp0[i - 2], dp1[i - 2]);
  append_i64(writer, result, '\n');
}

// Platform
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char **argv) {
  void *heap = malloc(HEAP_CAP);
  Arena arena = (Arena){heap, heap + HEAP_CAP};
  Stream reader = {0}; {
    reader.buf = reader.at = arena.beg;
    reader.cap = fread(reader.buf, 1, (1ll << 23) - 1, stdin);
    reader.buf[reader.cap++] = 0;
    arena.beg += reader.cap;
  }
  Stream writer = { .cap = 1ll << 13 }; {
    writer.buf = writer.at = new (&arena, U8, writer.cap);
  }
  run(arena, &reader, &writer);
  fwrite(writer.buf, 1, (writer.at - writer.buf), stdout);
  free(heap);
  return 0;
}

/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
