#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>


class TNode {
public:
    std::map<char, TNode*> children;
    TNode* suffixLink;
    int start;
    int* end;
    int suffInd;

    TNode(TNode* link, int start, int* end, int ind) : suffixLink(link), start(start), end(end), suffInd(ind) {}
};


class TSuffixTree {
public:
    TSuffixTree(const std::string& str) {
        text = str;
        root = new TNode(nullptr, -1, new int(-1), -1);
        activeNode = root;
        lastAddNode = nullptr;
        activeEdge = -1;
        activeLen = 0;
        remainder = 0;
        leafEnd = -1;
        for(int i = 0; i < str.size(); ++i) {
            InsertCharacter(i);
        }
    }

    ~TSuffixTree() {
        Destroy(root);
    }

    std::vector<int> Search(const std::string& pattern) {
        TNode* current = root;
        for (auto it = pattern.begin(); it != pattern.end();) {
            auto findChild = current->children.find(*it);
            if (findChild != current->children.end()) {
                current = findChild->second;
                for (int j = current->start; j <= *(current->end) && it != pattern.end(); j++, ++it) {
                    if (text[j] != *it) {
                        return {}; 
                    }
                }
            } else {
                return {};
            }
        }
        std::vector<int> result;
        CountIndex(current, result);
        std::sort(result.begin(), result.end());
        return result;
    }

private:
    std::string text;
    TNode* root;
    TNode* activeNode;
    TNode* lastAddNode;
    int activeEdge;
    int activeLen;
    int remainder;
    int leafEnd;

    void CountIndex(TNode* node, std::vector<int>& v) {
        for(auto it : node->children) {
            CountIndex(it.second, v);
        }
        if(node->suffInd != -1) {
            v.push_back(node->suffInd);
        }
    }

    int EdgeLen(TNode* node) {
        return *(node->end) - (node->start) + 1;
    }

    bool GoDown(TNode* node) {
        int edgeLen = EdgeLen(node);
        if(activeLen >= edgeLen) {
            activeEdge += edgeLen;
            activeLen -= edgeLen;
            activeNode = node;
            return true;
        }
        return false;
    }

    void InsertCharacter(int pos) {
        lastAddNode = nullptr;
        remainder++;
        leafEnd = pos;

        while(remainder > 0) {
            if(activeLen == 0) {
                activeEdge = pos;
            }
            auto findChild = activeNode->children.find(text[activeEdge]);

            if(findChild == activeNode->children.end()) {
                TNode* added = new TNode(root, pos, &leafEnd, pos - remainder + 1);
                activeNode->children[text[pos]] = added;
                if(lastAddNode) {
                    lastAddNode->suffixLink = activeNode;
                    lastAddNode = nullptr;
                }
            } else {
                TNode* next = findChild->second;
                if(GoDown(next)) {
                    continue;
                }
                if(text[pos] == text[next->start + activeLen]) {
                    if(lastAddNode && activeNode != root) {
                        lastAddNode->suffixLink = activeNode;
                    }
                    activeLen++;
                    break;
                }
                TNode* split = new TNode(root, next->start, new int(next->start + activeLen - 1), -1);
                activeNode->children[text[activeEdge]] = split;

                next->start += activeLen;
                split->children[text[pos]] = new TNode(root, pos, &leafEnd, pos - remainder + 1);
                split->children[text[next->start]] = next;

                if(lastAddNode != nullptr) {
                    lastAddNode->suffixLink = split;
                }
                lastAddNode = split;
            }
            remainder--;
            if(activeNode == root && activeLen > 0) {
                activeLen--;
                activeEdge = pos - remainder + 1;
            } else if(activeNode != root) {
                activeNode = activeNode->suffixLink;
            }
        }
    }

    void Destroy(TNode* node) {
        for(auto it : node->children) {
            Destroy(it.second);
        }
        if(node->suffInd == -1) {
            delete node->end;
        }
        delete node;
    }
};

std::string GenerateRandomString(int length) {
    static const char alphanum[] =
        "abcdefghijklmnopqrstuvwxyz";
    
    std::string str(length, ' ');
    for (int i = 0; i < length; ++i) {
        str[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return str;
}

std::vector<int> NaiveSearch(const std::string& text, const std::string& pattern) {
    std::vector<int> result;
    size_t pos = text.find(pattern);
    while (pos != std::string::npos) {
        result.push_back(pos);
        pos = text.find(pattern, pos + 1);
    }
    return result;
}

std::vector<std::string> GeneratePatternSet(const std::string& text, size_t numPatterns, size_t patternLength) {
    std::vector<std::string> patterns;
    for (size_t i = 0; i < numPatterns; ++i) {
        size_t startPos = rand() % (text.length() - patternLength + 1);
        patterns.push_back(text.substr(startPos, patternLength));
    }
    return patterns;
}

int main() {
    srand(time(0));
    double searchTime = 0;
    double findTime = 0;

    std::vector<int> strSize = {10000, 50000, 100000, 500000};
    for(int size : strSize) {
        std::string str = GenerateRandomString(size);
        TSuffixTree tree(str + "$");
        std::vector<std::string> patterns = GeneratePatternSet(str, 100, 50);

        for (const auto& pat : patterns) {

            auto start = std::chrono::steady_clock::now();
            std::vector<int> res = tree.Search(pat);
            auto end = std::chrono::steady_clock::now();
            searchTime += std::chrono::duration<double, std::milli>(end - start).count();

            auto start1 = std::chrono::steady_clock::now();
            std::vector<int> res1 = NaiveSearch(str, pat);
            auto end1 = std::chrono::steady_clock::now();
            findTime += std::chrono::duration<double, std::milli>(end1 - start1).count();

        }  

        std::cout << "Time for tree find: " << size << " " << searchTime << " ms" << std::endl;
        std::cout << "Time for standart find: " << size << " " << findTime << " ms" << std::endl;  
    }

    
    return 0;
}
