/*
  #+title: Farmer Feb
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
#define assert(c)    while((!(c))) __builtin_unreachable()
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

static void run_aux_generate_primes(Arena arena, Write_Buffer * restrict reader, Write_Buffer * restrict writer) {
  for (Size i = 0; i < 3000; i++) {
    B32 is_prime = 1;
    for (size t = 2; t < i; t++) {
      if ((i%t) == 0) { is_prime = 0; break; }
    }
    if (is_prime) append_i64(writer, i);
  }
}

static void run(Arena arena, Write_Buffer * restrict reader, Write_Buffer * restrict writer) {
  for (Size i = 0; i < 3000; i++) {
    B32 is_prime = 1;
    for (size t = 2; t < i; t++) {
      if ((i%t) == 0) { is_prime = 0; break; }
    }
    if (is_prime) append_i64(writer, i);
  }
}

// Platform
#define HEAP_CAP     (1ll << 30)
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

/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
