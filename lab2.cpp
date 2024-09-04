#include <iostream>
#include <fstream>
#include <map>
#include <chrono>
#include <iomanip>

static const size_t MAX_STRING_SIZE = 257;

class TString {
public:
    TString();
    TString(const char* str);
    TString(const TString& other);
    ~TString();

    TString& operator=(const TString& other);
    bool operator==(const char* str) const;

    friend bool operator>(const TString& lhs, const TString& rhs);
    friend bool operator<(const TString& lhs, const TString& rhs);
    const char& operator[](int index) const;
    char& operator[](int index);

    void Move(char* str);

    friend std::ostream& operator<<(std::ostream& os, const TString& str) {
        os << str.data;
        return os;
    }

    friend std::istream& operator>>(std::istream& is, TString& str) {
        str.Clear(); 
        char c;
        while (is.get(c) && (c == ' ' || c == '\n')) {}
        while (c != ' ' && c != '\n') {
            str.PushBack(c);
            is.get(c);
        }
        return is;
    }

    size_t Size() const;
    const char* GetData() const;

    void Clear();
    void PushBack(char c);
    void Resize(size_t new_capacity);
    void ToLower(TString* str);

private:
    char* data;
    size_t size;
    size_t capacity;


};

TString::TString() : size(0), data(nullptr), capacity(0) {}

TString::TString(const char* str) {
    size = 0;
    capacity = 0;
    while (size < MAX_STRING_SIZE - 1) {
        ++size;
    }
    capacity = size;
    data = new char[size + 1];
    for (size_t i = 0; i < size; ++i) {
        data[i] = str[i];
    }
    data[size] = '\0';
}

TString::TString(const TString& other) : data(nullptr), size(other.size), capacity(other.capacity) {
    data = new char[size + 1];
    for (size_t i = 0; i < size; ++i) {
        data[i] = other.data[i];
    }
    data[size] = '\0';
}

size_t TString::Size() const {
    return size;
}

const char* TString::GetData() const {
    return data;
}

TString::~TString() {
    delete[] data;
}

