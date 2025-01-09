#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

struct Point {
    double x, y;
};

bool isOnEdge(const Point& p, const Point& v1, const Point& v2) {
    double epsilon = 1e-9;
    if (min(v1.x, v2.x) - epsilon <= p.x && p.x <= max(v1.x, v2.x) + epsilon &&
        min(v1.y, v2.y) - epsilon <= p.y && p.y <= max(v1.y, v2.y) + epsilon) {

        double crossProduct = (p.y - v1.y) * (v2.x - v1.x) - (p.x - v1.x) * (v2.y - v1.y);
        return abs(crossProduct) < epsilon;
    }
    return false;
}

bool pointInPolygon(const vector<Point>& poly, const Point& p) {
    int intersectionCount = 0;
    int n = poly.size();
    double epsilon = 1e-9;

    for (int i = 0; i < n; ++i) {
        Point v1 = poly[i];
        Point v2 = poly[(i + 1) % n];

        if (isOnEdge(p, v1, v2)) {
            return true;
        }

        double y1 = v1.y, y2 = v2.y;
        if (y1 == p.y) y1 += epsilon;
        if (y2 == p.y) y2 += epsilon;

        if ((p.y > min(y1, y2)) && (p.y <= max(y1, y2)) && (p.x <= max(v1.x, v2.x))) {
            double intersectionX = (p.y - v1.y) * (v2.x - v1.x) / (v2.y - v1.y) + v1.x;

            if (intersectionX > p.x) {
                intersectionCount++;
            }
        }
    }
    return (intersectionCount % 2 != 0);
}

int main() {
    int m;
    cin >> m;

    vector<vector<Point>> polygons(m);

    for (int i = 0; i < m; i++) {
        int n;
        cin >> n;
        polygons[i].resize(n);
        for (int j = 0; j < n; j++) {
            cin >> polygons[i][j].x >> polygons[i][j].y;
        }
    }

    int q;
    cin >> q;

    vector<Point> queries(q);
    for (int i = 0; i < q; i++) {
        cin >> queries[i].x >> queries[i].y;
    }

    for (const auto& queryPoint : queries) {
        int ans = -1;
        
        for (int i = 0; i < m; i++) {
            if (pointInPolygon(polygons[i], queryPoint)) {
                ans = i;
                break;
            }
        }

        cout << ans << "\n";
    }

    return 0;
}
