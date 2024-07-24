/*
#+title: D - Polyomino
#+ref: https://atcoder.jp/contests/abc322/tasks/abc322_d
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
static void   write_i64  (Stream *io, I64 x, char separator);

static void run(Arena arena, Stream *reader, Stream *writer) {
#define STRIDE (4 + 1)
#define P(k, row, col) (ps[k])[(((row) * STRIDE) + (col))]
#define P_90(k, row, col) (ps[k])[(((row) * STRIDE) + (col))]
#define P_180(k, row, col) (ps[k])[(((row) * STRIDE) + (col))]
#define P_270(k, row, col) (ps[k])[(((row) * STRIDE) + (col))]

  U8 *ps[3] = {0};
  ps[0] = reader->at + 0 * STRIDE;
  ps[1] = reader->at + 1 * STRIDE;
  ps[2] = reader->at + 2 * STRIDE;
  reader->at += 3 * STRIDE;
  assert(reader->at <= reader->end);

  Size row_start[3], row_end[3], col_start[3], col_end[3];
  {
    Size total_occupied = 0;
    __builtin_memset(row_start, 0x3f, sizeof(row_start));
    __builtin_memset(col_start, 0x3f, sizeof(row_start));
    for (Size k = 0; k < 3; k++) {
      for (Size row = 0; row < 4; row++) {
        for (Size col = 0; col < 4; col++) {
          char c = P(k, row, col);
          if (c == '#') {
            total_occupied++;
            if (total_occupied > (4 * 4)) {
              write_i64(writer, 0, '\n');
              return;
            }
            row_start[k] = row < row_start[k] ? row : row_start[k];
            col_start[k] = col < col_start[k] ? col : col_start[k];
            row_end[k] = row > row_end[k] ? row : row_end[k];
            col_end[k] = col > col_end[k] ? col : col_end[k];
          }
        }
      }
      // one past last
      row_end[k]++;
      col_end[k]++;
    }
  }

  // we probably want to place the most constrained pieces first

  // in orig. orientation:
  // place horizontally 0..col_start[k]
  // place vertically   0..row_start[k]


  U8 grid[4][4] = {0};

  void place(Size k, Size orientation) {
    for (Size row_t = 0; row_t < 4; row_t++) {
      for (Size col_t = 0; col_t < 4; col_t++) {

        // place k's upper left corner at row_t, col_t
        Size height = row_end[k] - row_start[k];
        Size width  = col_end[k] - col_start[k];
        if (row_t + height > 4) { continue; }
        if (col_t + width  > 4) { continue; }

        // valid to place k at row, col in empty grid.
        // check for collision with already placed pieces.
        U8 grid2[4][4] = {0};
        __builtin_memcpy(grid2, grid, sizeof(grid));
        for (Size prow = row_start[k]; prow < row_end[k]; prow++) {
          for (Size pcol = col_start[k]; pcol < col_end[k]; pcol++) {
            if (grid2[row_t][col_t] == '#') { };
          }
        }

        // __builtin_memcpy(grid, grid2, sizeof(grid));
      }
    }
  }

  for (Size k = 0; k < 3; k++) {

    for (Size row_t = 0; row_t < 4; row_t++) {
      for (Size col_t = 0; col_t < 4; col_t++) {

        // place k's upper left corner at row_t, col_t
        Size row = row_t + row_start[k];
        Size col = col_t + col_start[k];
        Size height = row_end[k] - row_start[k];
        Size width  = col_end[k] - col_start[k];
        if (row + height > 4) { continue; }
        if (col + width > 4)  { continue; }

        // valid to place k at row, col in empty grid.
      }
    }

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

  free(heap);
  return 0;
}
#endif // ONLINE_JUDGE

/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
