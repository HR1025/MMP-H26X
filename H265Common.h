//
// H265Common.h
//
// Library: Coedec
// Package: H265
// Module:  H265
// 

#pragma once

#include <cstdint>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

namespace Mmp
{
namespace Codec
{

/**
 * @sa  ITU-T H.265 (2021) - Table 7-1 – NAL unit type codes and NAL unit type classes
 */
enum H265NaluType
{
    MMP_H265_NALU_TYPE_TRAIL_N            = 0,
    MMP_H265_NALU_TYPE_TRAIL_R            = 1,
    MMP_H265_NALU_TYPE_TSA_N              = 2,
    MMP_H265_NALU_TYPE_TSA_R              = 3,
    MMP_H265_NALU_TYPE_STSA_N             = 4,
    MMP_H265_NALU_TYPE_STSA_R             = 5,
    MMP_H265_NALU_TYPE_RADL_N             = 6,
    MMP_H265_NALU_TYPE_RADL_R             = 7,
    MMP_H265_NALU_TYPE_RASL_N             = 8,
    MMP_H265_NALU_TYPE_RASL_R             = 9,
    MMP_H265_NALU_TYPE_RSV_VCL_N10        = 10,
    MMP_H265_NALU_TYPE_RSV_VCL_N12        = 12,
    MMP_H265_NALU_TYPE_RSV_VCL_N14T       = 14,
    MMP_H265_NALU_TYPE_RSV_VCL_R11        = 11,
    MMP_H265_NALU_TYPE_RSV_VCL_R13        = 13,
    MMP_H265_NALU_TYPE_RSV_VCL_R15        = 15,
    MMP_H265_NALU_TYPE_BLA_W_LP           = 16,
    MMP_H265_NALU_TYPE_BLA_W_RADL         = 17,
    MMP_H265_NALU_TYPE_BLA_N_LP           = 18,
    MMP_H265_NALU_TYPE_IDR_W_RADL         = 19,
    MMP_H265_NALU_TYPE_IDR_N_LP           = 20,
    MMP_H265_NALU_TYPE_CRA_NUT            = 21,
    MMP_H265_NALU_TYPE_RSV_IRAP_VCL22     = 22,
    MMP_H265_NALU_TYPE_RSV_IRAP_VCL23     = 23,
    MMP_H265_NALU_TYPE_VPS_NUT            = 32,
    MMP_H265_NALU_TYPE_SPS_NUT            = 33,
    MMP_H265_NALU_TYPE_PPS_NUT            = 34,
    MMP_H265_NALU_TYPE_AUD_NUT            = 35,
    MMP_H265_NALU_TYPE_EOS_NUT            = 36,
    MMP_H265_NALU_TYPE_EOB_NUT            = 37,
    MMP_H265_NALU_TYPE_FD_NUT             = 38,
    MMP_H265_NALU_TYPE_PREFIX_SEI_NUT     = 39,
    MMP_H265_NALU_TYPE_SUFFIX_SEI_NUT     = 40
};

/**
 * @sa ITU-T H.265 (2021) - Table 7-7 – Name association to slice_type
 */
enum H265SliceType
{
    MMP_H265_P_SLICE                    = 0,
    MMP_H265_B_SLICE                    = 1,
    MMP_H265_I_SLICE                    = 2
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.7 Short-term reference picture set syntax
 */
class H265StRefPicSetSyntax
{
public:
    using ptr = std::shared_ptr<H265StRefPicSetSyntax>;
public:
    H265StRefPicSetSyntax();
    ~H265StRefPicSetSyntax() = default;
public:
    uint8_t  inter_ref_pic_set_prediction_flag;
    uint32_t delta_idx_minus1;
    uint8_t  delta_rps_sign;
    uint32_t abs_delta_rps_minus1;
    std::vector<uint8_t>  used_by_curr_pic_flag;
    std::vector<uint8_t>  use_delta_flag;
    uint32_t num_negative_pics;
    uint32_t num_positive_pics;
    std::vector<uint32_t> delta_poc_s0_minus1;
    std::vector<uint8_t>  used_by_curr_pic_s0_flag;
    std::vector<uint32_t> delta_poc_s1_minus1;
    std::vector<uint8_t>  used_by_curr_pic_s1_flag;
public:
    uint32_t NumDeltaPocs;
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.4 Scaling list data syntax
 */
class H265ScalingListDataSyntax
{
public:
    using ptr = std::shared_ptr<H265ScalingListDataSyntax>;
public:
    H265ScalingListDataSyntax() = default;
    ~H265ScalingListDataSyntax() = default;
public:
    std::vector<std::vector<uint8_t>>   scaling_list_pred_mode_flag;
    std::vector<std::vector<uint32_t>>  scaling_list_pred_matrix_id_delta;
    std::vector<std::vector<int32_t>>   scaling_list_dc_coef_minus8;
    std::vector<std::vector<std::vector<uint8_t>>> ScalingList;  
};

/**
 * @sa ITU-T H.265 (2021) - E.2.3 Sub-layer HRD parameters syntax
 */
class H265SubLayerHrdSyntax
{
public:
    using ptr = std::shared_ptr<H265SubLayerHrdSyntax>;
public:
    H265SubLayerHrdSyntax() = default;
    ~H265SubLayerHrdSyntax() = default;
public:
    std::vector<uint32_t>  bit_rate_value_minus1;
    std::vector<uint32_t>  cpb_size_value_minus1;
    std::vector<uint32_t>  cpb_size_du_value_minus1;
    std::vector<uint32_t>  bit_rate_du_value_minus1;
    std::vector<uint8_t>   cbr_flag;
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.2.2.2 Sequence parameter set range extension syntax
 */
class H265SpsRangeSyntax
{
public:
    using ptr = std::shared_ptr<H265SpsRangeSyntax>;
public:
    H265SpsRangeSyntax();
    ~H265SpsRangeSyntax() = default;
public:
    uint8_t  transform_skip_rotation_enabled_flag;
    uint8_t  transform_skip_context_enabled_flag;
    uint8_t  implicit_rdpcm_enabled_flag;
    uint8_t  explicit_rdpcm_enabled_flag;
    uint8_t  extended_precision_processing_flag;
    uint8_t  intra_smoothing_disabled_flag;
    uint8_t  high_precision_offsets_enabled_flag;
    uint8_t  persistent_rice_adaptation_enabled_flag;
    uint8_t  cabac_bypass_alignment_enabled_flag;
};

/**
 * @sa ITU-T H.265 (2021) - F.7.3.2.3.5 General colour mapping table syntax
 */
class H265ColourMappingTable
{
public:
    using ptr = std::shared_ptr<H265ColourMappingTable>;
public:
    H265ColourMappingTable();
    ~H265ColourMappingTable() = default;
public:
    uint32_t num_cm_ref_layers_minus1;
    std::vector<uint8_t> cm_ref_layer_id;
    uint8_t  cm_octant_depth;
    uint8_t  cm_y_part_num_log2;
    uint32_t luma_bit_depth_cm_input_minus8;
    uint32_t chroma_bit_depth_cm_input_minus8;
    uint32_t luma_bit_depth_cm_output_minus8;
    uint32_t chroma_bit_depth_cm_output_minus8;
    uint8_t  cm_res_quant_bits;
    uint8_t  cm_delta_flc_bits_minus1;
    int32_t  cm_adapt_threshold_u_delta;
    int32_t  cm_adapt_threshold_v_delta;
};

/**
 * @sa ITU-T H.265 (2021) - F.7.3.2.3.4 Picture parameter set multilayer extension syntax
 */
class H265PpsMultilayerSyntax
{
public:
    using ptr = std::shared_ptr<H265PpsMultilayerSyntax>;
public:
    H265PpsMultilayerSyntax();
    ~H265PpsMultilayerSyntax() = default;
public:
    uint8_t  poc_reset_info_present_flag;
    uint8_t  pps_infer_scaling_list_flag;
    uint8_t  pps_scaling_list_ref_layer_id;
    uint32_t num_ref_loc_offsets;
    std::vector<uint8_t>  ref_loc_offset_layer_id;
    std::vector<uint8_t>  scaled_ref_layer_offset_present_flag;
    int32_t  scaled_ref_layer_left_offset[1<<6u];
    int32_t  scaled_ref_layer_top_offset[1<<6u];
    int32_t  scaled_ref_layer_right_offset[1<<6u];
    int32_t  scaled_ref_layer_bottom_offset[1<<6u];
    std::vector<uint8_t> ref_region_offset_present_flag;
    int32_t  ref_region_left_offset[1<<6u];
    int32_t  ref_region_top_offset[1<<6u];
    int32_t  ref_region_right_offset[1<<6u];
    int32_t  ref_region_bottom_offset[1<<6u];
    std::vector<uint8_t> resample_phase_set_present_flag;
    uint32_t phase_hor_luma[1<<6u];
    uint32_t phase_ver_luma[1<<6u];
    uint32_t phase_hor_chroma_plus8[1<<6u];
    uint32_t phase_ver_chroma_plus8[1<<6u];
    uint8_t colour_mapping_enabled_flag;
    H265ColourMappingTable::ptr cmt;
};

/**
 * @sa ITU-T H.265 (2021) - F.7.3.2.2.4 Sequence parameter set multilayer extension syntax
 */
class H265SpsMultilayerSyntax
{
public:
    using ptr = std::shared_ptr<H265SpsMultilayerSyntax>;
public:
    H265SpsMultilayerSyntax() = default;
    ~H265SpsMultilayerSyntax() = default;
public:
    uint8_t inter_view_mv_vert_constraint_flag;
};

/**
 * @sa ITU-T H.265 (2021) - I.7.3.2.2.5 Sequence parameter set 3D extension syntax
 */
class H265Sps3DSyntax
{
public:
    using ptr = std::shared_ptr<H265Sps3DSyntax>;
public:
    H265Sps3DSyntax();
    ~H265Sps3DSyntax() = default;
public:
    uint8_t  iv_di_mc_enabled_flag[2];
    uint8_t  iv_mv_scal_enabled_flag[2];
    uint32_t log2_ivmc_sub_pb_size_minus3;
    uint8_t  iv_res_pred_enabled_flag;
    uint8_t  depth_ref_enabled_flag;
    uint8_t  vsp_mc_enabled_flag;
    uint8_t  dbbp_enabled_flag;
    uint8_t  tex_mc_enabled_flag;
    uint32_t log2_texmc_sub_pb_size_minus3;
    uint8_t  intra_contour_enabled_flag;
    uint8_t  intra_dc_only_wedge_enabled_flag;
    uint8_t  cqt_cu_part_pred_enabled_flag;
    uint8_t  inter_dc_only_enabled_flag;
    uint8_t  skip_intra_enabled_flag;
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.2.2.3 Sequence parameter set screen content coding extension syntax
 */
class H265SpsSccSyntax
{
public:
    using ptr = std::shared_ptr<H265SpsSccSyntax>;
public:
    H265SpsSccSyntax();
    ~H265SpsSccSyntax() = default;
public:
    uint8_t   sps_curr_pic_ref_enabled_flag;
    uint8_t   palette_mode_enabled_flag;
    uint32_t  palette_max_size;
    uint32_t  delta_palette_max_predictor_size;
    uint8_t   sps_palette_predictor_initializers_present_flag;
    uint32_t  sps_num_palette_predictor_initializers_minus1;
    std::vector<std::vector<uint32_t>>  sps_palette_predictor_initializer;
    uint8_t   motion_vector_resolution_control_idc;
    uint8_t   intra_boundary_filtering_disabled_flag;
};


/**
 * @sa ITU-T H.265 (2021) - 7.3.2.3.2 Picture parameter set range extension syntax
 */
class H265PpsRangeSyntax
{
public:
    using ptr = std::shared_ptr<H265PpsRangeSyntax>;
public:
    H265PpsRangeSyntax();
    ~H265PpsRangeSyntax() = default;
public:
    uint32_t log2_max_transform_skip_block_size_minus2;
    uint8_t  cross_component_prediction_enabled_flag;
    uint8_t  chroma_qp_offset_list_enabled_flag;
    uint32_t diff_cu_chroma_qp_offset_depth;
    uint32_t chroma_qp_offset_list_len_minus1;
    std::vector<int32_t> cb_qp_offset_list;
    std::vector<int32_t> cr_qp_offset_list;
    uint32_t log2_sao_offset_scale_luma;
    uint32_t log2_sao_offset_scale_chroma;
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.2.3.3 Picture parameter set screen content coding extension syntax
 */
class H265PpsSccSyntax
{
public:
    using ptr = std::shared_ptr<H265PpsSccSyntax>;
public:
    H265PpsSccSyntax();
    ~H265PpsSccSyntax() = default;
public:
    uint8_t  pps_curr_pic_ref_enabled_flag;
    uint8_t  residual_adaptive_colour_transform_enabled_flag;
    uint8_t  pps_slice_act_qp_offsets_present_flag;
    int32_t  pps_act_y_qp_offset_plus5;
    int32_t  pps_act_cb_qp_offset_plus5;
    int32_t  pps_act_cr_qp_offset_plus3;
    uint8_t  pps_palette_predictor_initializers_present_flag;
    uint32_t pps_num_palette_predictor_initializers;
    uint8_t  monochrome_palette_flag;
    uint32_t luma_bit_depth_entry_minus8;
    uint32_t chroma_bit_depth_entry_minus8;
    std::vector<std::vector<uint32_t>> pps_palette_predictor_initializer;
};

/**
 * @sa ITU-T H.265 (2021) - I.7.3.2.3.8 Delta depth look-up table syntax
 */
class H265DeltaDltSyntax
{
public:
    using ptr = std::shared_ptr<H265DeltaDltSyntax>;
public:
    H265DeltaDltSyntax();
    ~H265DeltaDltSyntax() = default;
public:
    uint32_t num_val_delta_dlt;
    uint32_t max_diff;
    uint32_t min_diff_minus1;
    uint32_t delta_dlt_val0;
    std::vector<uint32_t> delta_val_diff_minus_min;
};

/**
 * @sa ITU-T H.265 (2021) - I.7.3.2.3.7 Picture parameter set 3D extension syntax
 */
class H265Pps3dSyntax
{
public:
    using ptr = std::shared_ptr<H265Pps3dSyntax>;
public:
    H265Pps3dSyntax();
    ~H265Pps3dSyntax() = default;
public:
    uint8_t  dlts_present_flag;
    uint8_t  pps_depth_layers_minus1;
    uint8_t  pps_bit_depth_for_depth_layers_minus8;
    std::vector<uint8_t> dlt_flag;
    std::vector<uint8_t> dlt_pred_flag;
    std::vector<uint8_t> dlt_val_flags_present_flag;
    std::vector<std::vector<uint8_t>> dlt_value_flag;
    std::vector<H265DeltaDltSyntax::ptr> delta_dlt;
};

/**
 * @sa ITU-T H.265 (2021) - E.2.2 HRD parameters syntax
 */
class H265HrdSyntax
{
public:
    using ptr = std::shared_ptr<H265HrdSyntax>;
public:
    H265HrdSyntax();
    ~H265HrdSyntax() = default;
public:
    uint8_t  nal_hrd_parameters_present_flag;
    uint8_t  vcl_hrd_parameters_present_flag;
    uint8_t  sub_pic_hrd_params_present_flag;
    uint8_t  tick_divisor_minus2;
    uint8_t  du_cpb_removal_delay_increment_length_minus1;
    uint8_t  sub_pic_cpb_params_in_pic_timing_sei_flag;
    uint8_t  dpb_output_delay_du_length_minus1;
    uint8_t  bit_rate_scale;
    uint8_t  cpb_size_scale;
    uint8_t  cpb_size_du_scale;
    uint8_t  initial_cpb_removal_delay_length_minus1;
    uint8_t  au_cpb_removal_delay_length_minus1;
    uint8_t  dpb_output_delay_length_minus1;
    std::vector<uint8_t>  fixed_pic_rate_general_flag;
    std::vector<uint8_t>  fixed_pic_rate_within_cvs_flag;
    std::vector<uint32_t> elemental_duration_in_tc_minus1;
    std::vector<uint8_t>  low_delay_hrd_flag;
    std::vector<uint32_t> cpb_cnt_minus1;
    std::vector<H265SubLayerHrdSyntax::ptr> nal_hrd_parameters;
    std::vector<H265SubLayerHrdSyntax::ptr> vcl_hrd_parameters;
};

/**
 * @sa ITU-T H.265 (2021) - E.2.1 VUI parameters syntax
 */
class H265VuiSyntax
{
public:
    using ptr = std::shared_ptr<H265VuiSyntax>;
public:
    H265VuiSyntax();
    ~H265VuiSyntax() = default;
public:
    uint8_t   aspect_ratio_info_present_flag;
    uint8_t   aspect_ratio_idc;
    uint16_t  sar_width;
    uint16_t  sar_height;
    uint8_t   overscan_info_present_flag;
    uint8_t   overscan_appropriate_flag;
    uint8_t   video_signal_type_present_flag;
    uint8_t   video_format;
    uint8_t   video_full_range_flag;
    uint8_t   colour_description_present_flag;
    uint8_t   colour_primaries;
    uint8_t   transfer_characteristics;
    uint8_t   matrix_coeffs;
    uint8_t   chroma_loc_info_present_flag;
    uint32_t  chroma_sample_loc_type_top_field;
    uint32_t  chroma_sample_loc_type_bottom_field;
    uint8_t   neutral_chroma_indication_flag;
    uint8_t   field_seq_flag;
    uint8_t   frame_field_info_present_flag;
    uint8_t   default_display_window_flag;
    uint32_t  def_disp_win_left_offset;
    uint32_t  def_disp_win_right_offset;
    uint32_t  def_disp_win_top_offset;
    uint32_t  def_disp_win_bottom_offset;
    uint8_t   vui_timing_info_present_flag;
    uint32_t  vui_num_units_in_tick;
    uint32_t  vui_time_scale;
    uint8_t   vui_poc_proportional_to_timing_flag;
    uint32_t  vui_num_ticks_poc_diff_one_minus1;
    uint8_t   vui_hrd_parameters_present_flag;
    H265HrdSyntax::ptr hrd_parameters;
    uint8_t   bitstream_restriction_flag;
    uint8_t   tiles_fixed_structure_flag;
    uint8_t   motion_vectors_over_pic_boundaries_flag;
    uint8_t   restricted_ref_pic_lists_flag;
    uint32_t  min_spatial_segmentation_idc;
    uint32_t  max_bytes_per_pic_denom;
    uint32_t  max_bits_per_min_cu_denom;
    uint32_t  log2_max_mv_length_horizontal;
    uint32_t  log2_max_mv_length_vertical;
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.3 Profile, tier and level syntax
 */
class H265PTLSyntax
{
public:
    using ptr = std::shared_ptr<H265PTLSyntax>;
public:
    H265PTLSyntax();
    ~H265PTLSyntax() = default;
public:
    uint8_t  general_profile_space;
    uint8_t  general_tier_flag;
    uint8_t  general_profile_idc;
    uint8_t  general_profile_compatibility_flag[32];
    uint8_t  general_progressive_source_flag;
    uint8_t  general_interlaced_source_flag;
    uint8_t  general_non_packed_constraint_flag;
    uint8_t  general_frame_only_constraint_flag;
    uint8_t  general_max_12bit_constraint_flag;
    uint8_t  general_max_10bit_constraint_flag;
    uint8_t  general_max_8bit_constraint_flag;
    uint8_t  general_max_422chroma_constraint_flag;
    uint8_t  general_max_420chroma_constraint_flag;
    uint8_t  general_max_monochrome_constraint_flag;
    uint8_t  general_intra_constraint_flag;
    uint8_t  general_one_picture_only_constraint_flag;
    uint8_t  general_lower_bit_rate_constraint_flag;
    uint8_t  general_max_14bit_constraint_flag;
    uint64_t general_reserved_zero_33bits;
    uint64_t general_reserved_zero_34bits;
    uint8_t  general_reserved_zero_7bits;
    uint64_t general_reserved_zero_35bits;
    uint64_t general_reserved_zero_43bits;
    uint8_t  general_inbld_flag;
    uint8_t  general_reserved_zero_bit;
    uint8_t  general_level_idc;
    std::vector<uint8_t>  sub_layer_profile_present_flag;
    std::vector<uint8_t>  sub_layer_level_present_flag;
    std::vector<uint8_t>  reserved_zero_2bits;
    std::vector<uint8_t>  sub_layer_profile_space;
    std::vector<uint8_t>  sub_layer_tier_flag;
    std::vector<uint8_t>  sub_layer_profile_idc;
    std::vector<std::vector<uint8_t>>  sub_layer_profile_compatibility_flag;
    std::vector<uint8_t>  sub_layer_progressive_source_flag;
    std::vector<uint8_t>  sub_layer_interlaced_source_flag;
    std::vector<uint8_t>  sub_layer_non_packed_constraint_flag;
    std::vector<uint8_t>  sub_layer_frame_only_constraint_flag;
    std::vector<uint8_t>  sub_layer_max_12bit_constraint_flag;
    std::vector<uint8_t>  sub_layer_max_10bit_constraint_flag;
    std::vector<uint8_t>  sub_layer_max_8bit_constraint_flag;
    std::vector<uint8_t>  sub_layer_max_422chroma_constraint_flag;
    std::vector<uint8_t>  sub_layer_max_420chroma_constraint_flag;
    std::vector<uint8_t>  sub_layer_max_monochrome_constraint_flag;
    std::vector<uint8_t>  sub_layer_intra_constraint_flag;
    std::vector<uint8_t>  sub_layer_one_picture_only_constraint_flag;
    std::vector<uint8_t>  sub_layer_lower_bit_rate_constraint_flag;
    std::vector<uint8_t>  sub_layer_max_14bit_constraint_flag;
    std::vector<uint64_t> sub_layer_reserved_zero_33bits;
    std::vector<uint64_t> sub_layer_reserved_zero_34bits;
    std::vector<uint8_t>  sub_layer_reserved_zero_7bits;
    std::vector<uint64_t> sub_layer_reserved_zero_35bits;
    std::vector<uint64_t> sub_layer_reserved_zero_43bits;
    std::vector<uint8_t>  sub_layer_inbld_flag;
    std::vector<uint8_t>  sub_layer_reserved_zero_bit;
    std::vector<uint8_t>  sub_layer_level_idc;
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.2.1 Video parameter set RBSP syntax
 */
class H265VPSSyntax
{
public:
    using ptr = std::shared_ptr<H265VPSSyntax>;
public:
    H265VPSSyntax();
    ~H265VPSSyntax() = default;
public:
    uint8_t  vps_video_parameter_set_id;
    uint8_t  vps_base_layer_internal_flag;
    uint8_t  vps_base_layer_available_flag;
    uint8_t  vps_max_layers_minus1;
    uint8_t  vps_max_sub_layers_minus1;
    uint8_t  vps_temporal_id_nesting_flag;
    uint16_t vps_reserved_0xffff_16bits;
    H265PTLSyntax::ptr ptl;
    uint8_t  vps_sub_layer_ordering_info_present_flag;
    std::vector<uint32_t>  vps_max_dec_pic_buffering_minus1;
    std::vector<uint32_t>  vps_max_num_reorder_pics;
    std::vector<uint32_t>  vps_max_latency_increase_plus1;
    uint8_t   vps_max_layer_id;
    uint32_t  vps_num_layer_sets_minus1;
    std::vector<std::vector<uint8_t>> layer_id_included_flag;
    uint8_t  vps_timing_info_present_flag;
    uint32_t vps_num_units_in_tick;
    uint32_t vps_time_scale;
    uint8_t  vps_poc_proportional_to_timing_flag;
    uint32_t vps_num_ticks_poc_diff_one_minus1;
    uint32_t vps_num_hrd_parameters;
    std::vector<uint32_t>  hrd_layer_set_idx;
    std::vector<uint8_t>   cprms_present_flag;
    std::vector<H265HrdSyntax::ptr> hrds;
    uint8_t  vps_extension_flag;
    uint8_t  vps_extension_data_flag;
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.2.2.1 General sequence parameter set RBSP syntax
 */
class H265SpsSyntax
{
public:
    using ptr = std::shared_ptr<H265SpsSyntax>;
public:
    H265SpsSyntax();
    ~H265SpsSyntax() = default;
public:
    uint8_t  sps_video_parameter_set_id;
    uint8_t  sps_max_sub_layers_minus1;
    uint8_t  sps_temporal_id_nesting_flag;
    H265PTLSyntax::ptr ptl;
    uint32_t sps_seq_parameter_set_id;
    uint32_t chroma_format_idc;
    uint8_t  separate_colour_plane_flag;
    uint32_t pic_width_in_luma_samples;
    uint32_t pic_height_in_luma_samples;
    uint8_t  conformance_window_flag;
    uint32_t conf_win_left_offset;
    uint32_t conf_win_right_offset;
    uint32_t conf_win_top_offset;
    uint32_t conf_win_bottom_offset;
    uint32_t bit_depth_luma_minus8;
    uint32_t bit_depth_chroma_minus8;
    uint32_t log2_max_pic_order_cnt_lsb_minus4;
    uint8_t  sps_sub_layer_ordering_info_present_flag;
    std::vector<uint32_t> sps_max_dec_pic_buffering_minus1;
    std::vector<uint32_t> sps_max_num_reorder_pics;
    std::vector<uint32_t> sps_max_latency_increase_plus1;
    uint32_t log2_min_luma_coding_block_size_minus3;
    uint32_t log2_diff_max_min_luma_coding_block_size;
    uint32_t log2_min_luma_transform_block_size_minus2;
    uint32_t log2_diff_max_min_luma_transform_block_size;
    uint32_t max_transform_hierarchy_depth_inter;
    uint32_t max_transform_hierarchy_depth_intra;
    uint32_t scaling_list_enabled_flag;
    uint32_t sps_scaling_list_data_present_flag;
    H265ScalingListDataSyntax::ptr scaling_list_data;
    uint8_t  amp_enabled_flag;
    uint8_t  sample_adaptive_offset_enabled_flag;
    uint8_t  pcm_enabled_flag;
    uint8_t  pcm_sample_bit_depth_luma_minus1;
    uint8_t  pcm_sample_bit_depth_chroma_minus1;
    uint32_t log2_min_pcm_luma_coding_block_size_minus3;
    uint32_t log2_diff_max_min_pcm_luma_coding_block_size;
    uint8_t  pcm_loop_filter_disabled_flag;
    uint32_t num_short_term_ref_pic_sets;
    std::vector<H265StRefPicSetSyntax::ptr> stpss;
    uint8_t  long_term_ref_pics_present_flag;
    uint32_t num_long_term_ref_pics_sps;
    std::vector<uint32_t> lt_ref_pic_poc_lsb_sps;
    std::vector<uint8_t>  used_by_curr_pic_lt_sps_flag;
    uint8_t  sps_temporal_mvp_enabled_flag;
    uint8_t  strong_intra_smoothing_enabled_flag;
    uint8_t  vui_parameters_present_flag;
    H265VuiSyntax::ptr vui;
    uint8_t  sps_extension_present_flag;
    uint8_t  sps_range_extension_flag;
    uint8_t  sps_multilayer_extension_flag;
    uint8_t  sps_3d_extension_flag;
    uint8_t  sps_scc_extension_flag;
    uint8_t  sps_extension_4bits;
    H265SpsRangeSyntax::ptr spsRange;
    H265Sps3DSyntax::ptr sps3d;
    H265SpsSccSyntax::ptr spsScc;
    uint8_t  sps_extension_data_flag;
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.2.3.1 General picture parameter set RBSP syntax
 */
class H265PpsSyntax
{
public:
    using ptr = std::shared_ptr<H265PpsSyntax>;
public:
    H265PpsSyntax();
    ~H265PpsSyntax() = default;
public:
    uint32_t  pps_pic_parameter_set_id;
    uint32_t  pps_seq_parameter_set_id;
    uint8_t   dependent_slice_segments_enabled_flag;
    uint8_t   output_flag_present_flag;
    uint8_t   num_extra_slice_header_bits;
    uint8_t   sign_data_hiding_enabled_flag;
    uint8_t   cabac_init_present_flag;
    uint32_t  num_ref_idx_l0_default_active_minus1;
    uint32_t  num_ref_idx_l1_default_active_minus1;
    int32_t   init_qp_minus26;
    uint8_t   constrained_intra_pred_flag;
    uint8_t   transform_skip_enabled_flag;
    uint8_t   cu_qp_delta_enabled_flag;
    uint32_t  diff_cu_qp_delta_depth;
    int32_t   pps_cb_qp_offset;
    int32_t   pps_cr_qp_offset;
    uint8_t   pps_slice_chroma_qp_offsets_present_flag;
    uint8_t   weighted_pred_flag;
    uint8_t   weighted_bipred_flag;
    uint8_t   transquant_bypass_enabled_flag;
    uint8_t   tiles_enabled_flag;
    uint8_t   entropy_coding_sync_enabled_flag;
    uint32_t  num_tile_columns_minus1;
    uint32_t  num_tile_rows_minus1;
    uint8_t   uniform_spacing_flag;
    std::vector<uint32_t>  column_width_minus1;
    std::vector<uint32_t>  row_height_minus1;
    uint8_t   loop_filter_across_tiles_enabled_flag;
    uint8_t   pps_loop_filter_across_slices_enabled_flag;
    uint8_t   deblocking_filter_control_present_flag;
    uint8_t   deblocking_filter_override_enabled_flag;
    uint8_t   pps_deblocking_filter_disabled_flag;
    int32_t   pps_beta_offset_div2;
    int32_t   pps_tc_offset_div2;
    uint8_t   pps_scaling_list_data_present_flag;
    H265ScalingListDataSyntax::ptr scaling_list_data;
    uint8_t   lists_modification_present_flag;
    uint32_t  log2_parallel_merge_level_minus2;
    uint8_t   slice_segment_header_extension_present_flag;
    uint8_t   pps_extension_present_flag;
    uint8_t   pps_range_extension_flag;
    uint8_t   pps_multilayer_extension_flag;
    uint8_t   pps_3d_extension_flag;
    uint8_t   pps_scc_extension_flag;
    uint8_t   pps_extension_4bits;
    H265PpsRangeSyntax::ptr ppsRange;
    H265PpsMultilayerSyntax::ptr ppsMultilayer;
    H265Pps3dSyntax::ptr pps3d;
    H265PpsSccSyntax::ptr ppsScc;
    uint8_t   pps_extension_data_flag;
};

/**
 * @sa    ITU-T H.265 (2021) - F.7.3.2.3.4 Picture parameter set multilayer extension syntax
 * @todo  使用 std::unordered_map<uint8_t, int32_t> 代替 int32_t[1<<6u] 是否可行
 */
class H265PpsMultilayerExtensionSyntax
{
public:
    using ptr = std::shared_ptr<H265PpsMultilayerExtensionSyntax>;
public:
    H265PpsMultilayerExtensionSyntax();
    ~H265PpsMultilayerExtensionSyntax() = default;
public:
    uint8_t   poc_reset_info_present_flag;
    uint8_t   pps_infer_scaling_list_flag;
    uint8_t   pps_scaling_list_ref_layer_id;
    uint32_t  num_ref_loc_offsets;
    std::vector<uint8_t> ref_loc_offset_layer_id;
    std::vector<uint8_t> scaled_ref_layer_offset_present_flag;
    int32_t   scaled_ref_layer_left_offset[1<<6u];
    int32_t   scaled_ref_layer_top_offset[1<<6u];
    int32_t   scaled_ref_layer_right_offset[1<<6u];
    int32_t   scaled_ref_layer_bottom_offset[1<<6u];
    std::vector<uint8_t> ref_region_offset_present_flag;
    int32_t   ref_region_left_offset[1<<6u];
    int32_t   ref_region_top_offset[1<<6u];
    int32_t   ref_region_right_offset[1<<6u];
    int32_t   ref_region_bottom_offset[1<<6u];
    std::vector<uint8_t> resample_phase_set_present_flag;
    uint32_t  phase_hor_luma[1<<6u];
    uint32_t  phase_ver_luma[1<<6u];
    uint32_t  phase_hor_chroma_plus8[1<<6u];
    uint32_t  phase_ver_chroma_plus8[1<<6u];
    uint8_t   colour_mapping_enabled_flag;
    H265ColourMappingTable::ptr cmt;
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.6.2 Reference picture list modification syntax
 */
class H265RefPicListsModificationSyntax
{
public:
    using ptr = std::shared_ptr<H265RefPicListsModificationSyntax>;
public:
    H265RefPicListsModificationSyntax();
    ~H265RefPicListsModificationSyntax() = default;
public:
    uint8_t  ref_pic_list_modification_flag_l0;
    std::vector<uint32_t> list_entry_l0;
    uint8_t  ref_pic_list_modification_flag_l1;
    std::vector<uint32_t> list_entry_l1;
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.6.3 Weighted prediction parameters syntax
 */
class H265PredWeightTableSyntax
{
public:
    using ptr = std::shared_ptr<H265PredWeightTableSyntax>;
public:
    H265PredWeightTableSyntax();
    ~H265PredWeightTableSyntax() = default;
public:
    uint32_t luma_log2_weight_denom;
    int32_t  delta_chroma_log2_weight_denom;
    std::vector<uint8_t> luma_weight_l0_flag;
    std::vector<uint8_t> chroma_weight_l0_flag;
    std::vector<int32_t> delta_luma_weight_l0;
    std::vector<int32_t> luma_offset_l0;
    std::vector<std::vector<int32_t>> delta_chroma_weight_l0;
    std::vector<std::vector<int32_t>> delta_chroma_offset_l0;
    std::vector<uint8_t> luma_weight_l1_flag;
    std::vector<uint8_t> chroma_weight_l1_flag;
    std::vector<int32_t> delta_luma_weight_l1;
    std::vector<int32_t> luma_offset_l1;
    std::vector<std::vector<int32_t>> delta_chroma_weight_l1;
    std::vector<std::vector<int32_t>> delta_chroma_offset_l1;
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.6.1 General slice segment header syntax
 */
class H265SliceHeaderSyntax
{
public:
    using ptr = std::shared_ptr<H265SliceHeaderSyntax>;
public:
    H265SliceHeaderSyntax();
    ~H265SliceHeaderSyntax() = default;
public:
    uint8_t  first_slice_segment_in_pic_flag;
    uint8_t  no_output_of_prior_pics_flag;
    uint32_t slice_pic_parameter_set_id;
    uint8_t  dependent_slice_segment_flag;
    uint32_t slice_segment_address;
    std::vector<uint8_t>  slice_reserved_flag;
    uint32_t slice_type;
    uint8_t  pic_output_flag;
    uint8_t  colour_plane_id;
    uint32_t slice_pic_order_cnt_lsb;
    uint8_t  short_term_ref_pic_set_sps_flag;
    H265StRefPicSetSyntax::ptr stps;
    uint32_t short_term_ref_pic_set_idx;
    uint32_t num_long_term_sps;
    uint32_t num_long_term_pics;
    std::vector<uint32_t> lt_idx_sps;
    std::vector<uint32_t> poc_lsb_lt;
    std::vector<uint8_t> used_by_curr_pic_lt_flag;
    std::vector<uint8_t> delta_poc_msb_present_flag;
    std::vector<uint32_t> delta_poc_msb_cycle_lt;
    uint8_t  slice_temporal_mvp_enabled_flag;
    uint8_t  slice_sao_luma_flag;
    uint8_t  slice_sao_chroma_flag;
    uint8_t  num_ref_idx_active_override_flag;
    uint32_t num_ref_idx_l0_active_minus1;
    uint32_t num_ref_idx_l1_active_minus1;
    H265RefPicListsModificationSyntax::ptr rplm;
    uint8_t  mvd_l1_zero_flag;
    uint8_t  cabac_init_flag;
    uint8_t  collocated_from_l0_flag;
    uint32_t collocated_ref_idx;
    H265PredWeightTableSyntax::ptr pwt;
    uint32_t five_minus_max_num_merge_cand;
    uint8_t  use_integer_mv_flag;
    int32_t  slice_qp_delta;
    int32_t  slice_cb_qp_offset;
    int32_t  slice_cr_qp_offset;
    int32_t  slice_act_y_qp_offset;
    int32_t  slice_act_cb_qp_offset;
    int32_t  slice_act_cr_qp_offset;
    uint8_t  cu_chroma_qp_offset_enabled_flag;
    uint8_t  deblocking_filter_override_flag;
    uint8_t  slice_deblocking_filter_disabled_flag;
    int32_t  slice_beta_offset_div2;
    int32_t  slice_tc_offset_div2;
    uint8_t  slice_loop_filter_across_slices_enabled_flag;
    uint32_t num_entry_point_offsets;
    uint32_t offset_len_minus1;
    std::vector<uint32_t> entry_point_offset_minus1;
    uint32_t slice_segment_header_extension_length;
    std::vector<uint8_t> slice_segment_header_extension_data_byte;
};

/**
 * @sa ITU-T H.265 (2021) - 7.3.1.2 NAL unit header syntax
 */
class H265NalUnitHeaderSyntax
{
public:
    using ptr = std::shared_ptr<H265NalUnitHeaderSyntax>;
public:
    H265NalUnitHeaderSyntax();
    ~H265NalUnitHeaderSyntax() = default;
public:
    uint8_t  forbidden_zero_bit;
    uint8_t  nal_unit_type;
    uint8_t  nuh_layer_id;
    uint8_t  nuh_temporal_id_plus1;
};

class H265NalUnitSyntax
{
public:
    using ptr = std::shared_ptr<H265NalUnitSyntax>;
public:
    H265NalUnitSyntax() = default;
    ~H265NalUnitSyntax() = default;
public:
    H265NalUnitHeaderSyntax::ptr header;
    // TODO
};

class H265ContextSyntax
{
public:
    using ptr = std::shared_ptr<H265ContextSyntax>;
public:
    H265ContextSyntax();
    ~H265ContextSyntax() = default;
public:
    std::unordered_map<int32_t, H265VPSSyntax::ptr> vpsSet;
    std::unordered_map<int32_t, H265SpsSyntax::ptr> spsSet;
    std::unordered_map<int32_t, H265PpsSyntax::ptr> ppsSet;
};

} // namespace Codec
} // namespace Mmp