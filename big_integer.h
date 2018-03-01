#pragma once

#include <memory>
#include <string>

struct big_integer {
    big_integer();

    big_integer(big_integer const& other);
    big_integer(int a);
    explicit big_integer(std::string const& str);
    ~big_integer();


    big_integer& operator=(big_integer const& other);

    big_integer& operator+=(big_integer const& rhs);
    big_integer& operator-=(big_integer const& rhs);
    big_integer& operator*=(big_integer const& rhs);
    big_integer& operator/=(big_integer const& rhs);
    big_integer& operator%=(big_integer const& rhs);

    big_integer& operator&=(big_integer const& rhs);
    big_integer& operator|=(big_integer const& rhs);
    big_integer& operator^=(big_integer const& rhs);

    big_integer& operator<<=(int rhs);
    big_integer& operator>>=(int rhs);

    big_integer operator+() const;
    big_integer operator-() const;
    big_integer operator~() const;

    big_integer& operator++();
    big_integer operator++(int);

    big_integer& operator--();
    big_integer operator--(int);

    friend bool operator==(big_integer const& a, big_integer const& b);
    friend bool operator!=(big_integer const& a, big_integer const& b);
    friend bool operator<(big_integer const& a, big_integer const& b);
    friend bool operator>(big_integer const& a, big_integer const& b);
    friend bool operator<=(big_integer const& a, big_integer const& b);
    friend bool operator>=(big_integer const& a, big_integer const& b);

    friend std::string to_string(big_integer const& a);

private:
    struct vector {
        struct node {
            std::shared_ptr<unsigned> arr;
            unsigned capacity;
        };

        void ensureCapacity(unsigned new_size);
        const static size_t MODE_BIT = 31;
        const static size_t SIGN_BIT = 30;
        const static size_t SIZE_CONST   = (1ll << SIGN_BIT) - 1;
        const static size_t MODE_CONST = ~(1llu << MODE_BIT);
        const static size_t SIGN_CONST = ~(1llu << SIGN_BIT);
        const static size_t SMALL_OBJECT_SIZE = sizeof(node) / sizeof(unsigned);

        void toSmallObject();
        void fromSmallObject(unsigned capacity = 10);

        vector();
        ~vector();
        vector(const vector& x);


        vector(unsigned sz, unsigned value, int sg = 0);
        vector& operator=(vector const& other);
        union {
            unsigned small[SMALL_OBJECT_SIZE];
            node large;
        };

        void push_back(unsigned);
        void resize(unsigned);
        void pop_back();

        unsigned size() const;
        int sign() const;
        int mode() const;

        void setSize(unsigned new_size);
        void setMode(unsigned new_mode);
        void setSign(unsigned new_sign);

        unsigned& back();
        const unsigned back() const;
        bool empty() const;


        unsigned* begin();
        unsigned const* begin() const;
        unsigned* end();
        unsigned const* end() const;

        unsigned& operator[](size_t i);
        unsigned const operator[](size_t i) const;
        unsigned sz;
    };
    vector digits;


    inline int sign() const;
    inline unsigned size() const;
    inline int mode() const;

    big_integer(int sign, const big_integer::vector& other);

    friend void independent_copy(big_integer::vector& a);
    friend void swap(big_integer::vector& a, big_integer::vector& b);
    friend big_integer::vector to_ABC(big_integer::vector digits, int sign);
    friend void from_ABC(big_integer::vector& digits);
    friend int compare(big_integer::vector const& a, big_integer::vector const& b);
    friend int compare(big_integer const& a, big_integer const& b, bool absCompare);
    friend int update(big_integer::vector& a, int sign);
    friend void add(const big_integer::vector& a, const big_integer::vector& b, big_integer::vector& res);
    friend void subtract(const big_integer::vector& a, const big_integer::vector& b, big_integer::vector& res);
    friend void multiply_on_short(const big_integer::vector& a, uint64_t b, big_integer::vector& res);
    friend void multiply(const big_integer::vector& a, const big_integer::vector& b, big_integer::vector& res);
    friend unsigned divide_on_short(const big_integer::vector& a, int64_t b, big_integer::vector& res, int64_t base);
    friend unsigned divide_on_short_log_base(const big_integer::vector& a, int64_t b, big_integer::vector& res, int64_t log_base);
    friend big_integer::vector divide(big_integer::vector& a, const big_integer::vector& b);
};

bool operator==(big_integer const& a, big_integer const& b);
bool operator!=(big_integer const& a, big_integer const& b);
bool operator<(big_integer const& a, big_integer const& b);
bool operator>(big_integer const& a, big_integer const& b);
bool operator<=(big_integer const& a, big_integer const& b);
bool operator>=(big_integer const& a, big_integer const& b);

big_integer operator+(big_integer a, big_integer const& b);
big_integer operator-(big_integer a, big_integer const& b);
big_integer operator*(big_integer a, big_integer const& b);
big_integer operator/(big_integer a, big_integer const& b);
big_integer operator%(big_integer a, big_integer const& b);

big_integer operator&(big_integer a, big_integer const& b);
big_integer operator|(big_integer a, big_integer const& b);
big_integer operator^(big_integer a, big_integer const& b);
big_integer operator<<(big_integer a, int b);
big_integer operator>>(big_integer a, int b);

std::ostream& operator<<(std::ostream& s, big_integer const& a);

void independent_copy(big_integer::vector& a);
void swap(big_integer::vector& a, big_integer::vector& b);
big_integer::vector to_ABC(big_integer::vector digits, int sign);
void from_ABC(big_integer::vector& digits);
int compare(big_integer::vector const& a, big_integer::vector const& b);
int compare(big_integer const& a, big_integer const& b, bool absCompare = false);
int update(big_integer::vector& a, int sign = 0);
void add(const big_integer::vector& a, const big_integer::vector& b, big_integer::vector& res);
void subtract(const big_integer::vector& a, const big_integer::vector& b, big_integer::vector& res);
void multiply_on_short(const big_integer::vector& a, uint64_t b, big_integer::vector& res);
void multiply(const big_integer::vector& a, const big_integer::vector& b, big_integer::vector& res);
unsigned divide_on_short(const big_integer::vector& a, int64_t b, big_integer::vector& res, int64_t base);
unsigned divide_on_short_log_base(const big_integer::vector& a, int64_t b, big_integer::vector& res, int64_t log_base);
big_integer::vector divide(big_integer::vector& a, const big_integer::vector& b);
