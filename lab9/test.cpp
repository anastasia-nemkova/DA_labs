#include <iostream>
#include <vector>
#include <limits>
#include <queue>
#include <chrono>
#include <set>
#include <cstdlib>
#include <ctime> 

const long long INF = std::numeric_limits<long long>::max();


void dfs(int node, long long current_distance, std::vector<std::vector<std::pair<int, long long>>>& graph, std::vector<long long>& distance) {
    if (current_distance >= distance[node]) {
        return;
    }

    distance[node] = current_distance;

    for (auto& edge : graph[node]) {
        int neighbor = edge.first;
        long long weight = edge.second;
        dfs(neighbor, current_distance + weight, graph, distance);
    }
}


// Алгоритм Дейкстры
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
    std::cout << distance[n] << '\n';
}


std::vector<std::vector<std::pair<int, long long>>> generateRandomGraph(int n, int m, int maxWeight) {
    std::vector<std::vector<std::pair<int, long long>>> graph(n + 1);
    std::set<std::pair<int, int>> edges; 
    std::srand(std::time(0)); 

    int edgeCount = 0;

    for (int i = 1; i < n; ++i) {
        long long weight = std::rand() % maxWeight + 1;
        graph[i].push_back({i + 1, weight});
        graph[i + 1].push_back({i, weight});
        edges.insert({i, i + 1});
        edges.insert({i + 1, i});
        edgeCount++;
    }

    while (edgeCount < m) {
        int u = std::rand() % n + 1;
        int v = std::rand() % n + 1;

        if (u != v && edges.find({u, v}) == edges.end() && edges.find({v, u}) == edges.end()) {
            long long weight = std::rand() % maxWeight + 1;

            graph[u].push_back({v, weight});
            graph[v].push_back({u, weight});

            edges.insert({u, v});
            edges.insert({v, u});
            edgeCount++;
        }
    }

    return graph;
}


int main() {
    int n = 10000;
    int maxWeight = 100;
    int start = 1;

    for (int m = 10; m <= 100000; m *= 10) {
        std::cout << "Testing with m = " << m << " edges\n";

        std::vector<std::vector<std::pair<int, long long>>> graph = generateRandomGraph(n, m, maxWeight);

        std::vector<long long> distanceNaive(n + 1, INF);
        std::vector<long long> distanceDijkstra(n + 1, INF);

        auto startTimeDijkstra = std::chrono::high_resolution_clock::now();
        dijkstra(n, start, graph, distanceDijkstra);
        auto endTimeDijkstra = std::chrono::high_resolution_clock::now();

        double durationDijkstra = std::chrono::duration<double>(endTimeDijkstra - startTimeDijkstra).count();
        std::cout << "Dijkstra: " << durationDijkstra << " seconds\n";

        auto startTimeNaive = std::chrono::high_resolution_clock::now();
        dfs(start, 0, graph, distanceNaive);
        auto endTimeNaive = std::chrono::high_resolution_clock::now();

        double durationNaive = std::chrono::duration<double>(endTimeNaive - startTimeNaive).count();
        std::cout << "DFS (Naive): " << durationNaive << " seconds\n";

        std::cout << distanceNaive[n] << '\n';
        std::cout << "----------------------------------------\n";
    }

    return 0;
}
