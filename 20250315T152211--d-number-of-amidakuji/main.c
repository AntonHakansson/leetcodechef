/*
#+title: D - Number of Amidakuji
#+ref: https://atcoder.jp/contests/abc113/tasks/abc113_d
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public domain.
*/

#define countof(s)    (Size)(sizeof((s)) / sizeof(*(s)))
#define assert(c)     while((!(c))) __builtin_trap()
#define breakpoint(c) ((c) ? ({ asm volatile ("int3; nop"); }) : 0)
#define new(a, t, n)  ((t *) arena_alloc(a, (Size)sizeof(t), (Size)_Alignof(t), (n)))

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
  enum { MOD = 1000000007, };

  Size H = read_i64(reader);
  Size W = read_i64(reader);
  Size K = read_i64(reader);

  I64 *dp_ = new(&arena, I64, (H + 1) * W);
  #define dp(i, j) dp_[((i) * (W)) + (j)]

  dp(0, 0) = 1;
  for (Size i = 0; i < H; i++) {
    for (Size j = 0; j < W; j++) {
      Size choices = W - 1;
      for (Size placements = 0; placements < (1ll << choices); placements++) {
        _Bool ok = 1;
        for (Size l = 0; l < choices - 1; l++) {
          _Bool left = (placements >> (l + 0)) & 1;
          _Bool right = (placements >> (l + 1)) & 1;
          if (left && right) {
            ok = 0;
          }
        }
        if (ok) {
          I64 dpij = dp(i, j);
          // Go left
          if (j > 0 && ((placements >> (j - 1)) & 1)) {
            dp(i + 1, j - 1) += dpij;
            dp(i + 1, j - 1) %= MOD;
          }
          // Go right
          else if (j < choices && ((placements >> (j - 0)) & 1)) {
            dp(i + 1, j + 1) += dpij;
            dp(i + 1, j + 1) %= MOD;
          }
          // Go straight
          else {
            dp(i + 1, j) += dpij;
            dp(i + 1, j) %= MOD;
          }
        }
      }
    }
  }
  write_i64(writer, dp(H, K - 1), '\n');
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
  int i = countof(tmpbuf);
  do { tmpbuf[--i] = (char)(x % 10) + '0'; } while (x /= 10);
  do { *io->at++ = tmpbuf[i++]; } while (i < countof(tmpbuf));
  *(io->at++) = separator;
  assert(io->at < io->end);
}

// Platform
#include <unistd.h>
#include <stdlib.h>

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
  enum { HEAP_CAP = (1ll << 30) };
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
