#include "worker.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace fs = boost::filesystem;

#define DEBUG // TODO: Remove

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
        std::cout << val.size() << '\n';
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

    if(m_mapOfPaths.size() < 2)
    {
        return;
    }

    auto it = m_mapOfPaths.begin();
    for(;it != m_mapOfPaths.end(); ++it) // Цикл по мапу
    {
#ifdef DEBUG
        std::cout << "Map Cycle\n";
#endif
        fs::ifstream::pos_type file_pos; // Позиция в файле
        fs::path path(it->second);
        auto file_size = fs::file_size(path);
        size_t read_size = 0;
        size_t second_read_size = 0;

        while(1) // Цикл пока не проверяться все файлы из 1-ого бакета
        {
#ifdef DEBUG
            std::cout << "File Cycle\n";
#endif
            file.open(it->second);
            if(!file.is_open())
            {
                std::cout << "Failed to open " << it->second << " file\n";
            }

            if(!file_size) // Если файл пустой
            { //TODO: Могут быть проблемы при переходе на другой бакет?
                break;
            }

            if(file_size - read_size < m_bulk_size) // Если в файле осталось меньше символов чем размер блока
            {
                size_t rest = file_size - read_size;
                file.seekg(file_pos); // Переход на предыдущую поз-цию
                file.read(m_buffer, rest);
                read_size += rest;
#ifdef DEBUG
                    std::cout << "First Rest:" << rest << '\n';
#endif
                for(size_t i = rest - 1; i < m_bulk_size; ++i)
                {
                    m_buffer[i] = 'X'; // Дозаполняем нулями до размера блока
                }
            } else {
                file.seekg(file_pos); // Переход на предыдущую поз-цию
                file.read(m_buffer, m_bulk_size);
                read_size += m_bulk_size;
            }

            file.close();

            m_hash->calcHash(m_buffer, m_bulk_size);
            std::string main_hash_string = m_hash->getHash();

            auto bucket = m_mapOfPaths.bucket(it->first);
            auto bucket_it = m_mapOfPaths.begin(bucket);
            ++bucket_it; // Т.к. сравнивать с самим собой нету смысла
            auto count = m_mapOfPaths.count(it->first);

            fs::path second_path(bucket_it->second);
            auto second_file_size = fs::file_size(second_path);

            // char x;
            // std::cout << "Enter: ";
            // std::cin >> x;
            // std::cout << "Buffer = " << m_buffer << '\n';

            for(;bucket_it != m_mapOfPaths.end(bucket); ++bucket_it) // Цикл по бакету
            {
#ifdef DEBUG
                std::cout << "Bucket Cycle\n";
#endif

                file.open(bucket_it->second);
                if(!file.is_open() || second_read_size >= second_file_size)
                {
                    std::cout << "Failed To Open " << bucket_it->second << " file\n";
                    continue;
                    //TODO: Подумать что тут сделать, т.е. прерывать всю операцию или нет(т.е. просто убрать файл из списка)
                }
#ifdef DEBUG
                    std::cout << "Second Size:" << second_file_size << '\n';
                    std::cout << "Second Read:" << second_read_size << '\n';

#endif
                if(second_file_size - second_read_size < m_bulk_size) // Если в файле осталось меньше символов чем размер блока
                {
                    size_t second_rest = second_file_size - second_read_size;
                    file.seekg(file_pos);
                    file.read(m_buffer, second_rest);
                    second_read_size += second_rest;
#ifdef DEBUG
                    std::cout << "Second Rest:" << second_rest << '\n';
#endif
                    for(size_t i = second_rest - 1; i < m_bulk_size; ++i)
                    {
                        m_buffer[i] = 'X'; // Дозаполняем нулями до размера блока
                    }
                } else {
                    file.seekg(file_pos);
                    file.read(m_buffer, m_bulk_size);
                    second_read_size += m_bulk_size;
                }
                file_pos = file.tellg();
                file.close();
                std::cout << "Buffer 2 = " << m_buffer << '\n';

                m_hash->calcHash(m_buffer, m_bulk_size);
                std::string second_hash_string = m_hash->getHash();
#ifdef DEBUG
                std::cout << "Hash 1: " << main_hash_string << '\n'
                          << "Hash 2: " << second_hash_string << '\n';
#endif
                if(main_hash_string == second_hash_string) // Если хэши у первой строки и у другой одинаковы
                {
                    if(second_read_size >= second_file_size) // Если хэши все еще одинаковые на конце файлов
                    {
                        m_mapOfDuples.insert(std::make_pair(it->second, bucket_it->second));
                    }
                    continue;
                } else {
                    //Надо как-то сигнализировать что хэш у другого файла другой(удалять нельзя, иначе эти файлы не будут проверены)
                    //Менять ключ?
                }
            }
#ifdef DEBUG
            std::cout << "Read = " << read_size << " | File = " << file_size << '\n';
#endif
            if(read_size >= file_size) // Если кол-во считаных файлов не меньше чем размер файла, то смещаем итератор на следующий
            {//Бакет и выходим из цикла
                std::advance(it, count - 1);
                break;
            } 
        }
    }
}

void worker::printDuplicate()
{
    if(m_mapOfDuples.empty())
    {
        std::cout << "There are no duplicate\n";
    } else {
        for(const auto& val: m_mapOfDuples)
        {
            std::cout << "File:" << val.first << " | Duplicate:" << val.second << '\n';
        }
        //TODO: Печать разделить разные дубликаты н-р пустой строкой
    }
}

worker::~worker()
{
    delete[] m_buffer;
}