#include <cstddef> // for size_t
#include <cstring> // for memcpy

template <typename T>
class Matrix {
private:
    T* data;
    size_t rows;
    size_t cols;

public:
    Matrix(size_t r, size_t c) : rows(r), cols(c) {
        data = new T[r * c];
    }

    ~Matrix() {
        delete[] data;
    }

    // first operato []
    T* operator[](size_t i) {
        return data + (i * cols); // second operator [] will automatically apply to this pointer
    }

    // constant read version
    const T* operator[](size_t i) const {
        return data + (i * cols);
    }

    T* raw() { return data; }

    size_t get_rows() const { return rows; }
    size_t get_cols() const { return cols; }

    // Deep Copy (instead of the default implementation)
    Matrix(const Matrix& other) : rows(other.rows), cols(other.cols) {
        data = new T[rows * cols];
        std::memcpy(data, other.data, rows * cols * sizeof(T));
    }

    // Copy-and-Swap idiom or Direct (instead of the default implementation)
    Matrix& operator=(const Matrix& other) {
        if (this != &other) { // protection against self-assignment (m = m)
            delete[] data;
            rows = other.rows;
            cols = other.cols;
            data = new T[rows * cols];
            std::memcpy(data, other.data, rows * cols * sizeof(T));
        }
        return *this;
    }

};

int main() {

    int n = 534;

    Matrix<int> m(n, n);
    m[2][5] = 42;     
    int val = m[2][5];

    return 0;
}