#include <iostream>

static const size_t MAX_STRING_SIZE = 2049;

class TString {
public:
    TString();
    TString(const char* str);
    TString(const TString& other);
    ~TString();

    TString& operator=(const TString& other);

    friend std::ostream& operator<<(std::ostream& os, const TString& str) {
        for (size_t i = 0; i < str.size; ++i) {
            os << str.data[i];
        }
        return os;
    }

private:
    char* data;
    size_t size;
};

TString::TString() : size(0), data(nullptr) {}

TString::TString(const char* str) {
    for (size_t i = 0; i < MAX_STRING_SIZE; ++i) {
        if (str[i] == 0) {
            size = i;
            break;
        }
    }
    data = new char[size];
    for(int i = 0; i < size; ++i) {
        data[i] = str[i];
    }
}

TString::TString(const TString& other) : data(nullptr), size(other.size) {
    data = new char[size];
    for(int i = 0; i < other.size; ++i) {
        data[i] = other.data[i];
    }
}

TString::~TString() {
    delete[] data;
}

TString& TString::operator=(const TString& other) {
    if (this != &other) {
        delete[] data;
        size = other.size;
        data = new char[size];
        for(int i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
    }
    return *this;
}


template <typename T>
class TVector {
public:
    TVector();
    TVector(size_t newSize);
    TVector(const TVector& other);
    ~TVector();

    size_t Size() const;
    void PushBack(const T& value);
    void Assign(size_t& newSize, T value);

    const T& operator[](size_t index) const;
    T& operator[](size_t index);

private:
    void Resize(size_t& newCapacity);

    size_t capacity;
    size_t size;
    T* data;
};

template <typename T>
TVector<T>::TVector() : capacity(0), size(0), data(nullptr) {}

template <typename T>
TVector<T>::TVector(size_t newSize) {
    capacity = 2 * newSize;
    size = newSize;
    data = new T[capacity];
}

template <typename T>
TVector<T>::TVector(const TVector& other){
    capacity = other.capacity;
    size = other.size;
    data = new T[capacity];
    for(int i = 0; i < other.size; ++i) {
        data[i] = other.data[i];
    }
}

template <typename T>
TVector<T>::~TVector() {
    delete[] data;
}

template <typename T>
size_t TVector<T>::Size() const {
    return size;
}

template <typename T>
void TVector<T>::PushBack(const T& value) {
    if(size >= capacity) {
        size_t newCapacity = capacity == 0 ? 1 : capacity * 2;
        Resize(newCapacity);
    }
    data[size++] = value;
}

template <typename T>
void TVector<T>::Assign(size_t& newSize, T value) {
    for(size_t i = 0; i < newSize; ++i) {
        data[i] = value;
    }
}

template <typename T>
const T& TVector<T>::operator[](size_t index) const {
    return data[index];
}

template <typename T>
T& TVector<T>::operator[](size_t index) {
    return data[index];
}

template <typename T>
void TVector<T>::Resize(size_t& newCapacity) {
    T* newData = new T[newCapacity];
    for(int i = 0; i < size; ++i) {
        newData[i] = data[i];
    }
    delete[] data;
    data = newData;
    capacity = newCapacity;
}

struct TPair {
    uint64_t key;
    size_t value;
};


void radixSort(TVector<TPair>& arr) {
    const int BITS = 8;
    const int MASK = (1ull << BITS) - 1;
    const int RADIX = 1ull << BITS;
    const int VALUE_SIZE = 64;

    TVector<TPair> buffer(arr.Size());
    TVector<uint64_t> count(RADIX);

    for (int shift = 0; shift < VALUE_SIZE; shift += BITS) {
        for (size_t i = 0; i < RADIX; ++i) {
            count[i] = 0;   
        }

        for (size_t i = 0; i < arr.Size(); ++i) {
            uint64_t key = (arr[i].key >> shift) & MASK;
            count[key]++;
        }

        for (size_t i = 1; i < RADIX; ++i) {
            count[i] += count[i - 1];
        }

        for (int i = static_cast<int>(arr.Size()) - 1; i >= 0; --i) {
            uint64_t key = (arr[i].key >> shift) & MASK;
            buffer[--count[key]] = arr[i];
        }

        for (size_t i = 0; i < arr.Size(); ++i) {
            arr[i] = buffer[i];
        }
    }
}


int main() {
    std::ios::sync_with_stdio(false);
    std::cout.tie(0);
    std::cin.tie(0);

    TVector<TPair> arr;
    TVector<TString> val;
    TPair pair;

    char value[MAX_STRING_SIZE];
    while (std::cin >> pair.key >> value) {
        pair.value = arr.Size();
        arr.PushBack(pair);
        val.PushBack(TString(value));
    }

    radixSort(arr);

    for (size_t i = 0; i < arr.Size(); ++i) {
        std::cout << arr[i].key << "\t" << val[arr[i].value] << "\n";
    }
}
