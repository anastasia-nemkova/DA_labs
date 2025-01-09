// попытки внедрить персистентное декартово дерево и использовать сканирующую прямую
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <queue>

using namespace std;


struct Segment {
    int x1, y1, x2, y2;
    int polygon_index;

    Segment(int _x1, int _y1, int _x2, int _y2, int _polygon_index)
        : x1(_x1), y1(_y1), x2(_x2), y2(_y2), polygon_index(_polygon_index) {}

    bool isAbove(int x, int y) const {
        double slope = (double)(y2 - y1) / (x2 - x1);
        double y_on_seg = y1 + slope * (x - x1);
        return y < y_on_seg;
    }

    bool isPointOnSegment(int x, int y) const {
        if ((x == x1 && y == y1) || (x == x2 && y == y2)) {
            return true;
        }
        return min(x1, x2) <= x && x <= max(x1, x2) && min(y1, y2) <= y && y <= max(y1, y2) &&
            (y2 - y1) * (x - x1) == (y - y1) * (x2 - x1);
    }

    bool isRight() const {
        return x1 < x2; 
    }

    bool operator<(const Segment& other) const {
        if(y1 == other.y1) return (x1 < other.x1);
        return y1 < other.y1;
    }
};

struct Event {
    int x;
    bool is_addition;
    Segment seg;

    Event(int _x, bool _is_addition, Segment _seg) : x(_x), is_addition(_is_addition), seg(_seg) {}

    bool operator<(const Event& other) const {
        return x < other.x || (x == other.x && is_addition > other.is_addition);
    }
};

struct Node {
    Segment seg;
    Node *left, *right;
    int priority;

    Node(Segment _seg) : seg(_seg), left(nullptr), right(nullptr), priority(rand()) {}
};

class PersistentTreap {
public:
    vector<Node*> versions;

    PersistentTreap() {
        versions.push_back(nullptr);
    }

    void insert(Segment seg) {
        Node* new_root = clone(versions.back());
        insert(new_root, seg);
        versions.push_back(new_root);
    }

    void remove(Segment seg) {
        Node* new_root = clone(versions.back());
        remove(new_root, seg);
        versions.push_back(new_root);
    }

    Segment findSegment(Node* root, int x, int y) {
        Segment nearestSegment(-1, -1, -1, -1, -1);
        findSegment(root, x, y, nearestSegment);
        return nearestSegment;
    }

    int binarySearch(int x, int y) {
        int left = 0, right = versions.size() - 1;
        int version_index = -1;

        while (left < right) {
            int mid = (left + right + 1) / 2;
            if (versions[mid] && versions[mid]->seg.isPointOnSegment(x, y)) {
                
                version_index = mid;
                break;
            } else if (versions[mid] && versions[mid]->seg.x1 <= x) {
                version_index = mid;
                left = mid;
            } else {
                right = mid - 1;
            }
        }
        return version_index;
    }

    void printTree(Node* root, string indent = "") {
        if (!root) return;

        cout << indent << "Node (" << root->seg.x1 << ", " << root->seg.y1 << ") -> (" 
             << root->seg.x2 << ", " << root->seg.y2 << "), Polygon Index: " 
             << root->seg.polygon_index << endl;

        if (root->left) {
            cout << indent << " Left Child:" << endl;
            printTree(root->left, indent + "    ");
        } else {
            cout << indent << " Left Child: NULL" << endl;
        }

        if (root->right) {
            cout << indent << " Right Child:" << endl;
            printTree(root->right, indent + "    ");
        } else {
            cout << indent << " Right Child: NULL" << endl;
        }
    }

    void printCurrentVersion(int version_index) {
        cout << "Current Tree for Version " << version_index << ":\n";
        printTree(versions[version_index], "");
        cout << "-------------------------" << endl;
    }

    void printAllVersions() {
        for (int i = 0; i < versions.size(); ++i) {
            cout << "Version " << i << ":" << endl;
            printTree(versions[i], "");
            cout << "-------------------------" << endl;
        }
    }

private:
    void insert(Node*& root, Segment seg) {
        if (!root) {
            root = new Node(seg);
            return;
        }
        if (seg < root->seg) {
            insert(root->left, seg);
            if (root->left->priority > root->priority) {
                rotateRight(root);
            }
        } else {
            insert(root->right, seg);
            if (root->right->priority > root->priority) {
                rotateLeft(root);
            }
        }
    }

