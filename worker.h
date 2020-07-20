#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include "hash.h"

class worker {
    std::unordered_multimap<size_t, std::string> m_mapOfPaths;
    IHash *m_hash;
    size_t m_bulk_size;
    void remDup();
public:
    worker(std::string hashString, size_t bulk_size, const std::vector<std::string> &vectorOfPaths);
    void printDuplicate();
    void calculate();
};