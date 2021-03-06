#include "hash.h"
#include <boost/algorithm/hex.hpp>
#include <boost/lexical_cast.hpp>

//TODO: Глянуть будет ли наложение стрингов, т.к. в алгоритме перевода в хекс, использутеся back_inserter 
//TODO: Нужен ли toUpper для стрингов хэша?

Crc16::Crc16()
{

}

void Crc16::calcHash(char *buffer, size_t size)
{
    m_hash.clear();
    m_crc16.reset();
    m_crc16.process_bytes(buffer, size);
    std::string bareStr = boost::lexical_cast<std::string>(m_crc16.checksum());
    boost::algorithm::hex(bareStr, std::back_inserter(m_hash));
}

Crc32::Crc32()
{
    
}

void Crc32::calcHash(char *buffer, size_t size)
{
    m_hash.clear();
    m_crc32.reset();
    m_crc32.process_bytes(buffer, size);
    std::string bareStr = boost::lexical_cast<std::string>(m_crc32.checksum());
    boost::algorithm::hex(bareStr, std::back_inserter(m_hash));
}

Md5::Md5()
{
    
}

std::string Md5::toString(const boost::uuids::detail::md5::digest_type &digest)
{
    const auto charDigest = reinterpret_cast<const char*>(&digest);
    std::string result;
    boost::algorithm::hex(charDigest, charDigest + sizeof(boost::uuids::detail::md5::digest_type), 
                        std::back_inserter(result));
    return result;
}

void Md5::calcHash(char *buffer, size_t size)
{
    m_hash.clear();

    boost::uuids::detail::md5 m_md5;

    m_md5.process_bytes(buffer, size);
    m_md5.get_digest(m_gt_md5);

    m_hash = toString(m_gt_md5);
}

Sha1::Sha1()
{
    
}

std::string Sha1::toString(const boost::uuids::detail::sha1::digest_type &digest)
{
    const auto charDigest = reinterpret_cast<const char*>(&digest);
    std::string result;
    boost::algorithm::hex(charDigest, charDigest + sizeof(boost::uuids::detail::sha1::digest_type), 
                        std::back_inserter(result));
    return result;
}

void Sha1::calcHash(char *buffer, size_t size)
{
    m_hash.clear();

    boost::uuids::detail::sha1 m_sha1;

    m_sha1.process_bytes(buffer, size);
    m_sha1.get_digest(m_gt_sha1);

    m_hash = toString(m_gt_sha1);
}