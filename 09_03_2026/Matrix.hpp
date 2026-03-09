#ifndef MATRIX_HPP
#define MATRIX_HPP
#include <iostream>
#include <fstream>
#include <cstddef> // for size_t
//#include <cstring> // for memcpy
#include <algorithm> // for copy

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
        std::copy(other.data, other.data + rows * cols, data); // std::memcpy(data, other.data, rows * cols * sizeof(T));
    }

    // Move constructor
    Matrix(Matrix&& other) noexcept {
        data = other.data;
        rows = other.rows;
        cols = other.cols;

        other.data = nullptr;
        other.rows = 0;
        other.cols = 0;
    }

    // Copy-and-Swap idiom or Direct (instead of the default implementation)
    Matrix& operator=(const Matrix& other) {
        if (this != &other) { // protection against self-assignment (m = m)
            delete[] data;
            rows = other.rows;
            cols = other.cols;
            data = new T[rows * cols];
            std::copy(other.data, other.data + rows * cols, data); // std::memcpy(data, other.data, rows * cols * sizeof(T));
        }
        return *this;
    }

#ifdef USE_STD_SWAP
    // Copy-and-Swap by std::swap (use move)
    Matrix& operator=(Matrix other){
        std::swap(data, other.data);
        std::swap(rows, other.rows);
        std::swap(cols, other.cols);

        return *this;
    }

#else
    // Move assignment
    Matrix& operator=(Matrix&& other) noexcept {
        if (this != &other) {
            delete[] data;

            data = other.data;
            rows = other.rows;
            cols = other.cols;

            other.data = nullptr;
            other.rows = 0;
            other.cols = 0;
        }

        return *this;
    }
#endif

    // ======================
    // Fill
    // ======================

    void fill(){
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++){
                //data[i][j] = rand() % 10 + 1;
                (*this)[i][j] = 1;
            }
    }

    // ======================
    // Input
    // ======================

    friend std::istream& operator>>(std::istream& is, Matrix& m) {
        for (size_t i = 0; i < m.rows; i++) {
            for (size_t j = 0; j < m.cols; j++) {
                std::cout << "Enter [" << i << "][" << j << "] = ";
                is >> m[i][j];
            }
        }
        return is;
    }

    // ======================
    // Output
    // ======================

    friend std::ostream& operator<<(std::ostream& os, const Matrix& m) {
        for (size_t i = 0; i < m.rows; ++i){
            for (size_t j = 0; j < m.cols; ++j){
                os << m[i][j];
                if (j + 1 < m.cols)
                    os << ' ';
            }
            os << '\n';
        }
        return os;
    }

    // ======================
    // Addition
    // ======================

    Matrix operator+(const Matrix& other) {
        if (rows != other.rows || cols != other.cols)
            throw std::runtime_error("Matrix size mismatch");

        Matrix result(rows, cols);

        for (size_t i = 0; i < rows * cols; ++i)
            result.data[i] = data[i] + other.data[i];

        return result;
    }

    Matrix& operator+=(const Matrix& other) {
        if (rows != other.rows || cols != other.cols)
            throw std::runtime_error("Matrix size mismatch");

        for (size_t i = 0; i < rows * cols; ++i)
            data[i] += other.data[i];

        return *this;
    }

    // ======================
    // Subtraction
    // ======================

    Matrix operator-(const Matrix& other) {
        if (rows != other.rows || cols != other.cols)
            throw std::runtime_error("Matrix size mismatch");

        Matrix result(rows, cols);

        for (size_t i = 0; i < rows * cols; ++i)
            result.data[i] = data[i] - other.data[i];

        return result;
    }

    Matrix& operator-=(const Matrix& other) {
        if (rows != other.rows || cols != other.cols)
            throw std::runtime_error("Matrix size mismatch");

        for (size_t i = 0; i < rows * cols; ++i)
            data[i] -= other.data[i];

        return *this;
    }

    // ======================
    // Multiplication
    // ======================

    Matrix operator*(const Matrix& other) {
        if (cols != other.rows)
            throw std::runtime_error("Matrix multiplication mismatch");

        Matrix result(rows, other.cols);

#if defined(MATRIX_BLOCK_MULT) || defined(MATRIX_TRANSPOSE_MULT)
        size_t m = rows;
        size_t n = cols;
        size_t p = other.cols;
        T* A = data;
        // T* B = ...
        T* C = result.data;
#else
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < other.cols; ++j)
                result[i][j] = 0;
