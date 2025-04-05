/*
#+title: D - 756
#+ref: https://atcoder.jp/contests/abc114/tasks/abc114_d
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

static I64 primes[] = { 2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733,739,743,751,757,761,769,773,787,797,809,811,821,823,827,829,839,853,857,859,863,877,881,883,887,907,911,919,929,937,941,947,953,967,971,977,983,991,997,1009,1013,1019,1021,1031,1033,1039,1049,1051,1061,1063,1069,1087,1091,1093,1097,1103,1109,1117,1123,1129,1151,1153,1163,1171,1181,1187,1193,1201,1213,1217,1223,1229,1231,1237,1249,1259,1277,1279,1283,1289,1291,1297,1301,1303,1307,1319,1321,1327,1361,1367,1373,1381,1399,1409,1423,1427,1429,1433,1439,1447,1451,1453,1459,1471,1481,1483,1487,1489,1493,1499,1511,1523,1531,1543,1549,1553,1559,1567,1571,1579,1583,1597,1601,1607,1609,1613,1619,1621,1627,1637,1657,1663,1667,1669,1693,1697,1699,1709,1721,1723,1733,1741,1747,1753,1759,1777,1783,1787,1789,1801,1811,1823,1831,1847,1861,1867,1871,1873,1877,1879,1889,1901,1907,1913,1931,1933,1949,1951,1973,1979,1987,1993,1997,1999,2003,2011,2017,2027,2029,2039,2053,2063,2069,2081,2083,2087,2089,2099,2111,2113,2129,2131,2137,2141,2143,2153,2161,2179,2203,2207,2213,2221,2237,2239,2243,2251,2267,2269,2273,2281,2287,2293,2297,2309,2311,2333,2339,2341,2347,2351,2357,2371,2377,2381,2383,2389,2393,2399,2411,2417,2423,2437,2441,2447,2459,2467,2473,2477,2503,2521,2531,2539,2543,2549,2551,2557,2579,2591,2593,2609,2617,2621,2633,2647,2657,2659,2663,2671,2677,2683,2687,2689,2693,2699,2707,2711,2713,2719,2729,2731,2741,2749,2753,2767,2777,2789,2791,2797,2801,2803,2819,2833,2837,2843,2851,2857,2861,2879,2887,2897,2903,2909,2917,2927,2939,2953,2957,2963,2969,2971,2999, };

static void run(Arena arena, Stream *reader, Stream *writer) {
  Size N = read_i64(reader);

  // Ex. 10! -> 1 shichi-go. (362880 = 2^4 * 3^4 * 5^2 because (4 + 1) x (4 + 1) x (2 + 1) = 75)

  // Given number with prime factorisation 
  //   p1^a x p2^b x ... x pn^c
  // the number of divisors are (a + 1) x (b + 1) x ... x (c + 1)
  // Solve for 75 = (a + 1) x (b + 1) x ... x (c + 1)

  // exponents(i) :: exponents for prime factor p_i that constitute N!
  I64 exponents_count = N / 2 + 1; // (I64)sqrtl(N) + 1;
  I64 *exponents = new(&arena, I64, exponents_count);
  for (Size fac = 2; fac <= N; fac++) {
    for (Size j = 0; (j < exponents_count); j++) {
      Size cur = fac;
      while ((cur % primes[j]) == 0) {
        exponents[j] += 1;
        cur = cur / primes[j];
      }
    }
  }
  // So if we can find a divisor X whose prime factorization has exponents e_1, ..., e_n s.t. (e_1 + 1) (e_2 + 1)  ... =  75, then X is shichi-go.
  // Solving for e_i we get four cases:
  //   case 0: Ei e_i=74
  //   case 1: Ei Ej e_i=2, e_j=24
  //   case 2: Ei Ej e_i=4, e_j=14
  //   case 3: Ei Ej Ek e_i=2, e_j=4, e_k=4
  Size n75 = 0;
  Size n25 = 0;
  Size n15 = 0;
  Size n5 = 0;
  Size n3 = 0;
  for (Size i = 0; i < exponents_count; i++) {
    n75 += (exponents[i] >= 74);
    n25 += (exponents[i] >= 24);
    n15 += (exponents[i] >= 14);
    n5  += (exponents[i] >= 4);
    n3  += (exponents[i] >= 2);
  }
  Size r = 0;
  r += n75;              // case 0
  r += n25 * (n3 - 1);   // case 1
  r += n15 * (n5 - 1);   // case 2
  r += ((n5 * (n5 - 1)) / 2) * (n3 - 2); // case 3
  write_i64(writer, r, '\n');
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

// Platform
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
  Size nbytes = write(1, s->beg, s->at - s->beg);
  s->at = s->beg;
}
static void fill(Stream *s) {
  assert(s->end > s->at);
  Size cap = s->end - s->beg;
  Size unprocessed = (s->at == s->beg) ? 0 : (s->end - s->at);
  s->at = s->beg = __builtin_memmove(s->beg, s->at, unprocessed);
  int nbytes = read(0, s->beg + unprocessed, cap - unprocessed);
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

/* Local Variables: */
/* compile-command: "cc main.c -fsanitize=undefined -g3" */
/* End: */
