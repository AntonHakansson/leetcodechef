#include <string>
#include <iostream>
#include <cmath>

class Solution {
public:
    int myAtoi(std::string s) {
        if (s.size() == 0) return 0;

        int at = 0;
        bool positive = true;

        while(s[at] == ' ') { at += 1; }
        if (s[at] == '-') {
            positive = false;
            at += 1;
        }
        else if (s[at] == '+') {
            at += 1;
        }

        unsigned long result = 0;
        auto limit = pow(2, 31) - int(positive);
        while (at < s.size() && isDigit(s[at])) {
            result *= 10;
            result += (s[at] - '0');
            if (result >= limit) {
                return positive ? limit : -limit;
            }
            at += 1;
        }


        return positive ? result : -result;
    }
private:
        bool isDigit(char c) {
            return c >= '0' && c <= '9';
        }
};

int main(int argc, char *argv[]) {
    Solution sol;

    std::cout << sol.myAtoi("2147483646") << "==2147483646\n"
              << sol.myAtoi("-91283472332") << "==-2147483648\n"
              << sol.myAtoi("21474836460") << "==2147483647\n"
              << sol.myAtoi("-6147483648") << "==-2147483648\n"
              << std::endl;

    return 0;
}
