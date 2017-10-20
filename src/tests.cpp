/* include area */
#include "boiermur.hpp"
#include <string.h>
#include <exception>
#include <iostream>
#include <vector>
#include <iterator>

using std::size_t;
using std::vector;
using std::string;
using std::ostream;
using std::cout;
using std::endl;

/* auxiliary macros */
#define STR(s) #s
#define XSTR(s) STR(s)

#define GREEN_TEXT(text) "\x1B[1;92m" text "\x1B[0m"
#define RED_TEXT(text) "\x1B[1;31m" text "\x1B[0m"

/* test macros */
#define ASSERT(e)                     \
    if (!(e))                         \
    {                                 \
        throw XSTR(__LINE__) ": " #e; \
    }

#define Z_TEST(text, expected)                                                \
    do                                                                        \
    {                                                                         \
        string s{text};                                                       \
        size_t *z_num = new size_t[s.length()];                               \
        BM::Z(z_num, BM::RString{s});                                         \
        for (size_t i = 0; i < expected.size(); i++)                          \
        {                                                                     \
            if (z_num[i] != expected[i])                                      \
            {                                                                 \
                cout << "at iteration " << i << endl;                         \
                cout << "expected: " << expected << endl;                     \
                cout << "obtained: " << to_vector(z_num, s.length()) << endl; \
                ASSERT(z_num[i] == expected[expected.size() - i - 1]);        \
            }                                                                 \
        }                                                                     \
    } while (0)


template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v)
{
    if (!v.empty())
    {
        out << '[';
        std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
        out << "\b\b]";
    }
    return out;
}

vector<size_t> to_vector(size_t *a, size_t len)
{
    vector<size_t> v;
    for (size_t i = 0; i < len; i++)
    {
        v.push_back(a[i]);
    }
    return std::move(v);
}

/**
 * Tests the Z algorithm implementation (on a reversed string).
 */
void test_z()
{
    {
        const char *test = "abcabcd";
        vector<size_t> expected = {7, 0, 0, 0, 0, 0, 0};

        Z_TEST(test, expected);
    }
    {
        const char *test = "dcbacba";
        vector<size_t> expected = {7, 0, 0, 3, 0, 0, 0};

        Z_TEST(test, expected);
    }
    {
        const char *test = "banana";
        vector<size_t> expected = {6, 0, 3, 0, 1, 0};

        Z_TEST(test, expected);
    }
    {
        const char *test = "aaaaaaaaaabbbbb";
        vector<size_t> expected = {15, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        Z_TEST(test, expected);
    }
}

/**
 * Tests the final Boyer-Moore algorithm.
 */
void test_boiermur()
{
#define TEST_MATCHES(P, T, ...)                      \
    do                                               \
    {                                                \
        vector<size_t> expected = {__VA_ARGS__};     \
        auto obtained = P.find(T);                   \
        ASSERT(expected.size() == obtained.size());  \
        for (size_t i = 0; i < expected.size(); i++) \
        {                                            \
            ASSERT(expected[i] == obtained[i]);      \
        }                                            \
    } while (0)

#define ASSERT_NO_MATCH(P, T)         \
    do                                \
    {                                 \
        auto obtained = P.find(T);    \
        ASSERT(obtained.size() == 0); \
    } while (0)

    {
        BM::Pattern p{"hello"};

        ASSERT_NO_MATCH(p, "");
        ASSERT_NO_MATCH(p, "h");
        ASSERT_NO_MATCH(p, "he");
        ASSERT_NO_MATCH(p, "hel");
        ASSERT_NO_MATCH(p, "hell");

        TEST_MATCHES(p, "hello", 0);
        TEST_MATCHES(p, "hello world!", 0);
        TEST_MATCHES(p, "hello hello", 0, 6);
        TEST_MATCHES(p, "aello hello", 6);
        TEST_MATCHES(p, "hell hello", 5);
        TEST_MATCHES(p, "hellohellohello", 0, 5, 10);
    }
    {
        BM::Pattern p{"ere"};

        TEST_MATCHES(p, "there is a problem here where mere faces appear", 2, 20, 26, 31);
        TEST_MATCHES(p, "here", 1);
    }
    {
        BM::Pattern p{"hello hello"};

        TEST_MATCHES(p, "well, hello hello my friend... oh! hello bye bye LOL", 6);
        ASSERT_NO_MATCH(p, "hellohello heee llooo wehehe low hello hell oops");
    }
}

#ifdef __TESTS__
int main(int argc, const char *argv[])
#else
int test_main(int argc, const char *argv[])
#endif
{
    try
    {
        test_z();
        test_boiermur();
    }
    catch (const char *e)
    {
        cout << RED_TEXT("[ FAILED ]") " at " << e << endl;
        return 1;
    }

    cout << GREEN_TEXT("[ SUCCESS ]") << endl;
    return 0;
}