    void remove(Node*& root, const Segment& seg) {
        if (!root) return;

        if (root->seg.x1 == seg.x1 && root->seg.y1 == seg.y1 && root->seg.x2 == seg.x2 && root->seg.y2 == seg.y2) {
            root = deleteNode(root);
        } else if (seg < root->seg) {
            remove(root->left, seg);
        } else {
            remove(root->right, seg);
        }
    }

    Node* deleteNode(Node* root) {
        if (!root->left && !root->right) {
            delete root;
            return nullptr;
        } else if (!root->left || !root->right) {
            Node* child = (root->left) ? root->left : root->right;
            delete root;
            return child;
        } else {
            if (root->left->priority > root->right->priority) {
                rotateRight(root);
                root->right = deleteNode(root->right);
            } else {
                rotateLeft(root);
                root->left = deleteNode(root->left);
            }
        }
        return root;
    }

    void findSegment(Node* root, int x, int y, Segment& nearestSegment) {
        if (!root) return;

        if (root->seg.isPointOnSegment(x, y)) {
            nearestSegment = root->seg;
            return;
        }

        if (root->seg.isAbove(x, y)) {
            nearestSegment = root->seg;
            findSegment(root->right, x, y, nearestSegment);
        } else {
            findSegment(root->left, x, y, nearestSegment);
        }
    }

    Node* clone(Node* root) {
        if (!root) return nullptr;
        Node* new_node = new Node(root->seg);
        new_node->left = clone(root->left);
        new_node->right = clone(root->right);
        return new_node;
    }

    void rotateRight(Node*& root) {
        Node* new_root = root->left;
        root->left = new_root->right;
        new_root->right = root;
        root = new_root;
    }

    void rotateLeft(Node*& root) {
        Node* new_root = root->right;
        root->right = new_root->left;
        new_root->left = root;
        root = new_root;
    }
};

int main() {
    int n, m;
    cin >> n;

    vector<Event> events;

    for (int i = 0; i < n; ++i) {
        int vertices;
        cin >> vertices;
        int prev_x, prev_y, first_x, first_y;
        vector<Segment> polygon_segments;
        for (int j = 0; j < vertices; ++j) {
            int x, y;
            cin >> x >> y;
            if (j > 0 && prev_x != x) {
                polygon_segments.emplace_back(prev_x, prev_y, x, y, i);
                events.push_back(Event(min(prev_x, x), true, polygon_segments.back()));
                events.push_back(Event(max(prev_x, x), false, polygon_segments.back()));
            } else if (j == 0) {
                first_x = x;
                first_y = y;
            }
            prev_x = x;
            prev_y = y;
        }
        if (prev_x != first_x) {
            polygon_segments.emplace_back(prev_x, prev_y, first_x, first_y, i);
            events.push_back(Event(min(prev_x, first_x), true, polygon_segments.back()));
            events.push_back(Event(max(prev_x, first_x), false, polygon_segments.back()));
        }
    }

    sort(events.begin(), events.end());

    PersistentTreap treap;
    for (const auto& event : events) {
        if (event.is_addition) {
            treap.insert(event.seg);
        } else {
            treap.remove(event.seg);
        }
    }

    treap.printAllVersions();

    cin >> m;
    for (int i = 0; i < m; ++i) {
        int x, y;
        cin >> x >> y;

        int version_index = treap.binarySearch(x, y);
        if (version_index == -1) {
            cout << -1 << endl;
            continue;
        }

        treap.printCurrentVersion(version_index);

        Segment nearestSegment = treap.findSegment(treap.versions[version_index], x, y);
        if (nearestSegment.polygon_index == -1) {
            cout << -1 << endl;
            continue;
        }

        cout << "Segment: (" << nearestSegment.x1 << ", " << nearestSegment.y1 << ") -> ("
             << nearestSegment.x2 << ", " << nearestSegment.y2 << ")" << endl;

        if (nearestSegment.isPointOnSegment(x, y)) {
            cout << nearestSegment.polygon_index << endl;
        } else if (nearestSegment.isRight()) {
            cout << nearestSegment.polygon_index << endl;
        } else {
            cout << -1 << endl;
        }
    }

    return 0;
}
