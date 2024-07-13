/*
#+title: E - Friendship
#+ref: https://atcoder.jp/contests/abc131/tasks/abc131_e
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
  Size N = read_i64(reader);
  Size K = read_i64(reader);

  Size limit = ((N - 1) * (N - 2)) / 2;
  if (K > limit) {
    write_i64(writer, -1, '\n');
    return;
  }

  I64 *pairs_ = new(&arena, I64, limit);
  #define pairs(i, j) pairs_[(((i) * 2) + (j))]
  Size pairs_count = 0;
  Size k = limit;
  for (Size i = 1; i < N; i++) {
    pairs(pairs_count, 0) = 0;
    pairs(pairs_count, 1) = i;
    pairs_count++;
  }
  for (Size i = 1; k > K && i < N; i++) {
    for (Size j = i + 1; k > K && j < N; j++) {
      pairs(pairs_count, 0) = i;
      pairs(pairs_count, 1) = j;
      pairs_count++;
      k--;
    }
  }
  write_i64(writer, pairs_count, '\n');
  for (Size i = 0; i < pairs_count; i++) {
    write_i64(writer, pairs(i, 0) + 1, ' ');
    write_i64(writer, pairs(i, 1) + 1, '\n');
  }
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
