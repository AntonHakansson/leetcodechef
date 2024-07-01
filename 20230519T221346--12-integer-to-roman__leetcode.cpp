#include <iostream>
#include <string.h>
#include <unordered_map>

using namespace std;

class Solution {
public:
  string intToRoman(int num) {
    //                    1    2     3      4     5    6     7      8       9
    string thnd[] = {"", "M", "MM", "MMM"};
    string hund[] = {"",  "C",  "CC",  "CCC",  "CD",
                     "D", "DC", "DCC", "DCCC", "CM"};
    string tens[] = {"",  "X",  "XX",  "XXX",  "XL",
                     "L", "LX", "LXX", "LXXX", "XC"};
    string ones[] = {"",  "I",  "II",  "III",  "IV",
                     "V", "VI", "VII", "VIII", "IX"};
    return thnd[num / 1000] + hund[(num % 1000) / 100] +
           tens[(num % 100) / 10] + ones[(num % 10) / 1];
  }

  string intToRomanStupid(int num) {
    string result = "";
    // I am very smart
    while (num >= 1000) {
      result += "M";
      num -= 1000;
    }
    if (num >= 900) {
      result += "CM";
      num -= 900;
    }
    if (num >= 500) {
      result += "D";
      num -= 500;
    }
    if (num >= 400) {
      result += "CD";
      num -= 400;
    }
    while (num >= 100) {
      result += "C";
      num -= 100;
    }
    if (num >= 90) {
      result += "XC";
      num -= 90;
    }
    if (num >= 50) {
      result += "L";
      num -= 50;
    }
    if (num >= 40) {
      result += "XL";
      num -= 40;
    }
    while (num >= 10) {
      result += "X";
      num -= 10;
    }
    if (num >= 9) {
      result += "IX";
      num -= 9;
    }
    if (num >= 5) {
      result += "V";
      num -= 5;
    }
    if (num >= 4) {
      result += "IV";
      num -= 4;
    }
    while (num >= 1) {
      result += "I";
      num -= 1;
    }

    return result;
  }
};

int main(int argc, char **argv) {
  Solution sol;
  cout << sol.intToRoman(3) << endl;
  ;
  cout << sol.intToRoman(58) << endl;
  ;
  cout << sol.intToRoman(1994) << endl;
  ;
  cout << sol.intToRoman(20) << endl;
  ;
}
