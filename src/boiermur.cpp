/* include area */
#include "boiermur.hpp"
#include <algorithm>
#include <assert.h>
#include <cstring>
#include <iostream>
#include <string.h>

using std::size_t;
using std::string;
using std::vector;
using std::max;
using BM::Pattern;
using BM::RString;

/**
 * Calculates the Z number of \c s using the Z algorithm.
 *
 * @param z   Z array (output).
 * @param s   String to preprocess.
 * @param len The length of \c s.
 */
template <typename S>
void BM::Z(vector<size_t>& z, S s) {
    z[0] = s.length();

    size_t l = 0, r = 0;
    for (size_t k = 1; k < s.length(); k++) {
        /* checks if the index is inside a Z box */
        if (k > l) {
            /* finds the current Z box */
            size_t n = 0;
            while (n + k < s.length() && s[n] == s[n + k]) {
                n += 1;
            }

            /* sets the current Z box limits */
            z[k] = n;
            l = k;
            r = k + n - 1;
        } else {
            /* checks if the match could be expanded */
            if (z[k - l] < r - k + 1) {
                /* copies the mirrored Z value */
                z[k] = z[k - l];
            } else {
                /* expands the Z box */
                size_t n = r + 1;
                while (n < s.length() && s[n] == s[r + 1 - k]) {
                    n += 1;
                }

                /* updates the current Z box limits */
                z[k] = n - k;
                l = k;
                r = n - 1;
            }
        }
    }
}

/**
 * Pattern constructor implementation.
 *
 * @param pattern C string to be used as a pattern.
 */
Pattern::Pattern(const char* pattern) : Pattern(pattern, strlen(pattern)) {}

Pattern::Pattern(const char* pattern, size_t len) : pattern(pattern), len(len) {
    /* builds the bad-character rule table */
    for (size_t i = this->len; i > 0; i--) {
        assert(pattern[i - 1] >= 0);
        size_t c = static_cast<size_t>(pattern[i - 1]);
        this->bad_char_table[c].push_back(i - 1);
    }

    /* builds the good-suffix rule table */
    vector<size_t> z_array;
    z_array.reserve(this->len);
    BM::Z(z_array, RString{pattern, this->len});

    this->Lp = new int[this->len + 1]; // TODO: use vector
    this->Lp[this->len] = 1;           // used by the good-suffix rule when no character matched
    memset(this->Lp, 0, this->len * sizeof(int));
    for (size_t i = 0; i < this->len - 1; i++) {
        size_t N_j = z_array[this->len - i - 1];
        if (N_j == 0) {
            continue;
        }
        size_t k = this->len - N_j;
        this->Lp[k] = this->len - i - 1;
    }

    this->lp = new int[this->len]; // TODO: use vector
    size_t longest = 0;
    for (size_t i = this->len; i > 0; i--) {
        if (this->len - i + 1 == z_array[i - 1]) {
            longest = max(longest, z_array[i - 1]);
        }
        this->lp[i - 1] = this->len - longest;
    }
}

Pattern::Pattern(const string& T) : Pattern(T.c_str(), T.length()) {}

Pattern::~Pattern() {
    delete[] this->Lp;
    delete[] this->lp;
}

/**
 * Finds the repetitions of pattern in the given text.
 *
 * @param T Text where the pattern is searched.
 */
vector<size_t> Pattern::find(const string& T) const {
    vector<size_t> matches;

    if (T.length() < this->len) {
        return matches;
    }

    size_t shift = this->len;
    while (shift <= T.length()) {
        size_t i = this->len;
        size_t k = shift;

        while (i > 0 && this->pattern[i - 1] == T[k - 1]) {
            i -= 1;
            k -= 1;
        }

        if (i == 0) {
            matches.push_back(k);
            shift += std::max(this->len - this->lp[1], size_t(1));
        } else {
            size_t bc_shift = this->bad_char_shift(T[k - 1], i - 1);
            size_t gs_shift = this->good_suffix_shift(i);

            shift += std::max(bc_shift, gs_shift);
        }
    }
    return matches;
}

vector<size_t> Pattern::find(const char* T) const {
    return this->find(string{T});
}

std::ostream& BM::operator<<(std::ostream& os, const BM::Pattern& p) {
    // os << "[";
    // for (size_t i = 0; i < 256; i++) {
    // size_t i = static_cast<size_t>('e');
    // os << "[";
    // for( size_t j; j < p.bad_char_table[i].size() - 1; j++ ) {
    //    os << p.bad_char_table[i][j] << ", ";
    //}
    // os << p.bad_char_table[i][p.bad_char_table[i].size()-1];
    // os << "]" << std::endl;
    //}
    // os << "]" << std::endl;

    vector<size_t> z_array;
    z_array.reserve(p.len);
    BM::Z(z_array, RString{"ana banana", p.len});

    os << "Z  = [";
    for (size_t i = 0; i < p.len - 1; i++) {
        os << z_array[i] << ", ";
    }
    os << z_array[p.len - 1] << "]" << std::endl;

    os << "Lp = [ ";
    for (size_t i = 0; i < p.len - 1; i++) {
        os << p.Lp[i] << ", ";
    }
    os << p.Lp[p.len - 1] << "]" << std::endl;

    os << "lp = [";
    for (size_t i = 0; i < p.len - 1; i++) {
        os << p.lp[i] << ", ";
    }
    os << p.lp[p.len - 1] << "]" << std::endl;

    return os;
}
