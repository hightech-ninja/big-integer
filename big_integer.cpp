#include <bits/stdc++.h>
#include "big_integer.h"

const int64_t  POW  = 32;
const uint64_t BASE = (1ull << POW);
const uint64_t WORD = BASE - 1;
std::shared_ptr<unsigned> reseted;

big_integer::vector::vector() : sz(0) {
    memset(small, 0, sizeof(small));
}

std::shared_ptr<unsigned> makeSharedArray(unsigned *d) {
    return std::shared_ptr<unsigned> (d, [](unsigned* p) { delete[] p; });
}

big_integer::vector::vector(unsigned sz, unsigned value, int sg) : sz(sz) {
    memset(small, 0, sizeof(small));
    if (sz > big_integer::vector::SMALL_OBJECT_SIZE) {
        large.capacity = sz;
        new (&large.arr) std::shared_ptr<unsigned>(makeSharedArray(new unsigned[sz]));
    }
    setMode(size() <= SMALL_OBJECT_SIZE ? 0 : 1);
    setSign(sg);
    std::fill(begin(), begin() + sz, value);
}

big_integer::vector::~vector() {
    if (mode()) {
        large.arr.~shared_ptr();
    }
}

big_integer::vector::vector(const big_integer::vector& other) : sz(other.sz) {
    if (other.mode()) {
        large.capacity = size();
        memcpy(&large.arr, &reseted, sizeof(large.arr));
        large.arr = other.large.arr;
    } else {
        std::copy(other.begin(), other.begin() + SMALL_OBJECT_SIZE, begin());
    }
}


big_integer::vector& big_integer::vector::operator=(vector const& other) {
    big_integer::vector cur(other);
    swap(*this, cur);
    return *this;
}

void swap(big_integer::vector& a, big_integer::vector& b) {
    std::swap(a.sz, b.sz);
    std::swap(a.small, b.small);
}

void independent_copy(big_integer::vector& a) {
    if (a.mode() && a.large.arr.use_count() - 1) {
        big_integer::vector other;
        other.sz = a.sz;
        other.large.capacity = a.large.capacity;
        other.large.arr = makeSharedArray(new unsigned[a.size()]);
        std::copy(a.begin(), a.end(), other.begin());
        std::swap(a.small, other.small);
    }
}

int big_integer::vector::sign() const {
    return (sz >> SIGN_BIT) & 1;
}
int big_integer::vector::mode() const {
    return (sz >> MODE_BIT) & 1;
}
unsigned big_integer::vector::size() const {
    return sz & SIZE_CONST;
}

bool big_integer::vector::empty() const {
    return size() == 0;
}

unsigned& big_integer::vector::back() {
    return operator[](size() - 1);
}

const unsigned big_integer::vector::back() const {
    return operator[](size() - 1);
}

unsigned& big_integer::vector::operator[](size_t i) {
    return *(begin() + i);
}

const unsigned big_integer::vector::operator[](size_t i) const {
    return *(begin() + i);
}

unsigned* big_integer::vector::begin() {
    return (mode() ? large.arr.get() : small);
}
unsigned* big_integer::vector::end() {
    return begin() + size();
}
unsigned const* big_integer::vector::begin() const {
    return (mode() ? large.arr.get() : small);
}
unsigned const* big_integer::vector::end() const {
    return begin() + size();
}

void big_integer::vector::setSize(unsigned new_size) {
    sz = new_size | (sz & (1 << SIGN_BIT));
}

void big_integer::vector::setMode(unsigned new_mode) {
    sz = (sz & MODE_CONST) | (new_mode << MODE_BIT);
}

void big_integer::vector::setSign(unsigned new_sign) {
    sz = (sz & SIGN_CONST) | (new_sign << SIGN_BIT);
}

void big_integer::vector::toSmallObject() {
    std::shared_ptr<unsigned> sharedPtr = large.arr;
    large.arr.~shared_ptr();
    memcpy(small, sharedPtr.get(), sizeof(small));
    sharedPtr.reset();
    setMode(0);
}

void big_integer::vector::fromSmallObject(unsigned capacity) {
    unsigned *arr = new unsigned[capacity];
    memcpy(arr, small, sizeof(small));
    new (&large.arr) std::shared_ptr<unsigned>(makeSharedArray(arr));
    large.capacity = capacity;
    setMode(1);
}

void big_integer::vector::ensureCapacity(unsigned new_size) {
    if (new_size > large.capacity) {
        unsigned new_capacity = large.capacity << 1;
        unsigned* arr = new unsigned[new_capacity];
        memcpy(arr, large.arr.get(), large.capacity * sizeof(unsigned));
        large.arr.reset();
        large.arr = makeSharedArray(arr);
        large.capacity = new_capacity;
    }
}

