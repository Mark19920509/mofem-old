#pragma once

#include <iostream>
#include <assert.h>
#include <vector>
#include <array>
#include <Eigen/Dense>
#include <Eigen/Sparse>

const int Dynamic = Eigen::Dynamic;

// Variable/fixed sized vector
template <typename T> using Vector = Eigen::Matrix < T, Dynamic, 1 > ;
template <typename T, int R> using VectorF = Eigen::Matrix < T, R, 1 > ;

// Variable/fixed sized matrix
template <typename T> using Matrix = Eigen::Matrix < T, Dynamic, Dynamic > ;
template <typename T, int R, int C> using MatrixF = Eigen::Matrix < T, R, C > ;

// Sparse Matrix
template <typename T> using SparseMatrix = Eigen::SparseMatrix < T > ;
template <typename T> using Triplet = Eigen::Triplet < T > ;
template <typename T> using TripletVector = std::vector < Triplet<T> > ;


template<typename T>
class Array {
protected:
    T*  _contents;
    int _cols;
    int _rows;
    int _memsize;

    void releaseMemory(){
        if (_contents != nullptr) delete[] _contents;
    }

    void emptyInit(){
        _contents = nullptr;
        _cols = 0;
        _rows = 0;
        _memsize = 0;
    }

public:

    // Create completely empty array
    Array() : _rows(0), _cols(0), _contents(nullptr)
    {
    }

    // Create array with given number of rows and columns
    // NOT initialized
    Array(int rows, int cols = 1)
    {
        emptyInit();
        resize(rows, cols);
    }

    // Delete array
    ~Array()
    {
        releaseMemory();
    }

    // Allocate memory for this array
    void resize(int rows, int cols = 1){
        assert(rows > 0);
        assert(cols > 0);

        // No need to reallocate memory
        if (rows == _rows && cols == _cols) return;

        releaseMemory();
        _rows = rows;
        _cols = cols;
        _memsize = _rows * _cols;
        _contents = new T[_memsize];
    }

    void init(T val)
    {
        assert(_memsize > 0);
        std::fill(_contents, _contents + _memsize, val);
    }

    // Internal data
    inline T* data(){
        return _contents;
    }

    // Number of columns
    inline int cols() const
    {
        return _cols;
    }

    // Number of rows
    inline int rows() const
    {
        return _rows;
    }

    // 1D Indexing
    inline T& operator() (const int i)
    {
        assert(_contents != nullptr);
        assert(i < rows() && i >= 0);

        return _contents[i];
    }

    // 2D Indexing
    inline T& operator() (const int r, const int c)
    {
        assert(_contents != nullptr);
        assert(r < rows() && r >= 0);
        assert(c < cols() && c >= 0);

        return  _contents[r*_cols + c];
    }

    // 1D indexing (const)
    inline T const& operator() (const int i) const
    {
        assert(_contents != nullptr);
        assert(i < rows() && i >= 0);

        return _contents[i];
    }

    // 2D Indexing (const)
    inline T const& operator() (const int r, const int c) const
    {
        assert(_contents != nullptr);
        assert(r < rows() && r >= 0);
        assert(c < cols() && c >= 0);

        return  _contents[r*_cols + c];
    }

    // Copy constructor
    Array(const Array<T>& A){
        emptyInit();
        assert(A._contents != nullptr);
        resize(A._rows, A._cols);
        std::memcpy(_contents, A._contents, _memsize*sizeof(T));
    }

private:
    Array& operator= (const Array<T>&);
};


// Array with variables sized columns
template<typename T>
class ArrayVC{
    T* _contents;
    int* _row_start;
    int _rows;
    int* _cols;
    int _memsize;
    
    void releaseMemory(){
        if (_contents != nullptr) delete[] _contents;
        if (_cols != nullptr) delete[] _cols;
        if (_row_start != nullptr) delete[] _row_start;
    }

public:

    ArrayVC() : _rows(0), _cols(nullptr), _row_start(nullptr), _contents(nullptr)
    {
    }

    ArrayVC(int rows, std::vector<int> cols_per_row)
    {
        resize(rows, cols_per_row);
    }

    ArrayVC(int rows, int* const cols_per_row)
    {
        resize(rows, cols_per_row);
    }

    ~ArrayVC()
    {
        releaseMemory();
    }

    void resize(int rows, std::vector<int> cols_per_row){
        assert(rows > 0);

        releaseMemory();

        // Size information
        _rows = rows;
        _cols = new int[rows];

        // Keeps track of where each row starts
        _row_start = new int[rows];

        // Total size required
        _memsize = 0;
        for (int r = 0; r < rows; r++){
            _cols[r] = cols_per_row[r];
            _row_start[r] = _memsize;
            _memsize += cols_per_row[r];
        }
        _contents = new T[_memsize];
    }

    void resize(int rows, int* cols_per_row){
        assert(rows > 0);
        assert(cols_per_row != nullptr);
        //for (int r = 0; r < rows; r++) assert(cols_per_row[r] >0);

        releaseMemory();

        // Size information
        _rows = rows;
        _cols = new int[rows];

        // Keeps track of where each row starts
        _row_start = new int[rows];

        // Total size required
        _memsize = 0;
        for (int r = 0; r < rows; r++){
            _cols[r] = cols_per_row[r];
            _row_start[r] = _memsize;
            _memsize += cols_per_row[r];
        }
        _contents = new T[_memsize];
    }

