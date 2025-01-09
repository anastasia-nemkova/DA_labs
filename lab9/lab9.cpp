#include <iostream>
#include <vector>
#include <set>
#include <limits>

const long long INF = std::numeric_limits<long long>::max();

void dijkstra(int n, int start, std::vector<std::vector<std::pair<int, long long>>>& graph, std::vector<long long>& distance) {
    distance[start] = 0;
    std::set<std::pair<long long, int>> s;
    s.insert({0, start});

    while (!s.empty()) {
        long long w = s.begin()->first;
        int u = s.begin()->second;
        s.erase(s.begin());

        for (auto& edge: graph[u]) {
            int v = edge.first;
            long long w1 = edge.second;
            
            if (w1 + distance[u] < distance[v]) {

                if (distance[v] != INF) {
                    s.erase({distance[v], v});
                }

                distance[v] = w1 + distance[u];
                s.insert({distance[v], v});
            }
        }
    }
}

int main() {
    int n, m, start, finish;
    std::cin >> n >> m >> start >> finish;

    std::vector<std::vector<std::pair<int, long long>>> graph(n + 1);

    for (int i = 0; i < m; ++i) {
        int u, v;
        long long w;
        std::cin >> u >> v >> w;
        graph[u].push_back({v, w});
        graph[v].push_back({u, w});
    }

    std::vector<long long> distance(n + 1, INF);

    dijkstra(n, start, graph, distance);

    if (distance[finish] == INF) {
        std::cout << "No solution" << std::endl;
    } else {
        std::cout << distance[finish] << std::endl;
    }

    return 0;
}
