/*
#+title: Codechef: SUPW
#+ref: https://www.codechef.com/practice/course/dynamic-programming/INTDP01/problems/ZCO14002
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public domain.

In ICO School, all students have to participate regularly in SUPW. There is a different SUPW activity each day, and each activity has its own duration. The SUPW schedule for the next term has been announced, including information about the number of minutes taken by each activity.

Nikhil has been designated SUPW coordinator. His task is to assign SUPW duties to students, including himself. The school's rules say that no student can go three days in a row without any SUPW duty.

Nikhil wants to find an assignment of SUPW duty for himself that minimizes the number of minutes he spends overall on SUPW.

* Input format

Line 1: A single integer /N/, the number of days in the future for which SUPW data is available.

Line 2: /N/ non-negative integers, where the integer in position /i/ represents the number of minutes required for SUPW work on day /i/.


* Output format

The output consists of a single non-negative integer, the minimum number of minutes that Nikhil needs to spend on SUPW duties this term

* Test data

There is only one subtask worth 100 marks. In all inputs:

- 1 ≤ /N/ ≤ 2×10^{5}
- The number of minutes of SUPW each day is between 0 and 10^{4}, inclusive.

* Sample 1:

#+begin_example
10
3 2 1 1 2 3 1 3 2 1
#+end_example

#+begin_example
4
#+end_example

Explanation: 1+1+1+1

* Sample 2:

#+begin_example
8
3 2 3 2 3 5 1 3
#+end_example

#+begin_example
5
#+end_example

Explanation: 2+2+1

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
typedef struct { Size end; U8 *buf; U8 *beg; } Stream;

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
I64 expect_i64(Stream *io) {
  U64 u = 0, s = 0;
  while(*io->beg && *io->beg <= 32) { ++io->beg; }
  if (*io->beg == '-') { s = ~s; ++io->beg; } else if (*io->beg == '+') { ++io->beg; }
  while ((*io->beg >= '0') && (*io->beg <= '9')) { u = u*10 + (*(io->beg++) - '0'); }
  return (u^s) + !!s;
}
void append_i64(Stream *io, I64 x, char separator) {
  char tmpbuf[20];
  if (x < 0) { *io->beg++ = '-'; x = -x; }
  int i = count_of(tmpbuf);
  do { tmpbuf[--i] = (x % 10) + '0'; } while (x /= 10);
  do { *io->beg++ = tmpbuf[i++]; } while (i < count_of(tmpbuf));
  *(io->beg++) = separator;
}

// Solution
#define min(a, b) ((a) < (b)) ? (a) : (b)
#define min3(a, b, c) min((a), min((b), (c)))

typedef struct { I64 *items; Size count; } Test;

void run(Arena arena, Stream * restrict reader, Stream * restrict writer) {
  Test test = {0}; {
    test.count = expect_i64(reader);
    test.items = new(&arena, typeof(*test.items), test.count);
    for (Size i = 0; i < test.count; i++)
      test.items[i] = expect_i64(reader);
  }

  I64 *dp = new(&arena, I64, test.count + 3) + 3; // dp[-3..test.count-1] safe
  for (Size i = 0; i < test.count; i++) {
    dp[i] = test.items[i];
    dp[i] += min3(dp[i - 1], dp[i - 2], dp[i - 3]);
  }
  append_i64(writer, min3(dp[test.count - 1], dp[test.count - 2], dp[test.count - 3]), '\n');
}

// Platform
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char **argv) {
  void *heap = malloc(HEAP_CAP);
  Arena arena = (Arena){heap, heap + HEAP_CAP};
  Stream reader = (Stream){.buf = arena.beg, .beg = arena.beg};
  reader.end = fread(reader.buf, 1, (1ll << 24), stdin);
  arena.beg += reader.end;
  Stream writer = (Stream){.end = 1ll << 13, .buf = arena.beg, .beg = arena.beg};
  arena.beg += writer.end;
  run(arena, &reader, &writer);
  fwrite(writer.buf, 1, (writer.beg - writer.buf), stdout);
  free(heap);
  return 0;
}

/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