    void resize(int rows, Array<int>& cols_per_row){
        assert(rows > 0);

        releaseMemory();

        // Size information
        _rows = rows;
        _cols = new int[rows];

        // Keeps track of where each row starts
        _row_start = new int[rows];

        // Total size required
        _memsize = 0;
        for (int r = 0; r < rows; r++){
            _cols[r] = cols_per_row.data()[r];
            _row_start[r] = _memsize;
            _memsize += cols_per_row.data()[r];
        }
        _contents = new T[_memsize];
    }

    void reshapeCols(int* cols_per_row){
        assert(cols_per_row != nullptr);
        int pos = 0;
        for (int r = 0; r < rows; r++){
            _cols[r] = cols_per_row[r];
            _row_start[r] = pos;
            pos += cols_per_row[r];
        }
    }

    inline int cols(int r) const{
        assert(r >= 0 && r < rows());
        assert(_cols != nullptr);
        return _cols[r];
    }

    inline int rows() const{
        return _rows;
    }

    inline int* cols() const{
        return _cols;
    }

    // Internal data
    inline T* data(){
        return _contents;
    }

    // Offsets for each rows
    inline int* offsets(){
        return _row_start;
    }

    T& operator() (const int r, const int c)
    {
        assert(_contents != nullptr);
        assert(r < rows() && r >= 0);
        assert(c < cols(r) && c >= 0);
        return _contents[_row_start[r] + c];
    }

    T const& operator() (const int r, const int c) const
    {
        assert(_contents != nullptr);
        assert(r < rows() && r >= 0);
        assert(c < cols(r) && c >= 0);
        return _contents[_row_start[r] + c];
    }

    ArrayVC(const ArrayVC<T>& A){
        _contents = nullptr;
        _row_start = nullptr;
        _cols = nullptr;
        _rows = 0;

        resize(A._rows, A._cols);
        assert(A._contents != nullptr);
        std::memcpy(_contents, A._contents, sizeof(T)*_memsize);
    }

private:
    ArrayVC& operator= (const ArrayVC<T>&);
};


template <class T>
void copy(std::vector<T>& vec, Array<T>& arr){
    int size = (int) vec.size();
    arr.resize(size);
    for (int i = 0; i < size; i++) arr(i) = vec[i];
}

template <class T>
void copy(std::vector<std::vector<T>>& vec, ArrayVC<T>& arr){
    int rows = vec.size();
    std::vector<int> cols_per_row(rows);

    for (int i = 0; i < rows; i++){
        cols_per_row[i] = vec[i].size();
    }

    arr.resize(rows, cols_per_row);
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < cols_per_row[r]; c++){
            arr(r, c) = vec[r][c];
        }
    }
}


template <class T, int NCOL>
void copy(std::vector<std::array<T,NCOL>>& vec, Array<T>& arr){
    int rows = vec.size();
    arr.resize(rows, NCOL);
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < NCOL; c++){
            arr(r, c) = vec[r][c];
        }
    }
}

template <class T, int NCOL1, int NCOL2>
void copy(std::vector<std::array<T, NCOL1>>& vec, MatrixF<T, Dynamic, NCOL2>& mat){
    assert(NCOL1 == NCOL2);
    int rows = vec.size();
    mat.resize(rows, NCOL1);
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < NCOL1; c++){
            mat(r, c) = vec[r][c];
        }
    }
}

// Permutation copy
template <class T>
void copy(std::vector<T>& vec, Array<T>& arr, std::vector<int>& perm){
    int size = (int)vec.size();
    arr.resize(size);
    for (int i = 0; i < size; i++) arr(i) = vec[perm[i]];
}

template <class T>
void copy(std::vector<std::vector<T>>& vec, ArrayVC<T>& arr, std::vector<int>& perm){
    int rows = vec.size();
    std::vector<int> cols_per_row(rows);

    for (int r = 0; r < rows; r++){
        cols_per_row[r] = vec[perm[r]].size();
    }

    arr.resize(rows, cols_per_row);
    for (int r = 0; r < rows; r++){
        for (int c = 0; c < cols_per_row[r]; c++){
            arr(r, c) = vec[perm[r]][c];
        }
    }
}

#include <string>

namespace std{
    template <class T>
    string to_string(Array<T>& arr){
        std::string str;
        for (int r = 0; r < arr.rows(); r++){
            for (int c = 0; c < arr.cols(); c++){
                str += std::to_string(arr(r, c)) + " ";
            }
            if (arr.cols() > 1) str += "\n";
        } 
        return str;
    }

    template <class T>
    string to_string(std::vector<T>& arr){
        std::string str;
        for (std::size_t r = 0; r < arr.size(); r++){
            str += std::to_string(arr[r]) + " ";
        } str += "\n";
        return str;
    }

    template <class T>
    string to_string(ArrayVC<T>& arr){
        std::string str;
        for (int r = 0; r < arr.rows(); r++){
            for (int c = 0; c < arr.cols(r); c++){
                str += std::to_string(arr(r, c)) + " ";
            }
            if (arr.cols(r) == 0) str += "EMPTY";
            str += "\n";
        }
        return str;
    }
}
