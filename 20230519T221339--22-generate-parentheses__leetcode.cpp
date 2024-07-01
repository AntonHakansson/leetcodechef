#include <cstdio>
#include <iostream>
#include <stack>
#include <string.h>
#include <unordered_map>
#include <vector>

using namespace std;

class Solution {
public:
  vector<string> generateParenthesis(int n) {
    vector<string> result;
    char path[n * 2];

    struct rstate {
      int len;
      int opened;
      char paren;
    };
    vector<rstate> rstack = {};

    rstack.push_back(rstate{1, 1, '('});

    while (!rstack.empty()) {
      rstate s = rstack.back();
      rstack.pop_back();

      path[s.len - 1] = s.paren;

      if (s.len == n * 2) {
        result.push_back(string(path, s.len));
      } else {
        auto closed = (s.len - s.opened);
        if (closed < s.opened) {
          rstack.push_back(rstate{s.len + 1, s.opened, ')'});
        }
        if (s.opened < n) {
          rstack.push_back(rstate{s.len + 1, s.opened + 1, '('});
        }
      }
    }

    return result;
  }
};

int main(int argc, char **argv) {
  Solution sol;
  {
    auto res = sol.generateParenthesis(3);
    for (auto i : res) {
      cout << i << '\n';
    }
    cout << "------------------" << '\n';
  }
  {
    auto res = sol.generateParenthesis(5);
    for (auto i : res) {
      cout << i << '\n';
    }
    cout << "------------------" << '\n';
  }
}
