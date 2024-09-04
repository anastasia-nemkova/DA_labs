#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <queue>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <random>

class TNode {
public:
    friend class TTrie;
    TNode() : linkFail(nullptr) {};
    ~TNode() {};
private:
    std::map<std::string, TNode*> child;
    TNode* linkFail;
    std::vector<int> ind;
};


void ToLower(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

class TTrie {
public:

    TTrie() {
        root = new TNode();
        root->linkFail = root;
        noJoker = 0;
    }

    void Create(const std::vector<std::string> &patterns) {
        int patternNumber = 0;
        int patLen = patterns.size();
        sizePat.resize(patLen);
        int i;

        TNode* curNode = root;
        for (i = 0; i < patLen; ++i) {
            if (patterns[i] == "?") {
                if (curNode != root) {
                    curNode->ind.push_back(patternNumber);
                    sizePat[patternNumber] = i - patternNumber;
                    ++noJoker;
                    curNode = root;
                }
                patternNumber = i + 1;
                continue;
            }

            std::string lowerPattern = patterns[i];
            ToLower(lowerPattern);

            if (curNode->child.find(lowerPattern) != curNode->child.end()) {
                curNode = curNode->child.find(lowerPattern)->second;
            } else {
                TNode *newNode = new TNode();
                curNode->child[lowerPattern] = newNode;
                curNode = newNode;
            }
        }

        if (curNode != root) {
            curNode->ind.push_back(patternNumber);
            sizePat[patternNumber] = i - patternNumber;
            ++noJoker;
        }

        CreateLinks();
    }

    void Search(const std::vector<std::string> &text, const int &patSize, std::vector<std::pair<int, int>> &answer) {
        int textSize = text.size();
        std::vector<int> entry(textSize, 0);
        TNode* curNode = root;

        for (int i = 0; i < textSize; ++i) {
            std::string lowerText = text[i];
            ToLower(lowerText);
            auto exisNode = curNode->child.find(lowerText);

            while (exisNode == curNode->child.end()) {
                curNode = curNode->linkFail;
                exisNode = curNode->child.find(lowerText);
                if (curNode == curNode->linkFail) {
                    break;
                }
            }

            if (exisNode != curNode->child.end()) {
                curNode = exisNode->second;
                if (!curNode->ind.empty()) {
                    for (auto patternIdx : curNode->ind) {
                        int patternId = i - sizePat[patternIdx] - patternIdx + 1;
                        if (patternId < 0) {
                            continue;
                        }
                        entry[patternId]++;
                    }
                }
            }
        }

        for (int i = 0; i < textSize; ++i) {
            if ((entry[i] == noJoker) && (i + patSize <= textSize)) {
                std::cout << answer[i].first << ", " << answer[i].second << "\n";
            }
        }
    }
    
    ~TTrie() {};

private:
    TNode* root;
    std::vector<int> sizePat;
    int noJoker;

    void CreateLinks() {
        TNode* curNode = root;
        std::queue<TNode*> queue;
        queue.push(curNode);

        while (!queue.empty()) {
            curNode = queue.front();
            queue.pop();

            for (auto iter = curNode->child.begin(); iter != curNode->child.end(); ++iter) {
                TNode* child = iter->second;
                TNode* parentFail = curNode->linkFail;
                std::string subPattern = iter->first;

                queue.push(child);

                while(true) {
                    auto exisNode = parentFail->child.find(subPattern);

                    if (exisNode != parentFail->child.end()) {
                        child->linkFail = (exisNode->second != child) ? exisNode->second : root;
                        child->ind.insert(child->ind.end(), child->linkFail->ind.begin(), child->linkFail->ind.end());
                        break;
                    }

                    if (parentFail == root) {
                        child->linkFail = root;
                        break;
                    } else {
                        parentFail = parentFail->linkFail;
                    }
                }
            }
        }
    }
};


int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    std::vector<std::pair<int, int>> answer;
    std::vector<std::string> pattern;
    std::vector<std::string> text;

    std::string input, token;
    TTrie trie;

    std::getline(std::cin, input);
    std::stringstream patStr(input);

    while (patStr >> token) {
        pattern.push_back(token);
    }

    int lineCnt = 1;
    int wordCnt = 1;

    while (std::getline(std::cin, input)) {
        std::stringstream textSS(input);

        while (textSS >> token) {
            text.push_back(token);
            answer.push_back(std::make_pair(lineCnt, wordCnt));
            ++wordCnt;
        }
        ++lineCnt;
        wordCnt = 1;
    }

    trie.Create(pattern);
    trie.Search(text, pattern.size(), answer);

}

// Генерация случайной строки заданной длины
// std::string GenerateRandomString(int length) {
//     static const char alphanum[] =
//         "abcdefghijklmnopqrstuvwxyz";
    
//     std::string str(length, ' ');
//     for (int i = 0; i < length; ++i) {
//         str[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
//     }
//     return str;
// }

// std::vector<std::string> GeneratePattern(int numWords, int wordLength) {
//     std::vector<std::string> pattern;
//     for (int i = 0; i < numWords; ++i) {
//         pattern.push_back(GenerateRandomString(wordLength));
//     }

//     return pattern;
// }

// std::vector<std::string> GenerateText(int numLines, int wordsPerLine, const std::vector<std::string>& pattern) {
//     std::vector<std::string> text;

//     for (int i = 0; i < numLines; ++i) {
//         std::string line;
//         int patternWords = pattern.size();
//         int remainingWords = wordsPerLine;

//         while (remainingWords > 0) {
//             if (rand() % 3 == 0 && remainingWords >= patternWords) {
//                 for (const auto& word : pattern) {
//                     if (!line.empty()) {
//                         line += " ";
//                     }
//                     line += word;
//                 }
//                 remainingWords -= patternWords;
//             } else {
//                 std::string randomWord = GenerateRandomString(5);
//                 if (!line.empty()) {
//                     line += " ";
//                 }
//                 line += randomWord;
//                 remainingWords--;
//             }
//         }

//         text.push_back(line);
//     }

//     return text;
// }

// void MeasureTrieSearchTime(const std::vector<std::string>& pattern, const std::vector<std::string>& text) {
//     TTrie trie;
//     trie.Create(pattern);

//     auto start = std::chrono::steady_clock::now();
//     trie.Search(text);
//     auto end = std::chrono::steady_clock::now();

//     std::cout << "Trie search time: " << std::chrono::duration<double>(end - start).count() << " sec\n";
// }

// void MeasureNaiveSearchTime(const std::vector<std::string>& patterns, const std::vector<std::string>& text) {
//     auto start = std::chrono::steady_clock::now();
//     for (const auto& line : text) {
//         for (const auto& pattern : patterns) {
//             size_t pos = line.find(pattern);
//             while (pos != std::string::npos) {
//                 pos = line.find(pattern, pos + pattern.size());
//             }
//         }
//     }
//     auto end = std::chrono::steady_clock::now();
//     std::cout << "Naive search time: " << std::chrono::duration<double>(end - start).count() << " sec\n";
// }

// int main() {
//     srand(time(nullptr));
//     std::vector<std::string> pattern = GeneratePattern(100, 5);
//     std::vector<std::string> text = GenerateText(10000, 1000, pattern); 

//     MeasureTrieSearchTime(pattern, text);
//     MeasureNaiveSearchTime(pattern, text);

//     return 0;
// }