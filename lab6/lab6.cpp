#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>

class TBigInt {
private:
    const static int64_t LEN = 4;
    const static int64_t MAX_DIGIT = 10000;
    std::vector<int64_t> numbers;
public:
    TBigInt() = default;
    TBigInt(const std::string& str) {
        int size = str.size();
        for(int i = size - 1; i >= 0; i -= TBigInt::LEN) {
            if(i < TBigInt::LEN) {
                numbers.push_back(atoi(str.substr(0, i + 1).c_str()));
            } else {
                numbers.push_back(atoi(str.substr(i - TBigInt::LEN + 1, TBigInt::LEN).c_str()));
            }
        }
        DeleteZeros();
    }

    void DeleteZeros() {
        while(!numbers.empty() && numbers.back() == 0) {
            numbers.pop_back();
        }
    }

    friend TBigInt operator+ (const TBigInt& num1, const TBigInt& num2) {
        TBigInt result;
        int64_t carry = 0;
        int maxSize = std::max(num1.numbers.size(), num2.numbers.size());
        result.numbers.resize(maxSize);
        for(int i = 0; i < maxSize; ++i) {
            int64_t sum = carry;
            if(i < num1.numbers.size()) {
                sum += num1.numbers[i];
            }
            if(i < num2.numbers.size()) {
                sum += num2.numbers[i];
            }
            carry = sum / TBigInt::MAX_DIGIT;
            result.numbers[i] = sum % TBigInt::MAX_DIGIT;
        }
        if (carry != 0) {
            result.numbers.push_back(carry);
        }
        result.DeleteZeros();
        return result;
    }

    friend TBigInt operator- (const TBigInt& num1, const TBigInt& num2) {
        TBigInt result;
        result.numbers.resize(num1.numbers.size());
        int64_t borrow = 0;
        for (size_t i = 0; i < num1.numbers.size(); ++i) {
            int64_t diff = 0;
            if (i < num2.numbers.size()) {
                diff = num1.numbers[i] - num2.numbers[i] - borrow;
            } else {
                diff = num1.numbers[i] - borrow;
            }
            if (diff < 0) {
                diff += TBigInt::MAX_DIGIT;
                borrow = 1;
            } else {
                borrow = 0;
            }
            result.numbers[i] = diff;
        }

        result.DeleteZeros();
        return result;
    }


    friend TBigInt operator* (const TBigInt& num1, const TBigInt& num2) {
        TBigInt result;
        result.numbers.resize(num1.numbers.size() + num2.numbers.size());

        for (int i = 0; i < num1.numbers.size(); ++i) {
            int64_t carry = 0;
            for(int j = 0; j <= num2.numbers.size(); ++j) {
                int64_t cur = 0;
                if(j < num2.numbers.size()) {
                    cur = result.numbers[i + j] + num1.numbers[i] * num2.numbers[j] + carry;
                } else {
                    cur = result.numbers[i + j] + carry;
                }
                carry = cur / TBigInt::MAX_DIGIT;
                result.numbers[i + j] = cur % TBigInt::MAX_DIGIT;
            }
        }
        result.DeleteZeros();
        return result;
    }

    friend TBigInt operator/ (const TBigInt& num1, const TBigInt& num2) {
        TBigInt result("0"), remainder("0");
        result.numbers.resize(num1.numbers.size());

        for (int i = num1.numbers.size() - 1; i >= 0; --i) {
            remainder.numbers.insert(remainder.numbers.begin(), num1.numbers[i]);
            remainder.DeleteZeros(); 

            int64_t left = 0;
            int64_t right = TBigInt::MAX_DIGIT;
            int64_t x = 0;

            while (left <= right) {
                int64_t mid = (left + right) / 2;
                TBigInt midVal = num2 * TBigInt(std::to_string(mid));
                if (midVal < remainder || midVal == remainder) {
                    x = mid;
                    left = mid + 1;
                } else {
                    right = mid - 1;
                }
            }

            result.numbers[i] = x;
            remainder = remainder - num2 * TBigInt(std::to_string(x));
        }

        result.DeleteZeros();
        return result;
    }


