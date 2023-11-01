#include <iostream>
#include <fstream>
#include <cassert>
#include <sstream>

#include "AbstractH264ByteReader.h"
#include "H264BinaryReader.h"
#include "H264Deserialize.h"

namespace Mmp
{
namespace Codec
{

/**
 * @brief simple AbstractH264ByteReader implemention based on std::ifstream 
 */
class FileH264ByteReader : public Mmp::Codec::AbstractH264ByteReader
{
public:
    explicit FileH264ByteReader(const std::string& h264Path);
    ~FileH264ByteReader();
public:
    size_t Read(void* data, size_t bytes) override;
    bool Seek(size_t offset) override;
    size_t Tell() override;
    bool Eof() override;
private:
    std::ifstream _ifs;
};

FileH264ByteReader::FileH264ByteReader(const std::string& h264Path)
{
    _ifs.open(h264Path, std::ios::in | std::ios::binary);
    if (!_ifs.is_open())
    {
        assert(false);
        exit(255);
    }
}

FileH264ByteReader::~FileH264ByteReader()
{
    _ifs.close();
}

size_t FileH264ByteReader::Read(void* data, size_t bytes)
{
    _ifs.read((char*) data, bytes);
    return _ifs.gcount();
}

bool FileH264ByteReader::Seek(size_t offset)
{
    _ifs.seekg(offset);
    return _ifs.tellg() == offset;
}

size_t FileH264ByteReader::Tell()
{
    return _ifs.tellg();
}

bool FileH264ByteReader::Eof()
{
    return _ifs.eof();
}

} // namespace Codec
} // namespace Mmp

using namespace Mmp::Codec;

static std::string NalUintTypeToStr(uint32_t nal_unit_type)
{
    switch (nal_unit_type)
    {
        case H264NaluType::MMP_H264_NALU_TYPE_NULL: return "null";
        case H264NaluType::MMP_H264_NALU_TYPE_SLICE: return "slice";
        case H264NaluType::MMP_H264_NALU_TYPE_DPA: return "dpa";
        case H264NaluType::MMP_H264_NALU_TYPE_DPB: return "dpb";
        case H264NaluType::MMP_H264_NALU_TYPE_DPC: return "dpc";
        case H264NaluType::MMP_H264_NALU_TYPE_IDR: return "idr";
        case H264NaluType::MMP_H264_NALU_TYPE_SEI: return "sei";
        case H264NaluType::MMP_H264_NALU_TYPE_SPS: return "sps";
        case H264NaluType::MMP_H264_NALU_TYPE_PPS: return "pps";
        case H264NaluType::MMP_H264_NALU_TYPE_AUD: return "aud";
        case H264NaluType::MMP_H264_NALU_TYPE_EOSEQ: return "eoseq";
        case H264NaluType::MMP_H264_NALU_TYPE_EOSTREAM: return "eostream";
        case H264NaluType::MMP_H264_NALU_TYPE_FILL: return "fill";
        case H264NaluType::MMP_H264_NALU_TYPE_SPSEXT: return "spsext";
        case H264NaluType::MMP_H264_NALU_TYPE_PREFIX: return "prefix";
        case H264NaluType::MMP_H264_NALU_TYPE_SUB_SPS: return "subsps";
        case H264NaluType::MMP_H264_NALU_TYPE_SLC_EXT: return "slcext";
        case H264NaluType::MMP_H264_NALU_TYPE_VDRD: return "vord";
        default:
            return "unkonwn";
    }
}

void Usage()
{
    std::stringstream ss;
    ss << "[usage] ./Sample xxx.h264" << std::endl;
    std::cout << ss.str() << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc != 2 || std::string(argv[1]).find(".h264") == std::string::npos)
    {
        Usage();
        return -1;
    }
    AbstractH264ByteReader::ptr byteReader = std::make_shared<FileH264ByteReader>(std::string(argv[1]));
    H264BinaryReader::ptr binaryReader = std::make_shared<H264BinaryReader>(byteReader);
    H264Deserialize::ptr deserialize = std::make_shared<H264Deserialize>();
    std::vector<H264NalSyntax::ptr> nals;
    bool res = true;
    int num = 0;
    do
    {
        num++;
        H264NalSyntax::ptr nal = std::make_shared<H264NalSyntax>();
        res = deserialize->DeserializeByteStreamNalUnit(binaryReader, nal);
        std::cout << "(" << num << ")" << "  "  << "[" << NalUintTypeToStr(nal->nal_unit_type) << "]" << std::endl;
        if (res)
        {
            nals.push_back(nal);
        }
    } while (res && !binaryReader->Eof());
    
    return 0;
}