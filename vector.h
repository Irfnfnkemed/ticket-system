#ifndef TICKET_SYSTEM_VECTOR_H
#define TICKET_SYSTEM_VECTOR_H

#include "exceptions.h"

template<class T>
struct my_type_traits;

template<class T>
struct my_iterator_traits;

template<typename T>
class vector {

private:

    T **data;//动态数组，每个元素指向vector的元素
    int len_max;//vector可容许的最大长度,1-based
    int len;//vector已使用的长度,1-based

public:

    class iterator;

    class const_iterator;

    class iterator {
        friend class my_type_traits<iterator>;

        friend class my_type_traits<const_iterator>;

        friend class const_iterator;

    private:
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::random_access_iterator_tag;

        vector *vec;
        int index;//当前迭代器指向元素在vector中的索引,0-based

    public:

        iterator() : vec(nullptr) {}

        iterator(vector *vec_, int index_) :
                vec(vec_), index(index_) {}

        iterator(const iterator &it) :
                vec(it.vec), index(it.index) {}

        iterator operator+(const int &n) const {
            if (index + n > vec->len || index + n < 0) { throw runtime_error(); }
            else { return iterator(vec, index + n); }
        }

        iterator operator-(const int &n) const {
            if (index - n > vec->len || index - n < 0) { throw runtime_error(); }
            else { return iterator(vec, index - n); }
        }

        int operator-(const iterator &rhs) const {
            if (vec != rhs.vec) { throw invalid_iterator(); }
            else return index - rhs.index;
        }

        iterator &operator+=(const int &n) {
            if (index + n > vec->len || index + n < 0) { throw runtime_error(); }
            else {
                index += n;
                return *this;
            }
        }

        iterator &operator-=(const int &n) {
            if (index - n > vec->len || index - n < 0) { throw runtime_error(); }
            else {
                index -= n;
                return *this;
            }
        }

        iterator operator++(int) {
            if (index + 1 > vec->len) { throw runtime_error(); }
            else {
                ++index;
                return iterator(vec, index - 1);
            }
        }

        iterator &operator++() {
            if (index + 1 > vec->len) { throw runtime_error(); }
            else {
                ++index;
                return *this;
            }
        }

        iterator operator--(int) {
            if (index - 1 < 0) { throw runtime_error(); }
            else {
                --index;
                return iterator(vec, index + 1);
            }
        }

        iterator &operator--() {
            if (index - 1 < 0) { throw runtime_error(); }
            else {
                --index;
                return *this;
            }
        }

        T &operator*() const {
            if (index == vec->len) { throw runtime_error(); }
            return *(vec->data[index]);
        }

        T *operator->() const {
            if (index == vec->len) { throw runtime_error(); }
            return vec->data[index];
        }

        bool operator==(const iterator &rhs) const {
            return vec == rhs.vec && index == rhs.index;
        }

        bool operator==(const const_iterator &rhs) const {
            return vec == rhs.vec && index == rhs.index;
        }

        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }

        bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
    };

    class const_iterator {

        friend class my_type_traits<iterator>;

        friend class my_type_traits<const_iterator>;

        friend class const_iterator;

    private:

        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using iterator_category = std::random_access_iterator_tag;

        const vector *vec;
        int index;//当前迭代器指向元素在vector中的索引,0-based

    public:
        const_iterator() : vec(nullptr) {}

        const_iterator(const vector *vec_, int index_) :
                vec(vec_), index(index_) {}

        const_iterator(const const_iterator &it) :
                vec(vec), index(it.index) {}

        const_iterator operator+(const int &n) const {
            if (index + n > vec->len || index + n < 0) { throw runtime_error(); }
            else { return iterator(vec, index + n); }
        }

        const_iterator operator-(const int &n) const {
            if (index - n > vec->len || index - n < 0) { throw runtime_error(); }
            else { return iterator(vec, index - n); }
        }

        int operator-(const const_iterator &rhs) const {
            if (vec != rhs.vec) { throw invalid_iterator(); }
            else return index - rhs.index;
        }

        const_iterator &operator+=(const int &n) {
            if (index + n > vec->len || index + n < 0) { throw runtime_error(); }
            else {
                index += n;
                return *this;
            }
        }

        const_iterator &operator-=(const int &n) {
            if (index - n > vec->len || index - n < 0) { throw runtime_error(); }
            else {
                index -= n;
                return *this;
            }
        }

        const_iterator operator++(int) {
            if (index + 1 > vec->len) { throw runtime_error(); }
            else {
                ++index;
                return iterator(vec, index - 1);
            }
        }

        const_iterator &operator++() {
            if (index + 1 > vec->len) { throw runtime_error(); }
            else {
                ++index;
                return *this;
            }
        }

        const_iterator operator--(int) {
            if (index - 1 < 0) { throw runtime_error(); }
            else {
                --index;
                return iterator(vec, index + 1);
            }
        }

        const_iterator &operator--() {
            if (index - 1 < 0) { throw runtime_error(); }
            else {
                --index;
                return *this;
            }
        }

        T operator*() const {
            if (index == vec->len) { throw runtime_error(); }
            return *(vec->data[index]);
        }

        bool operator==(const iterator &rhs) const {
            return vec == rhs.vec && index == rhs.index;
        }

        bool operator==(const const_iterator &rhs) const {
            return vec == rhs.vec && index == rhs.index;
        }

        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }

        bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
    };

    vector() {
        data = new T *[128];
        len_max = 128;
        len = 0;
    }

    vector(const vector &other) {
        len_max = other.len_max;
        len = other.len;
        data = new T *[len_max];
        for (int i = 0; i < len; ++i) { data[i] = new T(*other.data[i]); }
    }

    ~vector() {
        for (int i = 0; i < len; ++i) { delete data[i]; }
        delete[]data;
    }

    vector &operator=(const vector &other) {
        if (this == &other) { return *this; }
        else {
            for (int i = 0; i < len; ++i) { delete data[i]; }
            delete[]data;
            len_max = other.len_max;
            len = other.len;
            data = new T *[len_max];
            for (int i = 0; i < len; ++i) { data[i] = new T(*other.data[i]); }
            return *this;
        }
    }

    T &at(const size_t &pos) {
        if (pos > len - 1 || pos < 0) { throw index_out_of_bound(); }
        else { return *data[pos]; }
    }

    const T &at(const size_t &pos) const {
        if (pos > len - 1 || pos < 0) { throw index_out_of_bound(); }
        else { return *data[pos]; }
    }

    T &operator[](const size_t &pos) {
        if (pos > len - 1 || pos < 0) { throw index_out_of_bound(); }
        else { return *data[pos]; }
    }

    const T &operator[](const size_t &pos) const {
        if (pos > len - 1 || pos < 0) { throw index_out_of_bound(); }
        else { return *data[pos]; }
    }

    const T &front() const {
        if (len == 0) { throw container_is_empty(); }
        else { return *data[0]; }
    }

    const T &back() const {
        if (len == 0) { throw container_is_empty(); }
        else { return *data[len - 1]; }
    }

    iterator begin() { return iterator(this, 0); }

    const_iterator cbegin() const { return const_iterator(this, 0); }

    iterator end() { return iterator(this, len); }

    const_iterator cend() const { return const_iterator(this, len); }

    bool empty() const { return len == 0; }

    size_t size() const { return len; }

    void clear() {
        for (int i = 0; i < len; ++i) { delete data[i]; }
        delete[]data;
        data = new T *[128];
        len_max = 128;
        len = 0;
    }

    iterator insert(iterator pos, const T &value) {
        if (pos.vec != this) { throw invalid_iterator(); }
        else if (pos.index == len) { throw runtime_error(); }
        else if (len == len_max) {
            T **tmp = new T *[len_max * 2];
            for (int i = 0; i < pos.index; ++i) {
                tmp[i] = data[i];
            }
            tmp[pos.index] = new T(value);
            for (int i = pos.index + 1; i <= len_max; ++i) {
                tmp[i] = data[i - 1];
            }
            delete[]data;
            data = tmp;
            len_max *= 2;
            ++len;
        } else {
            for (int i = len; i >= pos.index + 1; --i) {
                data[i] = data[i - 1];
            }
            data[pos.index] = new T(value);
            ++len;
        }
        return iterator(this, pos.index);
    }

    iterator insert(const size_t &ind, const T &value) {
        if (ind >= len || ind < 0) { throw index_out_of_bound(); }
        else if (len == len_max) {
            T **tmp = new T *[len_max * 2];
            for (int i = 0; i < ind; ++i) {
                tmp[i] = data[i];
            }
            tmp[ind] = new T(value);
            for (int i = ind + 1; i <= len_max; ++i) {
                tmp[i] = data[i - 1];
            }
            delete[]data;
            data = tmp;
            len_max *= 2;
            ++len;
        } else {
            for (int i = len; i >= ind; --i) {
                data[i] = data[i - 1];
            }
            data[ind] = new T(value);
            ++len;
        }
        return iterator(this, ind);
    }

    iterator erase(iterator pos) {
        if (pos.vec != this) { throw invalid_iterator(); }
        else if (pos.index == len) { throw runtime_error(); }
        else if (len == 0) { throw container_is_empty(); }
        else if (len - 1 >= len_max / 4 || len_max <= 128) {
            delete data[pos.index];
            for (int i = pos.index; i < len - 1; ++i) { data[i] = data[i + 1]; }
            --len;
        } else {
            T **tmp = new T *[len_max / 2];
            for (int i = 0; i < pos.index; ++i) { tmp[i] = data[i]; }
            delete data[pos.index];
            for (int i = pos.index; i < len - 1; ++i) { tmp[i] = data[i + 1]; }
            delete[]data;
            data = tmp;
            --len;
            len_max /= 2;
        }
        return iterator(this, pos.index);
    }

    iterator erase(const size_t &ind) {
        if (ind >= len || ind < 0) { throw index_out_of_bound(); }
        else if (ind == len) { throw runtime_error(); }
        else if (len == 0) { throw container_is_empty(); }
        else if (len - 1 >= len_max / 4 || len_max <= 128) {
            delete data[ind];
            for (int i = ind; i < len - 1; ++i) { data[i] = data[i + 1]; }
            --len;
        } else {
            T **tmp = new T *[len_max / 2];
            for (int i = 0; i < ind; ++i) { tmp[i] = data[i]; }
            delete data[ind];
            for (int i = ind; i < len - 1; ++i) { tmp[i] = data[i + 1]; }
            delete[]data;
            data = tmp;
            --len;
            len_max /= 2;
        }
        return iterator(this, ind);
    }

    void push_back(const T &value) {
        if (len == len_max) {
            T **tmp = new T *[len_max * 2];
            for (int i = 0; i < len_max; ++i) {
                tmp[i] = new T(*data[i]);
            }
            tmp[len_max] = new T(value);
            for (int i = 0; i < len_max; ++i) { delete data[i]; }
            delete[]data;
            data = tmp;
            len_max *= 2;
            ++len;
        } else {
            data[len] = new T(value);
            ++len;
        }
    }

    void pop_back() {
        if (len == 0) { throw container_is_empty(); }
        if (len - 1 >= len_max / 4 || len_max <= 128) {
            delete data[len - 1];
            --len;
        } else {
            T **tmp = new T *[len_max / 2];
            for (int i = 0; i < len - 1; ++i) {
                tmp[i] = data[i];
            }
            delete data[len - 1];
            delete[]data;
            data = tmp;
            len_max /= 2;
            --len;
        }
    }

    void sort() {
        T **tmp = new T *[len];
        sort(0, len - 1, tmp);
        delete[]tmp;
    }

    void sort(int l, int r, T **tmp) {
        if (l >= r) { return; }
        int m = (l + r) >> 1;
        sort(l, m, tmp);
        sort(m + 1, r, tmp);
        int l_point = l, r_point = m + 1, tmp_point = l;
        while (l_point <= m && r_point <= r) {
            if (*(data[l_point]) < *(data[r_point])) {
                tmp[tmp_point++] = data[l_point++];
            } else {
                tmp[tmp_point++] = data[r_point++];
            }
        }
        while (l_point <= m) { tmp[tmp_point++] = data[l_point++]; }
        while (r_point <= r) { tmp[tmp_point++] = data[r_point++]; }
        for (int i = l; i <= r; ++i) { data[i] = tmp[i]; }
    }

    struct my_true_type {
    };//标识真

    struct my_false_type {
    };//标识假

    template<typename T_>
    struct my_type_traits {
        using value_type = typename T_::value_type;
        using type_assignable = my_true_type;
        using type_default_constructor = my_true_type;
        using type_copy_constructor = my_true_type;
        using type_destructor = my_true_type;
    };

    template<typename T_>
    struct my_iterator_traits {
        using value_type = typename T::value_type;
        using difference_type = typename T::difference_type;
        using pointer = typename T::pointer;
        using reference = typename T::reference;
        using iterator_category = typename T::iterator_category;
        using iterator_assignable = my_true_type;
        using iterator_default_constructor = my_true_type;
        using iterator_copy_constructor = my_true_type;
        using iterator_destructor = my_true_type;
    };
};

#endif //TICKET_SYSTEM_VECTOR_H
