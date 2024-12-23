#include <iostream>
#include <vector>
#include <string>
#include <chrono>

// int main() {
//     long long n;
//     int m;

//     std::cin >> n >> m;

//     std::string nStr = std::to_string(n);
//     long long count = 0;
//     auto start = std::chrono::high_resolution_clock::now();
//     for(long long i = m; i < n; i += m) {
//         std::string iStr = std::to_string(i);
//         if(iStr < nStr) {
//             ++count;
//         }
//     }
//     std::cout << count << '\n';
//     auto end = std::chrono::high_resolution_clock::now();
//     std::cout << "Time: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";
// }



long long countNumber(const std::string &n, int m) {
    int length = n.size();
    std::vector<std::vector<std::vector<long long>>> dp(length + 1, std::vector<std::vector<long long>>(m, std::vector<long long>(2, 0)));

    dp[0][0][0] = 1;

    for (int pos = 0; pos < length; pos++) {
        for (int mod = 0; mod < m; mod++) {
            for (int is_less = 0; is_less < 2; is_less++) {
                if (dp[pos][mod][is_less] == 0) continue;

                int limit = is_less ? 9 : (n[pos] - '0');

                for (int d = 0; d <= limit; d++) {
                    if (pos == 0 && d == 0) continue;
                    dp[pos + 1][(mod * 10 + d) % m][is_less || (d < limit)] += dp[pos][mod][is_less];
                }
            }
        }
    }

    long long result = 0;
    for(int len = 1; len <= length; ++len) {
        for(int is_less = 0; is_less < 2; ++is_less) {
            result += dp[len][0][is_less];
        }
    }
    
    return result;
}


int main() {
    std::string n;
    int m;
    std::cin >> n >> m;

    // auto start = std::chrono::high_resolution_clock::now();
    long long ans = countNumber(n, m);
    
    long long number = stoll(n);
    std::cout << ans - (number % m == 0) << std::endl;
    // auto end = std::chrono::high_resolution_clock::now();
    // std::cout << "Time: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";

    return 0;
}
