#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "commandhandler.h"

namespace fs = boost::filesystem;

//#define DEBUG //TODO: Remove

commandhandler::commandhandler(po::variables_map &vm, po::options_description& desc)
        : m_vm(vm), m_desc(desc)
{
    if (m_vm.count("help"))
        std::cout << desc << '\n';
}

size_t commandhandler::filesCount()
{
    size_t count = 0;
    for(const auto& val: m_vecOfPaths)
    {
        fs::path path(val);
        if(fs::is_directory(path))
        {
            fs::recursive_directory_iterator rdi(path);
            while(rdi != fs::recursive_directory_iterator())
            {
                m_vecOfFiles.emplace_back(boost::lexical_cast<std::string>(*rdi));
                ++count;
                ++rdi;
            }
        } else {
            m_vecOfFiles.emplace_back(val);
            ++count;
        }
    }

#ifdef DEBUG
    std::cout << "Files Count:" << count << '\n';
#endif

    return count;
}

bool commandhandler::isHashStrGood()
{
    return m_hash == "sha1" || m_hash == "md5" || m_hash == "crc16" || m_hash == "crc32";
}

bool commandhandler::isPathsGood()
{
    for(const auto& path : m_vecOfPaths)
    {
        if(access(path.c_str(), F_OK) == -1)
        {
            std::cout << "Can't get access to " << path << " file\n";
            return false;
        }
    }
    
    if(filesCount() < 2) 
    {
        std::cout << "Number of files less than 2\n";
        return false;
    }
    return true;
}

bool commandhandler::proccess()
{
    try
    {
        m_vecOfPaths = m_vm["input"].as<std::vector<std::string>>();
    }
    catch(const std::exception& e)
    {
        std::cout << "You have not set filename(s)" << '\n';
        std::cerr << e.what() << '\n';
        return false;
    }

    //Проверка доступа к файлам/директориям
    if(!isPathsGood()) return false;

    m_chunk_size = m_vm["size"].as<size_t>();
    if(m_chunk_size > 1000)
    {
        std::cout << "Chunk Size if too big\n"
                  << "Set chank to default(5) value\n";
    }

    //Проверка и установка хэша
    m_hash = m_vm["hash"].as<std::string>();
    if(!isHashStrGood())
    {
        std::cout << "Wrong Hash Name\n"
                  << "Hash set to default(md5) value\n";
        m_hash = "md5";
    }

#ifdef DEBUG
    std::cout << "Chunk Size:" << m_chunk_size << '\n';
    std::cout << "Hash:" << m_hash << '\n';
    std::cout << "Paths Size:" << m_vecOfPaths.size() << '\n';
#endif

    return true;
}

size_t commandhandler::getChunkSize()
{
    return m_chunk_size;
}

std::string commandhandler::getHashString()
{
    return m_hash;
}

const std::vector<std::string>& commandhandler::getVector()
{
    return m_vecOfFiles;
}