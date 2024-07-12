/*
#+title: D - equeue
#+ref: https://atcoder.jp/contests/abc128/tasks/abc128_d
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public domain.

* Problem Statement

Your friend gave you a dequeue \(D\) as a birthday present.

\(D\) is a horizontal cylinder that contains a row of \(N\) jewels.

The /values/ of the jewels are \(V_{1},V_{2},...,V_{N}\) from left to right.
There may be jewels with negative values.

In the beginning, you have no jewel in your hands.

You can perform at most \(K\) operations on \(D\) chosen from the following, at most \(K\) times (possibly zero):
- Operation A: Take out the leftmost jewel contained in \(D\) and have it in your hand. You cannot do this operation when \(D\) is empty.
- Operation B: Take out the rightmost jewel contained in \(D\) and have it in your hand. You cannot do this operation when \(D\) is empty.
- Operation C: Choose a jewel in your hands and insert it to the left end of \(D\) You cannot do this operation when you have no jewel in your hand.
- Operation D: Choose a jewel in your hands and insert it to the right end of \(D\) You cannot do this operation when you have no jewel in your hand.

Find the maximum possible sum of the values of jewels in your hands after the operations.

 */

#define count_of(s)  (Size)(sizeof((s)) / sizeof(*(s)))
#define assert(c)    while((!(c))) __builtin_unreachable()
#define new(a, t, n) ((t *) arena_alloc(a, (Size)sizeof(t), (Size)_Alignof(t), (n)))

typedef unsigned char       U8;
typedef unsigned long long  U64;
typedef signed   long long  I64;
typedef typeof((char *)0-(char *)0)  Size;
typedef typeof((sizeof(0)))          USize;
typedef struct { U8 *beg, *end; } Arena;
typedef struct Stream { U8 *beg, *end, *at; void (*update)(struct Stream *); } Stream;

static U8    *arena_alloc(Arena *a, Size objsize, Size align, Size count);
static I64    read_i64   (Stream *io);
#define read_i64s(arena, N) ({ I64 *r = new(arena, I64, (N)); for (Size i = 0; i < (N); i++) r[i] = read_i64(reader); r;})
static void   write_i64  (Stream *io, I64 x, char separator);

#define min(a, b) ((a) < (b)) ? (a) : (b)
#define swap(Type, a, b) do { Type tmp = (a); (a) = (b); (b) = tmp; } while (0)
#define minheap_init(heapobj, arena, cap) \
  (heapobj)->items = (void *)arena_alloc((arena), sizeof((heapobj)->items[0]), _Alignof((heapobj)->items[0]), (cap)); \
  (heapobj)->count = 0; \
  (heapobj)->capacity = cap;
#define minheap_push(heapobj, item) ({ \
  __typeof__ (item) item_copy = item; \
  Size i = (heapobj)->count; \
  (heapobj)->items[(heapobj)->count++] = item_copy; \
  for (Size parent = (i - 1) / 2; i > 0 && (heapobj)->items[i] < (heapobj)->items[parent]; parent = (i - 1) / 2) { \
    swap(I64, (heapobj)->items[i], (heapobj)->items[parent]); \
    i = parent; \
  } \
  item_copy; \
  })
#define minheap_pop(heapobj) ({ \
  __typeof__ ((heapobj)->items[0]) result = (heapobj)->items[0]; \
  (heapobj)->items[0] = (heapobj)->items[--(heapobj)->count]; \
  Size parent = 0; \
  for (Size child_left = parent * 2 + 1; child_left < (heapobj)->count; child_left = parent * 2 + 1) { \
    Size child_right = child_left + 1; \
    Size child = (heapobj)->items[child_left] < (heapobj)->items[child_right] ? child_left : child_right; \
    if (child < (heapobj)->count && (heapobj)->items[child] < (heapobj)->items[parent]) { \
      swap(I64, (heapobj)->items[parent], (heapobj)->items[child]); \
      parent = child; \
    } \
    else { \
      break; \
    } \
  } \
  result; \
  })

