/**
 * Tom Brunet (login: tomab)
 * CS 766
 * Programming Project 3
 * Due 11/09/04
 */
#include "ArgManager.h"

ArgManager::ArgManager(int argc, char** argv) {
    m_progName = new char[strlen(argv[0])+1];
    strcpy(m_progName, argv[0]);
    m_keys = new char*[argc-1];
    m_values = new char*[argc-1];
    for (int i=1; i<argc; ++i) {
        char* eq = strchr(argv[i], '=');
        if (eq) {
            __w64 keyLen = eq-argv[i];
            m_keys[i-1] = new char[keyLen+1];
            strncpy(m_keys[i-1], argv[i], keyLen);
            *(m_keys[i-1]+keyLen) = 0;
            m_values[i-1] = new char[strlen(argv[i])-keyLen];
            strcpy(m_values[i-1], eq+1);
        } else {
            m_keys[i-1] = new char[strlen(argv[i])+1];
            strcpy(m_keys[i-1], argv[i]);
            m_values[i-1] = 0;
        }
    }
    m_numArgs = argc-1;
}

void ArgManager::copy(const ArgManager& rhs) {
    m_numArgs = rhs.m_numArgs;
    m_progName = new char[strlen(rhs.m_progName)+1];
    strcpy(m_progName, rhs.m_progName);
    m_keys = new char*[m_numArgs];
    m_values = new char*[m_numArgs];
    for (int i=0; i<m_numArgs; ++i) {
        if (rhs.m_keys[i]) {
            m_keys[i] = new char[strlen(rhs.m_keys[i]+1)];
            strcpy(m_keys[i], rhs.m_keys[i]);
        } else {
            m_keys[i] = 0;
        }
        if (rhs.m_values[i]) {
            m_values[i] = new char[strlen(rhs.m_values[i]+1)];
            strcpy(m_values[i], rhs.m_values[i]);
        } else {
            m_values[i] = 0;
        }
    }
}

void ArgManager::cleanup() {
    delete []m_progName;
    for (int i=0; i<m_numArgs; ++i) {
        if (m_keys[i])
            delete []m_keys[i];
        if (m_values[i])
            delete []m_values[i];
    }
    delete []m_keys;
    delete []m_values;
}

ArgManager::ArgManager(const ArgManager& rhs) {
    copy(rhs);
}

ArgManager& ArgManager::operator=(const ArgManager& rhs) {
    if (this != &rhs) {
        cleanup();
        copy(rhs);
    }
    return *this;
}

ArgManager::~ArgManager() {
    cleanup();
}

bool ArgManager::hasKey(const char* const key, bool bCompareCase) const {
    if (bCompareCase) {
        for (int i=0; i<m_numArgs; ++i)
            if (!strcmp(key, m_keys[i])) return true;
    } else {
        for (int i=0; i<m_numArgs; ++i)
            if (!stricmp(key, m_keys[i])) return true;
    }
    return false;
}

int ArgManager::keyPos(const char* const key, bool bCompareCase) const {
    if (bCompareCase) {
        for (int i=0; i<m_numArgs; ++i)
            if (!strcmp(key, m_keys[i])) return i;
    } else {
        for (int i=0; i<m_numArgs; ++i)
            if (!stricmp(key, m_keys[i])) return i;
    }
    return -1;
}

const char* const ArgManager::getKey(int idx) {
    if (idx >= 0 && idx < m_numArgs)
        return m_keys[idx];
    else return 0;
}

const char* const ArgManager::getValue(const char* const key, bool bCompareCase) const {
    if (bCompareCase) {
        for (int i=0; i<m_numArgs; ++i)
            if (!strcmp(key, m_keys[i])) return m_values[i];
    } else {
        for (int i=0; i<m_numArgs; ++i)
            if (!stricmp(key, m_keys[i])) return m_values[i];
    }
    return 0;
}

const char* const ArgManager::getValue(const char* const key,
                                       const char* const defaultVal,
                                       bool bCompareCase) const
{
    if (bCompareCase) {
        for (int i=0; i<m_numArgs; ++i)
            if (!strcmp(key, m_keys[i])) return m_values[i];
    } else {
        for (int i=0; i<m_numArgs; ++i)
            if (!stricmp(key, m_keys[i])) return m_values[i];
    }
    return defaultVal;
}

const char* const ArgManager::getProgName() const {
    return m_progName;
}

int ArgManager::getNumArgs() const {
    return m_numArgs;
}
