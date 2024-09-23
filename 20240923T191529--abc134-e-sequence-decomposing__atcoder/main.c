/*
#+title: Sequence Decomposing
#+ref: https://atcoder.jp/contests/abc134/tasks/abc134_e
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public domain.
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
static void   write_i64  (Stream *io, I64 x, char separator);

static void run(Arena arena, Stream *reader, Stream *writer) {
  Size T = read_i64(reader);
  U64 *a = new(&arena, U64, T);
  for (Size i = 0; i < T; i++){
    a[i] = read_i64(reader);
  }

  // O(N^2) solution -> time limit exceeded.
#if 0
  // Observation: We are dealing with subsequences --- an approach like the longest increasing subsequence might be relevant.
  // -> minumum colorings of strictly increasing subsequences <=> the longest decreasing subsequence.
  #define max(a, b) (((a) > (b)) ? (a) : (b))

  U64 *dp = new(&arena, U64, T);
  dp[T - 1] = 1;
  for (Size i = T - 1; i >= 0; i--) {
    U64 v = 1;
    // We need to lookup j, i < j < T where a[i] >= a[j] with maximum dp[j] in O(log N) time or better.
    for (Size j = i + 1; j < T; j++) {
      if (a[i] >= a[j]) {
        v = max(v, 1 + dp[j]);
      }
    }
    dp[i] = v;
  }
  write_i64(writer, dp[0], '\n');
#endif

  // O(N log N)
  // Count number of increasing subsequences
#if 1
  U64 *set = new(&arena, U64, T);
  Size set_count = 0;
  set[set_count++] = a[0];
  for (Size i = 1; i < T; i++) {
    Size lb = set_count;
    { // lb = lower_bound(set, a[i])
      Size l = 0, r = set_count - 1;
      while (l <= r) {
        Size m = l + (r - l) / 2;
        if (a[i] <= set[m]) { lb = m; r = m - 1; }
        else                { l = m + 1; }
      }
    }
    if (lb == 0) { // a[i] <= set[j] for all j
      __builtin_memmove(set + 1, set, set_count * sizeof(*set));
      set[0] = a[i];
      set_count++;
    }
    else { // a[i] > set[j], j >= lb - 1
      set[lb - 1] = a[i];
    }
  }
  write_i64(writer, set_count, '\n');
#endif
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
static Stream stream(Arena *arena, Size cap, void (*update_fn)(Stream *)) {
  Stream r = { .update = update_fn };
  r.beg = r.at = new(arena, U8, cap);
  r.end = r.beg + cap;
  return r;
}
static I64 read_i64(Stream *io) {
  if (io->update && io->at + 256 >= io->end) { io->update(io); }
  U64 u = 0, s = 0;
  while(*io->at && *io->at <= 32) { ++io->at; }
  if     (*io->at == '-') { s = ~s; ++io->at; } else if (*io->at == '+') { ++io->at; }
  while ((*io->at >= '0') && (*io->at <= '9')) { u = u * 10 + (*(io->at++) - '0'); }
  assert(io->at < io->end);
  return (u^s) + !!s;
}
static void write_i64(Stream *io, I64 x, char separator) {
  if (io->update && io->at + 256 >= io->end) { io->update(io); }
  if (x < 0) { *io->at++ = '-'; x = -x; }
  char tmpbuf[20];
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

static void flush(Stream *s) {
  assert(s->at >= s->beg);
  Size nbytes = write(1, s->beg, s->at - s->beg);
  s->at = s->beg;
}
static void fill(Stream *s) {
  assert(s->end > s->at);
  Size cap = s->end - s->beg;
  Size unprocessed = (s->at == s->beg) ? 0 : (s->end - s->at);
  s->at = __builtin_memmove(s->beg, s->at, unprocessed);
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
  return 0;
}
#endif // ONLINE_JUDGE

/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
