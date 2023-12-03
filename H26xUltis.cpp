#include "H26xUltis.h"

#include "H264Common.h"

namespace Mmp
{
namespace Codec
{

std::string H264NaluTypeToStr(uint8_t nal_unit_type)
{
    switch (nal_unit_type)
    {
        case H264NaluType::MMP_H264_NALU_TYPE_NULL: return "NULL";
        case H264NaluType::MMP_H264_NALU_TYPE_SLICE: return "SLICE";
        case H264NaluType::MMP_H264_NALU_TYPE_DPA: return "DPA";
        case H264NaluType::MMP_H264_NALU_TYPE_DPB: return "DPB";
        case H264NaluType::MMP_H264_NALU_TYPE_DPC: return "DPC";
        case H264NaluType::MMP_H264_NALU_TYPE_IDR: return "IDR";
        case H264NaluType::MMP_H264_NALU_TYPE_SEI: return "SEI";
        case H264NaluType::MMP_H264_NALU_TYPE_SPS: return "SPS";
        case H264NaluType::MMP_H264_NALU_TYPE_PPS: return "PPS";
        case H264NaluType::MMP_H264_NALU_TYPE_AUD: return "AUD";
        case H264NaluType::MMP_H264_NALU_TYPE_EOSEQ: return "EOSEQ";
        case H264NaluType::MMP_H264_NALU_TYPE_EOSTREAM: return "EOSTREAM";
        case H264NaluType::MMP_H264_NALU_TYPE_FILL: return "FILL";
        case H264NaluType::MMP_H264_NALU_TYPE_SPSEXT: return "SPSEXT";
        case H264NaluType::MMP_H264_NALU_TYPE_PREFIX: return "PREFIX";
        case H264NaluType::MMP_H264_NALU_TYPE_SUB_SPS: return "SUB_SPS";
        case H264NaluType::MMP_H264_NALU_TYPE_SLICE_AUX: return "SLICE_AUX";
        case H264NaluType::MMP_H264_NALU_TYPE_SLC_EXT: return "SLC_EXT";
        case H264NaluType::MMP_H264_NALU_TYPE_VDRD: return "VDRD";
        default: return "UNKNOWN";
    }
}

std::string H264SliceTypeToStr(uint8_t slice_type)
{
    switch (slice_type)
    {
        case H264SliceType::MMP_H264_P_SLICE: return "P";
        case H264SliceType::MMP_H264_B_SLICE: return "B";
        case H264SliceType::MMP_H264_I_SLICE: return "I";
        case H264SliceType::MMP_H264_SP_SLICE: return "SP";
        case H264SliceType::MMP_H264_SI_SLICE: return "SI";
        default: return "?";
    }
}

} // namespace Codec
} // namespace Mmp