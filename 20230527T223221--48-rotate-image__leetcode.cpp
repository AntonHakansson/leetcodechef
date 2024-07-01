#include <algorithm>
#include <cstring>
#include <iostream>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

using namespace std;

class Solution {

public:
  // https://leetcode.com/problems/rotate-image/
  void rotate(vector<vector<int>> &matrix) {
    const int N = matrix.size();

    int I = 0;
    while (I < N - 1) {
      for (int bias = 0; bias < N - I * 2 - 1; bias++) {
        int row = I;
        int col = I + bias;
        int prev_val = matrix[N - 1 - I - bias][I];

        for (int rot_idx = 0; rot_idx < 4; rot_idx++) {
          // row,        col
          // 0: I,            I + bias
          // 1: I + bias,     N - I
          // 2: N - I,        N - I - bias
          // 3: N - I - bias, I

          // swap
          int tmp = matrix[row][col];
          matrix[row][col] = prev_val;
          prev_val = tmp;

          // rotate
          int dest_row = col;
          int dest_col = N - 1 - row;
          row = dest_row;
          col = dest_col;
        }
      }

      I += 1;
    }
  }
};

int main(int argc, char **argv) { Solution sol; }
