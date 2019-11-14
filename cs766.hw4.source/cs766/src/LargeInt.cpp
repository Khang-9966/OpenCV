///////////////////////////////////////////////////////////////////////////////
// Written by Tom Brunet
// for CS739 Project, Spring 2004
// Added to VascViz Summer 2004
////

#pragma warning(push)
#pragma warning(disable:4312)

#include "LargeInt.h"
#include <debug.h>
#include <string>

#pragma warning(pop)

LargeInt::LargeInt() {
}

LargeInt::LargeInt(int count) {
    if (count)
        m_pieces.push_back((unsigned int)count);
}

LargeInt::~LargeInt() {
}

bool LargeInt::isNeg() const {
    return m_pieces.size() && ((m_pieces.back() & 0x80000000) != 0);
}

void LargeInt::pad(unsigned int numPad) {
    unsigned int val = isNeg() ? 0xffffffff : 0;
    while (m_pieces.size() < numPad)
        m_pieces.push_back(val);
}

void LargeInt::unpad() {
    bool bNeg = isNeg();
    unsigned int val = bNeg ? 0xffffffff : 0;
    // Remove excessive padding
    while (m_pieces.size() && m_pieces.back()==val)
        m_pieces.pop_back();
    // Put the padding back if it was needed to establish sign
    if (isNeg() != bNeg)
        m_pieces.push_back(val);
}

int LargeInt::toInt() const {
    if (m_pieces.size() > 1) IMERR_THROW("LargeInt too big");
    if (m_pieces.size() == 0) return 0;
    return m_pieces[0];
}

LargeInt& LargeInt::operator+=(const LargeInt& lc) {
    bool bNegOne = isNeg();
    bool bNegTwo = lc.isNeg();

    // Pad the two numbers to be the same number of bits
    LargeInt temp(lc);
    int size = (int)lc.m_pieces.size();
    if (m_pieces.size() > lc.m_pieces.size())
        size = (int)m_pieces.size();
    temp.pad(size);
    pad(size);

    // Do an unsigned add
    std::vector<unsigned int>::iterator store = m_pieces.begin();
    std::vector<unsigned int>::iterator other = temp.m_pieces.begin();
    // Carry bit is either 1 or 0
    unsigned int carry = 0;
    while (store != m_pieces.end()) {
        unsigned int sum = *store+*other;
        // Detect overflows for the next carry bit
        if (sum < *other || (carry + sum == (unsigned int)-1)) {
            sum += carry;
            carry = 1;
        } else {
            sum += carry;
            carry = 0;
        }
        *store = sum;
        ++store;
        ++other;
    }

    // If both numbers were positive, just carry
    if (!bNegOne && !bNegTwo && isNeg())
        m_pieces.push_back(0);
    else if (bNegOne && bNegTwo && !isNeg())
        m_pieces.push_back(0xffffffff);
    unpad();
    return *this;
}

LargeInt& LargeInt::operator-=(const LargeInt& lc) {
    return *this += -lc;
}

LargeInt& LargeInt::operator*=(const LargeInt& lc) {
    // Do positive mults
    bool bNegOne = isNeg();
    bool bNegTwo = lc.isNeg();
    LargeInt a;
    LargeInt b;
    if (bNegOne)
        a = -*this;
    else
        a = *this;
    if (bNegTwo)
        b = -lc;
    else
        b = lc;

    m_pieces.clear();
    std::vector<unsigned int>::const_iterator i = a.m_pieces.begin();
    for (; i != a.m_pieces.end(); ++i) {
        int bitMask = 0x1;
        for (int j=0; j<32; ++j) {
            if ((bitMask & *i) != 0)
                *this += b;
            b <<= 1;
            bitMask <<= 1;
        }
    }
    if (isNeg())
        m_pieces.push_back(0);
    unpad();
    if (bNegOne != bNegTwo) *this = -*this;
    return *this;
}

