/*
#+title: Atcoder beginner contest 126 E - 1 or 2
#+ref: https://atcoder.jp/contests/abc126/tasks/abc126_e
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public domain.
*/

typedef unsigned char      U8;
typedef unsigned long long U64;
typedef signed   long long I64;
typedef signed   long long Size;

#define size_of(s)  (Size)sizeof(s)
#define count_of(s) (size_of((s)) / size_of(*(s)))
#define assert(c)   ((c) ? 0 : ({ asm volatile ("int3; nop"); }))
#define swap(t, a, b) do { t tmp = a; a = b; b= tmp; } while(0);
#define new(a, t, n) ((t *) arena_alloc(a, size_of(t), (Size)_Alignof(t), (n)))

typedef struct { U8 *beg, *end; } Arena;
typedef struct { Size cap; U8 *buf; U8 *at; } Write_Buffer;

__attribute((malloc, alloc_size(2,4), alloc_align(3)))
static U8 *arena_alloc(Arena *a, Size objsize, Size align, Size count) {
  Size padding = -(U64)(a->beg) & (align - 1);
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

static Size union_find(I64 *parent, Size i) {
  while (i != parent[i]) {
    parent[i] = parent[parent[i]];
    i = parent[i];
  }
  return i;
}
static Size union_join(I64 * restrict parent, I64 * restrict rank, I64 a, I64 b) {
  if (rank[a] < rank[b]) { swap(I64, a, b); }
  parent[b] = a;
  rank[a] += rank[b];
}
static void run(Arena arena, Write_Buffer * restrict reader, Write_Buffer * restrict writer) {
  Size N = expect_i64(reader);
  Size M = expect_i64(reader);

  I64 *parent = new(&arena, I64, N + 1); {
    I64 *rank = new(&arena, I64, N + 1);
    for (Size i = 1; i <= N; i++) { parent[i] = i; rank[i] = 1; }
    for (Size i = 0; i < M; i++) {
      I64 X = expect_i64(reader);
      I64 Y = expect_i64(reader);
      I64 Z = expect_i64(reader);
      union_join(parent, rank, union_find(parent, X), union_find(parent, Y));
    }
  }
  I64 cost = 0; {
    for (Size i = 1; i <= N; i++) cost += (parent[i] == i);
  }
  append_i64(writer, cost, '\n');
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
    reader.buf[(reader.cap = fread(reader.buf, 1, (1ll << 23) - 1, stdin)) + 1] = '\0';
    arena.beg += reader.cap;
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
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
