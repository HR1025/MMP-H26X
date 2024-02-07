#include "H265Deserialize.h"

#include <cmath>
#include <cassert>
#include <cstdint>
#include <memory>
#include <algorithm>

#include "H265Common.h"
#include "H26xUltis.h"

namespace Mmp
{
namespace Codec
{

bool H265Deserialize::DeserializePpsSyntax(H26xBinaryReader::ptr br, H265PpsSyntax::ptr pps)
{
    // See also : ITU-T H.265 (2021) - 7.3.2.3.1 General picture parameter set RBSP syntax
    try
    {
        br->UE(pps->pps_pic_parameter_set_id);
        {
            // Hint : The value of pps_pic_parameter_set_id shall be in the range of 0 to 63, inclusive.
            MPP_H26X_SYNTAXT_STRICT_CHECK(/* pps->pps_pic_parameter_set_id>=0 && */ pps->pps_pic_parameter_set_id<=63, "[pps] pps_pic_parameter_set_id out of range", return false);
        }
        br->UE(pps->pps_seq_parameter_set_id);
        {
            // Hint : The value of pps_seq_parameter_set_id shall be in the range of 0 to 15, inclusive.
            MPP_H26X_SYNTAXT_STRICT_CHECK(/* pps->pps_seq_parameter_set_id>=0 && */ pps->pps_seq_parameter_set_id<=15, "[pps] pps_seq_parameter_set_id out of range", return false);
        }
        if (_contex->spsSet.count(pps->pps_seq_parameter_set_id) == 0)
        {
            assert(false);
            return false;
        }
        H265SpsSyntax::ptr sps = _contex->spsSet[pps->pps_seq_parameter_set_id];
        br->U(1, pps->dependent_slice_segments_enabled_flag);
        br->U(1, pps->output_flag_present_flag);
        br->U(3, pps->num_extra_slice_header_bits);
        br->U(1, pps->sign_data_hiding_enabled_flag);
        br->U(1, pps->cabac_init_present_flag);
        br->UE(pps->num_ref_idx_l0_default_active_minus1);
        {
            // Hint : The value of num_ref_idx_l0_default_active_minus1 shall be in the range of 0 to 14, inclusive.
            MPP_H26X_SYNTAXT_STRICT_CHECK(/* pps->num_ref_idx_l0_default_active_minus1>=0 && */ pps->num_ref_idx_l0_default_active_minus1<=14, "[pps] num_ref_idx_l0_default_active_minus1 out of range", return false);
        }
        br->UE(pps->num_ref_idx_l1_default_active_minus1);
        {
            // Hint : The value of num_ref_idx_l1_default_active_minus1 shall be in the range of 0 to 14, inclusive.
            MPP_H26X_SYNTAXT_STRICT_CHECK(/* pps->num_ref_idx_l1_default_active_minus1>=0 && */ pps->num_ref_idx_l1_default_active_minus1<=14, "[pps] num_ref_idx_l1_default_active_minus1 out of range", return false);
        }
        br->SE(pps->init_qp_minus26);
        br->U(1, pps->constrained_intra_pred_flag);
        br->U(1, pps->transform_skip_enabled_flag);
        br->U(1, pps->cu_qp_delta_enabled_flag);
        if (pps->cu_qp_delta_enabled_flag)
        {
            br->UE(pps->diff_cu_qp_delta_depth);
            {
                // Hint : The value of diff_cu_qp_delta_depth shall be in the range of 0 to log2_diff_max_min_luma_coding_block_size, inclusive.
                MPP_H26X_SYNTAXT_STRICT_CHECK(/* pps->diff_cu_qp_delta_depth>=0 && */ pps->diff_cu_qp_delta_depth<=sps->log2_diff_max_min_luma_coding_block_size, "[pps] diff_cu_qp_delta_depth out of range", return false);
            }
        }
        br->SE(pps->pps_cb_qp_offset);
        br->SE(pps->pps_cr_qp_offset);
        {
            // Hint : The values of pps_cb_qp_offset and pps_cr_qp_offset shall be in the range of −12 to +12, inclusive.
            MPP_H26X_SYNTAXT_STRICT_CHECK(pps->pps_cb_qp_offset>=-12 && pps->pps_cb_qp_offset<=12, "[pps] pps_cb_qp_offset out of range", return false);
            MPP_H26X_SYNTAXT_STRICT_CHECK(pps->pps_cr_qp_offset>=-12 && pps->pps_cr_qp_offset<=12, "[pps] pps_cr_qp_offset out of range", return false);
        }
        br->U(1, pps->pps_slice_chroma_qp_offsets_present_flag);
        br->U(1, pps->weighted_pred_flag);
        br->U(1, pps->weighted_bipred_flag);
        br->U(1, pps->transquant_bypass_enabled_flag);
        br->U(1, pps->tiles_enabled_flag);
        br->U(1, pps->entropy_coding_sync_enabled_flag);
        if (pps->tiles_enabled_flag)
        {
            br->UE(pps->num_tile_columns_minus1);
            br->UE(pps->num_tile_rows_minus1);
            br->U(1, pps->uniform_spacing_flag);
            if (!pps->uniform_spacing_flag)
            {
                pps->column_width_minus1.resize(pps->num_tile_columns_minus1 + 1);
                pps->row_height_minus1.resize(pps->num_tile_columns_minus1 + 1);
                for (uint32_t i=0; i<pps->num_tile_columns_minus1; i++)
                {
                    br->UE(pps->column_width_minus1[i]);
                }
                for (uint32_t i=0; i<pps->num_tile_columns_minus1; i++)
                {
                    br->UE(pps->row_height_minus1[i]);
                }
            }
            br->U(1, pps->loop_filter_across_tiles_enabled_flag);
        }
        br->U(1, pps->pps_loop_filter_across_slices_enabled_flag);
        br->U(1, pps->deblocking_filter_control_present_flag);
        if (pps->deblocking_filter_control_present_flag)
        {
            br->U(1, pps->deblocking_filter_override_enabled_flag);
            br->U(1, pps->pps_deblocking_filter_disabled_flag);
            if (!pps->pps_deblocking_filter_disabled_flag)
            {
                br->SE(pps->pps_beta_offset_div2);
                br->SE(pps->pps_tc_offset_div2);
                {
                    // Hint : The values of pps_beta_offset_div2 and pps_tc_offset_div2 shall both be in the range of −6 to 6, inclusive. 
                    MPP_H26X_SYNTAXT_STRICT_CHECK(pps->pps_beta_offset_div2>=-6 && pps->pps_beta_offset_div2<=6, "[pps] pps_beta_offset_div2 out of range", return false);
                    MPP_H26X_SYNTAXT_STRICT_CHECK(pps->pps_tc_offset_div2>=-6 && pps->pps_tc_offset_div2<=6, "[pps] pps_tc_offset_div2 out of range", return false);
                }
            }
        }
        br->U(1, pps->pps_scaling_list_data_present_flag);
        if (pps->pps_scaling_list_data_present_flag)
        {
            pps->scaling_list_data = std::make_shared<H265ScalingListDataSyntax>();
            if (!DeserializeScalingListDataSyntax(br, pps->scaling_list_data))
            {
                assert(false);
                return false;
            }
        }
        br->U(1, pps->lists_modification_present_flag);
        br->UE(pps->log2_parallel_merge_level_minus2);
        br->U(1, pps->slice_segment_header_extension_present_flag);
        br->U(1, pps->pps_extension_present_flag);
        if (pps->pps_extension_present_flag)
        {
            br->U(1, pps->pps_range_extension_flag);
            br->U(1, pps->pps_multilayer_extension_flag);
            br->U(1, pps->pps_3d_extension_flag);
            br->U(1, pps->pps_scc_extension_flag);
            br->U(4, pps->pps_extension_4bits);
        }
        if (pps->pps_range_extension_flag)
        {
            pps->ppsRange = std::make_shared<H265PpsRangeSyntax>();
            if (!DeserializePpsRangeSyntax(br, pps, pps->ppsRange))
            {
                assert(false);
                return false;
            }
        }
        if (pps->pps_multilayer_extension_flag)
        {
            pps->ppsMultilayer = std::make_shared<H265PpsMultilayerSyntax>();
            if (!DeserializePpsMultilayerSyntax(br, pps->ppsMultilayer))
            {
                assert(false);
                return false;
            }
        }
        if (pps->pps_3d_extension_flag)
        {
            pps->pps3d = std::make_shared<H265Pps3dSyntax>();
            if (!DeserializePps3dSyntax(br, pps, pps->pps3d))
            {
                assert(false);
                return false;
            }
        }
        if (pps->pps_scc_extension_flag)
        {
            pps->ppsScc = std::make_shared<H265PpsSccSyntax>();
            if (!DeserializePpsSccSyntax(br, pps->ppsScc))
            {
                assert(false);
                return false;
            }
        }
        if (pps->pps_extension_4bits)
        {
            while (br->more_rbsp_data())
            {
                br->U(1, pps->pps_extension_data_flag);
            }
        }
        br->rbsp_trailing_bits();
        _contex->ppsSet[pps->pps_pic_parameter_set_id] = pps;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializeSpsSyntax(H26xBinaryReader::ptr br, H265SpsSyntax::ptr sps)
{
    // See also : 7.3.2.2.1 General sequence parameter set RBSP syntax
    try
    {
        br->U(4, sps->sps_video_parameter_set_id);
        if (_contex->vpsSet.count(sps->sps_video_parameter_set_id) == 0)
        {
            return false;
        }
        H265VPSSyntax::ptr vps = _contex->vpsSet[sps->sps_video_parameter_set_id];
        br->U(3, sps->sps_max_sub_layers_minus1);
        {
            // Hint : The value of sps_max_sub_layers_minus1 shall be in the range of 0 to 6, inclusive. 
            MPP_H26X_SYNTAXT_STRICT_CHECK(/* sps->sps_max_sub_layers_minus1>=0 && */ sps->sps_max_sub_layers_minus1<=6, "[sps] sps_max_sub_layers_minus1 out of range", return false);
        }
        br->U(1, sps->sps_temporal_id_nesting_flag);
        sps->ptl = std::make_shared<H265PTLSyntax>();
        if (!DeserializePTLSyntax(br,1, sps->sps_max_sub_layers_minus1, sps->ptl))
        {
            assert(false);
            return false;
        }
        br->UE(sps->sps_seq_parameter_set_id);
        {
            // Hint : The  value  of sps_seq_parameter_set_id shall be in the range of 0 to 15, inclusive. 
            MPP_H26X_SYNTAXT_STRICT_CHECK(/* sps->sps_seq_parameter_set_id>=0 && */ sps->sps_seq_parameter_set_id<=15, "[sps] sps_seq_parameter_set_id out of range", return false);
        }
        br->UE(sps->chroma_format_idc);
        {
            // Hint : The value of chroma_format_idc shall be in the range of 0 to 3, inclusive. 
            MPP_H26X_SYNTAXT_STRICT_CHECK(/* sps->chroma_format_idc>=0 && */ sps->chroma_format_idc<=3, "[sps] chroma_format_idc out of range", return false);
        }
        if (sps->chroma_format_idc == 3)
        {
            br->U(1, sps->separate_colour_plane_flag);
        }
        br->UE(sps->pic_width_in_luma_samples);
        br->UE(sps->pic_height_in_luma_samples);
        br->U(1, sps->conformance_window_flag);
        if (sps->conformance_window_flag)
        {
            br->UE(sps->conf_win_left_offset);
            br->UE(sps->conf_win_right_offset);
            br->UE(sps->conf_win_top_offset);
            br->UE(sps->conf_win_bottom_offset);
        }
        br->UE(sps->bit_depth_luma_minus8);
        {
            // Hint : bit_depth_luma_minus8 shall be in the range of 0 to 8, inclusive.
            MPP_H26X_SYNTAXT_STRICT_CHECK(/* sps->bit_depth_luma_minus8>=0 && */ sps->bit_depth_luma_minus8<=8, "[sps] bit_depth_luma_minus8 out of range", return false);
        }
        br->UE(sps->bit_depth_chroma_minus8);
        {
            // Hint : bit_depth_chroma_minus8 shall be in the range of 0 to 8, inclusive.
            MPP_H26X_SYNTAXT_STRICT_CHECK(/* sps->bit_depth_chroma_minus8>=0 && */ sps->bit_depth_chroma_minus8<=8, "[sps] bit_depth_chroma_minus8 out of range", return false);
        }
        br->UE(sps->log2_max_pic_order_cnt_lsb_minus4);
        {
            // Hint : The value of log2_max_pic_order_cnt_lsb_minus4 shall be in the range of 0 to 12, inclusive. 
            MPP_H26X_SYNTAXT_STRICT_CHECK(/* sps->log2_max_pic_order_cnt_lsb_minus4>=0 && */ sps->log2_max_pic_order_cnt_lsb_minus4<=12, "[sps] log2_max_pic_order_cnt_lsb_minus4 out of range", return false);
        }
        br->U(1, sps->sps_sub_layer_ordering_info_present_flag);
        sps->sps_max_dec_pic_buffering_minus1.resize(sps->sps_max_sub_layers_minus1 + 1);
        sps->sps_max_num_reorder_pics.resize(sps->sps_max_sub_layers_minus1 + 1);
        sps->sps_max_latency_increase_plus1.resize(sps->sps_max_sub_layers_minus1 + 1);
        for (uint32_t i = (sps->sps_sub_layer_ordering_info_present_flag ? 0 : sps->sps_max_sub_layers_minus1); i<=sps->sps_max_sub_layers_minus1; i++)
        {
            br->UE(sps->sps_max_dec_pic_buffering_minus1[i]);
            br->UE(sps->sps_max_num_reorder_pics[i]);
            br->UE(sps->sps_max_latency_increase_plus1[i]);
        }
        br->UE(sps->log2_min_luma_coding_block_size_minus3);
        br->UE(sps->log2_diff_max_min_luma_coding_block_size);
        br->UE(sps->log2_min_luma_transform_block_size_minus2);
        br->UE(sps->log2_diff_max_min_luma_transform_block_size);
        br->UE(sps->max_transform_hierarchy_depth_inter);
        br->UE(sps->max_transform_hierarchy_depth_intra);
        br->U(1, sps->scaling_list_enabled_flag);
        if (sps->scaling_list_enabled_flag)
        {
            br->U(1, sps->sps_scaling_list_data_present_flag);
            if (sps->sps_scaling_list_data_present_flag)
            {
                sps->scaling_list_data = std::make_shared<H265ScalingListDataSyntax>();
                if (!DeserializeScalingListDataSyntax(br, sps->scaling_list_data))
                {
                    assert(false);
                    return false;
                }
            }
        }
        br->U(1, sps->amp_enabled_flag);
        br->U(1, sps->sample_adaptive_offset_enabled_flag);
        br->U(1, sps->pcm_enabled_flag);
        if (sps->pcm_enabled_flag)
        {
            br->U(4, sps->pcm_sample_bit_depth_luma_minus1);
            br->U(4, sps->pcm_sample_bit_depth_chroma_minus1);
            br->UE(sps->log2_min_pcm_luma_coding_block_size_minus3);
            br->UE(sps->log2_diff_max_min_pcm_luma_coding_block_size);
            br->U(1, sps->pcm_loop_filter_disabled_flag);
        }
        br->UE(sps->num_short_term_ref_pic_sets);
        sps->stpss.resize(sps->num_short_term_ref_pic_sets);
        for (uint32_t i=0; i<sps->num_short_term_ref_pic_sets; i++)
        {
            sps->stpss[i] = std::make_shared<H265StRefPicSetSyntax>();
            if (!DeserializeStRefPicSetSyntax(br, sps, i, sps->stpss[i]))
            {
                assert(false);
                return false;
            }
        }
        br->U(1, sps->long_term_ref_pics_present_flag);
        if (sps->long_term_ref_pics_present_flag)
        {
            br->UE(sps->num_long_term_ref_pics_sps);
            {
                // Hint : The value of num_long_term_ref_pics_sps shall be in the range of 0 to 32, inclusive. 
                MPP_H26X_SYNTAXT_STRICT_CHECK(/* sps->num_long_term_ref_pics_sps>=0 && */ sps->num_long_term_ref_pics_sps<=32, "[sps] num_long_term_ref_pics_sps out of range", return false);
            }
            sps->lt_ref_pic_poc_lsb_sps.resize(sps->num_long_term_ref_pics_sps);
            sps->used_by_curr_pic_lt_sps_flag.resize(sps->num_long_term_ref_pics_sps);
            for (uint32_t i=0; i<sps->num_long_term_ref_pics_sps; i++)
            {
                br->U(sps->log2_max_pic_order_cnt_lsb_minus4 + 4, sps->lt_ref_pic_poc_lsb_sps[i]);
                br->U(1, sps->used_by_curr_pic_lt_sps_flag[i]);
            }
        }
        br->U(1, sps->sps_temporal_mvp_enabled_flag);
        br->U(1, sps->strong_intra_smoothing_enabled_flag);
        br->U(1, sps->vui_parameters_present_flag);
        if (sps->vui_parameters_present_flag)
        {
            sps->vui = std::make_shared<H265VuiSyntax>();
            if (!DeserializeVuiSyntax(br, sps, sps->vui))
            {
                assert(false);
                return false;
            }
        }
        br->U(1, sps->sps_extension_present_flag);
        if (sps->sps_extension_present_flag)
        {
            br->U(1, sps->sps_range_extension_flag);
            br->U(1, sps->sps_multilayer_extension_flag);
            br->U(1, sps->sps_3d_extension_flag);
            br->U(1, sps->sps_scc_extension_flag);
            br->U(4, sps->sps_extension_4bits);
        }
        if (sps->sps_range_extension_flag)
        {
            sps->spsRange = std::make_shared<H265SpsRangeSyntax>();
            if (!DeserializeSpsRangeSyntax(br, sps->spsRange))
            {
                assert(false);
                return false;
            }
        }
        if (sps->sps_multilayer_extension_flag)
        {
            MPP_H26X_SYNTAXT_STRICT_CHECK(false, "[sps] missing feature sps_multilayer_extension_flag, not implement for now", return false);
            return false;
        }
        if (sps->sps_3d_extension_flag)
        {
            sps->sps3d = std::make_shared<H265Sps3DSyntax>();
            if (!DeserializeSps3DSyntax(br, sps->sps3d))
            {
                assert(false);
                return false;
            }
        }
        if (sps->sps_scc_extension_flag)
        {
            sps->spsScc = std::make_shared<H265SpsSccSyntax>();
            if (!DeserializeSpsSccSyntax(br, sps, sps->spsScc))
            {
                assert(false);
                return false;
            }
        }
        if (sps->sps_extension_4bits)
        {
            while (br->more_rbsp_data())
            {
                br->U(1, sps->sps_extension_data_flag);
            }
        }
        br->rbsp_trailing_bits();
        _contex->spsSet[sps->sps_seq_parameter_set_id] = sps;
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializeVPSSyntax(H26xBinaryReader::ptr br, H265VPSSyntax::ptr vps)
{
    // See also : ITU-T H.265 (2021) - 7.3.2.1 Video parameter set RBSP syntax
    try
    {
        br->U(4, vps->vps_video_parameter_set_id);
        br->U(1, vps->vps_base_layer_internal_flag);
        br->U(1, vps->vps_base_layer_available_flag);
        br->U(6, vps->vps_max_layers_minus1);
        br->U(3, vps->vps_max_sub_layers_minus1);
        br->U(1, vps->vps_temporal_id_nesting_flag);
        br->U(16, vps->vps_reserved_0xffff_16bits);
        MPP_H26X_SYNTAXT_STRICT_CHECK(vps->vps_reserved_0xffff_16bits==0xFFFF, "[vps] vps_reserved_0xffff_16bits is not equal to 0xffff", return false);
        vps->ptl = std::make_shared<H265PTLSyntax>();
        if (!DeserializePTLSyntax(br, 1, vps->vps_max_sub_layers_minus1, vps->ptl))
        {
            assert(false);
            return false;
        }
        br->U(1, vps->vps_sub_layer_ordering_info_present_flag);
        vps->vps_max_dec_pic_buffering_minus1.resize(vps->vps_max_layers_minus1 + 1);
        vps->vps_max_num_reorder_pics.resize(vps->vps_max_layers_minus1 + 1);
        vps->vps_max_latency_increase_plus1.resize(vps->vps_max_layers_minus1 + 1);
        for (uint32_t i= (vps->vps_sub_layer_ordering_info_present_flag ? 0 : vps->vps_max_sub_layers_minus1); i<=vps->vps_max_sub_layers_minus1; i++)
        {
            br->UE(vps->vps_max_dec_pic_buffering_minus1[i]);
            br->UE(vps->vps_max_num_reorder_pics[i]);
            br->UE(vps->vps_max_latency_increase_plus1[i]);
        }
        br->U(6, vps->vps_max_layer_id);
        br->UE(vps->vps_num_layer_sets_minus1);
        {
            // Hint : The  value  of vps_num_layer_sets_minus1 shall be in the range of 0to 1 023, inclusive.
            MPP_H26X_SYNTAXT_STRICT_CHECK(/* vps->vps_num_layer_sets_minus1>=0 && */ vps->vps_num_layer_sets_minus1<=1023, "[vps] vps_num_layer_sets_minus1 out of range", return false);
        }
        vps->layer_id_included_flag.resize(vps->vps_num_layer_sets_minus1 + 1);
        for (uint32_t i=1; i<=vps->vps_num_layer_sets_minus1; i++)
        {
            vps->layer_id_included_flag[i].resize(vps->vps_max_layer_id + 1);
            for (uint32_t j=0; j<=vps->vps_max_layer_id; j++)
            {
                br->U(1, vps->layer_id_included_flag[i][j]);
            }
        }
        br->U(1, vps->vps_timing_info_present_flag);
        if (vps->vps_timing_info_present_flag)
        {
            br->U(32, vps->vps_num_units_in_tick);
            br->U(32, vps->vps_time_scale);
            br->U(1, vps->vps_poc_proportional_to_timing_flag);
            if (vps->vps_poc_proportional_to_timing_flag)
            {
                br->UE(vps->vps_num_ticks_poc_diff_one_minus1);
            }
            br->UE(vps->vps_num_hrd_parameters);
            vps->hrd_layer_set_idx.resize(vps->vps_num_hrd_parameters);
            vps->cprms_present_flag.resize(vps->vps_num_hrd_parameters);
            vps->hrds.resize(vps->vps_num_hrd_parameters);
            for (uint32_t i=0; i<vps->vps_num_hrd_parameters; i++)
            {
                br->UE(vps->hrd_layer_set_idx[i]);
                if (i>0)
                {
                    br->U(1, vps->cprms_present_flag[i]);
                }
                vps->hrds[i] = std::make_shared<H265HrdSyntax>();
                if (!DeserializeHrdSyntax(br, vps->cprms_present_flag[i], vps->vps_max_sub_layers_minus1, vps->hrds[i]))
                {
                    assert(false);
                    return false;
                }
            }
        }
        br->U(1, vps->vps_extension_flag);
        if (vps->vps_extension_flag)
        {
            while (br->more_rbsp_data())
            {
                br->U(1, vps->vps_extension_data_flag);
            }
        }
        br->rbsp_trailing_bits();
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializeSliceHeaderSyntax(H26xBinaryReader::ptr br, H265NalUnitHeaderSyntax::ptr nal, H265SpsSyntax::ptr sps, H265PpsSyntax::ptr pps, H265SliceHeaderSyntax::ptr slice)
{
    // See also : ITU-T H.265 (2021) - 7.3.6.1 General slice segment header syntax
    try
    {
        int32_t CuQpDeltaVal = 0;
        br->U(1, slice->first_slice_segment_in_pic_flag);
        if (nal->nal_unit_type >= H265NaluType::MMP_H265_NALU_TYPE_BLA_W_LP && nal->nal_unit_type <= H265NaluType::MMP_H265_NALU_TYPE_RSV_IRAP_VCL23)
        {
            br->U(1, slice->no_output_of_prior_pics_flag);
        }
        br->UE(slice->slice_pic_parameter_set_id);
        if (!slice->first_slice_segment_in_pic_flag)
        {
            if (pps->dependent_slice_segments_enabled_flag)
            {
                br->U(1, slice->dependent_slice_segment_flag);
                // TODO
            }
        }
        if (!pps->dependent_slice_segments_enabled_flag)
        {
            slice->slice_reserved_flag.resize(pps->num_extra_slice_header_bits);
            for (uint32_t i=0; i<pps->num_extra_slice_header_bits; i++)
            {
                br->U(1, slice->slice_reserved_flag[i]);
            }
            br->UE(slice->slice_type);
            if (pps->output_flag_present_flag)
            {
                br->U(1, slice->pic_output_flag);
            }
            if (sps->separate_colour_plane_flag == 1)
            {
                br->U(2, slice->colour_plane_id);
            }
            if (nal->nal_unit_type != H265NaluType::MMP_H265_NALU_TYPE_IDR_W_RADL && nal->nal_unit_type != H265NaluType::MMP_H265_NALU_TYPE_IDR_N_LP)
            {
                br->U(sps->log2_max_pic_order_cnt_lsb_minus4 + 4, slice->slice_pic_order_cnt_lsb);
                br->U(1, slice->short_term_ref_pic_set_sps_flag);
                if (!slice->short_term_ref_pic_set_sps_flag)
                {
                    slice->stps = std::make_shared<H265StRefPicSetSyntax>();
                    if (!DeserializeStRefPicSetSyntax(br, sps, sps->num_short_term_ref_pic_sets, slice->stps))
                    {
                        assert(false);
                        return false;
                    }
                    else if (sps->num_short_term_ref_pic_sets > 1)
                    {
                        // TODO : short_term_ref_pic_set_idx
                    }
                    if (sps->long_term_ref_pics_present_flag)
                    {
                        if (sps->num_long_term_ref_pics_sps > 0)
                        {
                            br->UE(slice->num_long_term_sps);
                        }
                        br->UE(slice->num_long_term_pics);
                        slice->lt_idx_sps.resize(slice-> num_long_term_sps + slice->num_long_term_pics + 1);
                        slice->poc_lsb_lt.resize(slice-> num_long_term_sps + slice->num_long_term_pics + 1);
                        slice->used_by_curr_pic_lt_flag.resize(slice-> num_long_term_sps + slice->num_long_term_pics + 1);
                        slice->delta_poc_msb_present_flag.resize(slice-> num_long_term_sps + slice->num_long_term_pics + 1);
                        slice->delta_poc_msb_cycle_lt.resize(slice-> num_long_term_sps + slice->num_long_term_pics + 1);
                        for (uint32_t i=0; i<slice-> num_long_term_sps + slice->num_long_term_pics; i++)
                        {
                            // if (i<sps->slice-> num_long_term_sps)
                            // {
                            //     if (sps->num_long_term_ref_pics_sps > 1)
                            //     {
                            //         // TODO : lt_idx_sps
                            //     }
                            //     else
                            //     {
                            //         // TODO : poc_lsb_lt
                            //         br->U(1, slice->used_by_curr_pic_lt_flag[i]);
                            //     }
                            //     br->U(1, slice->delta_poc_msb_present_flag[i]);
                            //     if (slice->delta_poc_msb_present_flag[i])
                            //     {
                            //         br->UE(slice->delta_poc_msb_cycle_lt[i]);
                            //     }
                            // }
                        }
                        if (sps->sps_temporal_mvp_enabled_flag)
                        {
                            br->U(1, slice->slice_temporal_mvp_enabled_flag);
                        }
                        if (sps->sample_adaptive_offset_enabled_flag)
                        {
                            br->U(1, slice->slice_sao_luma_flag);
                            uint8_t ChromaArrayType = sps->separate_colour_plane_flag == 0 ? sps->chroma_format_idc : 0;
                            if (ChromaArrayType != 0)
                            {
                                br->U(1, slice->slice_sao_chroma_flag);
                            }
                        }
                        if (slice->slice_type == H265SliceType::MMP_H265_P_SLICE || slice->slice_type == H265SliceType::MMP_H265_B_SLICE)
                        {
                            br->U(1, slice->num_ref_idx_active_override_flag);
                            if (slice->num_ref_idx_active_override_flag)
                            {
                                br->UE(slice->num_ref_idx_l0_active_minus1);
                                if (slice->slice_type == H265SliceType::MMP_H265_B_SLICE)
                                {
                                    br->UE(slice->num_ref_idx_l1_active_minus1);
                                }
                            }
                        }
                        uint32_t NumPicTotalCurr = 0; // TODO (7-57)
                        if (pps->lists_modification_present_flag && NumPicTotalCurr>1)
                        {
                            slice->rplm = std::make_shared<H265RefPicListsModificationSyntax>();
                            if (!DeserializeRefPicListsModificationSyntax(br, slice, slice->rplm))
                            {
                                assert(false);
                                return false;
                            }
                        }
                        if (slice->slice_type == H265SliceType::MMP_H265_B_SLICE)
                        {
                            br->U(1, slice->mvd_l1_zero_flag);
                        }
                        if (pps->cabac_init_present_flag)
                        {
                            br->U(1, slice->cabac_init_flag);
                        }
                        if (slice->slice_temporal_mvp_enabled_flag)
                        {
                            if (slice->slice_type == H265SliceType::MMP_H265_B_SLICE)
                            {
                                br->U(1, slice->collocated_from_l0_flag);
                            }
                            if ((slice->collocated_from_l0_flag && slice->num_ref_idx_l0_active_minus1>0) ||
                                !(!slice->collocated_from_l0_flag && slice->num_ref_idx_l1_active_minus1 > 0)
                            )
                            {
                                br->UE(slice->collocated_ref_idx);
                            }
                        }
                        if ((pps->weighted_pred_flag && slice->slice_type == H265SliceType::MMP_H265_P_SLICE) ||
                            (pps->weighted_pred_flag && slice->slice_type == H265SliceType::MMP_H265_B_SLICE)
                        )
                        {
                            slice->pwt = std::make_shared<H265PredWeightTableSyntax>();
                            if (!DeserializePredWeightTableSyntax(br, sps, slice, slice->pwt))
                            {
                                assert(false);
                                return false;
                            }
                        }
                        br->UE(slice->five_minus_max_num_merge_cand);
                        if (sps->sps_scc_extension_flag && sps->spsScc->motion_vector_resolution_control_idc == 2)
                        {
                            br->U(1, slice->use_integer_mv_flag);
                        }
                        if (pps->pps_slice_chroma_qp_offsets_present_flag)
                        {
                            br->SE(slice->slice_cb_qp_offset);
                            br->SE(slice->slice_cr_qp_offset);
                        }
                        if (pps->pps_scc_extension_flag && pps->ppsScc->pps_slice_act_qp_offsets_present_flag)
                        {
                            br->SE(slice->slice_act_y_qp_offset);
                            br->SE(slice->slice_act_cb_qp_offset);
                            br->SE(slice->slice_act_cr_qp_offset);
                        }
                        if (pps->pps_range_extension_flag && pps->ppsRange->chroma_qp_offset_list_enabled_flag)
                        {
                            br->U(1, slice->cu_chroma_qp_offset_enabled_flag);
                        }
                        if (pps->deblocking_filter_override_enabled_flag)
                        {
                            br->U(1, slice->deblocking_filter_override_flag);
                            if (slice->deblocking_filter_override_flag)
                            {
                                br->SE(slice->slice_beta_offset_div2);
                                br->SE(slice->slice_tc_offset_div2);
                            }
                        }
                    }
                    if (pps->pps_loop_filter_across_slices_enabled_flag &&
                        (slice->slice_sao_luma_flag || slice->slice_sao_chroma_flag ||
                        !slice->slice_deblocking_filter_disabled_flag
                        )
                    )
                    {
                        br->U(1, slice->slice_loop_filter_across_slices_enabled_flag);
                    }
                }
                if (pps->tiles_enabled_flag || pps->entropy_coding_sync_enabled_flag)
                {
                    br->UE(slice->num_entry_point_offsets);
                    if (slice->num_entry_point_offsets>0)
                    {
                        br->UE(slice->offset_len_minus1);
                        slice->entry_point_offset_minus1.resize(slice->num_entry_point_offsets);
                        for (uint32_t i=0; i<slice->num_entry_point_offsets; i++)
                        {
                            // TODO : entry_point_offset_minus1
                        }
                    }
                }
                if (pps->slice_segment_header_extension_present_flag)
                {
                    br->UE(slice->slice_segment_header_extension_length);
                    slice->slice_segment_header_extension_data_byte.resize(slice->slice_segment_header_extension_length);
                    for (uint32_t i=0; i<slice->slice_segment_header_extension_length; i++)
                    {
                        br->U(8, slice->slice_segment_header_extension_data_byte[i]);
                    }
                }
                
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializePps3dSyntax(H26xBinaryReader::ptr br, H265PpsSyntax::ptr pps, H265Pps3dSyntax::ptr pps3d)
{
    // See also : ITU-T H.265 (2021) - I.7.3.2.3.7 Picture parameter set 3D extension syntax
    try
    {
        br->U(1, pps3d->dlts_present_flag);
        if (pps3d->dlts_present_flag)
        {
            br->U(6, pps3d->pps_depth_layers_minus1);
            br->U(4, pps3d->pps_bit_depth_for_depth_layers_minus8);
            pps3d->dlt_flag.resize(pps3d->pps_bit_depth_for_depth_layers_minus8 + 1);
            pps3d->dlt_pred_flag.resize(pps3d->pps_bit_depth_for_depth_layers_minus8 + 1);
            pps3d->dlt_val_flags_present_flag.resize(pps3d->pps_bit_depth_for_depth_layers_minus8 + 1);
            pps3d->dlt_value_flag.resize(pps3d->pps_bit_depth_for_depth_layers_minus8 + 1);
            pps3d->delta_dlt.resize(pps3d->pps_bit_depth_for_depth_layers_minus8 + 1);
            for (uint32_t i=0; i<=pps3d->pps_bit_depth_for_depth_layers_minus8; i++)
            {
                br->U(1, pps3d->dlt_flag[i]);
                if (pps3d->dlt_flag[i])
                {
                    br->U(1, pps3d->dlt_pred_flag[i]);
                    if (!pps3d->dlt_pred_flag[i])
                    {
                        br->U(1, pps3d->dlt_val_flags_present_flag[i]);
                    }
                    if (pps3d->dlt_val_flags_present_flag[i])
                    {
                        uint32_t depthMaxValue = (1 << (pps3d->pps_bit_depth_for_depth_layers_minus8 + 8)) - 1;
                        pps3d->dlt_value_flag[i].resize(depthMaxValue + 1);
                        for (uint32_t j=0; j<=depthMaxValue; j++)
                        {
                            br->U(1, pps3d->dlt_value_flag[i][j]);
                        }
                    }
                    else
                    {
                        pps3d->delta_dlt[i] = std::make_shared<H265DeltaDltSyntax>();
                        if (!DeserializeDeltaDltSyntax(br, pps3d, pps3d->delta_dlt[i]))
                        {
                            assert(false);
                            return false;
                        }
                    }
                }
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializePpsRangeSyntax(H26xBinaryReader::ptr br, H265PpsSyntax::ptr pps, H265PpsRangeSyntax::ptr ppsRange)
{
    // See also : ITU-T H.265 (2021) - 7.3.2.3.2 Picture parameter set range extension syntax
    try
    {
        if (pps->transform_skip_enabled_flag)
        {
            br->UE(ppsRange->log2_max_transform_skip_block_size_minus2);   
        }
        br->U(1, ppsRange->cross_component_prediction_enabled_flag);
        br->U(1, ppsRange->chroma_qp_offset_list_enabled_flag);
        if (ppsRange->chroma_qp_offset_list_enabled_flag)
        {
            br->UE(ppsRange->diff_cu_chroma_qp_offset_depth);
            br->UE(ppsRange->chroma_qp_offset_list_len_minus1);
            ppsRange->cb_qp_offset_list.resize(ppsRange->chroma_qp_offset_list_len_minus1 + 1);
            ppsRange->cr_qp_offset_list.resize(ppsRange->chroma_qp_offset_list_len_minus1 + 1);
            for (uint32_t i=0; i<=ppsRange->chroma_qp_offset_list_len_minus1; i++)
            {
                br->SE(ppsRange->cb_qp_offset_list[i]);
                br->SE(ppsRange->cr_qp_offset_list[i]);
            }
        }
        br->UE(ppsRange->log2_sao_offset_scale_luma); 
        br->UE(ppsRange->log2_sao_offset_scale_chroma); 
        return true;
    }
    catch (...)
    {
        return false;
    }   
}

bool H265Deserialize::DeserializePpsSccSyntax(H26xBinaryReader::ptr br, H265PpsSccSyntax::ptr ppsScc)
{
    // See also : ITU-T H.265 (2021) - 7.3.2.3.3 Picture parameter set screen content coding extension syntax
    try
    {
        br->U(1, ppsScc->pps_curr_pic_ref_enabled_flag);
        br->U(1, ppsScc->residual_adaptive_colour_transform_enabled_flag);
        if (ppsScc->residual_adaptive_colour_transform_enabled_flag)
        {
            br->U(1, ppsScc->pps_slice_act_qp_offsets_present_flag);
            br->SE(ppsScc->pps_act_y_qp_offset_plus5);
            br->SE(ppsScc->pps_act_cb_qp_offset_plus5);
            br->SE(ppsScc->pps_act_cr_qp_offset_plus3);
            {
                // Hint : It is a requirement of bitstream conformance that the values of PpsActQpOffsetY, PpsActQpOffsetCb, and 
                // PpsActQpOffsetCr shall be in the range of −12 to +12, inclusive.
                MPP_H26X_SYNTAXT_STRICT_CHECK(ppsScc->pps_act_y_qp_offset_plus5-5>=-12 && ppsScc->pps_act_y_qp_offset_plus5-5<=12, "[pps] pps_act_y_qp_offset_plus5 out of range", return false);
                MPP_H26X_SYNTAXT_STRICT_CHECK(ppsScc->pps_act_cb_qp_offset_plus5-5>=-12 && ppsScc->pps_act_cb_qp_offset_plus5-5<=12, "[pps] pps_act_cb_qp_offset_plus5 out of range", return false);
                MPP_H26X_SYNTAXT_STRICT_CHECK(ppsScc->pps_act_cr_qp_offset_plus3-3>=-12 && ppsScc->pps_act_cr_qp_offset_plus3-3<=12, "[pps] pps_act_cr_qp_offset_plus3 out of range", return false);
            }
        }
        br->U(1, ppsScc->pps_palette_predictor_initializers_present_flag);
        if (ppsScc->pps_palette_predictor_initializers_present_flag)
        {
            br->UE(ppsScc->pps_num_palette_predictor_initializers);
            if (ppsScc->pps_num_palette_predictor_initializers > 0)
            {
                br->U(1, ppsScc->monochrome_palette_flag);
                br->UE(ppsScc->luma_bit_depth_entry_minus8);
                if (!ppsScc->monochrome_palette_flag)
                {
                    br->UE(ppsScc->chroma_bit_depth_entry_minus8);
                }
                uint32_t numComps = ppsScc->monochrome_palette_flag ? 1 : 3;
                ppsScc->pps_palette_predictor_initializer.resize(numComps);
                for (uint32_t comp = 0; comp < numComps; comp++)
                {
                    ppsScc->pps_palette_predictor_initializer[comp].resize(ppsScc->pps_num_palette_predictor_initializers);
                    for (uint32_t i=0; i<ppsScc->pps_num_palette_predictor_initializers; i++)
                    {
                        if (comp == 0)
                        {
                            br->U(ppsScc->luma_bit_depth_entry_minus8 + 8, ppsScc->pps_palette_predictor_initializer[comp][i]);
                        }
                        else if (comp == 1 || comp == 2)
                        {
                            br->U(ppsScc->chroma_bit_depth_entry_minus8 + 8, ppsScc->pps_palette_predictor_initializer[comp][i]);
                        }
                    }
                }
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }  
}

bool H265Deserialize::DeserializeSpsRangeSyntax(H26xBinaryReader::ptr br, H265SpsRangeSyntax::ptr spsRange)
{
    // See also : ITU-T H.265 (2021) - 7.3.2.2.2 Sequence parameter set range extension syntax
    try
    {
        br->U(1, spsRange->transform_skip_rotation_enabled_flag);
        br->U(1, spsRange->transform_skip_context_enabled_flag);
        br->U(1, spsRange->implicit_rdpcm_enabled_flag);
        br->U(1, spsRange->explicit_rdpcm_enabled_flag);
        br->U(1, spsRange->intra_smoothing_disabled_flag);
        br->U(1, spsRange->high_precision_offsets_enabled_flag);
        br->U(1, spsRange->persistent_rice_adaptation_enabled_flag);
        br->U(1, spsRange->cabac_bypass_alignment_enabled_flag);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializeSps3DSyntax(H26xBinaryReader::ptr br, H265Sps3DSyntax::ptr sps3d)
{
    // See also : ITU-T H.265 (2021) - I.7.3.2.2.5 Sequence parameter set 3D extension syntax
    try
    {
        for (uint32_t d=0; d<=1; d++)
        {
            br->U(1, sps3d->iv_di_mc_enabled_flag[d]);
            br->U(1, sps3d->iv_mv_scal_enabled_flag[d]);
            if (d == 0)
            {
                br->UE(sps3d->log2_ivmc_sub_pb_size_minus3);
                br->U(1, sps3d->iv_res_pred_enabled_flag);
                br->U(1, sps3d->depth_ref_enabled_flag);
                br->U(1, sps3d->vsp_mc_enabled_flag);
                br->U(1, sps3d->dbbp_enabled_flag);
            }
            else
            {
                br->U(1, sps3d->tex_mc_enabled_flag);
                br->UE(sps3d->log2_texmc_sub_pb_size_minus3);
                br->U(1, sps3d->intra_contour_enabled_flag);
                br->U(1, sps3d->intra_dc_only_wedge_enabled_flag);
                br->U(1, sps3d->cqt_cu_part_pred_enabled_flag);
                br->U(1, sps3d->inter_dc_only_enabled_flag);
                br->U(1, sps3d->skip_intra_enabled_flag);
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializeSpsSccSyntax(H26xBinaryReader::ptr br, H265SpsSyntax::ptr sps, H265SpsSccSyntax::ptr spsScc)
{
    // See also : ITU-T H.265 (2021) - 7.3.2.2.3 Sequence parameter set screen content coding extension syntax
    try
    {
        br->U(1, spsScc->sps_curr_pic_ref_enabled_flag);
        br->U(1, spsScc->palette_mode_enabled_flag);
        if (spsScc->palette_mode_enabled_flag)
        {
            br->UE(spsScc->palette_max_size);
            br->UE(spsScc->delta_palette_max_predictor_size);
            br->U(1, spsScc->sps_palette_predictor_initializers_present_flag);
            if (spsScc->sps_palette_predictor_initializers_present_flag)
            {
                br->UE(spsScc->sps_num_palette_predictor_initializers_minus1);
                uint32_t numComps = (sps->chroma_format_idc == 0) ? 1 : 3;
                spsScc->sps_palette_predictor_initializer.resize(numComps);
                for (uint32_t comp=0; comp < numComps; comp++)
                {
                    spsScc->sps_palette_predictor_initializer[comp].resize(spsScc->sps_num_palette_predictor_initializers_minus1 + 1);
                    for (uint32_t i=0; i<=spsScc->sps_num_palette_predictor_initializers_minus1; i++)
                    {
                        br->U(i==0 ? sps->bit_depth_luma_minus8 + 8 : sps->bit_depth_chroma_minus8 + 8, spsScc->sps_palette_predictor_initializer[comp][i]);
                    }
                }
            }
        }
        br->U(2, spsScc->motion_vector_resolution_control_idc);
        br->U(1, spsScc->intra_boundary_filtering_disabled_flag);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializeVuiSyntax(H26xBinaryReader::ptr br, H265SpsSyntax::ptr sps, H265VuiSyntax::ptr vui)
{
    // See also : ITU-T H.265 (2021) - E.2.1 VUI parameters syntax
    try
    {
        constexpr uint8_t EXTENDED_SAR = 255;

        static auto getSar = [](uint32_t aspect_ratio_idc, uint16_t& sar_width, uint16_t& sar_height) -> void
        {
            // See also : ITU-T H.265 (2021) - Table E.1 – Interpretation of sample aspect ratio indicator
            #define MMP_ASPECT_RATION_IDC_MAP(_aspect_ratio_idc, _sar_width, _sar_height) case _aspect_ratio_idc:\
                                                                                          {\
                                                                                              sar_width = _sar_width;\
                                                                                              sar_height = _sar_height;\
                                                                                              break;\
                                                                                          }
            switch (aspect_ratio_idc)
            {
                MMP_ASPECT_RATION_IDC_MAP(1, 1, 1);
                MMP_ASPECT_RATION_IDC_MAP(2, 12, 11);
                MMP_ASPECT_RATION_IDC_MAP(3, 10, 11);
                MMP_ASPECT_RATION_IDC_MAP(4, 16, 11);
                MMP_ASPECT_RATION_IDC_MAP(5, 40, 33);
                MMP_ASPECT_RATION_IDC_MAP(6, 24, 11);
                MMP_ASPECT_RATION_IDC_MAP(7, 20, 11);
                MMP_ASPECT_RATION_IDC_MAP(8, 32, 11);
                MMP_ASPECT_RATION_IDC_MAP(9, 80, 33);
                MMP_ASPECT_RATION_IDC_MAP(10, 18, 11);
                MMP_ASPECT_RATION_IDC_MAP(11, 15, 11);
                MMP_ASPECT_RATION_IDC_MAP(12, 64, 33);
                MMP_ASPECT_RATION_IDC_MAP(13, 160, 99);
                MMP_ASPECT_RATION_IDC_MAP(14, 4, 3);
                MMP_ASPECT_RATION_IDC_MAP(15, 3, 2);
                MMP_ASPECT_RATION_IDC_MAP(16, 2, 1);
                default:
                    assert(false);
                    sar_width = 1;
                    sar_height = 1;
                    break;
            }
            #undef MMP_ASPECT_RATION_IDC_MAP
        };

        br->U(1, vui->aspect_ratio_info_present_flag);
        if (vui->aspect_ratio_info_present_flag)
        {
            br->U(8, vui->aspect_ratio_idc);
            if (vui->aspect_ratio_idc >=1 && vui->aspect_ratio_idc<=16)
            {
                getSar(vui->aspect_ratio_idc, vui->sar_width, vui->sar_height);
            }
            else if (vui->aspect_ratio_idc == EXTENDED_SAR)
            {
                br->U(16, vui->sar_width);
                br->U(16, vui->sar_height);
            }
        }
        br->U(1, vui->overscan_info_present_flag);
        if (vui->overscan_info_present_flag)
        {
            br->U(1, vui->overscan_appropriate_flag);
        }
        br->U(1, vui->video_signal_type_present_flag);
        if (vui->video_signal_type_present_flag)
        {
            br->U(3, vui->video_format);
            br->U(1, vui->video_full_range_flag);
            br->U(1, vui->colour_description_present_flag);
            if (vui->colour_description_present_flag)
            {
                br->U(8, vui->colour_primaries);
                br->U(8, vui->transfer_characteristics);
                br->U(8, vui->matrix_coeffs);
            }
        }
        br->U(1, vui->chroma_loc_info_present_flag);
        if (vui->chroma_loc_info_present_flag)
        {
            br->UE(vui->chroma_sample_loc_type_top_field);
            br->UE(vui->chroma_sample_loc_type_bottom_field);
        }
        br->U(1, vui->neutral_chroma_indication_flag);
        br->U(1, vui->field_seq_flag);
        br->U(1, vui->frame_field_info_present_flag);
        br->U(1, vui->default_display_window_flag);
        if (vui->default_display_window_flag)
        {
            br->UE(vui->def_disp_win_left_offset);
            br->UE(vui->def_disp_win_right_offset);
            br->UE(vui->def_disp_win_top_offset);
            br->UE(vui->def_disp_win_bottom_offset);
        }
        br->U(1, vui->vui_timing_info_present_flag);
        if (vui->vui_timing_info_present_flag)
        {
            br->U(32, vui->vui_num_units_in_tick);
            br->U(32, vui->vui_time_scale);
            br->U(1, vui->vui_poc_proportional_to_timing_flag);
            if (vui->vui_poc_proportional_to_timing_flag)
            {
                br->UE(vui->vui_num_ticks_poc_diff_one_minus1);
            }
            br->U(1, vui->vui_hrd_parameters_present_flag);
            if (vui->vui_hrd_parameters_present_flag)
            {
                vui->hrd_parameters = std::make_shared<H265HrdSyntax>();
                if (!DeserializeHrdSyntax(br, 1, sps->sps_max_sub_layers_minus1, vui->hrd_parameters))
                {
                    assert(false);
                    return false;
                }
            }
            br->U(1, vui->bitstream_restriction_flag);
            if (vui->bitstream_restriction_flag)
            {
                br->U(1, vui->tiles_fixed_structure_flag);
                br->U(1, vui->motion_vectors_over_pic_boundaries_flag);
                br->U(1, vui->restricted_ref_pic_lists_flag);
                br->UE(vui->min_spatial_segmentation_idc);
                br->UE(vui->max_bytes_per_pic_denom);
                br->UE(vui->max_bits_per_min_cu_denom);
                br->UE(vui->log2_max_mv_length_horizontal);
                br->UE(vui->log2_max_mv_length_vertical);
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    } 
}

bool H265Deserialize::DeserializeRefPicListsModificationSyntax(H26xBinaryReader::ptr br, H265SliceHeaderSyntax::ptr slice, H265RefPicListsModificationSyntax::ptr rplm)
{
    // See also : ITU-T H.265 (2021) - 7.3.6.2 Reference picture list modification syntax
    try
    {
        br->U(1, rplm->ref_pic_list_modification_flag_l0);
        if (rplm->ref_pic_list_modification_flag_l0)
        {
            rplm->list_entry_l0.resize(slice->num_ref_idx_l0_active_minus1 + 1);
            for (uint32_t i=0; i<=slice->num_ref_idx_l0_active_minus1; i++)
            {
                // TODO : list_entry_l0
            }
        }
        if (slice->slice_type == H265SliceType::MMP_H265_B_SLICE)
        {
            br->U(1, rplm->ref_pic_list_modification_flag_l1);
            if (rplm->ref_pic_list_modification_flag_l1)
            {
                rplm->list_entry_l1.resize(slice->num_ref_idx_l1_active_minus1 + 1);
                for (uint32_t i=0; i<=slice->num_ref_idx_l1_active_minus1; i++)
                {
                    // TODO : list_entry_l1
                }
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializePredWeightTableSyntax(H26xBinaryReader::ptr br, H265SpsSyntax::ptr sps, H265SliceHeaderSyntax::ptr slice, H265PredWeightTableSyntax::ptr pwt)
{
    // See also : ITU-T H.265 (2021) - 7.3.6.3 Weighted prediction parameters syntax
    try
    {
        // TODO
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializeHrdSyntax(H26xBinaryReader::ptr br, uint8_t commonInfPresentFlag, uint32_t maxNumSubLayersMinus, H265HrdSyntax::ptr hrd)
{
    // See also : ITU-T H.265 (2021) - E.2.2 HRD parameters syntax
    try
    {
        if (commonInfPresentFlag)
        {
            br->U(1, hrd->nal_hrd_parameters_present_flag);
            br->U(1, hrd->vcl_hrd_parameters_present_flag);
            if (hrd->nal_hrd_parameters_present_flag || hrd->vcl_hrd_parameters_present_flag)
            {
                br->U(1, hrd->sub_pic_hrd_params_present_flag);
                if (hrd->sub_pic_hrd_params_present_flag)
                {
                    br->U(8, hrd->tick_divisor_minus2);
                    br->U(5, hrd->du_cpb_removal_delay_increment_length_minus1);
                    br->U(1, hrd->sub_pic_cpb_params_in_pic_timing_sei_flag);
                    br->U(5, hrd->dpb_output_delay_du_length_minus1);
                }
                br->U(4, hrd->bit_rate_scale);
                br->U(4, hrd->cpb_size_scale);
                if (hrd->sub_pic_hrd_params_present_flag)
                {
                    br->U(4, hrd->cpb_size_du_scale);
                }
                br->U(5, hrd->initial_cpb_removal_delay_length_minus1);
                br->U(5, hrd->au_cpb_removal_delay_length_minus1);
                br->U(5, hrd->dpb_output_delay_length_minus1);
            }
        }
        hrd->fixed_pic_rate_general_flag.resize(maxNumSubLayersMinus + 1);
        hrd->fixed_pic_rate_within_cvs_flag.resize(maxNumSubLayersMinus + 1);
        hrd->elemental_duration_in_tc_minus1.resize(maxNumSubLayersMinus + 1);
        hrd->low_delay_hrd_flag.resize(maxNumSubLayersMinus + 1);
        hrd->cpb_cnt_minus1.resize(maxNumSubLayersMinus + 1);
        hrd->nal_hrd_parameters.resize(maxNumSubLayersMinus + 1);
        hrd->vcl_hrd_parameters.resize(maxNumSubLayersMinus + 1);
        for (uint32_t i=0; i<=maxNumSubLayersMinus; i++)
        {
            br->U(1, hrd->fixed_pic_rate_general_flag[i]);
            if (!hrd->fixed_pic_rate_general_flag[i])
            {
                br->U(1, hrd->fixed_pic_rate_general_flag[i]);
            }
            // To check condition of elemental_duration_in_tc_minus1
            // See also : https://github.com/FFmpeg/FFmpeg/commit/ded4478b8b6dbe939113b38df53778972e3af70e (FFmpeg 6.x)
            if (hrd->fixed_pic_rate_general_flag[i])
            {
                br->UE(hrd->elemental_duration_in_tc_minus1[i]);
            }
            else
            {
                br->U(1, hrd->low_delay_hrd_flag[i]);
            }
            if (!hrd->low_delay_hrd_flag[i])
            {
                br->UE(hrd->cpb_cnt_minus1[i]);
            }
            if (hrd->nal_hrd_parameters_present_flag)
            {
                hrd->nal_hrd_parameters[i] = std::make_shared<H265SubLayerHrdSyntax>();
                if (!DeserializeSubLayerHrdSyntax(br, i, hrd, hrd->nal_hrd_parameters[i]))
                {
                    assert(false);
                    return false;
                }
            }
            if (hrd->vcl_hrd_parameters_present_flag)
            {
                hrd->vcl_hrd_parameters[i] = std::make_shared<H265SubLayerHrdSyntax>();
                if (!DeserializeSubLayerHrdSyntax(br, i, hrd, hrd->vcl_hrd_parameters[i]))
                {
                    assert(false);
                    return false;
                }
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializeSubLayerHrdSyntax(H26xBinaryReader::ptr br, uint32_t subLayerId, H265HrdSyntax::ptr hrd, H265SubLayerHrdSyntax::ptr slHrd)
{
    // See also : ITU-T H.265 (2021) - E.2.3 Sub-layer HRD parameters syntax
    try
    {
        uint32_t CpbCnt = hrd->cpb_cnt_minus1[subLayerId] + 1;
        slHrd->bit_rate_value_minus1.resize(CpbCnt);
        slHrd->cpb_size_value_minus1.resize(CpbCnt);
        slHrd->cpb_size_du_value_minus1.resize(CpbCnt);
        slHrd->bit_rate_du_value_minus1.resize(CpbCnt);
        slHrd->cbr_flag.resize(CpbCnt);
        for (uint32_t i=0; i<CpbCnt; i++)
        {
            br->UE(slHrd->bit_rate_value_minus1[i]);
            br->UE(slHrd->cpb_size_value_minus1[i]);
            if (hrd->sub_pic_hrd_params_present_flag)
            {
                br->UE(slHrd->cpb_size_du_value_minus1[i]); 
                br->UE(slHrd->bit_rate_du_value_minus1[i]); 
            }
            br->U(1, slHrd->cbr_flag[i]);
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializePTLSyntax(H26xBinaryReader::ptr br, uint8_t profilePresentFlag, uint32_t maxNumSubLayersMinus1, H265PTLSyntax::ptr ptl)
{
    // See also : ITU-T H.265 (2021) - 7.3.3 Profile, tier and level syntax
    try
    {
        if (profilePresentFlag)
        {
            br->U(2, ptl->general_profile_space);
            br->U(1, ptl->general_tier_flag);
            br->U(5, ptl->general_profile_idc);
            for (uint32_t j=0; j<32; j++)
            {
                br->U(1, ptl->general_profile_compatibility_flag[j]);
            }
            br->U(1, ptl->general_progressive_source_flag);
            br->U(1, ptl->general_interlaced_source_flag);
            br->U(1, ptl->general_non_packed_constraint_flag);
            br->U(1, ptl->general_frame_only_constraint_flag);
            if (ptl->general_profile_idc == 4 || ptl->general_profile_compatibility_flag[4] ||
                ptl->general_profile_idc == 5 || ptl->general_profile_compatibility_flag[5] ||
                ptl->general_profile_idc == 6 || ptl->general_profile_compatibility_flag[6] ||
                ptl->general_profile_idc == 7 || ptl->general_profile_compatibility_flag[7] ||
                ptl->general_profile_idc == 8 || ptl->general_profile_compatibility_flag[8] ||
                ptl->general_profile_idc == 9 || ptl->general_profile_compatibility_flag[9] ||
                ptl->general_profile_idc == 10 || ptl->general_profile_compatibility_flag[10] ||
                ptl->general_profile_idc == 11 || ptl->general_profile_compatibility_flag[11]
            )
            {
                br->U(1, ptl->general_max_12bit_constraint_flag);
                br->U(1, ptl->general_max_10bit_constraint_flag);
                br->U(1, ptl->general_max_8bit_constraint_flag);
                br->U(1, ptl->general_max_422chroma_constraint_flag);
                br->U(1, ptl->general_max_420chroma_constraint_flag);
                br->U(1, ptl->general_max_monochrome_constraint_flag);
                br->U(1, ptl->general_intra_constraint_flag);
                br->U(1, ptl->general_one_picture_only_constraint_flag);
                br->U(1, ptl->general_lower_bit_rate_constraint_flag);
                if (ptl->general_profile_idc == 5 || ptl->general_profile_compatibility_flag[5] ||
                    ptl->general_profile_idc == 9 || ptl->general_profile_compatibility_flag[9] ||
                    ptl->general_profile_idc == 10 || ptl->general_profile_compatibility_flag[10] ||
                    ptl->general_profile_idc == 11 || ptl->general_profile_compatibility_flag[11]
                )
                {
                    br->U(1, ptl->general_max_14bit_constraint_flag);
                    br->U(33, ptl->general_reserved_zero_33bits);
                }
                else
                {
                    br->U(34, ptl->general_reserved_zero_34bits);
                }
            }
            else if (ptl->general_profile_idc == 2 || ptl->general_profile_compatibility_flag[2])
            {
                br->U(7, ptl->general_reserved_zero_7bits);
                br->U(1, ptl->general_one_picture_only_constraint_flag);
                br->U(35, ptl->general_reserved_zero_35bits);
            }
            else
            {
                br->U(43, ptl->general_reserved_zero_43bits);
            }
            if (ptl->general_profile_idc == 1 || ptl->general_profile_compatibility_flag[1] ||
                ptl->general_profile_idc == 2 || ptl->general_profile_compatibility_flag[2] ||
                ptl->general_profile_idc == 3 || ptl->general_profile_compatibility_flag[3] ||
                ptl->general_profile_idc == 4 || ptl->general_profile_compatibility_flag[4] ||
                ptl->general_profile_idc == 5 || ptl->general_profile_compatibility_flag[5] ||
                ptl->general_profile_idc == 9 || ptl->general_profile_compatibility_flag[9] ||
                ptl->general_profile_idc == 11 || ptl->general_profile_compatibility_flag[11]
            )
            {
                br->U(1, ptl->general_inbld_flag);
            }
            else
            {
                br->U(1, ptl->general_reserved_zero_bit);
            }
            br->U(8, ptl->general_level_idc);
            ptl->sub_layer_profile_present_flag.resize( maxNumSubLayersMinus1);
            ptl->sub_layer_level_present_flag.resize( maxNumSubLayersMinus1);
            for (uint32_t i=0; i<maxNumSubLayersMinus1; i++)
            {
                br->U(1, ptl->sub_layer_profile_present_flag[i]);
                br->U(1, ptl->sub_layer_level_present_flag[i]);
            }
            if (maxNumSubLayersMinus1 > 0)
            {
                ptl->reserved_zero_2bits.resize(8);
                for (uint32_t i=maxNumSubLayersMinus1; i<8; i++)
                {
                    br->U(2, ptl->reserved_zero_2bits[i]);
                }
            }
            ptl->sub_layer_profile_space.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_tier_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_profile_idc.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_profile_compatibility_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_progressive_source_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_interlaced_source_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_non_packed_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_frame_only_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_max_12bit_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_max_10bit_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_max_8bit_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_max_422chroma_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_max_420chroma_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_max_monochrome_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_intra_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_one_picture_only_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_lower_bit_rate_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_max_14bit_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_reserved_zero_33bits.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_reserved_zero_34bits.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_reserved_zero_7bits.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_one_picture_only_constraint_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_reserved_zero_35bits.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_reserved_zero_43bits.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_inbld_flag.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_reserved_zero_bit.resize(maxNumSubLayersMinus1);
            ptl->sub_layer_level_idc.resize(maxNumSubLayersMinus1);
            for (uint32_t i=0; i<maxNumSubLayersMinus1; i++)
            {
                if (ptl->sub_layer_profile_present_flag[i])
                {
                    br->U(2, ptl->sub_layer_profile_space[i]);
                    br->U(1, ptl->sub_layer_tier_flag[i]);
                    br->U(5, ptl->sub_layer_profile_idc[i]);
                    ptl->sub_layer_profile_compatibility_flag[i].resize(32);
                    for (uint32_t j=0; j<32; j++)
                    {
                        br->U(1, ptl->sub_layer_profile_compatibility_flag[i][j]);
                    }
                    br->U(1, ptl->sub_layer_progressive_source_flag[i]);
                    br->U(1, ptl->sub_layer_interlaced_source_flag[i]);
                    br->U(1, ptl->sub_layer_non_packed_constraint_flag[i]);
                    br->U(1, ptl->sub_layer_frame_only_constraint_flag[i]);
                    if (ptl->sub_layer_profile_idc[i] == 4 ||
                        ptl->sub_layer_profile_compatibility_flag[i][4] ||
                        ptl->sub_layer_profile_idc[i] == 5 ||
                        ptl->sub_layer_profile_compatibility_flag[i][5] ||
                        ptl->sub_layer_profile_idc[i] == 6 ||
                        ptl->sub_layer_profile_compatibility_flag[i][6] ||
                        ptl->sub_layer_profile_idc[i] == 7 ||
                        ptl->sub_layer_profile_compatibility_flag[i][7] ||
                        ptl->sub_layer_profile_idc[i] == 8 ||
                        ptl->sub_layer_profile_compatibility_flag[i][8] ||
                        ptl->sub_layer_profile_idc[i] == 9 ||
                        ptl->sub_layer_profile_compatibility_flag[i][9] ||
                        ptl->sub_layer_profile_idc[i] == 10 ||
                        ptl->sub_layer_profile_compatibility_flag[i][10] ||
                        ptl->sub_layer_profile_idc[i] == 11 ||
                        ptl->sub_layer_profile_compatibility_flag[i][11]
                    )
                    {
                        br->U(1, ptl->sub_layer_max_12bit_constraint_flag[i]);
                        br->U(1, ptl->sub_layer_max_10bit_constraint_flag[i]);
                        br->U(1, ptl->sub_layer_max_8bit_constraint_flag[i]);
                        br->U(1, ptl->sub_layer_max_422chroma_constraint_flag[i]);
                        br->U(1, ptl->sub_layer_max_420chroma_constraint_flag[i]);
                        br->U(1, ptl->sub_layer_max_monochrome_constraint_flag[i]);
                        br->U(1, ptl->sub_layer_intra_constraint_flag[i]);
                        br->U(1, ptl->sub_layer_one_picture_only_constraint_flag[i]);
                        br->U(1, ptl->sub_layer_lower_bit_rate_constraint_flag[i]);
                        if (ptl->sub_layer_profile_idc[i] == 5 ||
                            ptl->sub_layer_profile_compatibility_flag[i][5] ||
                            ptl->sub_layer_profile_idc[i] == 9 ||
                            ptl->sub_layer_profile_compatibility_flag[i][9] ||
                            ptl->sub_layer_profile_idc[i] == 10 ||
                            ptl->sub_layer_profile_compatibility_flag[i][10] ||
                            ptl->sub_layer_profile_idc[i] == 11 ||
                            ptl->sub_layer_profile_compatibility_flag[i][11]
                        )
                        {
                            br->U(1, ptl->sub_layer_max_14bit_constraint_flag[i]);
                            br->U(33, ptl->sub_layer_reserved_zero_33bits[i]);
                        }
                        else
                        {
                            br->U(34, ptl->sub_layer_reserved_zero_34bits[i]);
                        }
                    }
                }
                else if (ptl->sub_layer_profile_idc[i] == 2 ||
                        ptl->sub_layer_profile_compatibility_flag[i][2]
                )
                {
                    br->U(7, ptl->sub_layer_reserved_zero_7bits[i]);
                    br->U(1, ptl->sub_layer_one_picture_only_constraint_flag[i]);
                    br->U(35, ptl->sub_layer_reserved_zero_35bits[i]);
                }
                else
                {
                    br->U(43, ptl->sub_layer_reserved_zero_43bits[i]);
                }
                if (ptl->sub_layer_profile_idc[i] == 1 ||
                    ptl->sub_layer_profile_compatibility_flag[i][1] ||
                    ptl->sub_layer_profile_idc[i] == 2 ||
                    ptl->sub_layer_profile_compatibility_flag[i][2] ||
                    ptl->sub_layer_profile_idc[i] == 3 ||
                    ptl->sub_layer_profile_compatibility_flag[i][3] ||
                    ptl->sub_layer_profile_idc[i] == 4 ||
                    ptl->sub_layer_profile_compatibility_flag[i][4] ||
                    ptl->sub_layer_profile_idc[i] == 5 ||
                    ptl->sub_layer_profile_compatibility_flag[i][5] ||
                    ptl->sub_layer_profile_idc[i] == 9 ||
                    ptl->sub_layer_profile_compatibility_flag[i][9] ||
                    ptl->sub_layer_profile_idc[i] == 11 ||
                    ptl->sub_layer_profile_compatibility_flag[i][11]
                )
                {
                    br->U(1, ptl->sub_layer_inbld_flag[i]);
                }
                else
                {
                    br->U(1, ptl->sub_layer_reserved_zero_bit[i]);
                }
                if (ptl->sub_layer_level_present_flag[i])
                {
                    br->U(8, ptl->sub_layer_level_idc[i]);
                }
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializeScalingListDataSyntax(H26xBinaryReader::ptr br, H265ScalingListDataSyntax::ptr sld)
{
    // See also : ITU-T H.265 (2021) - 7.3.4 Scaling list data syntax
    try
    {
        int32_t scaling_list_delta_coef = 0;
        sld->scaling_list_pred_mode_flag.resize(4);
        sld->scaling_list_pred_matrix_id_delta.resize(4);
        sld->scaling_list_dc_coef_minus8.resize(4);
        sld->ScalingList.resize(4);
        for (uint32_t sizeId = 0; sizeId < 4; sizeId++)
        {
            sld->scaling_list_pred_mode_flag[sizeId].resize(6);
            sld->scaling_list_pred_matrix_id_delta[sizeId].resize(6);
            sld->scaling_list_dc_coef_minus8[sizeId].resize(6);
            sld->ScalingList[sizeId].resize(6);
            for (uint32_t matrixId=0; matrixId<6; matrixId+=(sizeId == 3)?3:1)
            {
                br->U(1, sld->scaling_list_pred_mode_flag[sizeId][matrixId]);
                if (sld->scaling_list_pred_mode_flag[sizeId][matrixId])
                {
                    br->UE(sld->scaling_list_pred_matrix_id_delta[sizeId][matrixId]);
                }
                else
                {
                    uint32_t nextCoef = 8;
                    uint32_t coefNum = std::min(64, (1<<(4+(sizeId<<1))));
                    if (sizeId > 1)
                    {
                        br->SE(sld->scaling_list_dc_coef_minus8[sizeId-2][matrixId]);
                        nextCoef = sld->scaling_list_dc_coef_minus8[sizeId-2][matrixId] + 8;
                    }
                    sld->ScalingList[sizeId][matrixId].resize(coefNum + 1);
                    for (uint32_t i=0; i<coefNum; i++)
                    {
                        br->SE(scaling_list_delta_coef);
                        nextCoef = (nextCoef + scaling_list_delta_coef + 256) % 256;
                        sld->ScalingList[sizeId][matrixId][i] = nextCoef;
                    }
                }
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializeStRefPicSetSyntax(H26xBinaryReader::ptr br, H265SpsSyntax::ptr sps, uint32_t stRpsIdx, H265StRefPicSetSyntax::ptr stps)
{
    // See also : ITU-T H.265 (2021) - 7.3.7 Short-term reference picture set syntax
    try
    {
        if (stRpsIdx != 0)
        {
            br->U(1, stps->inter_ref_pic_set_prediction_flag);
        }
        if (stps->inter_ref_pic_set_prediction_flag)
        {
            if (stRpsIdx == sps->num_short_term_ref_pic_sets)
            {
                br->UE(stps->delta_idx_minus1);
            }
            br->U(1, stps->delta_rps_sign);
            br->UE(stps->abs_delta_rps_minus1);
            uint32_t RefRpsIdx = stRpsIdx - (stps->abs_delta_rps_minus1 + 1);
            if (sps->stpss.size() < RefRpsIdx || sps->stpss[RefRpsIdx] == nullptr)
            {
                assert(false);
                return false;
            }
            stps->used_by_curr_pic_flag.resize(sps->stpss[RefRpsIdx]->NumDeltaPocs);
            stps->use_delta_flag.resize(sps->stpss[RefRpsIdx]->NumDeltaPocs);
            for (uint32_t j = 0; j<=sps->stpss[RefRpsIdx]->NumDeltaPocs; j++)
            {
                br->U(1, stps->used_by_curr_pic_flag[j]);
                if (!stps->used_by_curr_pic_flag[j])
                {
                    br->U(1, stps->use_delta_flag[j]);
                }
            }
        }
        else
        {
            br->UE(stps->num_negative_pics);
            br->UE(stps->num_positive_pics);
            stps->delta_poc_s0_minus1.resize(stps->num_negative_pics);
            stps->used_by_curr_pic_s0_flag.resize(stps->num_negative_pics);
            for (uint32_t i=0; i<stps->num_negative_pics; i++)
            {
                br->UE(stps->delta_poc_s0_minus1[i]);
                br->U(1, stps->used_by_curr_pic_s0_flag[i]);
            }
            stps->delta_poc_s1_minus1.resize(stps->num_positive_pics);
            stps->used_by_curr_pic_s1_flag.resize(stps->num_positive_pics);
            for (uint32_t i=0; i<stps->num_positive_pics; i++)
            {
                br->UE(stps->delta_poc_s1_minus1[i]);
                br->U(1, stps->used_by_curr_pic_s1_flag[i]);
            }
        }
        // determine NumDeltaPocs (7-71)
        {
            stps->NumDeltaPocs = stps->num_negative_pics + stps->num_positive_pics;
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializeColourMappingTable(H26xBinaryReader::ptr br, H265ColourMappingTable::ptr cmt)
{
    // See also : ITU-T H.265 (2021) - F.7.3.2.3.5 General colour mapping table syntax
    try
    {
        br->UE(cmt->num_cm_ref_layers_minus1);
        cmt->cm_ref_layer_id.resize(cmt->num_cm_ref_layers_minus1 + 1);
        for (uint32_t i=0; i<=cmt->num_cm_ref_layers_minus1; i++)
        {
            br->U(6, cmt->cm_ref_layer_id[i]);
        }
        br->U(2, cmt->cm_octant_depth);
        br->U(2, cmt->cm_y_part_num_log2);
        br->UE(cmt->luma_bit_depth_cm_input_minus8);
        br->UE(cmt->chroma_bit_depth_cm_input_minus8);
        br->UE(cmt->luma_bit_depth_cm_output_minus8);
        br->UE(cmt->chroma_bit_depth_cm_output_minus8);
        br->U(2, cmt->cm_res_quant_bits);
        br->U(2, cmt->cm_delta_flc_bits_minus1);
        if (cmt->cm_octant_depth == 1)
        {
            br->SE(cmt->cm_adapt_threshold_u_delta);
            br->SE(cmt->cm_adapt_threshold_v_delta);
        }
        // TODO : colour_mapping_octants( 0, 0, 0, 0, 1  <<  cm_octant_depth )
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializePpsMultilayerSyntax(H26xBinaryReader::ptr br, H265PpsMultilayerSyntax::ptr ppsMultilayer)
{
    // See also : ITU-T H.265 (2021) - F.7.3.2.3.4 Picture parameter set multilayer extension syntax
    try
    {
        br->U(1, ppsMultilayer->poc_reset_info_present_flag);
        br->U(1, ppsMultilayer->pps_infer_scaling_list_flag);
        if (ppsMultilayer->pps_infer_scaling_list_flag)
        {
            br->U(6, ppsMultilayer->pps_scaling_list_ref_layer_id);
        }
        br->UE(ppsMultilayer->num_ref_loc_offsets);
        ppsMultilayer->ref_loc_offset_layer_id.resize(ppsMultilayer->num_ref_loc_offsets);
        ppsMultilayer->scaled_ref_layer_offset_present_flag.resize(ppsMultilayer->num_ref_loc_offsets);
        ppsMultilayer->ref_region_offset_present_flag.resize(ppsMultilayer->num_ref_loc_offsets);
        ppsMultilayer->resample_phase_set_present_flag.resize(ppsMultilayer->num_ref_loc_offsets);
        for (uint32_t i=0; i<ppsMultilayer->num_ref_loc_offsets; i++)
        {
            br->U(6, ppsMultilayer->ref_loc_offset_layer_id[i]);
            br->U(1, ppsMultilayer->scaled_ref_layer_offset_present_flag[i]);
            if (ppsMultilayer->scaled_ref_layer_offset_present_flag[i])
            {
                br->SE(ppsMultilayer->scaled_ref_layer_left_offset[ppsMultilayer->ref_loc_offset_layer_id[i]]);
                br->SE(ppsMultilayer->scaled_ref_layer_top_offset[ppsMultilayer->ref_loc_offset_layer_id[i]]);
                br->SE(ppsMultilayer->scaled_ref_layer_right_offset[ppsMultilayer->ref_loc_offset_layer_id[i]]);
                br->SE(ppsMultilayer->scaled_ref_layer_bottom_offset[ppsMultilayer->ref_loc_offset_layer_id[i]]);
            }
            br->U(1, ppsMultilayer->ref_region_offset_present_flag[i]);
            if (ppsMultilayer->ref_region_offset_present_flag[i])
            {
                br->SE(ppsMultilayer->ref_region_left_offset[ppsMultilayer->ref_loc_offset_layer_id[i]]);
                br->SE(ppsMultilayer->ref_region_top_offset[ppsMultilayer->ref_loc_offset_layer_id[i]]);
                br->SE(ppsMultilayer->ref_region_right_offset[ppsMultilayer->ref_loc_offset_layer_id[i]]);
                br->SE(ppsMultilayer->ref_region_bottom_offset[ppsMultilayer->ref_loc_offset_layer_id[i]]);   
            }
            br->U(1, ppsMultilayer->resample_phase_set_present_flag[i]);
            if (ppsMultilayer->resample_phase_set_present_flag[i])
            {
                br->UE(ppsMultilayer->phase_hor_luma[ppsMultilayer->ref_loc_offset_layer_id[i]]);
                br->UE(ppsMultilayer->phase_ver_luma[ppsMultilayer->ref_loc_offset_layer_id[i]]);
                br->UE(ppsMultilayer->phase_hor_chroma_plus8[ppsMultilayer->ref_loc_offset_layer_id[i]]);
                br->UE(ppsMultilayer->phase_ver_chroma_plus8[ppsMultilayer->ref_loc_offset_layer_id[i]]);      
            }
        }
        br->U(1, ppsMultilayer->colour_mapping_enabled_flag);
        if (ppsMultilayer->colour_mapping_enabled_flag)
        {
            ppsMultilayer->cmt = std::make_shared<H265ColourMappingTable>();
            if (!DeserializeColourMappingTable(br, ppsMultilayer->cmt))
            {
                // TODO : support it
                assert(false);
                return false;
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool H265Deserialize::DeserializeDeltaDltSyntax(H26xBinaryReader::ptr br, H265Pps3dSyntax::ptr pps3d, H265DeltaDltSyntax::ptr dd)
{
    // See also : ITU-T H.265 (2021) - I.7.3.2.3.8 Delta depth look-up table syntax
    try
    {
        br->U(pps3d->pps_bit_depth_for_depth_layers_minus8+8, dd->num_val_delta_dlt);
        if (dd->num_val_delta_dlt > 0)
        {
            if (dd->num_val_delta_dlt>1)
            {
                br->U(pps3d->pps_bit_depth_for_depth_layers_minus8+8, dd->max_diff);
            }
            if (dd->num_val_delta_dlt>2 && dd->max_diff>0)
            {
                br->U((size_t)std::ceil(std::log2(dd->max_diff + 1)) ,dd->min_diff_minus1);
            }
            br->U(pps3d->pps_bit_depth_for_depth_layers_minus8+8, dd->delta_dlt_val0);
            if (dd->max_diff > (dd->min_diff_minus1 + 1))
            {
                uint32_t delta_val_diff_minus_min_len = (uint32_t)std::ceil(std::log2(dd->max_diff - dd->min_diff_minus1));
                dd->delta_val_diff_minus_min.resize(dd->num_val_delta_dlt);
                for (uint32_t k=1; k<dd->num_val_delta_dlt; k++)
                {
                    br->U(delta_val_diff_minus_min_len, dd->delta_val_diff_minus_min[k]);
                }
            }
        }
        return true;
    }
    catch (...)
    {
        return false;
    }
}

} // namespace Codec
} // namespace Mmp