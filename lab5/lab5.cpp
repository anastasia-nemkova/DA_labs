#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>


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


int main() {
    std::string text, pattern;
    std::cin >> text;
    TSuffixTree tree(text + "$");

    int ind = 1;
    while (std::cin >> pattern) {
        std::vector<int> result = tree.Search(pattern);
        
        if (!result.empty()) {
            std::cout << ind << ": ";
            for (size_t i = 0; i < result.size(); ++i) {
                std::cout << (i > 0 ? ", " : "") << result[i] + 1;
            }
            std::cout << std::endl;
        }
        ++ind;
    }
    return 0;
}
