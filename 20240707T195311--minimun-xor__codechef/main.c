/*
#+title: Minimum XOR
#+ref: https://www.codechef.com/practice/course/1-star-difficulty-problems/DIFF1200/problems/MINMXOR
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public domain.
 */

#include <stdint.h>
#include <stddef.h>
typedef uint8_t U8; typedef int64_t I64; typedef uint64_t  U64; typedef ptrdiff_t Size;

#define size_of(s)   (Size)sizeof(s)
#define count_of(s)  (size_of((s)) / size_of(*(s)))
#define assert(c)    while((!(c))) __builtin_unreachable()
#define new(a, t, n) ((t *) arena_alloc(a, size_of(t), (Size)_Alignof(t), (n)))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct { U8 *beg, *end; } Arena;
typedef struct { Size cap; U8 *buf; U8 *at; } Write_Buffer;

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
static I64 expect_i64(Write_Buffer *io) {
  U64 u = 0, s = 0;
  while(*io->at && *io->at <= 32) { ++io->at; }
  if (*io->at == '-') { s = ~s; ++io->at; } else if (*io->at == '+') { ++io->at; }
  while ((*io->at >= '0') && (*io->at <= '9')) { u = u*10 + (*(io->at++) - '0'); }
  return (u^s) + !!s;
}
static void append_i64(Write_Buffer *io, I64 x, char separator) {
  char tmpbuf[20];
  if (x < 0) { *io->at++ = '-'; x = -x; }
  int i = count_of(tmpbuf);
  do { tmpbuf[--i] = (x % 10) + '0'; } while (x /= 10);
  do { *io->at++ = tmpbuf[i++]; } while (i < count_of(tmpbuf));
  *(io->at++) = separator;
}

static void run(Arena arena, Write_Buffer *reader, Write_Buffer *writer) {
  Size T = expect_i64(reader);
  for (Size t = 0; t < T; t++) {
    Arena temp = arena;
    Size N = expect_i64(reader);
    U64 *a = new(&temp, U64, N);
    U64 totx = a[0] = expect_i64(reader);
    for (Size n = 1; n < N; n++) {
      a[n] = expect_i64(reader);
      totx ^= a[n];
    }
    U64 r = totx;
    for (Size n = 0; n < N; n++) {
      U64 drop_one = totx ^ a[n];
      r = min(r, drop_one);
    }
    append_i64(writer, min(totx, r), '\n');
  }
}

// Platform
#define HEAP_CAP     (1ll << 30)

#include <stdlib.h>
#include <stdio.h>
#ifndef ONLINE_JUDGE
#  include "test.c"
#else
int main(int argc, char **argv) {
  void *heap = malloc(HEAP_CAP);
  Arena arena = (Arena){heap, heap + HEAP_CAP};
  Write_Buffer reader = {0}; {
    reader.buf = reader.at = arena.beg;
    reader.cap = fread(reader.buf, 1, (1ll << 24), stdin);
    arena.beg += reader.cap + 32;
  }
  Write_Buffer writer = { .cap = 1ll << 13 }; {
    writer.buf = writer.at = new (&arena, U8, writer.cap);
  }
  run(arena, &reader, &writer);
  fwrite(writer.buf, 1, (writer.at - writer.buf), stdout);
  free(heap);
  return 0;
}
#endif // ONLINE_JUDGE

/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined,address -O3" */
/* End: */
