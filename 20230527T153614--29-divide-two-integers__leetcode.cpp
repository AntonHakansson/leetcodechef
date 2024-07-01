#include <climits>
#include <cstdio>
#include <iostream>
#include <limits.h>
#include <stack>
#include <string.h>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

using namespace std;

class Solution {
public:
  int divide(int dividend, int divisor) {
    if (dividend == INT_MIN && divisor == 1) {
      return INT_MIN;
    } else if ((dividend == INT_MAX && divisor == 1) ||
               (dividend == INT_MIN && divisor == -1)) {
      return INT_MAX;
    }

    int sign = ((dividend < 0) ^ (divisor < 0)) ? -1 : 1;
    unsigned int div = abs(dividend);
    long long dvs = abs(divisor);

    unsigned int count = 0;
    while (div >= dvs) {
      unsigned int bit = 0;
      while (div >= (dvs << bit)) {
        bit += 1;
      }
      bit = bit - 1;

      unsigned int dvs2 = (dvs << bit);
      {
        long long overflow_check = (long)div - dvs2;
        if (overflow_check <= INT_MIN)
          return INT_MIN;
        if (overflow_check >= INT_MAX)
          return INT_MAX;
      }
      div -= dvs2;
      count += (1 << bit);
    }

    return count * sign;
  }
};

int main(int argc, char **argv) {
  Solution sol;
  {
    cout << INT_MIN << ", " << INT_MAX << endl;
    cout << sol.divide(10, 3) << endl;
    cout << sol.divide(7, -3) << endl;
    cout << sol.divide(0, 3) << endl;
    cout << sol.divide(-2147483648, -1) << endl;
    cout << sol.divide(2147483647, 1) << endl;
    cout << sol.divide(-2147483648, 2) << endl;
  }
}
