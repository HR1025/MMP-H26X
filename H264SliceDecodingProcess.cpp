#include "H264SliceDecodingProcess.h"

#include <cassert>
#include <algorithm>

namespace Mmp
{
namespace Codec
{

constexpr int64_t no_long_term_frame_indices = -1;

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

/*************************************** 8.2.4 Decoding process for reference picture lists construction(Begin) ******************************************/

/**
 * @sa  ISO 14496/10(2020) - 8.2.4.1 Decoding process for picture numbers
 */
void H264SliceDecodingProcess::DecodingProcessForPictureNumbers(H264SliceHeaderSyntax::ptr slice, H264SpsSyntax::ptr sps, H264PictureContext::cache pictures, H264PictureContext::ptr picture)
{
    // determine FrameNumWrap
    {
        uint64_t MaxFrameNum = 1 << (sps->log2_max_frame_num_minus4 + 4); // (7-10)
        for (auto _picture : pictures)
        {
            if (_picture->referenceFlag & H264PictureContext::used_for_short_term_reference)
            {
                if (_picture->FrameNum > slice->frame_num)
                {
                    _picture->FrameNumWrap = _picture->FrameNum - MaxFrameNum;
                }
                else
                {
                    _picture->FrameNumWrap = _picture->FrameNum;
                }
            }
        }
    }
    // TODO : Each long-term reference picture has an associated value of LongTermFrameIdx (that was assigned to it as specified in subclause 8.2.5).
    assert(false);
    // determine PicNum and LongTermPicNum
    {
        for (auto _picture : pictures)
        {
            if (_picture->field_pic_flag == 0)
            {
                if (_picture->referenceFlag & H264PictureContext::used_for_short_term_reference)
                {
                    _picture->PicNum = _picture->FrameNumWrap; // (8-28)
                }
                if (_picture->referenceFlag & H264PictureContext::used_for_long_term_reference)
                {
                    _picture->LongTermPicNum = _picture->LongTermFrameIdx; // (8-29)
                }
            }
            else if (_picture->field_pic_flag == 1)
            {
                // TOCHECK : top field 一定出现在 bottom field 之前吗, 如何确定 same parity 和 oppsite partity
                // WORKAROUND : 
                //              1) 是不是直接不支持场编码问题就都解决了, 当前 h264 场编码已经应用得比较少了;
                //                 支持场编码一方面逻辑异常复杂,另一方面也不好进行测试验证
                //              2) 确认一下 FFmpeg 6.x (FFmpeg/libavcodec/h264_refs.c) 这部分的代码逻辑 (,但是看起来不是很好确认 ...) 
                if (_picture->referenceFlag & H264PictureContext::used_for_short_term_reference)
                {
                    if (_picture->bottom_field_flag)
                    {
                        _picture->PicNum = 2 * _picture->FrameNumWrap + 1; // (8-30)
                    }
                    else
                    {
                        _picture->PicNum = 2 * _picture->FrameNumWrap; // (8-31)
                    }
                }
                if (_picture->referenceFlag & H264PictureContext::used_for_long_term_reference)
                {
                    if (_picture->bottom_field_flag)
                    {
                        _picture->LongTermPicNum = 2 * _picture->LongTermFrameIdx + 1;
                    }
                    else
                    {
                        _picture->LongTermPicNum = 2 * picture->LongTermFrameIdx;
                    }
                }
            }
        }
    }
}

/**
 * @sa  ISO 14496/10(2020) - 8.2.4.2 Initialization process for reference picture lists
 */
void H264SliceDecodingProcess::InitializationProcessForReferencePictureLists(H264SliceHeaderSyntax::ptr slice, H264PictureContext::cache pictures)
{
    if (!(slice->slice_type == H264SliceType::MMP_H264_P_SLICE || slice->slice_type == H264SliceType::MMP_H264_SP_SLICE || slice->slice_type == H264SliceType::MMP_H264_B_SLICE))
    {
        return;
    }
    {
        _RefPicList0.clear();
        _RefPicList1.clear();
    }
    // 8.2.4.2.1 Initialization process for the reference picture list for P and SP slices in frames
    if ((slice->slice_type == H264SliceType::MMP_H264_P_SLICE || slice->slice_type == H264SliceType::MMP_H264_SP_SLICE) && slice->field_pic_flag == 0)
    {
        std::vector<uint64_t> shortTermRefPicList;
        std::vector<uint64_t> longTermRefList;
        for (auto picture : pictures)
        {
            if (picture->referenceFlag & H264PictureContext::used_for_short_term_reference)
            {
                shortTermRefPicList.push_back(picture->PicNum);
            }
            if (picture->referenceFlag & H264PictureContext::used_for_long_term_reference)
            {
                longTermRefList.push_back(picture->LongTermPicNum);
            }
            std::sort(shortTermRefPicList.begin(), shortTermRefPicList.end());
            std::reverse(shortTermRefPicList.begin(), shortTermRefPicList.end());
            std::sort(longTermRefList.begin(), longTermRefList.end());
            for (auto& PicNum : shortTermRefPicList)
            {
                _RefPicList0.push_back(PicNum);
            }
            for (auto& LongTermPicNum : longTermRefList)
            {
                _RefPicList0.push_back(LongTermPicNum);
            }
        }
    }
    // 8.2.4.2.2 Initialization process for the reference picture list for P and SP slices in fields
    else if ((slice->slice_type == H264SliceType::MMP_H264_P_SLICE || slice->slice_type == H264SliceType::MMP_H264_SP_SLICE) && slice->field_pic_flag == 1)
    {
        assert(false);
        // TODO
    }
    assert(false);
    // TODO
    {
        _RefPicList0.resize(std::min((size_t)slice-> num_ref_idx_l0_active_minus1 + 1, _RefPicList0.size()));
        _RefPicList1.resize(std::min((size_t)slice-> num_ref_idx_l1_active_minus1 + 1, _RefPicList1.size()));
    }
}

/*************************************** 8.2.4 Decoding process for reference picture lists construction(End) ******************************************/

/*************************************** 8.2.5 Decoded reference picture marking process(Begin) ******************************************/

/**
 * @sa ISO 14496/10(2020) - 8.2.5.1 Sequence of operations for decoded reference picture marking process 
 */
void H264SliceDecodingProcess::SequenceOfOperationsForDecodedReferencePictureMarkingProcess(H264SliceHeaderSyntax::ptr slice, H264PictureContext::cache pictures, H264PictureContext::ptr picture)
{
    if (slice->slice_type == H264SliceType::MMP_H264_I_SLICE)
    {
        pictures.clear();
        if (slice->drpm->long_term_reference_flag == 0)
        {
            picture->referenceFlag = H264PictureContext::used_for_short_term_reference;
            picture->MaxLongTermFrameIdx = no_long_term_frame_indices;
        }
        else
        {
            picture->referenceFlag = H264PictureContext::used_for_long_term_reference;
            picture->LongTermFrameIdx = 0;
            picture->MaxLongTermFrameIdx = 0;
        }     
    }
    else
    {
        if (slice->drpm->adaptive_ref_pic_marking_mode_flag == 0)
        {
            // TODO
            assert(false);
        }
        else
        {
            AdaptiveMemoryControlDecodedReferencePicutreMarkingPorcess(slice, pictures, picture);
        }
    }
}

/**
 * @sa ISO 14496/10(2020) - 8.2.5.3 Sliding window decoded reference picture marking process
 */
void H264SliceDecodingProcess::SlidingWindowDecodedReferencePictureMarkingProcess(H264SliceHeaderSyntax::ptr slice, H264PictureContext::cache pictures, H264PictureContext::ptr picture)
{
    // TODO
}

/**
 * @sa ISO 14496/10(2020) - 8.2.5.4 Adaptive memory control decoded reference picture marking process 
 */
void H264SliceDecodingProcess::AdaptiveMemoryControlDecodedReferencePicutreMarkingPorcess(H264SliceHeaderSyntax::ptr slice, H264PictureContext::cache pictures, H264PictureContext::ptr picture)
{
    for (const auto& memory_management_control_operation : slice->drpm->memory_management_control_operations)
    {
        switch (memory_management_control_operation)
        {
            // See also : 8.2.5.4.1 Marking process of a short-term reference picture as "unused for reference"
            case H264MmcoType::MMP_H264_MMOO_1:
            {
                uint64_t picNumX = 0; // (8-39)
                {
                    uint64_t CurrPicNum = 0;
                    if (slice->field_pic_flag == 0)
                    {
                        CurrPicNum = slice->frame_num;
                    }
                    else if (slice->field_pic_flag == 1)
                    {
                        CurrPicNum = 2 * slice->frame_num + 1;
                    }
                    picNumX = CurrPicNum - (slice->drpm->difference_of_pic_nums_minus1 + 1);
                }
                for (auto _picture : pictures)
                {
                    if (_picture)
                    {
                        if (_picture->field_pic_flag == 0)
                        {
                            _picture->referenceFlag = H264PictureContext::unused_for_reference;
                        }
                        else if (_picture->field_pic_flag == 1)
                        {
                            _picture->referenceFlag = _picture->referenceFlag ^ H264PictureContext::used_for_short_term_reference;
                        }
                    }
                }
                break;
            }
            // See also : 8.2.5.4.2 Marking process of a long-term reference picture as "unused for reference"
            case H264MmcoType::MMP_H264_MMOO_2:
            {
                for (auto _picture : pictures)
                {
                    if (slice->drpm->long_term_pic_num == _picture->LongTermPicNum)
                    {
                        if (_picture->field_pic_flag == 0)
                        {
                            _picture->referenceFlag = H264PictureContext::unused_for_reference;
                        }
                        else
                        {
                            _picture->referenceFlag = _picture->referenceFlag ^ H264PictureContext::used_for_long_term_reference;
                        }
                    }
                }
                break;
            }
            // See also : 8.2.5.4.3 Assignment process of a LongTermFrameIdx to a short-term reference picture
            case H264MmcoType::MMP_H264_MMOO_3:
            {
                for (auto _picture : pictures)
                {
                    if (slice->drpm->long_term_pic_num == _picture->LongTermPicNum)
                    {
                        _picture->referenceFlag = H264PictureContext::used_for_long_term_reference;
                        _picture->LongTermFrameIdx = _picture->long_term_frame_idx;
                    }
                }
                break;
            }
            // See also : 8.2.5.4.4 Decoding process for MaxLongTermFrameIdx
            case H264MmcoType::MMP_H264_MMOO_4:
            {
                int64_t MaxLongTermFrameIdx = slice->drpm->max_long_term_frame_idx_plus1 == 0 ? no_long_term_frame_indices : slice->drpm->max_long_term_frame_idx_plus1 - 1;
                for (auto _picture : pictures)
                {
                    if (_picture->LongTermFrameIdx >= slice->drpm->max_long_term_frame_idx_plus1 - 1)
                    {
                        if (_picture->referenceFlag & H264PictureContext::used_for_long_term_reference)
                        {
                            _picture->referenceFlag = H264PictureContext::unused_for_reference;
                            _picture->MaxLongTermFrameIdx = MaxLongTermFrameIdx;
                        }
                    }
                }
                break;
            }
            // See also : 8.2.5.4.5 Marking process of all reference pictures as "unused for reference" and setting
            //            MaxLongTermFrameIdx to "no long-term frame indices"
            case H264MmcoType::MMP_H264_MMOO_5:
            {
                for (auto _picture : pictures)
                {
                    _picture->MaxLongTermFrameIdx = no_long_term_frame_indices;
                    _picture->referenceFlag = H264PictureContext::unused_for_reference;
                }
                break;
            }
            // See also : 8.2.5.4.6 Process for assigning a long-term frame index to the current picture
            case H264MmcoType::MMP_H264_MMOO_6:
            {
                for (auto _picture : pictures)
                {
                    if (_picture->LongTermFrameIdx >= slice->drpm->long_term_frame_idx)
                    {
                        _picture->referenceFlag = H264PictureContext::unused_for_reference;
                    }
                }
                {
                    picture->referenceFlag |= H264PictureContext::used_for_long_term_reference;
                    picture->LongTermFrameIdx = picture->long_term_frame_idx;
                }
                // TODO
                assert(false);
                break;
            }
            default:
                break;
        }
    }
}

/*************************************** 8.2.5 Decoded reference picture marking process(End) ******************************************/


} // namespace Codec
} // namespace Mmp