void big_integer::vector::push_back(unsigned val) {
    if (size() == SMALL_OBJECT_SIZE) {
        fromSmallObject();
    }
    if (mode()) {
        ensureCapacity(size() + 1);
        large.arr.get()[size()] = val;
    } else {
        small[size()] = val;
    }
    sz++;
}

void big_integer::vector::pop_back() {
    sz--;
    if (size() == SMALL_OBJECT_SIZE) {
        toSmallObject();
    }
}

void big_integer::vector::resize(unsigned new_size) {
    while (size() < new_size) {
        push_back(0);
    }
    while (size() > new_size) {
        pop_back();
    }
}

int update(big_integer::vector& a, int sign) {
    if (a.empty()) {
        a.push_back(0);
    } else {
        unsigned pos = a.size() - 1;
        while (pos && a[pos] == 0) {
            pos--;
        }
        a.resize(pos + 1);
    }
    return (a.size() == 1 && a.back() == 0) ? 0 : sign;
}

int compare(big_integer::vector const& a, big_integer::vector const& b) {
    if (a.size() != b.size()) {
        return a.size() - b.size();
    }
    for (int i = (int)a.size() - 1; i >= 0; --i) {
        if (a[i] != b[i]) {
            return a[i] > b[i] ? 1 : -1;
        }
    }
    return 0;
}

int compare(big_integer const& a, big_integer const& b, bool absCompare) {
    return (!absCompare && a.sign() != b.sign()) ? a.sign() - b.sign() : compare(a.digits, b.digits);
}

void add(const big_integer::vector& a,
         const big_integer::vector& b,
         big_integer::vector& res) {
    uint64_t cur = 0;
    unsigned sz_a = a.size(), sz_b = b.size(), sz = std::max(sz_a, sz_b);
    res.resize(sz);
    for (size_t i = 0; i < sz; ++i) {
        cur += (int64_t) (i < sz_a ? a[i] : 0) + (i < sz_b ? b[i] : 0);
        res[i] = cur & WORD;
        cur >>= POW;
    }
    if (cur) {
       res.push_back(cur);
    }
}

void subtract(const big_integer::vector& a,
              const big_integer::vector& b,
              big_integer::vector& res) {
    int64_t cur = 0, flag;
    unsigned sz_a = a.size(), sz_b = b.size(), sz = std::min(sz_a, sz_b);
    res.resize(sz_a);
    for (size_t i = 0; i < sz; ++i) {
        cur = a[i] - cur - b[i];
        flag = cur < 0;
        cur += flag ? BASE : 0;
        res[i] = cur;
        cur = flag;
    }
    if (cur) {
        res[sz] = a[sz] - cur - b[sz];
    }
    update(res);
}

void multiply_on_short(const big_integer::vector& a, uint64_t b, big_integer::vector& res) {
    res.resize(a.size());
    uint64_t cur = 0;
    size_t sz_a = a.size();
    for (size_t i = 0; i < sz_a; ++i) {
        cur += (uint64_t)a[i] * b;
        res[i] = (cur & WORD);
        cur >>= POW;
    }
    if (cur) {
        res.push_back(cur);
    }
}

void multiply(const big_integer::vector& a, const big_integer::vector& b, big_integer::vector& res) {
    size_t sz_a = a.size(), sz_b = b.size();
    std::vector<uint64_t> c(sz_a + sz_b + 1, 0);
    uint64_t cur;
    for (size_t i = 0; i < sz_a; i++) {
        for (size_t j = 0; j < sz_b; j++) {
            cur = (uint64_t)a[i] * b[j] + c[i + j];
            c[i + j + 1] += (cur >> POW);
            c[i + j] = (cur & WORD);
        }
    }
    size_t sz_c = c.size();
    res.resize(sz_c - 1);
    for (size_t i = 0; i + 1 < sz_c; i++) {
        c[i + 1] += (c[i] >> POW);
        res[i] = c[i] & WORD;
    }
    update(res);
}

unsigned divide_on_short(const big_integer::vector& a, int64_t b, big_integer::vector& res, int64_t base) {
    uint64_t cur = 0;
    res.resize(a.size());
    for (int i = (int)a.size() - 1; i >= 0; --i) {
        cur = cur * base + a[i];
        res[i] = cur / b;
        cur %= b;
    }
    update(res);
    return cur;
}

unsigned divide_on_short_log_base(const big_integer::vector& a, int64_t b, big_integer::vector& res, int64_t log_base) {
    uint64_t cur = 0;
    res.resize(a.size());
    for (int i = (int)a.size() - 1; i >= 0; --i) {
        cur = (cur << log_base) + a[i];
        res[i] = cur / b;
        cur %= b;
    }
    update(res);
    return cur;
}