LargeInt& LargeInt::operator/=(const LargeInt& lc) {
    // Do positive divs
    bool bNegOne = isNeg();
    bool bNegTwo = lc.isNeg();
    LargeInt a;
    LargeInt b;
    if (bNegOne)
        a = -*this;
    else
        a = *this;
    if (bNegTwo)
        b = -lc;
    else
        b = lc;
    m_pieces.clear();

    LargeInt temp;
    for (unsigned int i=32*(unsigned int)a.m_pieces.size()-1; i != (unsigned int)-1; --i) {
        temp <<= 1;
        *this <<= 1;
        unsigned int bit = a.getBit(i);
        if (bit)
            temp += bit;
        if (temp >= b) {
            *this += 1;
            temp -= b;
        }
    }
    unpad();
    if (bNegOne != bNegTwo) *this = -*this;
    return *this;
}

LargeInt& LargeInt::operator%=(const LargeInt& lc) {
    // Do positive divs
    bool bNegOne = isNeg();
    bool bNegTwo = lc.isNeg();
    LargeInt a;
    LargeInt b;
    if (bNegOne)
        a = -*this;
    else
        a = *this;
    if (bNegTwo)
        b = -lc;
    else
        b = lc;
    m_pieces.clear();

    for (unsigned int i=32*(unsigned int)a.m_pieces.size()-1; i != (unsigned int)-1; --i) {
        *this <<= 1;
        unsigned int bit = a.getBit(i);
        if (bit)
            *this += bit;
        if (*this >= b) {
            *this -= b;
        }
    }
    unpad();
    return *this;
}

LargeInt& LargeInt::operator<<=(unsigned int leftshift) {
    if (!m_pieces.size()) return *this;
    while (leftshift > 32) {
        m_pieces.insert(m_pieces.begin(), 0);
        leftshift -= 32;
    }
    if (!leftshift) return *this;
    unsigned int carryVal = 0;
    unsigned int extractMask = ((int)0x80000000)>>(leftshift-1);
    unsigned int shiftRightAmount = 32 - leftshift;
    unsigned int insertMask = extractMask >> shiftRightAmount;
    std::vector<unsigned int>::iterator i = m_pieces.begin();
    for (; i != m_pieces.end(); ++i) {
        unsigned int newCarry = (*i & extractMask) >> shiftRightAmount;
        *i <<= leftshift;
        *i &= ~insertMask; // Zero out the insert spot
        *i |= (carryVal&insertMask); // Probably don't need the &, but be careful anyway
        carryVal = newCarry;
    }
    if (carryVal) m_pieces.push_back(carryVal);
    return *this;
}

LargeInt& LargeInt::operator>>=(unsigned int rightshift) {
    if (!m_pieces.size()) return *this;
    while (rightshift > 32 && m_pieces.size()) {
        m_pieces.erase(m_pieces.begin());
        rightshift -= 32;
    }
    if (!rightshift) return *this;
    unsigned int carryVal = 0;
    unsigned int insertMask = ((int)0x80000000)>>(rightshift-1);
    unsigned int shiftLeftAmount = 32 - rightshift;
    unsigned int extractMask = insertMask >> shiftLeftAmount;
    std::vector<unsigned int>::iterator i = m_pieces.end();
    while (i != m_pieces.begin()) {
        --i;
        unsigned int newCarry = (*i & extractMask) << shiftLeftAmount;
        *i >>= rightshift;
        *i &= ~insertMask; // Zero out the insert spot
        *i |= (carryVal&insertMask); // Probably don't need the &, but be careful anyway
        carryVal = newCarry;
    }
    unpad();
    return *this;
}

LargeInt LargeInt::operator+(const LargeInt& lc) const {
    LargeInt temp(*this);
    return temp += lc;
}

LargeInt LargeInt::operator-(const LargeInt& lc) const {
    LargeInt temp(*this);
    return temp -= lc;
}

LargeInt LargeInt::operator*(const LargeInt& lc) const {
    LargeInt temp(*this);
    return temp *= lc;
}

LargeInt LargeInt::operator/(const LargeInt& lc) const {
    LargeInt temp(*this);
    return temp /= lc;
}

LargeInt LargeInt::operator%(const LargeInt& lc) const {
    LargeInt temp(*this);
    return temp %= lc;
}

