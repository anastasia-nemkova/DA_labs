#include <iostream>
#include <vector>
#include <cmath>
#include <climits>
#include <chrono>

int main() {
    int n, p;
    unsigned int m;
    std::cin >> n >> p >> m;
    int st = n - 1;
    std::vector<int> nominals(n);
    int num = 1;
    for(int i = 0; i < n; ++i) {
        nominals[i] = num;
        num *= p; 
    }

    // auto start = std::chrono::high_resolution_clock::now();

    std::vector<int> counts(n, 0);
    for(int i = st; i >= 0; --i) {
        while (m >= nominals[i]) {
            m = m - nominals[i];
            ++counts[i];
        }
    }

    // auto end = std::chrono::high_resolution_clock::now();
    // std::cout << "Time: " << std::chrono::duration<double, std::milli>(end - start).count() << " ms\n";

    for(int i = 0; i < n; ++i) {
        std::cout << counts[i] << std::endl;
    }
}





// int main() {
//     int N, p;
//     long long M;
//     std::cin >> N >> p >> M;

//     std::vector<long long> denominations(N);
//     for (int i = 0; i < N; ++i) {
//         denominations[i] = pow(p, i);
//     }

//     std::vector<int> current(N, 0);
//     std::vector<int> best_combination(N, 0);
//     int min_coins = INT_MAX;

//     auto start = std::chrono::high_resolution_clock::now();
//     while (true) {
//         long long sum = 0;
//         int coin_count = 0;
//         for (int i = 0; i < N; ++i) {
//             sum += current[i] * denominations[i];
//             coin_count += current[i];
//         }

//         if (sum == M && coin_count < min_coins) {
//             min_coins = coin_count;
//             best_combination = current;
//         }

//         int idx = 0;
//         while (idx < N) {
//             current[idx]++;
//             if (current[idx] * denominations[idx] <= M) {
//                 break;
//             } else {
//                 current[idx] = 0;
//                 idx++;
//             }
//         }

//         if (idx == N) {
//             break;
//         }
//     }

//     auto end = std::chrono::high_resolution_clock::now();
//     std::cout << "Time: " << std::chrono::duration<double, std::milli>(end - start).count();

//     for (int i = 0; i < N; ++i) {
//         std::cout << best_combination[i] << std::endl;
//     }

//     return 0;
// }
