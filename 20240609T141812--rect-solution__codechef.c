/*
  #+title: Codechef: Rectangle
  #+ref:
  #+author: anton@hakanssn.com
  #+licence: This is free and unencumbered software released into the public domain.
*/

typedef unsigned char      U8;
typedef unsigned long long U64;
typedef signed   int       I32;
typedef signed   long long I64;
typedef typeof((char *)0-(char *)0) Size;
typedef typeof(sizeof(0))           USize;
typedef I32 B32;

#define size_of(s)   (Size)sizeof(s)
#define count_of(s)  (size_of((s)) / size_of(*(s)))
#define assert(c)    while((!(c))) __builtin_trap()
#define new(a, t, n) ((t *) arena_alloc(a, size_of(t), (Size)_Alignof(t), (n)))

typedef struct { U8 *beg, *end; } Arena;
typedef struct { Size cap; U8 *buf; U8 *at; } Stream;

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
static void append_buf(Stream *io, U8 *data, Size len, char separator) {
  for (Size i = 0; i < len; i++) { *(io->at++) = data[i]; }
  *(io->at++) = separator;
}
#define append_lit(io, lit, sep) append_buf((io), (lit), count_of((lit)) - 1, (sep))

// Rectangle solution using occurance table
// This solution allocates a lot of memory -- can we use a bloom filter instead?
static void run(Arena arena, Stream * restrict reader, Stream * restrict writer) {
  Size T = expect_i64(reader);
  for (Size i = 0; i < T; i++) {

    static U8 map[10001];
    Size A[4];
    for (int i = 0; i < 4; i++) {
      A[i] = expect_i64(reader);
      map[A[i]] = 0;
    }
    for (int i = 0; i < 4; i++) {
      map[A[i]]++;
    }

    B32 is_rect = 1;
    for (int i = 0; i < 4; i++) {
      if (map[A[i]] % 2) {
        is_rect = 0;
      }
    }

    append_lit(writer, is_rect ? "YES" : "NO", '\n');
  }
}

// Platform
#define HEAP_CAP (1ll << 30)
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
  void *heap = malloc(HEAP_CAP);
  Arena arena = (Arena){heap, heap + HEAP_CAP};
  Stream reader = {0}; {
    reader.buf = reader.at = arena.beg;
    reader.buf[(reader.cap = fread(reader.buf, 1, (1ll << 23) - 1, stdin)) + 1] = '\0';
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