LargeInt LargeInt::operator<<(unsigned int leftshift) const {
    LargeInt temp(*this);
    return temp <<= leftshift;
}

LargeInt LargeInt::operator>>(unsigned int rightshift) const {
    LargeInt temp(*this);
    return temp >>= rightshift;
}

LargeInt LargeInt::operator-() const {
    if (!m_pieces.size()) return *this;

    LargeInt retVal;
    bool bCarry = true;
    std::vector<unsigned int>::const_iterator i = m_pieces.begin();

    for (; bCarry && i != m_pieces.end(); ++i) {
        bCarry = *i == 0;
        retVal.m_pieces.push_back(~*i+1);
    }
    for (; i != m_pieces.end(); ++i) {
        retVal.m_pieces.push_back(~*i);
    }

    if (bCarry) 
        retVal.m_pieces.push_back(1);
    if (retVal.isNeg()==isNeg()) {
        // This should only happen in the case of 0x8000....00
        retVal.m_pieces.push_back(0);
    }
    return retVal;
}
   

bool LargeInt::operator==(const LargeInt& lc) const {
    if (m_pieces.size() != lc.m_pieces.size()) return false;
    std::vector<unsigned int>::const_iterator i = m_pieces.begin();
    std::vector<unsigned int>::const_iterator j = lc.m_pieces.begin();
    for (; i != m_pieces.end(); ++i, ++j) {
        if (*i != *j) return false;
    }
    return true;
}
 
bool LargeInt::operator>(const LargeInt& lc) const {
    bool bNegOne = isNeg();
    bool bNegTwo = lc.isNeg();
    // Pick off the easy decisions first
    if (bNegOne != bNegTwo) {
        return !bNegOne;
    }
    if (m_pieces.size() != lc.m_pieces.size()) {
        return (!bNegOne && m_pieces.size() > lc.m_pieces.size()) ||
                (bNegOne && m_pieces.size() < lc.m_pieces.size());
    }
    std::vector<unsigned int>::const_iterator i = m_pieces.end();
    std::vector<unsigned int>::const_iterator j = lc.m_pieces.end();
    while (i != m_pieces.begin()) {
        --i;  --j;
        if (*i != *j) {
            return *i > *j;
        }
    }
    // They're equal
    return false;
}

void LargeInt::outHex(std::ostream& out) const {
    out << "0x";
    if (m_pieces.size()) {
        std::vector<unsigned int>::const_iterator i = m_pieces.end();
        while (i != m_pieces.begin()) {
            --i;
            unsigned int val = *i;
            for (int i=0; i<8; ++i) {
                char ch = (char)((val & 0xf0000000)>>28);
                if (ch >= 10)
                    ch += 'a'-10;
                else ch += '0';
                out << ch;
                val <<= 4;
            }
        }
    } else {
        out << "0";
    }
}

std::ostream& operator<<(std::ostream& stream, const LargeInt& rhs) {
    LargeInt temp;
    if (rhs.isNeg()) {
        temp = -rhs;
        stream << "-";
    } else {
        temp = rhs;
    }
    std::string str;
    if (temp.m_pieces.size() == 0)
        stream << '0';
    while (temp.m_pieces.size() > 0) {
        LargeInt v = temp % LargeInt(10);
        if (!v.m_pieces.size()) 
            str = '0' + str;
        else {
            char ch = ('0' + v.m_pieces.front());
            str = ch + str;
        }
        temp /= 10;
    }
    stream << str;
    return stream;
}

LargeInt::operator const double() const {
    bool bNeg = isNeg();
    LargeInt posVal;
    if (bNeg)
        posVal = -*this;
    else
        posVal = *this;
    double shift = 4294967296;
    std::vector<unsigned int>::const_iterator i = posVal.m_pieces.end();
    double retVal = 0;
    for (; i != posVal.m_pieces.begin();) {
        --i;
        retVal *= shift;
        retVal += (double)*i;
    }

    if (bNeg)
        return -retVal;
    else
        return retVal;
}
