#include "H264SliceDecodingProcess.h"

namespace Mmp
{
namespace Codec
{

/*************************************** 8.2.1 Decoding process for picture order count(Begin) ******************************************/

/**
 * @sa ISO 14496/10(2020) - 8.2.1.1 Decoding process for picture order count type 0 
 */
void H264SliceDecodingProcess::DecodeH264PictureOrderCountType0(H264PictureContext::ptr prevPictrue, H264SpsSyntax::ptr sps, H264SliceHeaderSyntax::ptr slice, H264PictureContext::ptr picture)
{
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
            if (prevPictrue->memory_management_control_operations.count(5/* H264MmcoType::MMP_H264_MMOO_5 */))
            {
                if (!prevPictrue->bottom_field_flag)
                {
                    prevPicOrderCntMsb = 0;
                    prevPicOrderCntLsb  = prevPictrue->TopFieldOrderCnt;
                }
                else
                {
                    prevPicOrderCntMsb = 0;
                    prevPicOrderCntLsb = 0;
                }
            }
            else
            {
                prevPicOrderCntMsb = prevPictrue->prevPicOrderCntMsb;
                prevPicOrderCntLsb = prevPictrue->pic_order_cnt_lsb;
            }
        }
    }
    // determine PicOrderCntMsb (8-3)
    {
        int64_t MaxPicOrderCntLsb = 1 << (sps->log2_max_pic_order_cnt_lsb_minus4 + 4); // (7-11)
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
    // determine TopFieldOrderCnt and BottomFieldOrderCnt (8-4) and (8-5)
    {
        picture->TopFieldOrderCnt = PicOrderCntMsb + slice->pic_order_cnt_lsb;
        if (!slice->field_pic_flag)
        {
            picture->BottomFieldOrderCnt = picture->TopFieldOrderCnt + slice->delta_pic_order_cnt_bottom;
        }
        else
        {
            picture->BottomFieldOrderCnt = PicOrderCntMsb + slice->pic_order_cnt_lsb;
        }
    }
    // update context
    picture->prevPicOrderCntMsb = PicOrderCntMsb;
}

/**
 * @sa ISO 14496/10(2020) - 8.2.1.2 Decoding process for picture order count type 1 
 */
void H264SliceDecodingProcess::DecodeH264PictureOrderCountType1(H264PictureContext::ptr prevPictrue, H264SpsSyntax::ptr sps, H264SliceHeaderSyntax::ptr slice, uint8_t nal_ref_idc, H264PictureContext::ptr picture)
{
    int32_t  prevFrameNumOffset = 0;
    int64_t  absFrameNum = 0;
    int64_t  picOrderCntCycleCnt = 0;
    int64_t  frameNumInPicOrderCntCycle = 0;
    int64_t  expectedPicOrderCnt;
    // determine prevFrameNumOffset
    {
        if (slice->slice_type != H264SliceType::MMP_H264_I_SLICE)
        {
            if (prevPictrue->memory_management_control_operations.count(5 /* H264MmcoType::MMP_H264_MMOO_5 */))
            {
                prevFrameNumOffset = 0;
            }
            else
            {
                prevFrameNumOffset = prevPictrue->FrameNumOffset;
            }
        }
    }
    // determine FrameNumOffset (8-6)
    {
        if (slice->slice_type == H264SliceType::MMP_H264_I_SLICE)
        {
            picture->FrameNumOffset = 0;
        }
        else if (prevPictrue->FrameNum > slice->frame_num)
        {
            uint64_t MaxFrameNum = 1 << (sps->log2_max_frame_num_minus4 + 4); // (7-10)
            picture->FrameNumOffset = prevFrameNumOffset + MaxFrameNum;
        }
        else
        {
            picture->FrameNumOffset = prevFrameNumOffset;
        }
    }
    // determine absFrameNum (8-7)
    {
        if (sps->num_ref_frames_in_pic_order_cnt_cycle != 0)
        {
            absFrameNum = picture->FrameNumOffset + slice->frame_num;
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
    // determine picOrderCntCycleCnt and frameNumInPicOrderCntCycle (8-8)
    {
        if (absFrameNum > 0)
        {
            picOrderCntCycleCnt = (absFrameNum - 1) / sps->num_ref_frames_in_pic_order_cnt_cycle;
            frameNumInPicOrderCntCycle = (absFrameNum - 1) % sps->num_ref_frames_in_pic_order_cnt_cycle;
        }
    }
    // determine expectedPicOrderCnt (8-9)
    {
        if (absFrameNum > 0)
        {
            int64_t ExpectedDeltaPerPicOrderCntCycle = 0; // (7-12)
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
    // determine TopFieldOrderCnt and BottomFieldOrderCnt (8-10)
    {
        if (!slice->field_pic_flag)
        {
            picture->TopFieldOrderCnt = expectedPicOrderCnt + slice->delta_pic_order_cnt[0];
            picture->BottomFieldOrderCnt = picture->TopFieldOrderCnt + sps->offset_for_top_to_bottom_field + slice->delta_pic_order_cnt[1];
        }
        else if (!slice->bottom_field_flag)
        {
            picture->TopFieldOrderCnt = expectedPicOrderCnt + slice->delta_pic_order_cnt[0];
        }
        else
        {
            picture->BottomFieldOrderCnt = expectedPicOrderCnt + sps->offset_for_top_to_bottom_field + slice->delta_pic_order_cnt[0];
        }
    }
}

/**
 * @sa ISO 14496/10(2020) - 8.2.1.3 Decoding process for picture order count type 2
 */
void H264SliceDecodingProcess::DecodeH264PictureOrderCountType2(H264PictureContext::ptr prevPictrue, H264SpsSyntax::ptr sps, H264SliceHeaderSyntax::ptr slice, uint8_t nal_ref_idc, H264PictureContext::ptr picture)
{
    int64_t FrameNumOffset = 0;
    int64_t tempPicOrderCnt = 0;
    int64_t prevFrameNumOffset = 0;
    // determine prevFrameNumOffset
    {
        if (slice->slice_type != H264SliceType::MMP_H264_I_SLICE)
        {
            if (prevPictrue->memory_management_control_operations.count(5 /* H264MmcoType::MMP_H264_MMOO_5 */))
            {
                prevFrameNumOffset = 0;
            }
            else
            {
                prevFrameNumOffset = prevPictrue->FrameNumOffset;
            }
        }
    }
    // determine FrameNumOffset (8-11)
    {
        if (slice->slice_type == H264SliceType::MMP_H264_I_SLICE)
        {
            FrameNumOffset = 0;
        }
        else if (prevFrameNumOffset > slice->frame_num)
        {
            uint64_t MaxFrameNum = 1 << (sps->log2_max_frame_num_minus4 + 4); // (7-10)
            FrameNumOffset = prevFrameNumOffset + MaxFrameNum;
        }
        else
        {
            FrameNumOffset = prevFrameNumOffset;
        }
        picture->FrameNumOffset = FrameNumOffset;
    }
    // determine tempPicOrderCnt (8-12)
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
    // determine TopFieldOrderCnt and BottomFieldOrderCnt (8-13)
    {
        if (!slice->field_pic_flag)
        {
            picture->TopFieldOrderCnt = tempPicOrderCnt;
            picture->BottomFieldOrderCnt = tempPicOrderCnt;
        }
        else if (slice->bottom_field_flag)
        {
            picture->BottomFieldOrderCnt = tempPicOrderCnt;
        }
        else
        {
            picture->TopFieldOrderCnt = tempPicOrderCnt;
        }
    }
}

/**
 * @sa  ISO 14496/10(2020) - 8.2.1 Decoding process for picture order count
 */
void H264SliceDecodingProcess::DecodingProcessForPictureOrderCount(H264SliceHeaderSyntax::ptr slice, H264PictureContext::ptr picture)
{
    // TODO
}

/*************************************** 8.2.1 Decoding process for picture order count(End) ******************************************/


} // namespace Codec
} // namespace Mmp