TString& TString::operator=(const TString& other) {
    if (this != &other) {
        delete[] data;
        size = other.size;
        capacity = other.capacity;
        data = new char[size + 1];
        for (size_t i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
        data[size] = '\0';
    }
    return *this;
}

bool TString::operator==(const char* str) const {
    size_t i = 0;
    while (data[i] != '\0' && str[i] != '\0') {
        if (data[i] != str[i]) return false;
        ++i;
    }
    return (data[i] == '\0' && str[i] == '\0');
}

bool operator>(const TString& lhs, const TString& rhs) {
    TString l = lhs;
    TString r = rhs;
    size_t i = 0;
    while (l.data[i] != '\0' && r.data[i] != '\0') {
        if (l.data[i] > r.data[i])
            return true;
        else if (l.data[i] < r.data[i])
            return false;
        ++i;
    }
    return (l.data[i] != '\0' && r.data[i] == '\0');
}

bool operator<(const TString& lhs, const TString& rhs) {
    TString l = lhs;
    TString r = rhs;
    size_t i = 0;
    while (l.data[i] != '\0' && r.data[i] != '\0') {
        if (l.data[i] < r.data[i])
            return true;
        else if (l.data[i] > r.data[i])
            return false;
        ++i;
    }
    return (l.data[i] == '\0' && r.data[i] != '\0');
}

void TString::Move(char* str) {
    delete[] this->data;
    this->data = str;
    while (str[size] != '\0' && size < MAX_STRING_SIZE - 1) {
        ++size;
    }
    this->capacity = this->size + 1;
    str = nullptr;

}

void TString::Clear() {
    size = 0;
    if (data) {
        data[0] = '\0';
    }
}


const char& TString::operator[](int index) const {
    if (index >= 0 && index < size) {
        return data[index];
    } else {
        throw std::out_of_range("Index out of range");
    }
}

char& TString::operator[](int index) {
    if (index >= 0 && index < size) {
        return data[index];
    } else {
        throw std::out_of_range("Index out of range");
    }
}

void TString::PushBack(char c) {
    if (size + 1 > capacity) {
        Resize(size + 1);
    }
    data[size++] = c;
    data[size] = '\0';
}

void TString::Resize(size_t new_capacity) {
    capacity = new_capacity;
    char* new_data = new char[capacity + 1];
    for (size_t i = 0; i < size; ++i) {
        new_data[i] = data[i];
    }
    new_data[size] = '\0';
    delete[] data;
    data = new_data;
}

void TString::ToLower(TString* str) {
    for (size_t i = 0; i < str->Size(); ++i) {
        if(str->data[i] >= 'A' && str->data[i] <= 'Z') {
            str->data[i] = (str->data[i] - ('A' - 'a'));
        }
    }
}


template<typename T, typename K>
class TAVL {
private:
    struct TNode {
        T key;
        K value;
        TNode* left;
        TNode* right;
        unsigned char height;

        TNode(const T& k, const K& val) {
            key = k;
            value = val; 
            left = right = nullptr; 
            height = 1;
        } 
    };

    TNode* root;

    unsigned char GetHeight(TNode* n) {
        return n == nullptr ? 0 : n->height;
    }

    int BFactor(TNode* n) {
        return GetHeight(n->right) - GetHeight(n->left);
    }

    void FixHeight(TNode* n) {
        unsigned char hl = GetHeight(n->left);
        unsigned char hr = GetHeight(n->right);
        n->height = (hl > hr ? hl : hr) + 1;
    }

    TNode* RotateRight(TNode* n) {
        TNode* temp = n->left;
        n->left = temp->right;
        temp->right = n;
        FixHeight(n);
        FixHeight(temp);
        return temp;
    }

    TNode* RotateLeft(TNode* n) {
        TNode* temp = n->right;
        n->right = temp->left;
        temp->left = n;
        FixHeight(n);
        FixHeight(temp);
        return temp;
    }

    TNode* Balance(TNode* n) {
        FixHeight(n);
        short bf = BFactor(n);
        if(bf > 1) {
            if(BFactor(n->right) < 0) {
                n->right = RotateRight(n->right);
            }
            return RotateLeft(n);
        }
        if(bf < -1) {
            if(BFactor(n->left) > 0) {
                n->left = RotateLeft(n->left);
            }
            return RotateRight(n);
        }
        return n;
    }

    TNode* InsertNode(TNode* n, TNode* k) {
        if(n == nullptr) {
            return k;
        }
        if(k->key > n->key) {
            TNode* temp = InsertNode(n->right, k);
            if(temp == nullptr) {
                return nullptr;
            }
            n->right = temp;
        } else if(k->key < n->key) {
            TNode* temp = InsertNode(n->left, k);
            if(temp == nullptr) {
                return nullptr;
            }
            n->left = temp;
        } else {
            return nullptr;
        }
        return Balance(n);
    }

    TNode* FindMin(TNode* n) {
        return n->left == nullptr ? n : FindMin(n->left);
    }

    TNode* RemoveMin(TNode* n) {
        if(n->left == nullptr) {
            return n->right;
        }
        n->left = RemoveMin(n->left);
        return Balance(n);
    }

    TNode* RemoveNode(TNode* n, const T& k) {
        if(n == nullptr) {
            return nullptr;
        }
        if(k < n->key) {
            n->left = RemoveNode(n->left, k);
        } else if(k > n->key) {
            n->right = RemoveNode(n->right, k); 
        } else {
            if(n->right == nullptr) {
                TNode* temp = n->left;
                delete n;
                return temp;
            } else if(n->left == nullptr) {
                TNode* temp = n->right;
                delete n;
                return temp;
            } else {
                TNode* min = FindMin(n->right);
                TNode* temp = new TNode(min->key, min->value);
                temp->left = n->left;
                temp->right = RemoveMin(n->right);
                delete n;
                return Balance(temp);
            }
        }
        return Balance(n);
    }

    TNode* FindNode(TNode* n, const T& k) {
        while(n != nullptr) {
            if(k > n->key) {
                n = n->right;
            } else if(k < n->key) {
                n = n->left;
            } else {
                return n;
            }
        }
        return n;
    }

    void Delete(TNode* n) {
        if(n == nullptr) {
            return;
        }
        Delete(n->left);
        Delete(n->right);
        delete n;
    }

    void Save(TNode* n, std::ofstream& of) {
        if(n == nullptr) {
            return;
        }
        size_t keySize = n->key.Size();
        of.write(reinterpret_cast<char*>(&keySize), sizeof(keySize));
        of.write(n->key.GetData(), keySize);
        of.write((char*) &n->value, sizeof(n->value));

        bool hasL = (n->left != nullptr);
        bool hasR = (n->right != nullptr);

        of.write(reinterpret_cast<char*>(&hasL), sizeof(hasL));
		of.write(reinterpret_cast<char*>(&hasR), sizeof(hasR));

        if (hasL) {
            Save(n->left, of);
        }
        if (hasR) {
            Save(n->right, of);
        }
    }

    TNode* Load(std::ifstream& is) {
        size_t keySize = 0;

        is.read(reinterpret_cast<char *>(&keySize), sizeof(keySize));

        char *key = new char[keySize + 1];
		key[keySize] = '\0';
		is.read(key, keySize);
        
        K value = 0;
        is.read(reinterpret_cast<char *>(&value), sizeof(value));

        bool lSon = false;
        bool rSon = false;

        is.read((char*) &lSon, sizeof(lSon));
        is.read((char*) &rSon, sizeof(rSon));

        TNode* n = new TNode(key, value);
        n->key.Move(key);
        n->value = value;

        if (lSon) {
            n->left = Load(is);
        }

        if (rSon) {
            n->right = Load(is);
        }

        return n;
    }

public:
    TAVL() {
        root = nullptr;
    }

    int Insert(const T& k, const K& val) {
        if(FindNode(root, k) != nullptr) {
            return 0;
        }
        root = InsertNode(root, new TNode(k, val));
        return 1;
    }

    int Remove(const T& k) {
        if(FindNode(root, k) == nullptr) {
            return 0;
        }
        root = RemoveNode(root, k);
        return 1;
    }

    K* Find(const T& k) {
        TNode* temp = FindNode(root, k);
        return temp == nullptr ? nullptr : &(temp->value);
    }

    void SaveTree(std::ofstream& of) {
        Save(root, of);
    }

    void LoadTree(std::ifstream& is) {
        Delete(root);
        root = Load(is);
    }

    ~TAVL() {
        Delete(root);
    }
};


// int main() {
//     std::ios::sync_with_stdio(false);
//     std::cout.tie(0);
//     std::cin.tie(0);

//     TAVL<TString, unsigned long long> dictionary;
//     TString command, key, filename;
//     unsigned long long value;

//     while(true) {
//         std::cin >> std::ws;
//         if (std::cin.eof()) {
//             break;
//         }
//         std::cin >> command;
//         if(command == "+") {
//             std::cin >> key >> value;
//             key.ToLower(&key);
//             if(dictionary.Insert(key, value)) {
//                 std::cout << "OK" << "\n";
//             } else {
//                 std::cout << "Exist" << "\n";
//             }
//         } else if(command == "-") {
//             std::cin >> key;
//             key.ToLower(&key);
//             if (dictionary.Remove(key)) {
//                 std::cout << "OK" << "\n";
//             } else {
//                 std::cout << "NoSuchWord" << "\n";
//             }
//         } else if(command == "!") {
//             std::cin >> key;
//             if(key == "Save") {
//                 std::cin >> filename;
//                 std::ofstream of(filename.GetData(), std::ios::binary | std::ios::out);
//                 dictionary.SaveTree(of);
//                 std::cout << "OK" << "\n";
//             } else if(key == "Load") {
//                 std::cin >> filename;
//                 std::ifstream is(filename.GetData(), std::ios::binary | std::ios::in);
//                 dictionary.LoadTree(is);
//                 std::cout << "OK" << "\n";
//             }
//         } else {
//             command.ToLower(&command);
//             unsigned long long *tempValue;
//             if ((tempValue = dictionary.Find(command)) != nullptr) {
//                 std::cout << "OK: " << *tempValue << "\n";
//             } else {
//                 std::cout << "NoSuchWord" << "\n";
//             }
//         }
//     }
// }


//-------------------------------------
//            BENCHMARK
//-------------------------------------

// int main() {
//     std::ifstream input("rand100.txt");
//     if (!input.is_open()) {
//         std::cerr << "Failed to open file." << std::endl;
//         return 1;
//     }

//     TAVL<std::string, unsigned long long> avl;
//     std::map<std::string, unsigned long long> map;

//     std::string operation, key;
//     unsigned long long value;

//     double insertTimeAvl = 0.0;
//     double removeTimeAvl = 0.0;
//     double findTimeAvl = 0.0;

//     double insertTimeMap = 0.0;
//     double removeTimeMap = 0.0;
//     double findTimeMap = 0.0;

//     std::string line;
//     while (std::getline(input, line)) {

//         std::istringstream iss(line);
//         iss >> operation;
//         if (operation == "+") {
//             iss >> key >> value;

//             auto startInsertAvl = std::chrono::steady_clock::now();
//             avl.Insert(key, value);
//             auto endInsertAvl = std::chrono::steady_clock::now();
//             insertTimeAvl += std::chrono::duration<double, std::milli>(endInsertAvl - startInsertAvl).count();

//             auto startinsertMap = std::chrono::steady_clock::now();
//             map.insert(std::pair<std::string, unsigned long long>(key,value));
//             auto endInsertMap = std::chrono::steady_clock::now();
//             insertTimeMap += std::chrono::duration<double, std::milli>(endInsertMap - startinsertMap).count();
//         } else if (operation == "-") {
//             iss >> key;

//             auto startremoveAvl = std::chrono::steady_clock::now();
//             avl.Remove(key);
//             auto end_removeAvl = std::chrono::steady_clock::now();
//             removeTimeAvl += std::chrono::duration<double, std::milli>(end_removeAvl - startremoveAvl).count();

//             auto startremoveMap = std::chrono::steady_clock::now();
//             map.erase(key);
//             auto endRemoveMap = std::chrono::steady_clock::now();
//             removeTimeMap += std::chrono::duration<double, std::milli>(endRemoveMap - startremoveMap).count();
//         } else {
//             iss >> key;

//             auto startfindAvl = std::chrono::steady_clock::now();
//             avl.Find(key);
//             auto endFindAvl = std::chrono::steady_clock::now();
//             findTimeAvl += std::chrono::duration<double, std::milli>(endFindAvl - startfindAvl).count();

//             auto startfindMap = std::chrono::steady_clock::now();
//             map.find(key);
//             auto endFindMap = std::chrono::steady_clock::now();
//             findTimeMap += std::chrono::duration<double, std::milli>(endFindMap - startfindMap).count();
//         }
//     }

    

//     std::cout << "AVL insert time: " << insertTimeAvl << "ms" << std::endl;
//     std::cout << "AVL remove time: " << removeTimeAvl << "ms" << std::endl;
//     std::cout << "AVL find time: " << findTimeAvl << "ms" << std::endl;

//     std::cout << "std::map insert time: " << insertTimeMap << "ms" << std::endl;
//     std::cout << "std::map remove time: " << removeTimeMap << "ms" << std::endl;
//     std::cout << "std::map find time: " << findTimeMap << "ms" << std::endl;

//     return 0;
// }


//-------------------------------------
//             GPROF
//-------------------------------------

void RandString(char str[]) {
    char chars[27] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    for (int i = 0; i < 257 - 1; ++i) {
        str[i] = chars[rand() % 26];
    }
    str[257 - 1] = '\0';
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    std::cout.tie(0);

    TAVL<TString, unsigned long long> tree;
    TString command;
    TString key;

    char tmp[257];
    key = tmp;
    for (int i = 0; i < 10000; ++i) {
        RandString(tmp);
        key = tmp;
        tree.Insert(key, i);
    }

    for (int i = 0; i < 10000; ++i) {
        RandString(tmp);
        key = tmp;
        tree.Remove(key);
    }

    for (int i = 0; i < 10000; ++i) {
        RandString(tmp);
        key = tmp;
        tree.Find(key);
    }

    return 0;
}
