//
// H264BinaryReader.h
//
// Library: Coedec
// Package: H264
// Module:  H264
// 

#pragma once

#include "H264Common.h"
#include "AbstractH264ByteReader.h"

namespace Mmp
{
namespace Codec
{

/**
 * @sa ISO 14496/10(2020) - 7.2 Specification of syntax functions, categories, and descriptors
 */
class H264BinaryReader
{
public:
    using ptr = std::shared_ptr<H264BinaryReader>;
public:
    H264BinaryReader(AbstractH264ByteReader::ptr reader);
    virtual ~H264BinaryReader();
public:
    void UE(uint32_t& value);
public:
    void SE(int32_t& value);
public:
    void U(size_t bits);
    void U(size_t bits, uint64_t& value);
    void U(size_t bits, uint32_t& value, bool probe = false);
    void U(size_t bits, uint16_t& value);
    void U(size_t bits, uint8_t& value, bool probe = false);
public:
    void I(size_t bits, int64_t& value);
    void I(size_t bits, int32_t& value);
    void I(size_t bits, int16_t& value);
    void I(size_t bits, int8_t& value);
public:
    void B8(uint8_t& value);
public:
    void Skip(size_t bits);
public:
    bool more_rbsp_data();
    void rbsp_trailing_bits();
    bool more_data_in_byte_stream();
public:
    bool End();
    int8_t CurrentBits();
public:
    void ReadOneByteAuto(bool force = false);
    bool ReadBytes(size_t byte, uint8_t* value);
private:
    uint8_t  _curBitPos;
    uint8_t  _curValue;
private:
    AbstractH264ByteReader::ptr _reader;
};

} // namespace Codec
} // namespace Mmp