static void run(Arena arena, Stream *reader, Stream *writer) {
  Size N = read_i64(reader);
  Size K = read_i64(reader);
  I64 *V = read_i64s(&arena, N);

  // Presumably naive backtracking solution exceeds time limit. O(4^K).
  //
  // Observation: We can perform operations C and D at the very end as a "discard jewels" operation.
  // Observation: If we take A jewels from the left and B jewels from the right, then K - (A + B) of least valued jewels can be discarded to obtain optimal for that A, B, D ("discard") combination.
  // O(K^3 lg K)
  //
  I64 best = 0;
  Size M = min(K, N);
  for (Size a = 0; a <= M; a++) {
    Arena temp = arena;
    for (Size b = 0; b <= M - a; b++) {
      struct { I64 *items; Size count; Size capacity; } hand = {0};
      minheap_init(&hand, &temp, K);

      I64 sum = 0;
      for (Size i = 0; i < a; i++) { sum += minheap_push(&hand, V[i]); }
      for (Size i = 0; i < b; i++) { sum += minheap_push(&hand, V[N - 1 - i]); }

      Size ndiscard = min(K - (a + b), hand.count);
      while (hand.count > 0 && hand.items[0] < 0 && ndiscard-- > 0) {
        sum -= minheap_pop(&hand);
      }

      best = sum > best ? sum : best;
    }
  }
  write_i64(writer, best, '\n');
}

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
static I64 read_i64(Stream *io) {
  if (io->update && io->at + 32 >= io->end) { io->update(io); }
  U64 u = 0, s = 0;
  while(*io->at && *io->at <= 32) { ++io->at; }
  if     (*io->at == '-') { s = ~s; ++io->at; } else if (*io->at == '+') { ++io->at; }
  while ((*io->at >= '0') && (*io->at <= '9')) { u = u * 10 + (*(io->at++) - '0'); }
  assert(io->at < io->end);
  return (u^s) + !!s;
}
static void write_i64(Stream *io, I64 x, char separator) {
  if (io->update && io->at + 32 >= io->end) { io->update(io); }
  if (x < 0) { *io->at++ = '-'; x = -x; }
  char tmpbuf[32];
  int i = count_of(tmpbuf);
  do { tmpbuf[--i] = (x % 10) + '0'; } while (x /= 10);
  do { *io->at++ = tmpbuf[i++]; } while (i < count_of(tmpbuf));
  *(io->at++) = separator;
  assert(io->at < io->end);
}

// Platform
#include <unistd.h>
#include <stdlib.h>

#define HEAP_CAP     (1ll << 30)

static Stream stream(Arena *arena, Size cap, void (*update_fn)(Stream *)) {
  Stream r = { .update = update_fn };
  r.beg = r.at = new(arena, U8, cap);
  r.end = r.beg + cap;
  return r;
}
static void flush(Stream *s) {
  assert(s->at >= s->beg);
  Size nbytes = write(1, s->beg, s->at - s->beg);
  s->at = s->beg;
}
static void fill(Stream *s) {
  assert(s->end > s->at);
  Size cap = s->end - s->beg;
  Size unprocessed = (s->at == s->beg) ? 0 : (s->end - s->at);
  __builtin_memmove(s->beg, s->at, unprocessed);
  s->at = s->beg;
  int nbytes = read(0, s->beg + unprocessed, cap - unprocessed);
}

#ifndef ONLINE_JUDGE
#  include "test.c"
#else
int main(int argc, char **argv) {
  void *heap = malloc(HEAP_CAP);
  Arena arena = (Arena){heap, heap + HEAP_CAP};
  Stream reader = stream(&arena, (1ll << 18), fill);
  Stream writer = stream(&arena, (1ll << 18), flush);
  fill(&reader);
  run(arena, &reader, &writer);
  flush(&writer);
  free(heap);
  return 0;
}
#endif // ONLINE_JUDGE

/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