#endif

#ifdef MATRIX_FAST_MULT
        // cache-friendly version (i-k-j)
        for (size_t i = 0; i < rows; ++i)
            for (size_t k = 0; k < cols; ++k) {
                T dataIJTemp = (*this)[i][k]; // read optimization
                for (size_t j = 0; j < other.cols; ++j)
                    result[i][j] += dataIJTemp * other[k][j];
            }
#elif defined(MATRIX_BLOCK_MULT)
        // blocked multiplication
        T* B = other.data;
        std::fill(C, C + m * p, T(0));
        const size_t BS = 32;   // block size (L1-cache optimization)
        for (size_t ii = 0; ii < m; ii += BS)
            for (size_t kk = 0; kk < n; kk += BS)
                for (size_t jj = 0; jj < p; jj += BS)
                    for (size_t i = ii; i < std::min(ii + BS, m); ++i)
                        for (size_t k = kk; k < std::min(kk + BS, n); ++k) {
                            T a = A[i * n + k]; // read optimization
                            for (size_t j = jj; j < std::min(jj + BS, p); ++j)
                                C[i * p + j] += a * B[k * p + j];
                        }
#elif defined (MATRIX_TRANSPOSE_MULT)
        // multiplication by transpose
        T* B = other.transpose().data;
        for (size_t i = 0; i < m; ++i)
            for (size_t j = 0; j < p; ++j) {
                T sum = 0;
                for (size_t k = 0; k < n; ++k)
                    sum += A[i * n + k] * B[j * n + k];
                C[i * p + j] = sum;
            }
#else
        // classical version (i-j-k)
        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < other.cols; ++j)
                for (size_t k = 0; k < cols; ++k)
                    result[i][j] += (*this)[i][k] * other[k][j];
#endif

        return result;
    }

    Matrix& operator*=(const Matrix& other) {
        *this = (*this) * other;
        
        return *this;
    }

    // ======================
    // Power
    // ======================

    Matrix pow(int n) {
        if (cols != rows)
            throw std::runtime_error("Matrix power mismatch");

        if (n >= 1)
            for (size_t i = 1; i < n; ++i)
                *this = (*this) * (*this);
        else {
            for (size_t i = 0; i < rows; ++i)
                for (size_t j = 0; j < cols; ++j)
                    if (i == j)
                        (*this)[i][j] = 1;
                    else
                        (*this)[i][j] = 0;
        }

        return *this;
    }

    Matrix pow_(int n) {
        if (cols != rows)
            throw std::runtime_error("Matrix power mismatch");

        if (n >= 1) {
            if(n >= 2)
                Matrix reult = (*this) * (*this);
            for (size_t i = 2; i < n; ++i)
                *this = (*this) * (*this);
        }
        else {
            for (size_t i = 0; i < rows; ++i)
                for (size_t j = 0; j < cols; ++j)
                    if (i == j)
                        (*this)[i][j] = 1;
                    else
                        (*this)[i][j] = 0;
        }

        return *this;
    }

    // ======================
    // Transpose
    // ======================

    Matrix transpose() {
        Matrix result(cols, rows);

        for (size_t i = 0; i < rows; ++i)
            for (size_t j = 0; j < cols; ++j)
                result[j][i] = (*this)[i][j];

        return result;
    }

};
#endif