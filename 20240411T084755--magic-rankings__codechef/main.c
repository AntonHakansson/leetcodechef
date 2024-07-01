/*
* [[https://www.codechef.com/practice/course/dynamic-programming/INTDP01/problems/MGCRNK][Codechef: Magic Rankings]]
:PROPERTIES:
:ENTERED_ON: [2024-04-01 Mon]
:END:

Everybody loves magic, especially magicians who compete for glory on the Byteland Magic
Tournament. Magician Cyael is one such magician.

Cyael has been having some issues with her last performances and today she'll have to
perform for an audience of some judges, who will change her tournament ranking, possibly
increasing it. As she is a great magician she managed to gather a description of the fixed
judges' disposition on the room (which is represented as an \(N \times N\) square matrix),
such that she knows in advance the fixed points each judge will provide.

She also knows that the room is divided into several parallel corridors, such that we will
denote the \(j^{th}\) cell on corridor \(i\), as \(\lbrack i\rbrack\lbrack j\rbrack\).
Note that some judges can award Cyael, zero points or negative points, as they are never
pleased with her performance. There is just one judge at each cell of the matrix, except
the cells \(\lbrack 1\rbrack\lbrack 1\rbrack\) and \(\lbrack N\rbrack\lbrack N\rbrack\).
To complete her evaluation, she must start on the top leftmost corner of the room (cell
\(\lbrack 1\rbrack\lbrack 1\rbrack\)), and finish on the bottom right corner (cell
\(\lbrack N\rbrack\lbrack N\rbrack\)), moving either to the cell directly in front of her
on the same corridor (that is, moving from cell \(\lbrack r\rbrack\lbrack c\rbrack\) to
cell \(\lbrack r\rbrack\lbrack c + 1\rbrack\), where \(c + 1\) \(\leq\) \(N\)) or to the
cell in the next corridor directly in front of where she is (that is, moving from cell
\(\lbrack r\rbrack\lbrack c\rbrack\) to cell \(\lbrack r + 1\rbrack\lbrack c\rbrack\),
where \(r + 1\) \(\leq\) \(N\)). She will keep doing this until she reaches the end point
of the room, i.e. last cell \(\lbrack N\rbrack\lbrack N\rbrack\) on the last corridor.
Cyael will be judged at all visited cells with a judge.

Cyael wants to maximize her average score at end of her performance. More specifically, if
she passes \(K\) judges, each being on cell \(\lbrack i_{1}\rbrack\lbrack j_{1}\rbrack\),
cell \(\lbrack i_{2}\rbrack\lbrack j_{2}\rbrack\), \(\ldots\), cell \(\lbrack
i_{K}\rbrack\lbrack j_{K}\rbrack\) respectively, then she wants to maximize
\(\frac{(S\lbrack i_{1}\rbrack\lbrack j_{1}\rbrack + S\lbrack i_{2}\rbrack\lbrack
j_{2}\rbrack + \ldots + S\lbrack i_{K}\rbrack\lbrack j_{K}\rbrack)}{K}\), where \(S\lbrack
i\rbrack\lbrack j\rbrack\) denotes the points that the judge will give her on the cell
\(\lbrack i\rbrack\lbrack j\rbrack\). Help her determine the best path she has to follow
in order to maximize her average points.

** Input Format

- The first line contains a single integer \(T\) denoting the number of test cases. The
  description for \(T\) test cases follows.
- For each test case, the first line contains a single integer \(N\).
- Each of the next \(N\) lines contains \(N\) space-separated integers. The j-th integer
  \(S\lbrack i\rbrack\lbrack j\rbrack\) in i-th line denotes the points awarded by the
  judge at cell \(\lbrack i\rbrack\lbrack j\rbrack\).
- Note that the cells \(\lbrack 1\rbrack\lbrack 1\rbrack\) and \(\lbrack N\rbrack\lbrack
  N\rbrack\) have no judges, so \(S\lbrack 1\rbrack\lbrack 1\rbrack\) and \(S\lbrack
  N\rbrack\lbrack N\rbrack\) will be 0.

** Output Format

- For each test case, if the maximum possible average points Cyael can obtain is negative,
  output a single line containing "Bad Judges" (quotes for clarity).
- Otherwise, output the maximum possible average points.
- The answer will be considered correct if it has an absolute error no more than \(10^{-
  6}\).

** Constraints

- 1 \(\leq\) \(T\) \(\leq 20\)
- 2 \(\leq\) \(N\) \(\leq 100\)
- -2500 \(\leq\) \(S\lbrack i\rbrack\lbrack j\rbrack\) \(\leq 2500\)
- \(S\lbrack 1\rbrack\lbrack 1\rbrack\) = \(S\lbrack N\rbrack\lbrack N\rbrack = 0\)

** Sample 1:

#+begin_example
2
2
0 -4
8 0
2
0 -45
-3  0
#+end_example

#+begin_example
8.000000
Bad Judges
#+end_example

** Explanation:

*Test case \(1\):* An optimal path for Cyael would be \(\left.
(1,1)\rightarrow(2,1)\rightarrow(2,2) \right.\). This way Cyael faces \(1\) judge and gets
a total score of \(8\). Thus, the average score is \(\frac{8}{1} = 8\).

*Test case \(2\):* No matter what path Cyael chooses, the final score would be less than
 \(0\).

 */

