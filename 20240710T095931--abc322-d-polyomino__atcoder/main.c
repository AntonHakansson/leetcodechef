/*
#+title: D - Polyomino
#+ref: https://atcoder.jp/contests/abc322/tasks/abc322_d
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public domain.
*/

#define countof(s)   (Size)(sizeof((s)) / sizeof(*(s)))
#define assert(c)    while((!(c))) __builtin_unreachable()
#define new(a, t, n) ((t *) arena_alloc(a, (Size)sizeof(t), (Size)_Alignof(t), (n)))
#define s8(s)        (S8){(U8 *)s, countof(s)-1}
#define for_range(it, beg, end) for (Size it = (beg); it < (end); it++)

typedef unsigned char       U8;
typedef typeof((char *)0-(char *)0)  Size;
typedef typeof((sizeof(0)))          USize;
typedef struct { U8 *beg, *end; } Arena;
typedef struct Stream { U8 *beg, *end, *at; void (*update)(struct Stream *); } Stream;
typedef struct { U8 *data; Size len; } S8;

static U8    *arena_alloc(Arena *a, Size objsize, Size align, Size count);
static void   write_s8(Stream *io, S8 s, char separator);

static void run(Arena arena, Stream *reader, Stream *writer) {
  U8 ps[3][4][4] = {0};
  for_range(k, 0, 3) {
    for_range(row, 0, 4) {
      for_range(col, 0, 4) { ps[k][row][col] = *reader->at++; }
      while (*reader->at == '\n') reader->at++;
    }
  }

  _Bool dfs(U8 grid[4][4], Size k, Size drow, Size dcol) {
    if (k == 3) {
      _Bool filled = 1;
      for_range(row, 0, 4) for_range(col, 0, 4) filled &= (grid[row][col] == '#');
      return filled;
    }

    U8 grid_staging[4][4] = {0};
    if (k >= 0) {
      __builtin_memcpy(grid_staging, grid, sizeof(grid_staging));
      for_range(row, 0, 4) {
        for_range(col, 0, 4) {
          _Bool t = ps[k][row][col] == '#';
          if (!t) continue;

          Size r = row + drow;
          Size c = col + dcol;
          if (r < 0 || r >= 4) {
            // # out of bounds
            return 0;
          }
          if (c < 0 || c >= 4) {
            // # out of bounds
            return 0;
          }
          if (grid_staging[r][c] == '#') {
            // collision
            return 0;
          }
          grid_staging[r][c] = !!t * '#';
        }
      }
    }

    _Bool found = 0;
    for_range(drow, -3, 3) {
      for_range(dcol, -3, 3) {
        found |= dfs(grid_staging, k + 1, drow, dcol);
      }
    }
    return found;
  }

  void rotate_CCW(U8 piece[4][4]) {
    U8 staging[4][4] = {0};
    for_range(row, 0, 4) {
      for_range(col, 0, 4) {
        staging[row][col] = piece[3 - col][row];
      }
    }
    __builtin_memcpy(piece, staging, sizeof(staging));
  }

  _Bool found = 0;
  for_range(rot_b, 0, 4) {
    for_range(rot_c, 0, 4) {
      U8 grid[4][4] = {0};
      found |= dfs(grid, -1, 0, 0);
      rotate_CCW(ps[2]);
    }
    rotate_CCW(ps[1]);
  }
  write_s8(writer, found ? s8("Yes") : s8("No"),  '\n');
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
static void write_bytes(Stream *io, U8 *bytes, Size len, char separator) {
  if (io->update && io->at + len >= io->end) { io->update(io); }
  while (len--) *(io->at++) = *(bytes++);
  if (separator) { *(io->at++) = separator; }
  assert(io->at < io->end);
}
static void write_s8(Stream *io, S8 s, char separator) {
  write_bytes(io, s.data, s.len, separator);
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
