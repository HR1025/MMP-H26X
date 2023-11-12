//
// H264SliceDecodingProcess.h
//
// Library: Coedec
// Package: H264
// Module:  H264
// 

#include "H264Common.h"

namespace Mmp
{
namespace Codec
{

/**
 * @sa  8.2 Slice decoding process - ISO 14496/10(2020)
 */
class H264SliceDecodingProcess
{
public:
    using ptr = std::shared_ptr<H264SliceDecodingProcess>;
public:
    H264SliceDecodingProcess();
    ~H264SliceDecodingProcess();
private:
    void DecodingProcessForPictureOrderCount(H264SliceHeaderSyntax::ptr slice, H264PictureContext::ptr picture);
    void DecodeH264PictureOrderCountType0(H264PictureContext::ptr prevPictrue, H264SpsSyntax::ptr sps, H264SliceHeaderSyntax::ptr slice, H264PictureContext::ptr picture);
    void DecodeH264PictureOrderCountType1(H264PictureContext::ptr prevPictrue, H264SpsSyntax::ptr sps, H264SliceHeaderSyntax::ptr slice, uint8_t nal_ref_idc, H264PictureContext::ptr picture);
    void DecodeH264PictureOrderCountType2(H264PictureContext::ptr prevPictrue, H264SpsSyntax::ptr sps, H264SliceHeaderSyntax::ptr slice, uint8_t nal_ref_idc, H264PictureContext::ptr picture);
private:
    H264PpsSyntax::ptr _pps;
    H264SpsSyntax::ptr _sps;
    H264PictureContext::ptr _prevPicture;
};

} // namespace Codec
} // namespace Mmp