unsigned divide_pref_3_to_pref_2(unsigned (&a)[3], unsigned (&b)[2]) {
    __uint128_t c = a[0], d = WORD;
    c <<= 64;
    c += ((1ull * a[1]) << 32) + a[2];
    c /= ((1ull * b[1]) << 32) + b[0];
    return (unsigned)std::min(c, d);
}

big_integer::vector divide(big_integer::vector& a,
                           const big_integer::vector& b) {
    if (a.size() < b.size()) {
        return big_integer::vector(1, 0);
    }

    if (b.size() == 1) {
				independent_copy(a);
        divide_on_short_log_base(a, b[0], a, POW);
        return a;
    }
    size_t sz_a = a.size(), sz_b = b.size();

    unsigned pref_a[3], pref_b[2], d;
    pref_b[0] = b[sz_b - 2];
    pref_b[1] = b[sz_b - 1];
    big_integer::vector pref(sz_b + 1, 0), temp(sz_b + 1, 0);
		big_integer::vector res_(sz_a - sz_b + 1, 0);
    for (size_t j = 0, cur_pos = sz_a - sz_b; j <= sz_b; ++j, ++cur_pos) {
        pref[j] = (cur_pos < sz_a ? a[cur_pos] : 0);
    }
    for (int i = (int)(sz_a - sz_b); i >= 0; --i) {
        for (size_t j = 0; j < 3; ++j) {
            pref_a[j] = pref[sz_b - j];
        }
        d = divide_pref_3_to_pref_2(pref_a, pref_b);
        multiply_on_short(b, d, temp);
        temp.resize(sz_b + 1);
        if (compare(pref, temp) < 0) {
            --d;
            subtract(temp, b, temp);
        }
        subtract(pref, temp, pref);
        pref.resize(sz_b + 1);
        for (int j = sz_b; j > 0; --j) {
            pref[j] = pref[j - 1];
        }
        if (i) {
            pref[0] = a[i - 1];
        }
        res_[i] = d;
    }
    res_.setSign(update(res_, a.sign()));
    return res_;
}

// to Additional Binary Code
big_integer::vector to_ABC(big_integer::vector digits, int sign) {
    if (sign) {
        size_t sz_v = digits.size();
        for (size_t i = 0; i < sz_v; ++i) {
            digits[i] = ~digits[i] & WORD;
        }
        add(digits, big_integer::vector(1, 1), digits);
    }
    digits.push_back(sign ? WORD : 0);
    return digits;
}

// to Additional Binary Code
void from_ABC(big_integer::vector& digits) {
    int sign = 0;
    if (digits.back()) {
        sign = 1;
        subtract(digits, big_integer::vector(1, 1), digits);
        size_t sz_v = digits.size();
        for (size_t i = 0; i < sz_v; ++i) {
            digits[i] = ~digits[i] & WORD;
        }
    }
    digits.pop_back();
    digits.setSign(update(digits, sign));
}

int big_integer::sign() const {
    return digits.sign();
}
unsigned big_integer::size() const {
    return digits.size();
}
int big_integer::mode() const {
    return digits.mode();
}

big_integer::big_integer() : digits(1, 0) {
}

big_integer::big_integer(int sign, const vector& other) : digits(other) {
    digits.setSign(update(digits, sign));
}

big_integer::big_integer(big_integer const& other) : digits(other.digits) {}

big_integer::big_integer(int a) : digits(1, std::abs((int64_t) a), a < 0) {
}

big_integer::big_integer(std::string const& str) {
    vector temp;
    size_t sz_str = str.size();
    for (size_t i = 0; i < sz_str; ++i) {
        if (str[i] == '-') {
            digits.setSign(1);
        }
        else {
            temp.push_back(str[i] - '0');
        }
    }

    std::reverse(temp.begin(), temp.end());
    unsigned mod = 0;
    while (temp.size() != 1 || temp[0] != 0) {
        mod = divide_on_short(temp, BASE, temp, 10);
        digits.push_back(mod);
    }
    digits.setSign(update(digits, digits.sign()));
}

big_integer::~big_integer() {
}

big_integer& big_integer::operator=(big_integer const& other) {
    digits = other.digits;
    return *this;
}

