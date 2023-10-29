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
    FileH264ByteReader(const std::string& h264Path);
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
    H264NalSyntax::ptr nal = std::make_shared<H264NalSyntax>();
    deserialize->DeserializeByteStreamNalUnit(binaryReader, nal);
    
    return 0;
}