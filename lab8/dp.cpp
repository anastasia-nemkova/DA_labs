#include <iostream>
#include <vector>
#include <climits>

int main() {
    int n;
    unsigned int m;
    std::cin >> n >> m;
    std::vector<int> nominals(n);

    for (int i = 0; i < n; ++i) {
        std::cin >> nominals[i];
    }
    
    std::vector<int> dp(m + 1, INT_MAX);
    std::vector<int> coins(m + 1, -1);
    dp[0] = 0;

    for (unsigned int i = 1; i <= m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (nominals[j] <= i && dp[i - nominals[j]] != INT_MAX) {
                 if (dp[i] > dp[i - nominals[j]] + 1) {
                    dp[i] = dp[i - nominals[j]] + 1;
                    coins[i] = j;
                }
            }
        }
    }

    std::vector<int> counts(n, 0);
    while (m > 0) {
        int ind = coins[m];
        if (ind == -1) {
            break;
        }
        counts[ind]++;
        m -= nominals[ind];
    }

    for (int i = 0; i < n; ++i) {
        std::cout << counts[i] << std::endl;
    }

    return 0;
}