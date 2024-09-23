/*
#+title: Reference for common "boilerplate" code
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public domain.
*/

typedef unsigned char      U8;
typedef unsigned long long U64;
typedef signed   int       I32;
typedef signed   long long I64;
typedef typeof((char *)0-(char *)0) Size;
typedef typeof(sizeof(0))           USize;

#define size_of(s)  (Size)sizeof(s)
#define count_of(s) (size_of((s)) / size_of(*(s)))
#define assert(c)   ((c) ? 0 : ({ asm volatile ("int3; nop"); }))
#define swap(t, a, b) do { t tmp = a; a = b; b= tmp; } while(0);
#define new(a, t, n) ((t *) arena_alloc(a, size_of(t), (Size)_Alignof(t), (n)))

typedef struct { U8 *beg, *end; } Arena;
typedef struct { Size cap; U8 *buf; U8 *at; } Write_Buffer;

__attribute((malloc, alloc_size(2,4), alloc_align(3)))
static U8 *arena_alloc(Arena *a, Size objsize, Size align, Size count) {
  Size padding = -(USize)(a->beg) & (align - 1);
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


#include "stdio.h"

// DFS
static void run_dag_dfs(Arena arena, I64 *restrict nodes, I64 *restrict adj, Size N)
{
  I64 *preorder = new(&arena, I64, N);
  Size preorder_count = 0;
  I64 *postorder = new(&arena, I64, N);
  Size postorder_count = 0;

  #define adj_at(i, j) adj[((i) * N) + j]
  typedef struct Frame_Stack { I32 i; I32 c; } Frame_Stack;

  Frame_Stack *stack = new(&arena, typeof(*stack), 1 << 10);
  Size stack_count = 1;
  while (stack_count > 0) {
  next_frame:;
    Frame_Stack *frame = &stack[stack_count - 1];

    _Bool first_visit = (frame->c == frame->i);
    if (first_visit) { preorder[preorder_count++] = nodes[frame->i]; }
    else { /* we can revert changes to global state here */  }

    while (frame->c < N) {
      Size c = frame->c++;
      if (adj_at(frame->i, c)) {
        stack[stack_count++] = (Frame_Stack){ .i = c, .c = c, };
        goto next_frame;
      }
    }

    _Bool last_visit = (frame->c >= N);
    assert(last_visit);
    postorder[postorder_count++] = nodes[frame->i];

    stack_count--;
  }

  printf("preorder: ");
  for (Size i = 0; i < N; i++) {
    printf("%lld ", preorder[i]);
  }
  printf("\n");

  printf("postorder: ");
  for (Size i = 0; i < N; i++) {
    printf("%lld ", postorder[i]);
  }
  printf("\n");
}

static void run_graph_dfs(Arena arena, I64 *restrict nodes, I64 *restrict adj, Size N)
{
  I64 *preorder = new(&arena, I64, N);
  Size preorder_count = 0;
  I64 *postorder = new(&arena, I64, N);
  Size postorder_count = 0;

  #define adj_at(i, j) adj[((i) * N) + j]
  typedef struct Frame_Stack { I32 i; I32 c; } Frame_Stack;

  Frame_Stack *stack = new(&arena, typeof(*stack), 1 << 10);
  Size stack_count = 1;
  U8 *visited = new(&arena, U8, N);
  while (stack_count > 0) {
  next_frame:;
    Frame_Stack *frame = &stack[stack_count - 1];
    visited[frame->i] = 1;

    _Bool first_visit = (frame->c == frame->i);
    if (first_visit) { preorder[preorder_count++] = nodes[frame->i]; }
    else { /* we can revert changes to global state here */  }

    while (frame->c < N) {
      Size c = frame->c++;
      if (!visited[c] && adj_at(frame->i, c)) {
        stack[stack_count++] = (Frame_Stack){ .i = c, .c = c, };
        goto next_frame;
      }
    }

    _Bool last_visit = (frame->c >= N);
    assert(last_visit);
    postorder[postorder_count++] = nodes[frame->i];

    stack_count--;
  }

  printf("preorder: ");
  for (Size i = 0; i < N; i++) {
    printf("%lld ", preorder[i]);
  }
  printf("\n");

  printf("postorder: ");
  for (Size i = 0; i < N; i++) {
    printf("%lld ", postorder[i]);
  }
  printf("\n");
}

static void run_dfs(Arena arena)
{
  {
    static I64 nodes[] = {1, 2, 3, 4, 5, 6};
    static I64 adj[count_of(nodes)][count_of(nodes)] = {
      {0, 1, 1, 0, 0, 1},
      {0, 0, 0, 1, 1, 0},
      {0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0},
    };
    Size N = count_of(nodes);
    run_dag_dfs(arena, nodes, (I64 *)adj, N);
  }
  {
    static I64 nodes[] = {1, 2, 3, 4, 5, 6};
    static I64 adj[count_of(nodes)][count_of(nodes)] = {
      {0, 1, 1, 0, 0, 1},
      {0, 0, 0, 1, 1, 1},
      {0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0, 0},
    };
    Size N = count_of(nodes);
    run_graph_dfs(arena, nodes, (I64 *)adj, N);
  }
}

// Union Segment
static Size union_find(I64 *parent, Size i) {
  while (i != parent[i]) {
    parent[i] = parent[parent[i]];
    i = parent[i];
  }
  return i;
}
static void union_join(I64 * restrict parent, I64 * restrict rank, I64 a, I64 b) {
  if (rank[a] < rank[b]) { swap(I64, a, b); }
  parent[b] = a;
  rank[a] += rank[b];
}
static void run_union(Arena arena, Write_Buffer * restrict reader, Write_Buffer * restrict writer) {
  Size N = expect_i64(reader);
  Size M = expect_i64(reader);

  // parent[i]      :: parent of node i
  // parent[i] == i :: node i is representative for set
  I64 *parent = new(&arena, I64, N + 1);
  I64 *rank = new(&arena, I64, N + 1);
  for (Size i = 1; i <= N; i++) { parent[i] = i; rank[i] = 1; }
  for (Size i = 0; i < M; i++) {
    I64 X = expect_i64(reader);
    I64 Y = expect_i64(reader);
    union_join(parent, rank, union_find(parent, X), union_find(parent, Y));
  }

  I64 cost = 0;
  for (Size i = 1; i <= N; i++) cost += (parent[i] == i);
  append_i64(writer, cost, '\n');
}

// Platform
#define HEAP_CAP     (1ll << 30)
#include <stdlib.h>
int main(int argc, char **argv)
{
  void *heap = malloc(HEAP_CAP);
  Arena arena = (Arena){heap, heap + HEAP_CAP};
  run_dfs(arena);
  free(heap);
  return 0;
}

/* Local Variables: */
/* compile-command: "cc ref.c -fsanitize=undefined -g3" */
/* End: */
