/*
#+title: Break Up: Zonal Computing Olympiad 2015, 29 Nov 2014
#+ref: https://www.codechef.com/practice/course/dynamic-programming/INTDP01/problems/ZCO15001
#+author: anton@hakanssn.com
#+licence: This is free and unencumbered software released into the public domain.

A sequence of positive integers is a palindrome if it reads the same in both directions.
The sequences 23, 45, 23 and 23, 45, 56, 23, 23, 56, 45, 23 are examples of palindromes.
The sequence 23, 45, 56 is not a palindrome. The sequence 23, 32 is not a palindrome
either. A sequence of length 1 is always a palindrome.

A given sequence of integers can be broken up into parts such that each of them is a
palindrome. Consider the sequence 34,45,34,56,34. This can be broken up into 3 palindrome
sequences with 34, 45, 34 constituting the first, 56 constituting the second and 34
constituting the third. It can also be broken in 5 palindrome sequences each containing a
single number. Thus, there may be many different ways to break up a given sequence into
palindrome sequences. We want to determine the smallest number /C/ such that the given
sequence can be broken up into /C/ palindrome sequences.

Observe that for any palindrome sequence the value of /C/ is 1. For the sequence 34, 45,
34, 56, 34 the answer is 3. Your aim is to write a program that computes this number for
any given sequence.

* Input format

The first line contains /N/, the number of values in the sequence.

This is followed by a line containing /N/ positive integers separated by space giving the
values of the sequence.

You may assume that all integers in the input are in the range 1 to 10^8 inclusive.

* Output format

Output a single integer giving the smalllest number /C/ so that the given sequence can
broken up into /C/ palindrome sequences.

 */

#include <stdint.h>
#include <stddef.h>
typedef uint8_t U8; typedef int64_t I64; typedef uint64_t  U64; typedef ptrdiff_t Size;

#define size_of(s)   (Size)sizeof(s)
#define count_of(s)  (size_of((s)) / size_of(*(s)))
#define assert(c)    while((!(c))) __builtin_unreachable()
#define new(a, t, n) ((t *) arena_alloc(a, size_of(t), (Size)_Alignof(t), (n)))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

typedef struct { U8 *beg, *end; } Arena;
typedef struct { Size cap; U8 *buf; U8 *at; } Write_Buffer;

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

static void run(Arena arena, Write_Buffer *reader, Write_Buffer *writer) {
  Size N = expect_i64(reader);
  I64 *a = new(&arena, typeof(*a), N);
  for (Size i = 0; i < N; i++) { a[i] = expect_i64(reader); }

  // dp[0..i] :: minimum number of contiguous palindromes that a[0..i] can be broken up into.
  U64 *dp = new(&arena, U64, N + 1) + 1;
  __builtin_memset(dp, 0x3F, N * sizeof(*dp));

  // len[l][r] :: palindrome lengh from index l to r inclusive, 0 if not a palindrome
  #define len(l, r) len_[((r) * N) + (l)]
  U64 *len_ = new(&arena, U64, N * N);

  for (Size r = 0; r < N; r++) {
    len(r, r) = 1;
    dp[r] = dp[r - 1] + 1;

    if ((r > 0) && (a[r] == a[r - 1])) {
      len(r - 1, r) = 2;
      dp[r] = min(dp[r], dp[r - 2] + 1);
    }

    for (Size l = r - 2; l >= 0; l--) {
      U64 inner = len(l + 1, r - 1);
      if ((a[l] == a[r]) && (inner > 0)) {
        len(l, r) = max(len(l, r), inner + 2);
        U64 candidate = dp[l - 1] + 1;
        dp[r] = min(dp[r], candidate);
      }
    }
  }

  I64 result = dp[N - 1];
  append_i64(writer, result, '\n');
  #undef len
}

// Platform
#define HEAP_CAP     (1ll << 30)

#include <stdlib.h>
#include <stdio.h>
#ifndef ONLINE_JUDGE
#  include "test.c"
#else
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
#endif // ONLINE_JUDGE

/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
