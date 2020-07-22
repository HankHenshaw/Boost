#include <iostream>
#include <string>
#include <vector>
#include <boost/program_options.hpp>
#include "hash.h"
#include "commandhandler.h"
#include "worker.h"

//Через коммандную строку указывать имя1 имя2 ... размер блока S хэш ф-цию H
//Программа должна выводить полные пути до идентичных файлов, 1 файл на одной строке
//Группы идетичныйх файлов разделяются пустой сторокой
//Если файл не кратен блоку S, то он дополняется \0

//Н - Список хэшей md5, crc16, crc32, sha1
//S - Размер блока
//Имя файлов(минимум 2)

namespace po = boost::program_options;

int main(int argc, char *argv[])
{   
    try {
        po::options_description desc{"Options"};
        desc.add_options()
                ("help,h", "This screen")
                ("size,S", po::value<size_t>()->default_value(5), "Block Size")
                ("hash,H", po::value<std::string>()->default_value("md5"), "Hash Function, 1 of this [md5, crc16, crc32, sha1]")
                ("input,i", po::value<std::vector<std::string>>(), "Input files, should be 2 or more files, or directory");

        po::variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        commandhandler handler(vm, desc);
        bool res = handler.proccess();

        if(!res)
        {
            return 0;
        }

        worker w(handler.getHashString(), handler.getChunkSize(), handler.getVector());
        w.calculate();
        w.printDuplicate();
    } 
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}