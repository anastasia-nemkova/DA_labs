#include <iostream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <chrono>

struct Segments {
    int x, h, type;
    Segments(int x, int h, int type) : x(x), h(h), type(type) {}
};

struct Points {
    int x, y, id;
    Points(int x, int y, int id) : x(x), y(y), id(id) {}
};

class PersistentSegmentTree {
private:
    struct Node {
        int value;
        Node* left;
        Node* right;
        Node(int v = 0) : value(v), left(nullptr), right(nullptr) {}
    };

    std::vector<Node*> roots;
    std::vector<int> versionX;
    int size;

    Node* build(int start, int end) {
        Node* node = new Node();
        if (start == end) {
            return node;
        }
        int mid = (start + end) / 2;
        node->left = build(start, mid);
        node->right = build(mid + 1, end);
        return node;
    }

    int query(Node* node, int start, int end, int l, int r) {
        if (!node || r < start || end < l) {
            return 0;
        }
        if (l <= start && end <= r) {
            return node->value;
        }
        int mid = (start + end) / 2;
        return query(node->left, start, mid, l, r) + query(node->right, mid + 1, end, l, r);
    }

    Node* update(Node* node, int start, int end, int ind, int val) {
        Node* newNode = new Node(*node);
        if (start == end) {
            newNode->value += val;
        } else {
            int mid = (start + end) / 2;
            if (ind <= mid) {
                //std::cout << start << ' ' << end << '\n';
                newNode->left = update(node->left, start, mid, ind, val);
            } else {
                newNode->right = update(node->right, mid + 1, end, ind, val);
            }
            newNode->value = newNode->left->value + newNode->right->value;
        }
        return newNode;
    }

    int binarySearch(int x) {
        int low = 0;
        int high = versionX.size() - 1;
        while (low < high) {
            int mid = (low + high + 1) / 2;
            if (versionX[mid] <= x) {
                low = mid;
            } else {
                high = mid - 1;
            }
        }
        return low;
    }

    void printTree(Node* node, int level = 0) {
        if (!node) {
            return;
        }
        printTree(node->right, level + 1);
        std::cout << std::string(level * 4, ' ') << node->value << std::endl;
        printTree(node->left, level + 1);
    }

public:
    PersistentSegmentTree(int size) : size(size) {
        roots.push_back(build(0, size - 1));
        versionX.push_back(-1);
    }

    int query(int x, int l, int r) {
        int version = binarySearch(x);
        //std::cout << version << '\n';
        return query(roots[version], 0, size - 1, l, r);
    }

    void update(int x, int ind, int val) {
        roots.push_back(update(roots.back(), 0, size - 1, ind, val));
        versionX.push_back(x);
        // std::cout << "Версия дерева: " << roots.size() - 1 << "; х: " << x << "; y: " << ind << '\n';
        // printTree(roots.back());
        // std::cout << '\n';
    }
};

bool compareSegments(const Segments& a, const Segments& b) {
    return a.x < b.x;
}

bool comparePoints(const Points& a, const Points& b) {
    return a.x < b.x;
}

void performanceTest(int n, int m) {
    auto start = std::chrono::high_resolution_clock::now(); 

    std::vector<Segments> segment;
    std::vector<Points> point;
    std::vector<int> results(m);

    std::set<int> uniqH;

    for (int i = 0; i < n; ++i) {
        int l = rand() % 100000; 
        int r = l + rand() % 1000;
        int h = rand() % 1000 + 1;
        segment.emplace_back(l, h, 1);
        segment.emplace_back(r + 1, h, -1);
        uniqH.insert(h);
        //std::cout << l << ' ' << r << " " << h << '\n';
    }

    std::map<int, int> compressY;
    int coord = 0;
    for (int y : uniqH) {
        compressY[y] = coord++;
    }

    int maxY = uniqH.size();
    PersistentSegmentTree tree(maxY);

    std::sort(segment.begin(), segment.end(), compareSegments);

    for (auto& seg : segment) {
        tree.update(seg.x, compressY[seg.h], seg.type);
    }

    for (int i = 0; i < m; ++i) {
        int x = rand() % 100000; 
        int y = rand() % 1000;
        point.emplace_back(x, y, i);
        //std::cout << x << ' ' << y << '\n';
    }

    std::sort(point.begin(), point.end(), comparePoints);

    for (const auto& pt : point) {
        auto it = compressY.upper_bound(pt.y);
        if (it != compressY.end()) {
            results[pt.id] = tree.query(pt.x, it->second, maxY - 1);
        } else {
            results[pt.id] = 0;
        }
    }

    auto end = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> duration = end - start; 
    std::cout << "Time taken for n = " << n << " and m = " << m << ": " << duration.count() << " seconds\n";
}

int main() {
    performanceTest(1000, 1000);
    performanceTest(5000, 5000);
    performanceTest(10000, 10000);
    performanceTest(30000, 30000);
    performanceTest(50000, 50000);
    performanceTest(70000, 70000);
    performanceTest(100000, 100000);
    return 0;
}
