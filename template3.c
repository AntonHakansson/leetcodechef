/*
#+title:
#+ref:
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public domain.

 */

#define HEAP_CAP     (1ll << 30)

#include <stdint.h>
#include <stddef.h>
typedef uint8_t   U8;
typedef int64_t   I64;
typedef uint64_t  U64;
typedef ptrdiff_t Size;

#define size_of(s)   (Size)sizeof(s)
#define count_of(s)  (size_of((s)) / size_of(*(s)))
#define assert(c)    while((!(c))) __builtin_unreachable()
#define new(a, t, n) (t *) arena_alloc(a, size_of(t), (Size)_Alignof(t), (n))

typedef struct { U8 *beg, *end; } Arena;
typedef struct { Size cap; U8 *buf; U8 *at; } Stream;

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

// Solution
typedef struct {
  I64 *items;
  Size count;
} Test;

static void run(Arena arena, Stream * restrict reader, Stream * restrict writer) {
  Test test = {0}; {
    test.count = expect_i64(reader);
    test.items = new(&arena, typeof(*test.items), test.count);
    for (Size i = 0; i < test.count; i++)
      test.items[i] = expect_i64(reader);
  }

  { // solve
    I64 *dp = new(&arena, I64, test.count + 3) + 3; // dp[-3..test.count-1] safe
    for (Size i = 0; i < test.count; i++) { }
    I64 result = 0;
    append_i64(writer, result, '\n');
  }
}

// Platform
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char **argv) {
  void *heap = malloc(HEAP_CAP);
  Arena arena = (Arena){heap, heap + HEAP_CAP};
  Stream reader = {0}; {
    reader.buf = reader.at = arena.beg;
    reader.cap = fread(reader.buf, 1, (1ll << 24), stdin);
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
