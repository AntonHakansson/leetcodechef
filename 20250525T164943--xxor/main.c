/*
#+title:
#+ref:
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public
domain.
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

#define max(a, b) (((a) > (b)) ? (a) : (b))

static USize f(USize *A, USize count, USize x) {
  USize result = 0;
  for (USize i = 0; i < count; i++) {
    result += A[i] ^ x;
  }
  return result;
}

static void run_slow(Arena arena, Stream *reader, Stream *writer) {
  USize N = read_i64(reader);
  USize K = read_i64(reader);
  USize *A = new(&arena, USize, N);
  for (USize i = 0; i < N; i++) {
    A[i] = read_i64(reader);
  }
  USize r = 0;
  for (USize x = 0; x <= K; x++) {
    r = max(r, f(A, N, x));
  }
  write_i64(writer, r, '\n');
}

static void run_greedy(Arena arena, Stream *reader, Stream *writer) {
  Size N = read_i64(reader);
  USize K = read_i64(reader);
  USize *A = new(&arena, USize, N);
  // cnt[i] :: number of (1 << i) set for all j in A[j]
  Size cnt[64] = {0};
  {
    for (Size i = 0; i < N; i++) {
      A[i] = read_i64(reader);
      for (I64 b = 0; b < countof(cnt); b++) {
        cnt[b] += (A[i] >> b) & 1;
      }
    }
  }

  USize x = 0;
  for (I64 b = countof(cnt) - 1; b >= 0; b--) {
    USize bit_mask = (1ULL << b);
    if ((x | bit_mask) <= K) { // setting bit b of x doesn't exceed K
      // USize contrib_bit_0 = (cnt[b])     * (1ull << b); // Contribution if bit b of x set to 0
      // USize contrib_bit_1 = (N - cnt[b]) * (1ull << b); // Contribution if bit b of x set to 1
      // if (contrib_bit_1 > contrib_bit_0)
      if ((N - cnt[b]) > cnt[b]) { // setting bit b of x increases sum more
        x |= bit_mask;
      }
    }
  }
  write_i64(writer, f(A, N, x), '\n');
}

static void run(Arena arena, Stream *reader, Stream *writer) {
  run_greedy(arena, reader, writer);
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
  write(1, s->beg, s->at - s->beg);
  s->at = s->beg;
}
static void fill(Stream *s) {
  assert(s->end > s->at);
  Size cap = s->end - s->beg;
  Size unprocessed = (s->at == s->beg) ? 0 : (s->end - s->at);
  s->at = s->beg = __builtin_memmove(s->beg, s->at, unprocessed);
  read(0, s->beg + unprocessed, cap - unprocessed);
}
#ifdef ONLINE_JUDGE
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
