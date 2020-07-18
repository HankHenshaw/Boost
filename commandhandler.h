#pragma once

#include <boost/program_options.hpp>
#include <string>
#include <vector>

namespace po = boost::program_options;

class commandhandler
{
private:
    size_t m_chunk_size{5};
    std::string m_hash{"md5"};
    std::vector<std::string> m_vecOfPaths;

    po::variables_map m_vm;
    po::options_description m_desc;

    bool isHashStrGood();
    bool isPathsGood();
    size_t filesCount();
public:
    commandhandler(po::variables_map &vm, po::options_description& desc);
    bool proccess();
    ~commandhandler() = default;
};
