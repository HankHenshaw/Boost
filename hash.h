#pragma once

#include <boost/lexical_cast.hpp>
#include <boost/crc.hpp>
#include <boost/uuid/detail/md5.hpp>
#include <boost/uuid/detail/sha1.hpp>
#include <string>

class IHash {
public:
    virtual void calcHash(char *buffer, size_t size) = 0;
    virtual const std::string& getHash() = 0;
    virtual ~IHash() = default;
};

class Crc16 : public IHash {
    std::string m_hash;
    boost::crc_16_type m_crc16;
public:
    Crc16();
    virtual void calcHash(char *buffer, size_t size);
    virtual const std::string& getHash()
    {
        return m_hash;
    }
    virtual ~Crc16() = default;
};

class Crc32 : public IHash {
    std::string m_hash;
    boost::crc_32_type m_crc32;
public:
    Crc32();
    virtual void calcHash(char *buffer, size_t size);
    virtual const std::string& getHash()
    {
        return m_hash;
    }
    virtual ~Crc32() = default;
};

class Md5 : public IHash {
    std::string m_hash;
    boost::uuids::detail::md5::digest_type m_gt_md5;

    std::string toString(const boost::uuids::detail::md5::digest_type &digest);
public:
    Md5();
    virtual void calcHash(char *buffer, size_t size);
    virtual const std::string& getHash()
    {
        return m_hash;
    }
    virtual ~Md5() = default;
};

class Sha1 : public IHash {
    std::string m_hash;
    boost::uuids::detail::sha1::digest_type m_gt_sha1;

    std::string toString(const boost::uuids::detail::sha1::digest_type &digest);
public:
    Sha1();
    virtual void calcHash(char *buffer, size_t size);
    virtual const std::string& getHash()
    {
        return m_hash;
    }
    virtual ~Sha1() = default;
};