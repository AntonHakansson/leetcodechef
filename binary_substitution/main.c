/*
#+title: Codechef: Binary Substituition
#+ref: https://www.codechef.com/practice/course/dynamic-programming/INTDP01/problems/BINARYSUB
#+licence: This is free and unencumbered software released into the public domain.
#+author: anton@hakanssn.com

Chef has a binary string \(S\). He can replace any occurrence of -

- \(01\) with \(a\)
- \(10\) with \(b\)
- \(010\) with \(ab\)
- \(101\) with \(ba\)

While doing these operations, Chef noticed that he can end up with different strings
depending upon the order of application of the operations. Given the final string
containing only \(a\) and \(b\), Chef wants to know the number of possible strings he
might have began with.

As the number of initial strings can be large, print the result modulo \(998244353\).

** Input Format

- The first line of input will contain a single integer \(T\), denoting the number of test
  cases.
- Each test case consists of a single line of input denoting \(S\), the final string
  obtained after applying the operations.

** Output Format

For each test case, output the number of original strings that can result in the final string mod \(998244353\).

** Constraints

- \(1 \leq T \leq 5 \cdot 10^{4}\)
- \(1 \leq \mid S\mid \leq 10^{5}\)
- The sum of \(\mid S\mid\) over all test cases won't exceed \(5 \cdot 10^{5}\).
- \(S\) consists of \(a\) and \(b\) only.

** Sample 1:

Input //

Output //

#+begin_example
3
ab
aa
abb
#+end_example

#+begin_example
2
1
2
#+end_example

** Explanation:

*Test case \(1\):* The binary strings that can result in the string \(ab\) are \(0110\) and \(010\).

- \(0110\): Replace the first two characters \(01\) with \(a\) and last two characters \(10\) with \(b\). Thus, we get \(ab\).
- \(010\): Replace the characters \(010\) with \(ab\).

*Test case \(2\):* The only binary string that can result in the string \(aa\) is \(0101\). In \(0101\), we replace the first two characters with \(a\) and last two characters with \(a\) as well.

*Test case \(3\):* The binary strings that can result in the string \(abb\) are \(011010\) and \(01010\).

- \(011010\): Replace the first two characters \(01\) with \(a\), next two characters \(10\) with \(b\), and last two characters \(10\) with \(b\). Thus, we get \(abb\).
- \(01010\): Replace the characters \(010\) with \(ab\) and the characters \(10\) with \(b\).

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

typedef struct { Byte *beg, *end; }   Arena;
typedef struct { U8 *buf; Size len; } Str;
typedef struct { Str head, tail; }    Cut;

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

#define I64_MAX (9223372036854775807)
static I64 str_i64(Str s)
{
  I64 r = 0;
  Size i = 0;
  switch(s.buf[i]) {
  case 0x30: case 0x31: case 0x32: case 0x33: case 0x34:
  case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
    do {
      I64 v = s.buf[i++] - '0';
      assert(v >= 0 && v <= 9); // invalid character
      assert(r <= (I64_MAX/10)); // toolarge
      r *= 10;
      assert((I64_MAX - v) >= r); // toolarge
      r += v;
    } while(i < s.len);
    /* assert(r >= I64_MIN); // toosmall */
    return r;
  case 0x2d:
    return -str_i64((Str){s.buf + 1, s.len - 1});
  }
  assert(0 && "unreachable"); // invalid
  return 0;
}

static Cut cut(Str s, U8 delim)
{
  Size head_len = 0;
  for (; (head_len < s.len) && (s.buf[head_len] != delim); head_len++) {}
  Str head = { s.buf, head_len };
  Str tail = {0}; {
    Size one_past_delim = (head_len == s.len) ? head_len : head_len + 1;
    tail.buf = s.buf + one_past_delim;
    tail.len = s.len - one_past_delim;
  }
  return (Cut){ .head = head, .tail = tail };
}

////////////////////////////////////////////////////////////////////////////////
//- Solution
#include <stdlib.h>
#include <stdio.h>

typedef struct {
  Str s;
} Test;

#define MODULO (998244353)
static void solve(Arena arena, Test test)
{
  U64 *dp = new (&arena, U64, test.s.len + 1);
  dp++;
  dp[-1] = 1;
  dp[0]  = 1;
  for (Size i = 1; i < test.s.len; i++) {
    U64 substituitions = 0;
    B32 is_ab = (test.s.buf[i - 1] == 'a') && (test.s.buf[i] == 'b');
    B32 is_ba = (test.s.buf[i - 1] == 'b') && (test.s.buf[i] == 'a');
    if (is_ab || is_ba) {
      substituitions = dp[i - 2];
    }
    dp[i] = (dp[i - 1] + substituitions) % MODULO;
  }
  printf("%lu\n", dp[test.s.len - 1]);
}

int main(int argc, char **argv)
{
  Size heap_cap = (1ll << 30);
  void *heap = malloc(heap_cap);
  Arena arena = (Arena){heap, heap + heap_cap};

  Size input_chunk_size = (1ll << 20);
  Str input = {0}; {
    input.buf = new(&arena, U8, input_chunk_size);
    input.len = fread(input.buf, 1, input_chunk_size, stdin);
    assert(input.len < input_chunk_size && "did not read whole input");
    return 500;
  }

  Cut line = cut(input, '\n');
  I64 T = str_i64(line.head);
  while (T-- > 0) {
    Arena temp = arena;
    line = cut(line.tail, '\n');
    Test test = { .s = line.head };
    solve(temp, test);
  }
  free(heap);
  return 0;
}

/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