big_integer& big_integer::operator+=(big_integer const& rhs) {
    if (sign() == rhs.sign()) {
        add(digits, rhs.digits, digits);
    } else if (compare(*this, rhs, true) < 0) {
        subtract(rhs.digits, digits, digits);
        digits.setSign(rhs.sign());
    } else {
        subtract(digits, rhs.digits, digits);
    }
    digits.setSign(update(digits, sign()));
    return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
    return *this += -rhs;
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
    multiply(digits, rhs.digits, digits);
    digits.setSign(update(digits, sign() ^ rhs.sign()));
    return *this;
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
    digits = divide(digits, rhs.digits);
    digits.setSign(update(digits, sign() ^ rhs.sign()));
    return *this;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
    return (*this = *this - (*this / rhs) * rhs);
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
    big_integer::vector a(to_ABC(digits, sign())), b(to_ABC(rhs.digits, rhs.sign()));
    size_t sz_a = a.size(), sz_b = b.size(), sz = std::max(sz_a, sz_b);
    unsigned back_a = a.back(), back_b = b.back();
    digits.resize(sz);
    for(size_t i = 0; i < sz; i++) {
        digits[i] = (i < sz_a ? a[i] : back_a) & (i < sz_b ? b[i] : back_b);
    }
    from_ABC(digits);
    return *this;
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
    big_integer::vector a(to_ABC(digits, sign())), b(to_ABC(rhs.digits, rhs.sign()));
    size_t sz_a = a.size(), sz_b = b.size(), sz = std::max(sz_a, sz_b);
    unsigned back_a = a.back(), back_b = b.back();
    digits.resize(sz);
    for(size_t i = 0; i < sz; i++) {
        digits[i] = (i < sz_a ? a[i] : back_a) | (i < sz_b ? b[i] : back_b);
    }
    from_ABC(digits);
    return *this;
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
    big_integer::vector a(to_ABC(digits, sign())), b(to_ABC(rhs.digits, rhs.sign()));
    size_t sz_a = a.size(), sz_b = b.size(), sz = std::max(sz_a, sz_b);
    unsigned back_a = a.back(), back_b = b.back();
    digits.resize(sz);
    for(size_t i = 0; i < sz; i++) {
        digits[i] = (i < sz_a ? a[i] : back_a) ^ (i < sz_b ? b[i] : back_b);
    }
    from_ABC(digits);
    return *this;
}

big_integer& big_integer::operator<<=(int rhs) {
    big_integer::vector a(to_ABC(digits, sign()));
    digits.resize(rhs / POW);
    uint64_t cur = 0;
    unsigned back = a.back();
    rhs %= POW;
    size_t sz_a = a.size();
    for (size_t i = 0; i < sz_a; ++i) {
        cur += (uint64_t) a[i] << rhs;
        digits.push_back(cur & WORD);
        cur >>= POW;
    }
    digits.push_back(back);
    from_ABC(digits);
    return *this;
}

big_integer& big_integer::operator>>=(int rhs) {
    big_integer::vector a(to_ABC(digits, sign()));
    digits.resize(0);
    uint64_t cur = 0;
    unsigned back = a.back(), i = rhs / POW;
    rhs %= POW;
    cur = a[i] >> rhs;
    size_t sz_a = a.size();
    while(i + 1 < sz_a) {
        cur = cur + ((uint64_t)a[i + 1] << (POW - rhs));
        digits.push_back(cur & WORD);
        cur /= BASE;
        ++i;
    }
    digits.push_back(back);
    from_ABC(digits);
    return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    return big_integer(sign() ^ 1, digits);
}

big_integer big_integer::operator~() const {
    return -*this - big_integer(1);
}

big_integer& big_integer::operator++() {
    return *this += big_integer(1);
}

big_integer big_integer::operator++(int) {
    big_integer r = *this;
    ++*this;
    return r;
}

big_integer& big_integer::operator--() {
    return *this -= big_integer(1);
}

big_integer big_integer::operator--(int) {
    big_integer r = *this;
    --*this;
    return r;
}

big_integer operator+(big_integer a, big_integer const& b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const& b) {
    return a -= b;
}

big_integer operator*(big_integer a, big_integer const& b) {
    return a *= b;
}

big_integer operator/(big_integer a, big_integer const& b) {
    return a /= b;
}

big_integer operator%(big_integer a, big_integer const& b) {
    return a %= b;
}

big_integer operator&(big_integer a, big_integer const& b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b) {
    return a ^= b;
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}


big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

bool operator==(big_integer const& a, big_integer const& b) {
    return !compare(a, b);
}

bool operator!=(big_integer const& a, big_integer const& b) {
    return !!compare(a, b);
}

bool operator<(big_integer const& a, big_integer const& b) {
    return compare(a, b) < 0;
}

bool operator>(big_integer const& a, big_integer const& b) {
    return compare(a, b) > 0;
}

bool operator<=(big_integer const& a, big_integer const& b) {
    return compare(a, b) <= 0;
}

bool operator>=(big_integer const& a, big_integer const& b) {
    return compare(a, b) >= 0;
}

std::string to_string(big_integer const& a) {
    std::string res;
    big_integer::vector digits = a.digits;
    unsigned mod = 0;
    while (digits.size() != 1 || digits[0] != 0) {
        mod = divide_on_short_log_base(digits, 10, digits, POW);
        res += (char)(mod + '0');
    }
    if (a.sign()) {
       res += "-";
    }
    reverse(res.begin(), res.end());
    return (res.empty() ? "0" : res);
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
    return s << to_string(a);
}
