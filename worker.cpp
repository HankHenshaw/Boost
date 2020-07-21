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

    m_buffer = new char[m_bulk_size];
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

    fs::ifstream file;

    file.rdbuf()->pubsetbuf(nullptr, 0);

    auto it = m_mapOfPaths.begin();
    for(;it != m_mapOfPaths.end(); ++it) // Цикл по мапу
    {
        //Cycle


        auto bucket = m_mapOfPaths.bucket(it->first);
        auto bucket_it = m_mapOfPaths.begin(bucket);
        auto count = m_mapOfPaths.count(it->first);

        for(;bucket_it != m_mapOfPaths.end(bucket); ++bucket_it) // Цикл по бакеты
        {
            //Еще 1 цикл, пока файл не закончится или у него не будет отличающийся хэш
            file.open(bucket_it->second);
            if(!file.is_open())
            {
                std::cout << "Failed To Open " << bucket_it->second << " file\n";
                continue;
                //TODO: Подумать что тут сделать, т.е. прерывать всю операцию или нет(т.е. просто убрать файл из списка)
            } else {
                for(size_t i = 0; i < m_bulk_size; ++i)
                {
                    m_buffer[i] = '\0';
                }

                file.read(m_buffer, m_bulk_size);

                std::cout << "Cur Buffer:" << m_buffer << '\n';

                //TODO: Вся жара тут
                m_hash->calcHash(m_buffer, m_bulk_size);
                m_mapOfHashes.emplace(std::make_pair(bucket_it->second, m_hash->getHash()));

                file.close();
            }
        }

        std::advance(it, count - 1);
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

void worker::printHashes()
{
    for(const auto &val: m_mapOfHashes)
    {
        std::cout << "File Path:" << val.first << " | Hash:" << val.second << '\n';  
    }
}

worker::~worker()
{
    delete[] m_buffer;
}