#include <stdint.h>
#include <stddef.h>

typedef unsigned char Byte;
typedef uint8_t   U8;
typedef int32_t   I32;
typedef int64_t   I64;
typedef uint32_t  U32;
typedef uint64_t  U64;
typedef ptrdiff_t Size;
typedef size_t    USize;
typedef signed int B32;

#define size_of(s)   (Size)sizeof(s)
#define align_of(s)  (Size)_Alignof(s)
#define count_of(s)  (size_of((s)) / size_of(*(s)))
#define assert(c)    while((!(c))) __builtin_unreachable()
#define new(a, t, n) (t *) arena_alloc(a, size_of(t), align_of(t), (n))
#define S(s)         (Str){ .buf = (U8 *)(s), .len = count_of((s)) - 1, }
#define S_FMT(s)     (int)(s).len, (s).buf    /* printf("%.*s", S_FMT(str)) */
#define str_eq(a, b) ((a).len == (b).len) && (__builtin_memcmp((a).buf, (b).buf, (a).len) == 0);
#define IO_CAP (1ll << 23)

typedef struct { Byte *beg, *end; }   Arena;
typedef struct { U8 *buf; Size len; } Str;
typedef struct { U8 buf[IO_CAP]; U8 *at; } IO;

__attribute((malloc, alloc_size(2,4), alloc_align(3)))
static Byte *arena_alloc(Arena *a, Size objsize, Size align, Size count)
{
  Size padding = -(uintptr_t)(a->beg) & (align - 1);
  Size total   = padding + objsize * count;
  assert(total < (a->end - a->beg) && "out of memory");
  Byte *p = a->beg + padding;
  __builtin_memset(p, 0, objsize * count);
  a->beg += total;
  return p;
}

IO *io_init(Arena *arena)
{
  IO *r = new (arena, IO, 1);
  r->at = r->buf;
  return r;
}
I64 expect_i64(IO *io)
{
  U64 u = 0, s = 0;
  while(*io->at && *io->at <= 32) ++io->at;
  if (*io->at == '-') { s = ~s; ++io->at; } else if (*io->at == '+') { ++io->at; }
  while ((*io->at >= '0') && (*io->at <= '9'))
    u = u*10 + (*(io->at++) - '0');
  return (u^s) + !!s;
}
void append_str(IO *io, Str s, char separator)
{
  for (Size i = 0; i < s.len; i++) { *io->at++ = s.buf[i];}
  *io->at++ = separator;
}
void append_i64(IO *io, I64 x, char separator)
{
  char tmpbuf[20];
  if (x < 0) { *io->at++ = '-'; x = -x; }
  int i = sizeof(tmpbuf);
  do { tmpbuf[--i] = (x % 10) + '0'; } while (x /= 10);
  do { *io->at++ = tmpbuf[i++]; } while (i < sizeof(tmpbuf));
  *io->at++ = separator;
}


////////////////////////////////////////////////////////////////////////////////
//- Solution
#include <stdlib.h>
#include <stdio.h>

void io_read(IO *io) { io->buf[fread(io->buf, 1, IO_CAP - 4, stdin)] = 0; }
void io_write(IO *io) { fwrite(io->buf, 1, (io->at - io->buf), stdout); }

typedef struct {
  I64 N;
  I64 *scores;
} Test;
#define AT(row, col) (test.scores[((row) * test.N + (col))])

int main(int argc, char **argv)
{
  Size heap_cap = (1ll << 30);
  void *heap = malloc(heap_cap);
  Arena arena = (Arena){heap, heap + heap_cap};

  IO *reader = io_init(&arena);
  io_read(reader);
  I64 T = expect_i64(reader);
  while (T--) {
    Arena temp = arena;
    Test test = {0}; {
      test.N = expect_i64(reader);
      test.scores = new(&temp, typeof(*test.scores), test.N * test.N);
      for (Size row = 0; row < test.N; row++) {
        for (Size col = 0; col < test.N; col++) {
          AT(row, col) = expect_i64(reader);
        }
      }
    }
    { // solve
      #define dp(row, col) _dp[((row) * test.N) + (col)]
      I64 *_dp = new(&temp, I64, test.N * (test.N + 1)) + test.N;
      for (Size i = 1; i < test.N; i++) {
        dp(0, i) = dp(0, i - 1) + AT(0, i);
        dp(i, 0) = dp(i - 1, 0) + AT(i, 0);
      }
      for (Size row = 1; row < test.N; row++) {
        for (Size col = 1; col < test.N; col++) {
          I64 above = dp(row - 1, col) + AT(row, col);
          I64 left = dp(row, col - 1) + AT(row, col);
          dp(row, col) = (above > left) ? above : left;
        }
      }
      if (dp(test.N - 1, test.N - 1) < 0) {
        printf("Bad Judges\n");
      } else {
        int k = (test.N - 1) * 2 - 1; // number of times judged
        double avg = (double)(dp(test.N - 1, test.N - 1));
        printf("%f\n", (k != 0) ? (avg / (double)k) : avg);
      }
    }
  }
  free(heap);
  return 0;
}

/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
