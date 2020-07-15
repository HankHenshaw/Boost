#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <boost/program_options.hpp>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <boost/algorithm/hex.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <boost/circular_buffer.hpp> //Test

//Через коммандную строку указывать имя1 имя2 ... размер блока S хэш ф-цию H
//Программа должна выводить полные пути до идентичных файлов, 1 файл на одной строке
//Группы идетичныйх файлов разделяются пустой сторокой
//Если файл не кратен блоку S, то он дополняется \0

//Н - Список хэшей md5, crc16, crc32, sha1
//S - Размер блока
//Имя файлов(минимум 2)

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char *argv[])
{   
    try {
        po::options_description desc{"Options"};
        desc.add_options()
                ("help,h", "This screen")
                ("size,S", po::value<size_t>()->default_value(5), "Block Size")
                ("hash,H", po::value<std::string>()->default_value("md5"), "Hash Function, 1 of this [md5, crc16, crc32, sha1]")
                ("input,i", po::value<std::vector<std::string>>(), "Input files, should be 2 or more files");

        po::variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help"))
            std::cout << desc << '\n';
        else if (vm.count("hash"))
            std::cout << "Hash function set to: " << vm["hash"].as<std::string>() << '\n';
        else if (vm.count("size"))
            std::cout << "Block size set to : " << vm["size"].as<size_t>() << '\n';

        std::vector<std::string> input_vector;
        input_vector = vm["input"].as<std::vector<std::string>>();
        if(input_vector.size() < 1) //TODO 1 поменять 2
        {
            std::cout << desc << '\n'; //TODO: Поменять вывод на что-нибудь по лучше
            std::cout << "Fail\n";
            return -1;
        }
        else
        {
            for(auto i : input_vector)
            {
                std::cout << "File:" << i << '\n';
            }
        }
        //TODO: Проверка значения Н, что бы был один из реализованных хэшей
        //TODO: Проверка значения S, что бы было больше 0 и меньше нашего установленного буффера
        //TODO: Как вариант при не соответствии, ипользовать значения по умолчанию
        //TODO: Проверка что файл существует
        //TODO: Перед финальным коммитом изменить CMakeLists, чтобы boost нормльно искался на тревисе

        fs::path m_i(input_vector.at(0)); // Инициализация файлом
        std::cout << fs::absolute(m_i) << '\n'; // Вывод абсолютного пути
        std::cout << fs::canonical(m_i) << '\n'; // Убирает точки в отличае от абсолютного, так что лучше его использовать

        std::ifstream file;

        file.rdbuf()->pubsetbuf(nullptr, 0);
        file.open(input_vector.at(0));

        // char buffer[1000];
        // file.read(buffer, 8);
        // for(int i = 0; i < 8; ++i)
        //     std::cout << buffer[i];
        // std::cout << '\n';

        std::vector<char> v;
        boost::circular_buffer<char> cb{12}; // TODO: Доразобраться
        file.close();
    } 
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }

    /*Пример отключения буффера*/
    // std::ifstream file;

    // file.rdbuf()->pubsetbuf(nullptr, 0);
    // file.open();

    // char buffer[1000];
    // file.read(buffer, 11);
    // file.close();
    /*Пример отключения буффера*/
    return 0;
}