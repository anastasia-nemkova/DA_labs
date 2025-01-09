#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <sstream>
#include <chrono>
#include <random>

struct Point {
    double x, y;
};

struct MBR {
    Point lowLeft, upRight;

    bool contains(const Point& p) const {
        return p.x >= lowLeft.x && p.x <= upRight.x &&
               p.y >= lowLeft.y && p.y <= upRight.y;
    }

    void expand(const MBR& other) {
        lowLeft.x = std::min(lowLeft.x, other.lowLeft.x);
        lowLeft.y = std::min(lowLeft.y, other.lowLeft.y);
        upRight.x = std::max(upRight.x, other.upRight.x);
        upRight.y = std::max(upRight.y, other.upRight.y);
    }

    double area() const {
        return (upRight.x - lowLeft.x) * (upRight.y - lowLeft.y);
    }

    bool overlaps(const MBR& other) const {
        return !(upRight.x < other.lowLeft.x || upRight.y < other.lowLeft.y ||
                 lowLeft.x > other.upRight.x || lowLeft.y > other.upRight.y);
    }
};

struct Polygon {
    int id;
    std::vector<Point> vertics;

    MBR getMBR() const {
        MBR mbr = { vertics[0], vertics[0] };
        for (const auto& p : vertics) {
            mbr.expand({ p, p });
        }
        return mbr;
    }

    bool isOnEdge(const Point& p, const Point& v1, const Point& v2) {
        double epsilon = 1e-9;
        if (std::min(v1.x, v2.x) - epsilon <= p.x && p.x <= std::max(v1.x, v2.x) + epsilon &&
            std::min(v1.y, v2.y) - epsilon <= p.y && p.y <= std::max(v1.y, v2.y) + epsilon) {

            double crossProduct = (p.y - v1.y) * (v2.x - v1.x) - (p.x - v1.x) * (v2.y - v1.y);
            return std::abs(crossProduct) < epsilon;
        }
        return false;
    }

    bool rayCasting(const Point& p) {
        int intersection = 0;
        int n = vertics.size();
        double epsilon = 1e-9;

        for (int i = 0; i < n; ++i) {
            Point v1 = vertics[i];
            Point v2 = vertics[(i + 1) % n];


            if (isOnEdge(p, v1, v2)) {
                return true;
            }
            double y1 = v1.y;
            double y2 = v2.y;

            if (y1 == p.y) {
                y1 += epsilon;
            }
            if (y2 == p.y) {
                y2 += epsilon;
            }

            if ((p.y > std::min(y1, y2)) && (p.y <= std::max(y1, y2)) && (p.x <= std::max(v1.x, v2.x))) {
                double pIntersection = (p.y - v1.y) * (v2.x - v1.x) / (v2.y - v1.y) + v1.x;

                if (pIntersection > p.x) {
                    intersection++;
                }
            }
        }
        return (intersection % 2 != 0);
    }
};

struct Node {
    bool isLeaf;
    std::vector<MBR> entries;
    std::vector<int> indices;
    std::vector<Node*> children;

    Node(bool leaf = true) : isLeaf(leaf) {}
};

class RTree {
private:
    Node* root;
    const int maxEntries = 70;

    Node* chooseLeaf(Node* current, const MBR& mbr) {
        if (current->isLeaf) return current;

        double minEnlargement = std::numeric_limits<double>::max();
        Node* bestChild = nullptr;

        for (size_t i = 0; i < current->entries.size(); ++i) {
            double currentArea = current->entries[i].area();
            MBR temp = current->entries[i];
            temp.expand(mbr);
            double enlargement = temp.area() - currentArea;

            if (enlargement < minEnlargement) {
                minEnlargement = enlargement;
                bestChild = current->children[i];
            }
        }
        return bestChild ? chooseLeaf(bestChild, mbr) : current;
    }

