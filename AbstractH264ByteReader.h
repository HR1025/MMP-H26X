//
// AbstractH264ByteReader.h
//
// Library: Coedec
// Package: H264
// Module:  H264
// 

#pragma once

#include "H264Common.h"

namespace Mmp
{
namespace Codec
{

class AbstractH264ByteReader
{
public:
    using ptr = std::shared_ptr<AbstractH264ByteReader>;
public:
    AbstractH264ByteReader();
    virtual ~AbstractH264ByteReader();
public:
    /**
     * @brief     read raw data from the bytes stream
     * @param[out] data
     * @param[in]  bytes
     * @return     actual read bytes
     */
    virtual size_t Read(void* data, size_t bytes) = 0;
    /**
     * @brief     go to the bytes position of the bytes stream
     * @param[in] offset
     */
    virtual bool Seek(size_t offset) = 0;
    /**
     * @brief get current bytes postion of the bytes stream
     */
    virtual size_t Tell() = 0;
    /**
     * @brief reach the end of the stream
     */
    virtual bool Eof() = 0;
};

} // namespace Codec
} // namespace Mmp