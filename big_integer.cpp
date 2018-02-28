#include "big_integer.h"
#include <algorithm>
#include <iostream>

const long long POW = 32;
const long long BASE = (1ll << POW);
const long long WORD = BASE - 1;

int update(std::vector<unsigned> &digits, int sign = 0) {
    if (digits.empty()) {
        digits.push_back(0);
    }
    size_t pos = digits.size() - 1;
    while (pos && digits[pos] == 0) {
        --pos;
    }
    digits.resize(pos + 1);
		if (digits.size() == 1 && digits.back() == 0) {
			sign = 0;
		}
		return sign;
}

int compare(std::vector<unsigned> const &a, std::vector<unsigned> const &b) {
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


int compare(big_integer const& a, big_integer const& b, bool absCompare = false) {
    return (!absCompare && a.sign != b.sign) ? a.sign - b.sign : compare(a.digits, b.digits);
}

void add(const std::vector<unsigned>& a,
				 const std::vector<unsigned>& b,
				 std::vector<unsigned> &res) {
    unsigned long long cur = 0;
    size_t sz_a = a.size(), sz_b = b.size(), sz = std::max(a.size(), b.size());
    res.resize(sz);
    for (size_t i = 0; i < sz; ++i) {
        cur += (long long) (i < sz_a ? a[i] : 0) + (i < sz_b ? b[i] : 0);
        res[i] = cur & WORD;
        cur >>= POW;
    }
    if (cur) {
       res.push_back(cur);
    }
}

inline void subtract(const std::vector<unsigned>& a,
							const std::vector<unsigned>& b,
							std::vector<unsigned>& res) {
    long long cur = 0, flag;
    size_t sz_a = a.size(), sz_b = b.size();
    res.resize(sz_a);
    for (size_t i = 0; i < sz_a; ++i) {
        cur = a[i] - cur - (i < sz_b ? b[i] : 0);
        flag = cur < 0;
        cur += BASE * flag;
        res[i] = cur;
        cur = flag;
    }

    update(res);
}

void multiply_on_short(const std::vector<unsigned> &a,
                       unsigned long long b,
                       std::vector<unsigned> &res) {
    res.resize(a.size());
    unsigned long long cur = 0;
    size_t sz_a = a.size();
    for (size_t i = 0; i < sz_a; i++) {
        cur += 1ull * a[i] * b;
        res[i] = (cur & WORD);
        cur >>= POW;
    }
    if (cur) {
        res.push_back(cur);
    }
}

inline void multiply(const std::vector<unsigned>& a,
							const std::vector<unsigned>& b,
							std::vector<unsigned>& res) {
		size_t sz_a = a.size(), sz_b = b.size();
    std::vector<unsigned long long> c(sz_a + sz_b + 1, 0);
    unsigned long long cur;
    for (size_t i = 0; i < sz_a; i++) {
        for (size_t j = 0; j < sz_b; j++) {
            cur = 1ull * a[i] * b[j] + c[i + j];
            c[i + j + 1] += (cur >> POW);
            c[i + j] = (cur & WORD);
        }
    }
    size_t sz_c = c.size();
    res.resize(sz_c - 1);
    for (size_t i = 0; i + 1 < sz_c; i++) {
        c[i + 1] += (c[i] >> POW);
        res[i] = (c[i] & WORD);
    }
    update(res);
}

unsigned divide_on_short(const std::vector<unsigned>& a, const unsigned long long b,
												 std::vector<unsigned>& res, const long long base) {
    long long cur = 0;
    res.resize(a.size(), 0);
    for (int i = (int)a.size() - 1; i >= 0; --i) {
        cur = cur * base + a[i];
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

inline void divide(std::vector<unsigned> a, const std::vector<unsigned> &b, std::vector<unsigned> &res) {
		big_integer db;
    if (a.size() < b.size()) {
        res = {0};
        return;
    }
    if (b.size() == 1) {
        divide_on_short(a, b[0], res, BASE);
        return;
    }

    size_t sz_a = a.size(), sz_b = b.size();

    std::vector<unsigned> res_(sz_a - sz_b + 1);
    unsigned pref_a[3], pref_b[2], d;
    pref_b[0] = b[sz_b - 2];
    pref_b[1] = b[sz_b - 1];
    std::vector<unsigned> pref(sz_b + 1), temp(sz_b + 1);
    res.resize(std::max(sz_a - sz_b, sz_b));
    for (int i = (int)(sz_a - sz_b); i >= 0; --i) {
        for (size_t j = 0, ind = i + sz_b; j < 3; ++j, --ind) {
            pref_a[j] = ind < sz_a ? a[ind] : 0;
        }
        d = divide_pref_3_to_pref_2(pref_a, pref_b);
        for (size_t j = 0; j <= sz_b; ++j) {
            pref[j] = (i + j < sz_a ? a[i + j] : 0);
        }
        multiply_on_short(b, d, temp);
        temp.resize(sz_b + 1);
        if (compare(pref, temp) < 0) {
            --d;
            subtract(temp, b, temp);
        }
        subtract(pref, temp, pref);
        pref.resize(sz_b + 1);
        for (size_t j = 0; j <= sz_b && i + j < sz_a; ++j) {
            a[i + j] = pref[j];
        }
        res_[i] = d;
    }

    update(res_);
    res = res_;
}

// to Additinal Binary Code
std::vector<unsigned> to_ABC(std::vector<unsigned> digits, int sign) {
    if (sign < 0) {
				size_t sz_d = digits.size();
				for (size_t i = 0; i < sz_d; ++i)
					digits[i] = ~digits[i] & WORD;
				add(digits, std::vector<unsigned>(1, 1), digits);
    }
    digits.push_back(sign >= 0 ? 0 : WORD);
    return digits;
}

// from Additinal Binary Code
void from_ABC(std::vector<unsigned>& digits, int& sign) {
    sign = 1;
    if (digits.back() == WORD) {
				sign = -1;
				subtract(digits, std::vector<unsigned>(1, 1), digits);
				size_t sz_d = digits.size();
				for (size_t i = 0; i < sz_d; ++i) {
					digits[i] = ~digits[i] & WORD;
				}
    }
    digits.pop_back();
    sign = update(digits, sign);
}

big_integer::big_integer() : digits(1, 0), sign(0) {
}

big_integer::big_integer(const std::vector<unsigned> &other, int sign) : digits(other), sign(sign) {
}

big_integer::big_integer(big_integer const& other) : digits(other.digits), sign(other.sign) {
}

big_integer::big_integer(int a) : digits(1, std::abs(1ll * a)), sign(a == 0 ? 0 : (a > 0 ? 1 : -1)) {
}

big_integer::big_integer(std::string const& str) : sign(1) {
    std::vector<unsigned> temp;
    size_t sz_str = str.size();
		temp.reserve(sz_str);
		for (size_t i = 0; i < sz_str; ++i) {
			if (str[i] == '-') {
				sign = -1;
			} else {
				temp.push_back(str[i] - '0');
			}
		}

    reverse(temp.begin(), temp.end());
    unsigned mod = 0;
    while (temp.size() != 1 || temp[0] != 0) {
        mod = divide_on_short(temp, BASE, temp, 10);
        digits.push_back(mod);
    }
    sign =update(digits, sign);
}

big_integer::~big_integer() {}

big_integer& big_integer::operator=(big_integer const& other) {
    sign = other.sign;
    digits = other.digits;
    return *this;
}

big_integer& big_integer::operator+=(big_integer const& rhs) {
    if (sign == rhs.sign) {
        add(digits, rhs.digits, digits);
    }
    else if (compare(*this, rhs, true) < 0) {
        subtract(rhs.digits, digits, digits);
        sign = rhs.sign;
    }
    else {
        subtract(digits, rhs.digits, digits);
    }
    sign = update(digits, sign);
    return *this;
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
    return *this += -rhs;
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
    multiply(digits, rhs.digits, digits);
		sign *= rhs.sign;
    sign = update(digits, sign);
    return *this;
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
    divide(digits, rhs.digits, digits);
		sign *= rhs.sign;
    sign = update(digits, sign);
    return *this;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
    return (*this = *this - (*this / rhs) * rhs);
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
		std::vector<unsigned> a(to_ABC(digits, sign)), b(to_ABC(rhs.digits, rhs.sign));
		size_t sz_a = a.size(), sz_b = b.size(), sz_d = std::max(sz_a, sz_b);
		digits.resize(sz_d);
		for (size_t i = 0; i < sz_d; ++i) {
			digits[i] = (i < sz_a ? a[i] : a.back()) & (i < sz_b ? b[i] : b.back());
		}
		from_ABC(digits, sign);
		return *this;
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
		std::vector<unsigned> a(to_ABC(digits, sign)), b(to_ABC(rhs.digits, rhs.sign));
		size_t sz_a = a.size(), sz_b = b.size(), sz_d = std::max(sz_a, sz_b);
		digits.resize(sz_d);
		for (size_t i = 0; i < sz_d; ++i) {
			digits[i] = (i < sz_a ? a[i] : a.back()) | (i < sz_b ? b[i] : b.back());
		}
		from_ABC(digits, sign);
		return *this;
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
		std::vector<unsigned> a(to_ABC(digits, sign)), b(to_ABC(rhs.digits, rhs.sign));
		size_t sz_a = a.size(), sz_b = b.size(), sz_d = std::max(sz_a, sz_b);
		digits.resize(sz_d);
		for (size_t i = 0; i < sz_d; ++i) {
			digits[i] = (i < sz_a ? a[i] : a.back()) ^ (i < sz_b ? b[i] : b.back());
		}
		from_ABC(digits, sign);
		return *this;
}

big_integer& big_integer::operator<<=(int rhs) {
		std::vector<unsigned> a(to_ABC(digits, sign));
		digits.resize(rhs / POW, 0);
		unsigned long long cur = 0;
		unsigned back = a.back();
		rhs %= POW;
		size_t sz_a = a.size();
		digits.reserve(digits.size() + sz_a);
		for (size_t i = 0; i < sz_a; ++i) {
			cur += (unsigned long long)a[i] << rhs;
			digits.push_back(cur & WORD);
			cur >>= POW;
		}
		digits.push_back(back);
		from_ABC(digits, sign);
		return *this;
}

big_integer& big_integer::operator>>=(int rhs) {
		std::vector<unsigned> a(to_ABC(digits, sign));
		digits.resize(0);
		unsigned long long cur = 0;
		unsigned back = a.back(), i  = rhs / POW;
		rhs %= POW;
		cur = a[i] >> rhs;
		size_t sz_a = a.size();
		digits.reserve(a.size());
		while (i + 1 < sz_a) {
			cur = cur + ((unsigned long long)a[i + 1] << (POW - rhs));
			digits.push_back(cur & WORD);
			cur /= BASE;
			++i;
		}
		digits.push_back(back);
		from_ABC(digits, sign);
		return *this;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    return big_integer(digits, -sign);
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
    return compare(a, b);
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
    std::vector<unsigned> digits = a.digits;
    unsigned mod = 0;
    while (digits.size() != 1 || digits[0] != 0) {
        mod = divide_on_short(digits, 10, digits, BASE);
        res += (char)(mod + 48);
    }

    if (a.sign == -1) {
       res += "-";
    }
    reverse(res.begin(), res.end());
    return (res.empty() ? "0" : res);
}

std::ostream& operator<<(std::ostream& s, big_integer const& a) {
    return s << to_string(a);
}
