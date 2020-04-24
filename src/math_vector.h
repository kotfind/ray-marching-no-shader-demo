#ifndef __MATH_VECTOR__
#define __MATH_VECTOR__

#include <initializer_list>
#include <cstdlib>
#include <math.h>
#include <algorithm>
#include <ostream>

#include <iostream> //TODO delete it

#define uint unsigned int

template <typename T, uint SIZE>
struct vec {
    T data[SIZE];

    // FIXME
    // #if SIZE > 0
    //     T &x = data[0];
    // #endif
    // #if SIZE > 1
    //     T &y = data[1];
    // #endif
    // #if SIZE > 2
    //     T &z = data[2];
    // #endif

    vec() {}
    vec(const T &val) {
        for (uint i = 0; i < SIZE; ++i) {
            data[i] = val;
        }
    }
    vec(std::initializer_list<T> list) {
        if (list.size() != SIZE) {
            exit(4);
        }

        std::copy(list.begin(), list.end(), data);
    }

    const T &operator[](const uint& i) const {
        if (i >= SIZE) {
            exit(5);
        }
        return data[i];
    }

    T &operator[](const uint& i) {
        if (i >= SIZE) {
            exit(5);
        }
        return data[i];
    }

    double len() const {
        double sqsum = 0;
        for (int i = 0; i < SIZE; ++i) {
            sqsum += data[i] * data[i];
        }
        return sqrt(sqsum);
    }

    vec<T, SIZE> normalize() const {
        double l = len();
        if (l) {
            return (*this)/l;
        } else {
            return (*this);
        }
    }
};

template <typename T, uint SIZE>
vec<T, SIZE> operator+(const vec<T, SIZE> &lhs, const vec<T, SIZE> &rhs) {
    vec<T, SIZE> res;
    for (int i = 0; i < SIZE; ++i) {
        res[i] = lhs[i] + rhs[i];
    }
    return res;
}

template <typename T, uint SIZE>
vec<T, SIZE> operator-(const vec<T, SIZE> &lhs, const vec<T, SIZE> &rhs) {
    vec<T, SIZE> res;
    for (int i = 0; i < SIZE; ++i) {
        res[i] = lhs[i] - rhs[i];
    }
    return res;
}

template <typename T, uint SIZE>
double operator*(const vec<T, SIZE> &lhs, const vec<T, SIZE> &rhs) {
    double ans = 0;
    for (int i = 0; i < SIZE; ++i) {
        ans += lhs[i] * rhs[i];
    }
    return ans;
}

template <typename T, uint SIZE>
vec<T, SIZE> operator*(const vec<T, SIZE> &lhs, const T &rhs) {
    vec<T, SIZE> res;
    for (int i = 0; i < SIZE; ++i) {
        res[i] = lhs[i] * rhs;
    }
    return res;
}

template <typename T, uint SIZE>
vec<T, SIZE> operator/(const vec<T, SIZE> &lhs, const T &rhs) {
    vec<T, SIZE> res;
    for (int i = 0; i < SIZE; ++i) {
        res[i] = lhs[i] / rhs;
    }
    return res;
}

template <typename T, uint SIZE>
std::ostream &operator<<(std::ostream &out, const vec<T, SIZE> &rhs) {
    for (int i = 0; i < SIZE; ++i) {
        out << rhs[i];
        if (i != SIZE - 1) {
            out << ' ';
        }
    }
    return out;
}

template <typename T, uint SIZE>
vec<T, SIZE> operator%(const vec<T, SIZE> &lhs, const T &rhs) {
    vec<T, SIZE> res;
    for (int i = 0; i < SIZE; ++i) {
        res[i] = lhs[i] - (lhs[i] / rhs);
    }
    return res;
}

// template <typename T, uint SIZE>
// vec<T, SIZE> operator+=(vec<T, SIZE> &lhs, const vec<T, SIZE>&rhs) {
//     return (lhs = lhs + rhs);
// }

typedef vec<double, 2> double2;
typedef vec<double, 3> double3;

#endif
