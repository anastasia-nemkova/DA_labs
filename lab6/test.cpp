#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <gmp.h>
#include <cstdlib>

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

std::string GenerateRandomNumber(int size) {
    std::string num = "";
    num += std::to_string(rand() % 9 + 1);
    for (int i = 1; i < size; ++i) {
        num += std::to_string(rand() % 10);
    }
    return num;
}

void TestTBigIntOperations(const std::vector<std::string>& num1_list, const std::vector<std::string>& num2_list) {
    std::vector<TBigInt> numbers1, numbers2;
    for (const auto& num : num1_list) {
        numbers1.push_back(TBigInt(num));
    }
    for (const auto& num : num2_list) {
        numbers2.push_back(TBigInt(num));
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numbers1.size(); ++i) {
        TBigInt sum = numbers1[i] + numbers2[i];
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "TBigInt Addition: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numbers1.size(); ++i) {
        TBigInt diff = numbers1[i] - numbers2[i];
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "TBigInt Subtraction: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numbers1.size(); ++i) {
        TBigInt prod = numbers1[i] * numbers2[i];
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "TBigInt Multiplication: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numbers1.size(); ++i) {
        if (!(numbers2[i] == TBigInt("0"))) {
            TBigInt div = numbers1[i] / numbers2[i];
        }
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "TBigInt Division: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";
}

void TestGmpOperations(const std::vector<std::string>& num1_list, const std::vector<std::string>& num2_list) {
    std::vector<mpz_t> numbers1(num1_list.size()), numbers2(num2_list.size());
    
    for (size_t i = 0; i < num1_list.size(); ++i) {
        mpz_init_set_str(numbers1[i], num1_list[i].c_str(), 10);
        mpz_init_set_str(numbers2[i], num2_list[i].c_str(), 10);
    }

    mpz_t result;
    mpz_init(result);

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numbers1.size(); ++i) {
        mpz_add(result, numbers1[i], numbers2[i]);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "GMP Addition: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numbers1.size(); ++i) {
        mpz_sub(result, numbers1[i], numbers2[i]);
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "GMP Subtraction: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numbers1.size(); ++i) {
        mpz_mul(result, numbers1[i], numbers2[i]);
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "GMP Multiplication: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";

    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < numbers1.size(); ++i) {
        if (mpz_cmp_ui(numbers2[i], 0) != 0) {
            mpz_tdiv_q(result, numbers1[i], numbers2[i]);
        }
    }
    end = std::chrono::high_resolution_clock::now();
    std::cout << "GMP Division: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";

    mpz_clear(result);
    for (size_t i = 0; i < numbers1.size(); ++i) {
        mpz_clear(numbers1[i]);
        mpz_clear(numbers2[i]);
    }
}

int main() {

    srand(time(0));
    std::vector<int> lengths = {10, 100, 1000};
    std::vector<int> dataset_sizes = {100, 500, 1000};

    for (int length : lengths) {
        std::cout << "\nTesting with number length: " << length << "\n";
        for (int dataset_size : dataset_sizes) {
            std::cout << "Dataset size: " << dataset_size << "\n";

            std::vector<std::string> num1_list, num2_list;
            for (int i = 0; i < dataset_size; ++i) {
                num1_list.push_back(GenerateRandomNumber(length));
                num2_list.push_back(GenerateRandomNumber(length));
            }

            std::cout << "\nTBigInt Performance:\n";
            TestTBigIntOperations(num1_list, num2_list);

            std::cout << "\nGMP Performance:\n";
            TestGmpOperations(num1_list, num2_list);
        }
    }
    return 0;
}