    void linearSplit(Node* node, Node*& newNode) {
        newNode = new Node(node->isLeaf);
        size_t half = node->entries.size() / 2;
        
        for (size_t i = half; i < node->entries.size(); ++i) {
            newNode->entries.push_back(node->entries[i]);
            if (node->isLeaf) newNode->indices.push_back(node->indices[i]);
        }

        node->entries.resize(half);
        if (node->isLeaf) node->indices.resize(half);
    }


    void adjustTree(Node* node, Node* newNode) {
        if (node == root) {
            if (newNode) {
                Node* newRoot = new Node(false);
                newRoot->entries.push_back(calculateMBR(node));
                newRoot->entries.push_back(calculateMBR(newNode));
                newRoot->children.push_back(node);
                newRoot->children.push_back(newNode);
                root = newRoot;
            }
            return;
        }

        Node* parent = findParent(root, node);
        if (parent) {
            parent->entries.push_back(calculateMBR(newNode));
            parent->children.push_back(newNode);

            if (parent->entries.size() > maxEntries) {
                Node* newParentNode;
                linearSplit(parent, newParentNode);
                adjustTree(parent, newParentNode);
            }
        }
    }


    Node* findParent(Node* current, Node* child) {
        if (current->isLeaf) return nullptr;

        for (size_t i = 0; i < current->children.size(); ++i) {
            if (current->children[i] == child) return current;

            Node* result = findParent(current->children[i], child);
            if (result) return result;
        }
        return nullptr;
    }


    MBR calculateMBR(Node* node) {
        MBR mbr = node->entries[0];
        for (size_t i = 1; i < node->entries.size(); ++i) {
            mbr.expand(node->entries[i]);
        }
        return mbr;
    }


    void searchRecursive(Node* node, const Point& p, std::vector<int>& result) const {
        if (node->isLeaf) {
            for (size_t i = 0; i < node->entries.size(); ++i) {
                if (node->entries[i].contains(p)) {
                    result.push_back(node->indices[i]);
                }
            }
        } else {
            for (size_t i = 0; i < node->children.size(); ++i) {
                if (node->entries[i].overlaps(MBR{p, p})) {
                    searchRecursive(node->children[i], p, result);
                }
            }
        }
    }

public:
    RTree() {
        root = new Node();
    }

    void insert(const MBR& mbr, int index) {
        Node* leaf = chooseLeaf(root, mbr);
        leaf->entries.push_back(mbr);
        leaf->indices.push_back(index);

        if (leaf->entries.size() > maxEntries) {
            Node* newLeafNode;
            linearSplit(leaf, newLeafNode);
            adjustTree(leaf, newLeafNode);
        }
    }

    std::vector<int> search(const Point& p) const {
        std::vector<int> result;
        searchRecursive(root, p, result);
        return result;
    }

};

void performanceTest(int n, int m) {
    std::cout << "Testing with n = " << n << " and m = " << m << std::endl;

    std::vector<Polygon> polygons;
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(-1000.0, 1000.0);

    for (int i = 0; i < n; ++i) {
        int v = rand() % 30 + 3;
        Polygon polygon;
        polygon.id = i;

        for (int j = 0; j < v; ++j) {
            polygon.vertics.push_back({dist(rng), dist(rng)});
        }
        polygons.push_back(polygon);
    }

    RTree tree;

    auto start = std::chrono::high_resolution_clock::now();

    for (const auto& polygon : polygons) {
        MBR mbr = polygon.getMBR();
        tree.insert(mbr, polygon.id);
    }

    std::vector<Point> points(m);
    for (int i = 0; i < m; ++i) {
        points[i] = {dist(rng), dist(rng)};
    }

    for (const auto& p : points) {
        std::vector<int> result = tree.search(p);
        std::vector<int> containingPolygons;
        for (int ind : result) {
            if (polygons[ind].rayCasting(p)) {
                containingPolygons.push_back(polygons[ind].id);
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Time taken: " << duration.count() << " seconds\n" << std::endl;
}

int main() {
    performanceTest(100, 100);
    performanceTest(500, 100);
    performanceTest(1000, 100);
    performanceTest(1500, 100);
    performanceTest(2000, 100);

    return 0;
}
