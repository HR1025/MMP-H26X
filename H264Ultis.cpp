#include "H264Ultis.h"

namespace Mmp
{
namespace Codec
{

static void DecodeH264PictureOrderCountType0(H264SpsSyntax::ptr sps, H264SliceHeaderSyntax::ptr slice, H264PocContext::ptr poc)
{
    // ISO 14496/10(2020) - 8.2.1.1 Decoding process for picture order count type 0
    int32_t prevPicOrderCntMsb = 0;
    int32_t prevPicOrderCntLsb = 0;
    int32_t PicOrderCntMsb = 0;
    // determine prevPicOrderCntMsb and prevPicOrderCntLsb
    {
        if (slice->slice_type == H264SliceType::MMP_H264_I_SLICE)
        {
            prevPicOrderCntMsb = 0;
            prevPicOrderCntLsb = 0;
        }
        else
        {
            if (slice->drpm && slice->drpm->memory_management_control_operations.count(5))
            {
                if (!slice->bottom_field_flag)
                {
                    prevPicOrderCntMsb = 0;
                    prevPicOrderCntLsb  = poc->TopFieldOrderCnt;
                }
                else
                {
                    prevPicOrderCntMsb = 0;
                    prevPicOrderCntLsb = 0;
                }
            }
            else
            {
                prevPicOrderCntMsb = poc->prevPicOrderCntMsb;
                prevPicOrderCntLsb = poc->pic_order_cnt_lsb;
            }
        }
    }
    // determine PicOrderCntMsb
    {
        int64_t MaxPicOrderCntLsb = 1 << (sps->log2_max_pic_order_cnt_lsb_minus4 + 4);
        if ((slice->pic_order_cnt_lsb < prevPicOrderCntLsb) &&
            ((prevPicOrderCntLsb - slice->pic_order_cnt_lsb) >= (MaxPicOrderCntLsb / 2))
        )
        {
            PicOrderCntMsb = prevPicOrderCntMsb + MaxPicOrderCntLsb;
        }
        else if ((slice->pic_order_cnt_lsb > prevPicOrderCntLsb) && 
            ((slice->pic_order_cnt_lsb - prevPicOrderCntLsb) > (MaxPicOrderCntLsb / 2))
        )
        {
            PicOrderCntMsb = prevPicOrderCntMsb - MaxPicOrderCntLsb;
        }
        else
        {
            PicOrderCntMsb = prevPicOrderCntMsb;
        }
    }
    // determine TopFieldOrderCnt and BottomFieldOrderCnt
    {
#if 0 
        if (!slice->bottom_field_flag)
        {
            poc->TopFieldOrderCnt = PicOrderCntMsb + slice->pic_order_cnt_lsb;
        }
        else
        {
            poc->TopFieldOrderCnt = 0;
        }
#else 
        // Reference : FFmpeg 6.x (but not with consistence with ISO)
        // To Check
        poc->TopFieldOrderCnt = PicOrderCntMsb + slice->pic_order_cnt_lsb;
#endif
        if (!slice->field_pic_flag)
        {
            poc->BottomFieldOrderCnt = poc->TopFieldOrderCnt + slice->delta_pic_order_cnt_bottom;
        }
        else
        {
            poc->BottomFieldOrderCnt = PicOrderCntMsb + slice->pic_order_cnt_lsb;
        }
    }
    // update context
    poc->prevPicOrderCntMsb = PicOrderCntMsb;
}

static void DecodeH264PictureOrderCountType1(H264SpsSyntax::ptr sps, H264SliceHeaderSyntax::ptr slice, uint8_t nal_ref_idc, H264PocContext::ptr poc)
{
    // ISO 14496/10(2020) - 8.2.1.2 Decoding process for picture order count type 1
    int64_t  FrameNumOffset = 0;
    int64_t  absFrameNum = 0;
    int64_t  picOrderCntCycleCnt = 0;
    int64_t  frameNumInPicOrderCntCycle = 0;
    int64_t  expectedPicOrderCnt;
    // determine FrameNumOffset
    {
        if (slice->slice_type == H264SliceType::MMP_H264_I_SLICE)
        {
            FrameNumOffset = 0;
        }
        else if (poc->prevFrameNum > slice->frame_num)
        {
            uint64_t MaxFrameNum = 1 << (sps->log2_max_frame_num_minus4 + 4);
            FrameNumOffset = poc->prevFrameNumOffset + MaxFrameNum;
        }
        else
        {
            FrameNumOffset = poc->prevFrameNumOffset;
        }
    }
    // determine absFrameNum
    {
        if (sps->num_ref_frames_in_pic_order_cnt_cycle != 0)
        {
            absFrameNum = FrameNumOffset + slice->frame_num;
        }
        else
        {
            absFrameNum = 0;
        }
        if (nal_ref_idc == 0 && absFrameNum > 0)
        {
            absFrameNum = absFrameNum - 1;
        }
    }
    // determine picOrderCntCycleCnt and frameNumInPicOrderCntCycle
    {
        if (absFrameNum > 0)
        {
            picOrderCntCycleCnt = (absFrameNum - 1) / sps->num_ref_frames_in_pic_order_cnt_cycle;
            frameNumInPicOrderCntCycle = (absFrameNum - 1) % sps->num_ref_frames_in_pic_order_cnt_cycle;
        }
    }
    // determine expectedPicOrderCnt
    {
        if (absFrameNum > 0)
        {
            int64_t ExpectedDeltaPerPicOrderCntCycle = 0;
            {
                for (uint32_t i=0; i<sps->num_ref_frames_in_pic_order_cnt_cycle; i++)
                {
                    ExpectedDeltaPerPicOrderCntCycle += sps->offset_for_ref_frame[i];
                }
            }
            expectedPicOrderCnt = picOrderCntCycleCnt * ExpectedDeltaPerPicOrderCntCycle;
            for (int64_t i=0; i<=frameNumInPicOrderCntCycle; i++)
            {
                expectedPicOrderCnt = expectedPicOrderCnt + sps->offset_for_ref_frame[i];
            }
        }
        else
        {
            expectedPicOrderCnt = 0;
        }
        if (nal_ref_idc == 0)
        {
            expectedPicOrderCnt = expectedPicOrderCnt + sps->offset_for_non_ref_pic;
        }
    }
    // determine TopFieldOrderCnt and BottomFieldOrderCnt
    {
#if 0 
        if (!slice->field_pic_flag)
        {
            poc->TopFieldOrderCnt = expectedPicOrderCnt + slice->delta_pic_order_cnt[0];
            poc->BottomFieldOrderCnt = poc->TopFieldOrderCnt + sps->offset_for_top_to_bottom_field + slice->delta_pic_order_cnt[1];
        }
        else if (!slice->bottom_field_flag)
        {
            poc->TopFieldOrderCnt = expectedPicOrderCnt + slice->delta_pic_order_cnt[0];
        }
        else
        {
            poc->BottomFieldOrderCnt = expectedPicOrderCnt + sps->offset_for_top_to_bottom_field + slice->delta_pic_order_cnt[0];
        }
#else 
        // Reference : FFmpeg 6.x (but not with consistence with ISO)
        // To Check
        poc->TopFieldOrderCnt = expectedPicOrderCnt + slice->delta_pic_order_cnt[0];
        poc->BottomFieldOrderCnt = poc->TopFieldOrderCnt + sps->offset_for_top_to_bottom_field;
        if (slice->field_pic_flag == 1 && slice->bottom_field_flag == 1)
        {
            poc->BottomFieldOrderCnt = slice->delta_pic_order_cnt[1];
        }
#endif
    }
    // Update context
    poc->prevFrameNumOffset = FrameNumOffset;
}

static void DecodeH264PictureOrderCountType2(H264SpsSyntax::ptr sps, H264SliceHeaderSyntax::ptr slice, uint8_t nal_ref_idc, H264PocContext::ptr poc)
{
    // ISO 14496/10(2020) - 8.2.1.3 Decoding process for picture order count type 2
    int64_t FrameNumOffset = 0;
    int64_t tempPicOrderCnt = 0;
    // determine FrameNumOffset
    {
        if (slice->slice_type == H264SliceType::MMP_H264_I_SLICE)
        {
            FrameNumOffset = 0;
        }
        else if (poc->prevFrameNum > slice->frame_num)
        {
            uint64_t MaxFrameNum = 1 << (sps->log2_max_frame_num_minus4 + 4);
            FrameNumOffset = poc->prevFrameNum + MaxFrameNum;
        }
        else
        {
            FrameNumOffset = poc->prevFrameNumOffset;
        }
    }
    // determine tempPicOrderCnt
    {
        if (slice->slice_type == H264SliceType::MMP_H264_I_SLICE)
        {
            tempPicOrderCnt = 0;
        }
        else if (nal_ref_idc == 0)
        {
            tempPicOrderCnt = 2 * (FrameNumOffset + slice->frame_num) - 1;
        }
        else
        {
            tempPicOrderCnt = 2 * (FrameNumOffset + slice->frame_num);
        }
    }
    // determine TopFieldOrderCnt and BottomFieldOrderCnt
    {
        if (!slice->field_pic_flag)
        {
            poc->TopFieldOrderCnt = tempPicOrderCnt;
            poc->BottomFieldOrderCnt = tempPicOrderCnt;
        }
        else if (slice->bottom_field_flag)
        {
            poc->BottomFieldOrderCnt = tempPicOrderCnt;
        }
        else
        {
            poc->TopFieldOrderCnt = tempPicOrderCnt;
        }
    }
    // update context
    poc->prevFrameNumOffset = FrameNumOffset;
}

void DecodeH264PictureOrderCount(H264SpsSyntax::ptr sps, H264SliceHeaderSyntax::ptr slice, uint8_t nal_ref_idc, H264PocContext::ptr poc)
{
    if (slice->slice_type == H264SliceType::MMP_H264_I_SLICE)
    {
        poc->prevFrameNum = 0;
        poc->prevFrameNumOffset = 0;
        poc->prevPicOrderCntMsb = 0;
        poc->pic_order_cnt_lsb = 0;
    }
    if (sps->pic_order_cnt_type == 0)
    {
        DecodeH264PictureOrderCountType0(sps, slice, poc);
    }
    else if (sps->pic_order_cnt_type == 1)
    {
        DecodeH264PictureOrderCountType1(sps, slice, nal_ref_idc, poc);
    }
    else if (sps->pic_order_cnt_type == 2)
    {
        DecodeH264PictureOrderCountType2(sps, slice, nal_ref_idc, poc);
    }
    else
    {
        assert(false);
    }
    // Update
    poc->pic_order_cnt_lsb = slice->pic_order_cnt_lsb;
    poc->prevFrameNum = slice->frame_num;
}

} // namespace Codec
} // namespace Mmp