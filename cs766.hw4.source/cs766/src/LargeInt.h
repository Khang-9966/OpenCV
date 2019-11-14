///////////////////////////////////////////////////////////////////////////////
// Written by Tom Brunet
// for CS739 Project, Spring 2004
// Added to VascViz Summer 2004
////
#pragma once

#include <vector>
#include <iostream>

/**
 * Little endian large integer counter
 */
class LargeInt {
public:
    LargeInt();
    LargeInt(int count);

    ~LargeInt();

public: // Arithmetical operators
    LargeInt& operator+=(const LargeInt& lc);
    LargeInt& operator-=(const LargeInt& lc);
    LargeInt& operator*=(const LargeInt& lc);
    LargeInt& operator/=(const LargeInt& lc);
    LargeInt& operator%=(const LargeInt& lc);
    LargeInt& operator<<=(unsigned int leftshift);
    LargeInt& operator>>=(unsigned int rightshift);

    LargeInt operator+(const LargeInt& lc) const;
    LargeInt operator-(const LargeInt& lc) const;
    LargeInt operator*(const LargeInt& lc) const;
    LargeInt operator/(const LargeInt& lc) const;
    LargeInt operator%(const LargeInt& lc) const;
    LargeInt operator<<(unsigned int leftshift) const;
    LargeInt operator>>(unsigned int rightshift) const;
    LargeInt operator-() const;
    
public: // Comparison operators
    bool operator==(const LargeInt& lc) const;

    bool operator>(const LargeInt& lc) const;

    inline bool operator<=(const LargeInt& lc) const {
        return !(*this>lc); }

    inline bool operator!=(const LargeInt& lc) const {
        return !(*this==lc); }

    inline bool operator>=(const LargeInt& lc) const {
        return (*this==lc || *this > lc); }

    inline bool operator<(const LargeInt& lc) const {
        return !(*this==lc || *this>lc); }

public: // Type casts
#ifndef SWIG
    operator const double() const;
#endif

public: // Other
    int toInt() const;

    double toDouble() const {
        return (double) *this; }

    void outHex(std::ostream&) const;
#ifndef SWIG
    friend std::ostream& operator <<(std::ostream& stream, const LargeInt& rhs);
#endif
private:
    bool isNeg() const;
    void pad(unsigned int numPad);
    void unpad();
    inline unsigned int getBit(unsigned int bit) {
        return (m_pieces[bit/32] >> (bit%32)) & 0x1;
    }

private:
#pragma warning(push)
#pragma warning(disable:4251)
    std::vector<unsigned int> m_pieces;
#pragma warning(pop)
};
