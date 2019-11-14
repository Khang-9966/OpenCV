#ifndef ARGMANAGER_H
#define ARGMANAGER_H

#include <list>
#include <string>
#include <sstream>

template <typename out_type, typename in_type>
inline out_type typeConverter(const in_type& in) {
    std::stringstream convert;
    convert << in;
    out_type retVal;
    convert >> retVal;
    return retVal;
}

class ArgManager {
    char* m_progName;
    char** m_keys;
    char** m_values;
    int m_numArgs;

private:
    void copy(const ArgManager& rhs);
    void cleanup();
public:
    ArgManager(int argc, char** argv);
    ~ArgManager();
    ArgManager(const ArgManager& rhs);
#ifndef SWIG
    ArgManager& operator=(const ArgManager& rhs);
#endif
public:
    bool hasKey(const char* const key, bool bCompareCase=false) const;
    int keyPos(const char* const key, bool bCompareCase=false) const;
    const char* const getKey(int idx);
    const char* const getValue(const char* const key, bool bCompareCase=false) const;
    const char* const getValue(const char* const key,
                               const char* const defaultVal,
                               bool bCompareCase=false) const;
    const char* const getProgName() const;
    int getNumArgs() const;
};

#endif