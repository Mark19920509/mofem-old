#pragma once

#include <Util/Array.h>

#include <vector>
#include <tuple>
#include <unordered_set>
#include <algorithm>

using RowCol = std::tuple<int, int>;
using SparseArrayMask = std::vector < RowCol >;

template <typename T>
using RowColVal = std::tuple<int, int, T>;

template <typename T>
using SparseArrayValues = std::vector < RowColVal<T> > ;

template <typename T>
class SparseArray{
private:
    int _nnz  = 0;
    int _nrows = 0;
    int _ncols = 0;

    int* _row = nullptr;
    int* _col = nullptr;
    T*   _val = nullptr;

    void releaseMemory(){
        if (_row != nullptr) delete[] _row;
        if (_col != nullptr) delete[] _col;
        if (_val != nullptr) delete[] _val;
    }

public:
    SparseArray(){}

    void setMask(SparseArrayMask rc_list){
        releaseMemory();

        // Sort by row and column
        std::sort(rc_list.begin(), rc_list.end());

        // Remove duplicates
        RowCol prev_rc = RowCol(-1, -1);
        for (auto it = rc_list.begin(); it != rc_list.end(); it++){
            RowCol rc = (*it);
            if (rc == prev_rc){
                it = rc_list.erase(it) - 1;
            } else {
                prev_rc = rc;
            }
        }

        // Get number of non-zeros and rows
        _nnz = rc_list.size();
        _nrows = std::get<0>(rc_list.back()) + 1;

        // Allocate memory
        _row = new int[_nrows+1];
        _col = new int[_nnz];
        _val = new T[_nnz];

        // Fill columns
        int idx = 0;
        for (auto rc : rc_list){
            _col[idx] = std::get<1>(rc);
            if (_col[idx] + 1 > _ncols) _ncols = _col[idx] + 1;
            idx++;
        }
        
        // Fill rows
        idx = 0;
        for (int r = 0; r < _nrows; r++){
            while (std::get<0>(rc_list[idx]) < r) idx++;
            _row[r] = idx;
        }
        _row[_nrows] = _nnz;
    }

    ~SparseArray(){
        releaseMemory();
    }

    void setValues(SparseArrayValues<T> rcv_list){

        // Sort (row-col-val) list 
        std::sort(rcv_list.begin(), rcv_list.end());

        // Zero out
        std::fill(_val, _val + _nnz, 0);
    
        int prev_row = -1;
        int idx;
        for (auto rcv : rcv_list){
            int r = std::get<0>(rcv);
            int c = std::get<1>(rcv);
            T   v = std::get<2>(rcv);

            // Check if new row
            if (prev_row != r) idx = _row[r];

            // Continue searching on row
            while(idx < _row[r + 1]){
                if (_col[idx] == c){
                    _val[idx++] += v;
                    break;
                }
                idx++;
            }
            prev_row = r;
        }
    }

    void vecMul(Array<T> const& vec, Array<T>& out){
        assert(_ncols == vec.nrows());

        out.setSize(_nrows);

        for (int r = 0; r < _nrows; r++){
            T out_val = 0;
            for (int i = _row[r]; i < _row[r + 1]; i++)
                out_val += _val[i] * vec(_col[i]);
            out(r) = out_val;
        }

    }

    std::string visualizeMask(){
        std::string out = "";

        for (int r = 0; r < _nrows; r++){
            int idx = _row[r];
            for (int c = 0; c < _ncols; c++){
                if (idx < _row[r+1] && _col[idx] == c){
                    idx++;
                    out += "X";
                }else{
                    out += "-";
                }
            }
            out += "\n";
        }

        return out;
    }

};