#include "H264BinaryReader.h"

#include <cassert>
#include <cstdint>
#include <exception>
#include <stdexcept>
#include <string>

#include "H264Ultis.h"

namespace Mmp
{
namespace Codec
{

// Quick Note:
//
// See also : ISO 14496/10(2020) - 7.2 Specification of syntax functions, categories, and descriptors
//
//  - ae(v): context-adaptive arithmetic entropy-coded syntax element. The parsing process for this descriptor is specified 
//  in subclause 9.3.
//  - b(8): byte having any pattern of bit string (8 bits). The parsing process for this descriptor is specified by the return 
//  value of the function read_bits( 8 ).
//  - ce(v): context-adaptive variable-length entropy-coded syntax element with the left bit first. The parsing process for 
//  this descriptor is specified in subclause 9.2.
//  - f(n): fixed-pattern bit string using n bits written (from left to right) with the left bit first. The parsing process for this 
//  descriptor is specified by the return value of the function read_bits( n ).
//  - i(n): signed integer using n bits. When n is "v" in the syntax table, the number of bits varies in a manner dependent 
//  on the value of other syntax elements. The parsing process for this descriptor is specified by the return value of the 
//  function read_bits( n ) interpreted as a two's complement integer representation with most significant bit written first.
//  - me(v): mapped Exp-Golomb-coded syntax element with the left bit first. The parsing process for this descriptor is 
//  specified in subclause 9.1.
//  - se(v): signed integer Exp-Golomb-coded syntax element with the left bit first. The parsing process for this descriptor 
//  is specified in subclause 9.1.
//  - te(v): truncated Exp-Golomb-coded syntax element with left bit first. The parsing process for this descriptor is 
//  specified in subclause 9.1.
//  - u(n): unsigned integer using n bits. When n is "v" in the syntax table, the number of bits varies in a manner 
//  dependent on the value of other syntax elements. The parsing process for this descriptor is specified by the return 
//  value of the function read_bits( n ) interpreted as a binary representation of an unsigned integer with most significant 
//  bit written first.
//  - ue(v): unsigned integer Exp-Golomb-coded syntax element with the left bit first. The parsing process for this
//  descriptor is specified in subclause 9.1.
//

static uint8_t kLeftAndLookUp[8] = {0xFF, 0x7F, 0x3F, 0x1F, 0xFF, 0x07, 0x03, 0x01};
static uint8_t kRightAndLookUp[8] = {0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF};


H264BinaryReader::H264BinaryReader(AbstractH264ByteReader::ptr reader)
{
    _curBitPos = 8;
    _curValue = 0;
    _reader = reader;
}

H264BinaryReader::~H264BinaryReader()
{

}

void H264BinaryReader::UE(uint32_t& value)
{
    // See also : ISO 14496/10(2020) - 9.1 Parsing process for Exp-Golomb codes
    int32_t leadingZeroBits = -1;
    uint64_t tmp;
    for (uint8_t b = 0; !b; leadingZeroBits++)
    {
        U(1, b);
    }
    U(leadingZeroBits, tmp);
    value = (1 << leadingZeroBits) - 1 + tmp;
}

void H264BinaryReader::SE(int32_t& value)
{
    // See also : ISO 14496/10(2020) -  Table 9-3 – Assignment of syntax element to codeNum for signed Exp-Golomb coded syntax elements se(v)}
    uint32_t codeNum = 0;
    UE(codeNum);
    if (codeNum % 2 == 0)
    {
        value = -(codeNum >> 1);
    }
    else
    {
        value = (codeNum >> 1) + 1;
    }
}

#define MMP_U_OPERATION(bits, value)            value = 0;\
                                                bool firstFlag = false;\
                                                do\
                                                {\
                                                    ReadOneByteAuto();\
                                                    size_t readBits = bits <= (8 - _curBitPos)? bits : (8 - _curBitPos);\
                                                    bits = bits - readBits;\
                                                    value <<= readBits;\
                                                    if (readBits < 8 && !firstFlag)\
                                                    {\
                                                        value |= (_curValue & kLeftAndLookUp[_curBitPos]) >> (8 - _curBitPos - readBits);\
                                                    }\
                                                    else if (readBits < 8)\
                                                    {\
                                                        value |= (_curValue & kRightAndLookUp[_curBitPos]) >> (8 - _curBitPos - readBits);\
                                                    }\
                                                    else\
                                                    {\
                                                        value |= _curValue;\
                                                    }\
                                                    _curBitPos = _curBitPos + readBits;\
                                                    firstFlag = true;\
                                                } while (bits != 0);

#define MMP_U_PRED_OPERATION(bits, value)       uint8_t curBitPos = _curBitPos;\
                                                size_t curPosByte = curBitPos == 8 ? _reader->Tell() : _reader->Tell() - 1;\
                                                MMP_U_OPERATION(bits, value);\
                                                _reader->Seek(curPosByte);\
                                                if (curBitPos != 8)\
                                                {\
                                                    ReadOneByteAuto(true);\
                                                }\
                                                _curBitPos = curBitPos;

#define MMP_I_OPERATION(bits, value)            MMP_U_OPERATION(bits, value)

#define MMP_I_PRED_OPERATION(bits, value)       MMP_U_PRED_OPERATION(bits, value)

void H264BinaryReader::U(size_t bits)
{
    assert(false);
}

void H264BinaryReader::U(size_t bits, uint64_t& value)
{
    if (bits > 64)
    {
        throw std::out_of_range(std::string());
    }
    MMP_U_OPERATION(bits, value);
}

void H264BinaryReader::U(size_t bits, uint32_t& value, bool probe)
{
    if (bits > 32)
    {
        throw std::out_of_range(std::string());
    }
    if (!probe)
    {
        MMP_U_OPERATION(bits, value);
    }
    else
    {
        MMP_U_PRED_OPERATION(bits, value);
    }

}

void H264BinaryReader::U(size_t bits, uint16_t& value)
{
    if (bits > 16)
    {
        throw std::out_of_range(std::string());
    }
    MMP_U_OPERATION(bits, value);
}

void H264BinaryReader::U(size_t bits, uint8_t& value, bool probe)
{
    if (bits > 8)
    {
        throw std::out_of_range(std::string());
    }
    if (!probe)
    {
        MMP_U_OPERATION(bits, value);
    }
    else
    {
        MMP_U_PRED_OPERATION(bits, value);
    }
}

void H264BinaryReader::I(size_t bits, int64_t& value)
{
    if (bits > 64)
    {
        throw std::out_of_range(std::string());
    }
    MMP_I_OPERATION(bits, value);
}

void H264BinaryReader::I(size_t bits, int32_t& value)
{
    if (bits > 32)
    {
        throw std::out_of_range(std::string());
    }
    MMP_I_OPERATION(bits, value);
}

void H264BinaryReader::I(size_t bits, int16_t& value)
{
    if (bits > 16)
    {
        throw std::out_of_range(std::string());
    }
    MMP_I_OPERATION(bits, value);
}

void H264BinaryReader::I(size_t bits, int8_t& value)
{
    if (bits > 8)
    {
        throw std::out_of_range(std::string());
    }
    MMP_I_OPERATION(bits, value);
}

#undef MMP_I_PRED_OPERATION
#undef MMP_I_OPERATION
#undef MMP_U_PRED_OPERATION
#undef MMP_U_OPERATION

void H264BinaryReader::B8(uint8_t& value)
{
    U(8, value);
}

void H264BinaryReader::Skip(size_t bits)
{
    if (bits + _curBitPos < 8) // 不需要跳转至下一个字节
    {
        _curBitPos = bits + _curBitPos;
        return;
    }
    else // 需要跳转的 bits 先用消费当前 byte 剩余的 bits
    {
        bits = bits - (8 - _curBitPos);
        _curBitPos = 8;
    }
    size_t skipByte = bits / 8; // 计算需要跳转的字节数
    if (skipByte)
    {
        _reader->Seek(_reader->Tell() + skipByte);
    }
    ReadOneByteAuto(true);
    _curBitPos = bits % 8;
}

bool H264BinaryReader::more_rbsp_data()
{
    // Hint :
    // more_rbsp_data( ) is specified as follows:
    // - If there is no more data in the RBSP, the return value of more_rbsp_data( ) is equal to FALSE.
    // - Otherwise, the RBSP data is searched for the last (least significant, right-most) bit equal to 1 that is present in the 
    //   RBSP. Given the position of this bit, which is the first bit (rbsp_stop_one_bit) of the rbsp_trailing_bits( ) syntax 
    //   structure, the following applies:
    // - If there is more data in an RBSP before the rbsp_trailing_bits( ) syntax structure, the return value of 
    //   more_rbsp_data( ) is equal to TRUE.
    // - Otherwise, the return value of more_rbsp_data( ) is equal to FALSE.
    //
    // int8_t tmp = 0;

    // U(1, tmp, true);
    // if (tmp == 1 && CurrentBits() + 1 == 8)
    // {
    //     return false;
    // }

    return true;
}

void H264BinaryReader::rbsp_trailing_bits()
{
    // Hint :
    // The RBSP contains an SODB as follows:
    // - If the SODB is empty (i.e., zero bits in length), the RBSP is also empty.
    // - Otherwise, the RBSP contains the SODB as follows:
    //   1) The first byte of the RBSP contains the (most significant, left-most) eight bits of the SODB; the next byte of 
    //      the RBSP contains the next eight bits of the SODB, etc., until fewer than eight bits of the SODB remain.
    //   2) rbsp_trailing_bits( ) are present after the SODB as follows:
    //     i)   The first (most significant, left-most) bits of the final RBSP byte contains the remaining bits of the SODB 
    //          (if any).
    //     ii)  The next bit consists of a single rbsp_stop_one_bit equal to 1.
    //     iii) When the rbsp_stop_one_bit is not the last bit of a byte-aligned byte, one or more 
    //          rbsp_alignment_zero_bit is present to result in byte alignment.
    //   3) One or more cabac_zero_word 16-bit syntax elements equal to 0x0000 may be present in some RBSPs after the 
    //      rbsp_trailing_bits( ) at the end of the RBSP.
    // 
    // int8_t tmp = 0;
    // do
    // {
    //     U(1, tmp);
    //     assert(!(tmp == 0 && CurrentBits() != 7));
    // } while (!(tmp == 1 && CurrentBits() == 8));
}

bool H264BinaryReader::more_data_in_byte_stream()
{
    // Hint :
    // more_data_in_byte_stream( ), which is used only in the byte stream NAL unit syntax structure specified in Annex B, is 
    // specified as follows:
    // - If more data follow in the byte stream, the return value of more_data_in_byte_stream( ) is equal to TRUE.
    // - Otherwise, the return value of more_data_in_byte_stream( ) is equal to FALSE.
    assert(false);
    return true;
}

bool H264BinaryReader::End()
{
    assert(false);
    return true;
}

int8_t H264BinaryReader::CurrentBits()
{
    return 8;
}

bool H264BinaryReader::ReadBytes(size_t byte, uint8_t* value)
{
    size_t readByte = _reader->Read(value, byte);
    return readByte == byte;
}

void H264BinaryReader::ReadOneByteAuto(bool force)
{
    if (_curBitPos == 8 || force)
    {
        ReadBytes(1, &_curValue);
        _curBitPos = 0;
    }
}

} // namespace Codec
} // namespace Mmp