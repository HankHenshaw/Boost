#include "worker.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace fs = boost::filesystem;

worker::worker(std::string hashString, size_t bulk_size, const std::vector<std::string>& vectorOfPaths) 
        : m_bulk_size(bulk_size)
{
    if(hashString == "md5") {
        m_hash = new Md5;
    } else if(hashString == "crc16") {
        m_hash = new Crc16;
    } else if(hashString == "crc32") {
        m_hash = new Crc32;
    } else if(hashString == "sha1") {
        m_hash = new Sha1;
    }

    for(const auto &val : vectorOfPaths)
    {
        fs::path path(val);
        m_mapOfPaths.insert(std::make_pair(fs::file_size(path), val));
    }
}

void worker::remDup()
{
    //Удаляем все пути размеры которых не совпадают
#ifdef DEBUG
    std::cout << "Size Before = " << m_mapOfPaths.size() << "\n";
#endif
    for(auto &val : m_mapOfPaths)
    {
        if(m_mapOfPaths.count(val.first) < 2)
        {
            m_mapOfPaths.erase(val.first);
        }
    }
#ifdef DEBUG
    std::cout << "Size After = " << m_mapOfPaths.size() << "\n";
#endif
}

void worker::calculate()
{
    remDup(); // Убираем первичные дубликаты 

    auto it = m_mapOfPaths.begin();
    for(;it != m_mapOfPaths.end(); ++it)
    {

    }
}

void worker::printDuplicate()
{
    if(m_mapOfPaths.empty())
    {
        std::cout << "There are no duplicate\n";
    } else {
        for(const auto& val: m_mapOfPaths)
        {
            std::cout << val.second << '\n';
        }
        //TODO: Печать разделить разные дубликаты н-р пустой строкой
    }
}