    friend TBigInt operator^ (TBigInt num1, TBigInt num2) {
        TBigInt result("1");
        while(num2 > TBigInt("0")) {
            if(num2.numbers[0] % 2 == 0) {
                num2 = num2 / TBigInt("2");
                num1 = num1 * num1;
            } else {
                num2 = num2 - TBigInt("1");
                result = result * num1;
            }
        }
        return result;
    }

    friend bool operator== (const TBigInt& num1, const TBigInt& num2) {
        if(num1.numbers.size() != num2.numbers.size()) {
            return false;
        }
        for(int i = 0; i < num1.numbers.size(); ++i) {
            if(num1.numbers[i] != num2.numbers[i]) {
                return false;
            }
        }
        return true;
    }

    friend bool operator> (const TBigInt& num1, const TBigInt& num2) {
        if(num1.numbers.size() != num2.numbers.size()) {
            return num1.numbers.size() > num2.numbers.size();
        }
        for(int i = 0; i < num1.numbers.size(); ++i) {
             if(num1.numbers[num1.numbers.size() - i - 1] != num2.numbers[num2.numbers.size() - i - 1]) {
                return num1.numbers[num1.numbers.size() - i - 1] > num2.numbers[num1.numbers.size() - i -1];
            }
        }
        return false;
    }

    friend bool operator< (const TBigInt& num1, const TBigInt& num2) {
        if(num1.numbers.size() != num2.numbers.size()) {
            return num1.numbers.size() < num2.numbers.size();
        }
        for(int i = 0; i < num1.numbers.size(); ++i) {
             if(num1.numbers[num1.numbers.size() - i - 1] != num2.numbers[num2.numbers.size() - i - 1]) {
                return num1.numbers[num1.numbers.size() - i - 1] < num2.numbers[num1.numbers.size() - i -1];
            }
        }
        return false;
    }


    friend std::istream& operator>> (std::istream& in,TBigInt& num) {
        std::string str;
        in >> str;
        num = TBigInt(str);
        return in;
    }

    friend std::ostream& operator<< (std::ostream& out, const TBigInt& num) {
        if(num.numbers.empty()) {
            out << "0";
            return out;
        }
        out << num.numbers.back();
        for(int64_t i = num.numbers.size() - 2; i >= 0; --i) {
            out << std::setfill('0') << std::setw(TBigInt::LEN) << num.numbers[i];
        }
        return out;
    }
};

int main() {
    TBigInt num1, num2;
    char sign;
    
    while (std::cin >> num1 >> num2 >> sign) {
        if (sign == '+') {
            std::cout << num1 + num2 << std::endl;
        }
        if (sign == '-') {
            if (num1 < num2) {
                std::cout << "Error" << std::endl;
            } else {
                std::cout << num1 - num2 << std::endl;
            }
        }
        if (sign == '*') {
            std::cout << num1 * num2 << std::endl;
        }
        if (sign == '/') {
            if(num2 == TBigInt("0")) {
                std::cout << "Error" << std::endl;
            } else {
                std::cout << num1 / num2 << std::endl;
            }
        }
        if (sign == '^') {
            if (num1 == TBigInt("0") && num2 == TBigInt("0")) {
                std::cout << "Error" << std::endl;
            } else {
                std::cout << (num1 ^ num2) << std::endl;
            }
        }
        if (sign == '<') {
            if (num1 < num2) {
                std::cout << "true" << std::endl;
            } else {
                std::cout << "false" << std::endl;
            }
        }
        if (sign == '>') {
            if (num1 > num2) {
                std::cout << "true" << std::endl;
            } else {
                std::cout << "false" << std::endl;
            }
        }
        if (sign == '=') {
            if (num1 == num2) {
                std::cout << "true" << std::endl;
            } else {
                std::cout << "false" << std::endl;
            }
        }
    }
    return 0;
}