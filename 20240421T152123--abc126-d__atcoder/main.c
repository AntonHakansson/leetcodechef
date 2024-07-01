/*
#+title: Atcoder beginner contest 126 D - Even Relation
#+ref: https://atcoder.jp/contests/abc126/tasks/abc126_d
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public domain.

* Problem Statement

We have a tree with =N= vertices numbered =1= to =N=.
The =i=-th edge in the tree connects Vertex u_i and Vertex v_i, and its length is w_i.
Your objective is to paint each vertex in the tree white or black (it is fine to paint all vertices the same color) so that the following condition is satisfied:

- For any two vertices painted in the same color, the distance between them is an even number.

Find a coloring of the vertices that satisfies the condition and print it.
It can be proved that at least one such coloring exists under the constraints of this problem.

*/

typedef unsigned char      U8;
typedef unsigned long long U64;
typedef signed   long long I64;
typedef typeof(sizeof(0))           Uptr;
typedef typeof((char *)0-(char *)0) Size;
typedef typeof(sizeof(0))           USize;

#define size_of(s)   (Size)sizeof(s)
#define count_of(s)  (size_of((s)) / size_of(*(s)))
#define assert(c)    while((!(c))) __builtin_unreachable()
#define new(a, t, n) ((t *) arena_alloc(a, size_of(t), (Size)_Alignof(t), (n)))
#define swap(t, a, b) do { t tmp = a; a = b; b= tmp; } while(0);

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

static void run(Arena arena, Write_Buffer * restrict reader, Write_Buffer * restrict writer) {
  Size N = expect_i64(reader);

  U8 *coloring = new(&arena, U8, N); {
    I64 *adj_ = new(&arena, typeof(*adj_), N*N);
    #define adj(i, j) adj_[((i) * N) + (j)]
    for (Size i = 0; i < N - 1; i++) {
      I64 from = expect_i64(reader) - 1;
      I64 to   = expect_i64(reader) - 1;
      I64 weight = expect_i64(reader);
      adj(from, to) = weight;
      adj(to, from) = weight;
    }

    typedef struct Frame_Stack {
      Size i;
      I64 w;
    } Frame_Stack;

    Frame_Stack *stack = new(&arena, Frame_Stack, N * (N - 1));
    Size stack_count = 0;
    stack[stack_count++] = (Frame_Stack){0};

    U8 *visited = new (&arena, U8, N);
    while (stack_count > 0) {
      Frame_Stack frame = stack[stack_count--];
      if (visited[frame.i] > 0) { continue; }
      visited[frame.i] = 1;
      if ((frame.w % 2) == 1) { coloring[frame.i] = 1; }
      for (Size n = 0; n < N; n++) {
        I64 dist_to_neigh = adj(frame.i, n);
        I64 dist_to_root  = frame.w + dist_to_neigh;
        if ((visited[n] == 0) && (dist_to_neigh > 0)) {
          stack[stack_count++] = (Frame_Stack) { .i = n, .w = dist_to_root};
        }
      }
    }
  }
  for (Size i = 0; i < N; i++) {
    *(writer->at++) = coloring[i] + '0';
    *(writer->at++) = '\n';
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
