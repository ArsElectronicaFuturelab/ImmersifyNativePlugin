/**
 * \file    spinhevc.h
 * \brief   Spin Digital HEVC parameter set header file
 * \author  Chi Ching Chi <chi@spin-digital.com>
 *
 * Copyright/Rights of Use (Nutzungsrechte):
 * 2016-2018, Spin Digital Video Technologies GmbH
 * Berlin, Germany
 */

#ifndef __SPINHEVC_H__
#define __SPINHEVC_H__

/** \defgroup decoder SpinDec
 *  \brief Spin Digital HEVC parameter sets.
 *  @{
 */


#ifdef __cplusplus
extern "C" {
#endif



typedef struct SpinHEVC_RefPicSet
{
  unsigned char inter_ref_pic_set_prediction_flag;
  unsigned      delta_idx_minus1;
  unsigned char delta_rps_sign1;
  unsigned      abs_delta_rps_minus1;
  unsigned char used_by_curr_pic_flag[16];
  unsigned char use_delta_flag[16];
  unsigned      num_negative_pics;
  unsigned      num_positive_pics;
  unsigned      delta_poc_s0_minus1[16];
  unsigned char used_by_curr_pic_s0_flag[16];
  unsigned      delta_poc_s1_minus1[16];
  unsigned char used_by_curr_pic_s1_flag[16];
} SpinHEVC_RefPicSet;



typedef struct SpinHEVC_ProfileLevelTier
{
  unsigned char profile_space : 2;
  unsigned char tier_flag     : 1;
  unsigned char profile_idc   : 5;
  unsigned      profile_compatibility_flags : 32;
  unsigned char progressive_source_flag : 1;
  unsigned char interlaced_source_flag :1;
  unsigned char non_packed_constraint_flag :1;
  unsigned char frame_only_constraint_flag :1;
  unsigned char max_12bit_constraint_flag :1;
  unsigned char max_10bit_constraint_flag  :1;
  unsigned char max_8bit_constraint_flag :1;
  unsigned char max_422chroma_constraint_flag :1;
  unsigned char max_420chroma_constraint_flag :1;
  unsigned char max_monochrome_constraint_flag :1;
  unsigned char intra_constraint_flag :1;
  unsigned char one_picture_only_constraint_flag :1;
  unsigned char lower_bit_rate_constraint_flag :1;
//  unsigned char general_reserved_zero_35bits

  unsigned char level_idc;
} SpinHEVC_ProfileLevelTier ;



typedef struct SpinHEVC_SubLayerHRDParameters
{
  unsigned    bit_rate_value_minus1[32];
  unsigned    cpb_size_value_minus1[32];
  unsigned    cpb_size_du_value_minus1[32];
  unsigned    bit_rate_du_value_minus1[32];
  unsigned    cbr_flag[32];
} SpinHEVC_SubLayerHRDParameters;

typedef struct SpinHEVC_HRDParameters
{
  unsigned char nal_hrd_parameters_present_flag;
  unsigned char vcl_hrd_parameters_present_flag;
  unsigned char sub_pic_hrd_params_present_flag;
  unsigned char tick_divisor_minus2;
  unsigned char du_cpb_removal_delay_increment_length_minus1;
  unsigned char sub_pic_cpb_params_in_pic_timing_sei_flag;
  unsigned char dpb_output_delay_du_length_minus1;
  unsigned char bit_rate_scale;
  unsigned char cpb_size_scale;
  unsigned char cpb_size_du_scale;
  unsigned char initial_cpb_removal_delay_length_minus1;
  unsigned char au_cpb_removal_delay_length_minus1;
  unsigned char dpb_output_delay_length_minus1;

  unsigned char fixed_pic_rate_general_flag[7];
  unsigned char fixed_pic_rate_within_cvs_flag[7];
  unsigned      elemental_duration_in_tc_minus1[7];
  unsigned char low_delay_hrd_flag[7];
  unsigned      cpb_cnt_minus1[7];
  SpinHEVC_SubLayerHRDParameters sub_layer_nal_hrd_parameters[7];
  SpinHEVC_SubLayerHRDParameters sub_layer_vcl_hrd_parameters[7];

} SpinHEVC_HRDParameters;

typedef struct SpinHEVC_VPS
{
  unsigned char  vps_video_parameter_set_id;
  unsigned char  vps_base_layer_internal_flag;
  unsigned char  vps_base_layer_available_flag;
  unsigned char  vps_max_layers_minus1;
  unsigned char  vps_max_sub_layers_minus1;
  unsigned char  vps_temporal_id_nesting_flag;
  unsigned short vps_reserved_0xffff_16bits;
  SpinHEVC_ProfileLevelTier general;
  unsigned char sub_layer_profile_present_flag[7];
  unsigned char sub_layer_level_present_flag[7];
  SpinHEVC_ProfileLevelTier sub_layer[7];
  unsigned char  vps_sub_layer_ordering_info_present_flag;
  unsigned       vps_max_dec_pic_buffering_minus1[7];
  unsigned       vps_max_num_reorder_pics[7];
  unsigned       vps_max_latency_increase_plus1[7];
  unsigned char  vps_max_layer_id;
  unsigned       vps_num_layer_sets_minus1;

  unsigned long long layer_id_included_flag[1024]; //64 bits per set, 1 bit per layer starting with LSB.
  unsigned char  vps_timing_info_present_flag;
  unsigned int   vps_num_units_in_tick;
  unsigned int   vps_time_scale;
  unsigned char  vps_poc_proportional_to_timing_flag;
  unsigned       vps_num_ticks_poc_diff_one_minus1;

  unsigned       vps_num_hrd_parameters;
  unsigned short hrd_layer_set_idx[1024];
  unsigned char  cprms_present_flag[1024];
  SpinHEVC_HRDParameters hrd_parameters[4]; //only first 4 exposes of possible 1024 sets. Would otherwise be way too big.
} SpinHEVC_VPS;





typedef struct SpinHEVC_VUIParameters
{
  unsigned char aspect_ratio_info_present_flag;
  unsigned char aspect_ratio_idc;
  unsigned short sar_width;
  unsigned short sar_height;
  unsigned char overscan_info_present_flag;
  unsigned char overscan_appropriate_flag;
  unsigned char video_signal_type_present_flag;
  unsigned char video_format;
  unsigned char video_full_range_flag;
  unsigned char colour_description_present_flag;
  unsigned char colour_primaries;
  unsigned char transfer_characteristics;
  unsigned char matrix_coeffs;
  unsigned char chroma_loc_info_present_flag;
  unsigned      chroma_sample_loc_type_top_field;
  unsigned      chroma_sample_loc_type_bottom_field;
  unsigned char neutral_chroma_indication_flag;
  unsigned char field_seq_flag;
  unsigned char frame_field_info_present_flag;
  unsigned char default_display_window_flag;

  unsigned      def_disp_win_left_offset;
  unsigned      def_disp_win_right_offset;
  unsigned      def_disp_win_top_offset;
  unsigned      def_disp_win_bottom_offset;

  unsigned char vui_timing_info_present_flag;
  unsigned int  vui_num_units_in_tick;
  unsigned int  vui_time_scale;
  unsigned char vui_poc_proportional_to_timing_flag;
  unsigned      vui_num_ticks_poc_diff_one_minus1;
  unsigned char vui_hrd_parameters_present_flag;
  SpinHEVC_HRDParameters vui_hrd_parameters;

  unsigned char bitstream_restriction_flag;
  unsigned char tiles_fixed_structure_flag;
  unsigned char motion_vectors_over_pic_boundaries_flag;
  unsigned char restricted_ref_pic_lists_flag;
  unsigned      min_spatial_segmentation_idc;
  unsigned      max_bytes_per_pic_denom;
  unsigned      max_bits_per_min_cu_denom;
  unsigned      log2_max_mv_length_horizontal;
  unsigned      log2_max_mv_length_vertical;

} SpinHEVC_VUIParameters;



typedef struct SpinHEVC_SPS
{
  unsigned      sps_video_parameter_set_id;
  unsigned      sps_max_sub_layers_minus1;
  unsigned char sps_temporal_id_nesting_flag;

  SpinHEVC_ProfileLevelTier general;
  unsigned char sub_layer_profile_present_flag[7];
  unsigned char sub_layer_level_present_flag[7];
  SpinHEVC_ProfileLevelTier sub_layer[7];

  int           sps_seq_parameter_set_id;
  SE_ChromaFormat chroma_format_idc;
  unsigned char separate_colour_plane_flag;
  unsigned      pic_width_in_luma_samples;
  unsigned      pic_height_in_luma_samples;
  unsigned char conformance_window_flag;
  unsigned      conf_win_left_offset;
  unsigned      conf_win_right_offset;
  unsigned      conf_win_top_offset;
  unsigned      conf_win_bottom_offset;
  unsigned      bit_depth_luma_minus8;
  unsigned      bit_depth_chroma_minus8;
  unsigned      log2_max_pic_order_cnt_lsb_minus4;
  unsigned char sps_sub_layer_ordering_info_present_flag;
  unsigned      sps_max_dec_pic_bufferingminus1[7];
  unsigned      sps_max_num_reorder_pics[7];
  unsigned      sps_max_latency_increase_plus1[7];

  unsigned      log2_min_luma_coding_block_size_minus3;
  unsigned      log2_diff_max_min_luma_coding_block_size;
  unsigned      log2_min_luma_transform_block_size_minus2;
  unsigned      log2_diff_max_min_luma_transform_block_size;
  unsigned      max_transform_hierarchy_depth_inter;
  unsigned      max_transform_hierarchy_depth_intra;
  unsigned char scaling_list_enabled_flag;
  unsigned char sps_scaling_list_data_present_flag;
  int           sps_scaling_list_4x4[6][16];
  int           sps_scaling_list_8x8[6][64];
  int           sps_scaling_list_16x16[6][64]; // list can have maximum of 64 unique elements.
  int           sps_scaling_list_32x32[6][64];

  unsigned char amp_enabled_flag;
  unsigned char sample_adaptive_offset_enabled_flag;
  unsigned char pcm_enabled_flag;

  unsigned      pcm_sample_bit_depth_luma_minus1;
  unsigned      pcm_sample_bit_depth_chroma_minus1;
  unsigned      log2_min_pcm_luma_coding_block_size_minus3;
  unsigned      log2_diff_max_min_pcm_luma_coding_block_size;
  unsigned char pcm_loop_filter_disabled_flag;

  unsigned      num_short_term_ref_pic_sets;
  SpinHEVC_RefPicSet ref_pic_set[64];
  unsigned char long_term_ref_pics_present_flag;
  unsigned      num_long_term_ref_pics_sps;
  unsigned      lt_ref_pic_poc_lsb_sps[32];
  unsigned char used_by_curr_pic_lt_sps_flag[32];

  unsigned char sps_temporal_mvp_enabled_flag;
  unsigned char strong_intra_smoothing_enabled_flag;
  unsigned char vui_parameters_present_flag;
  SpinHEVC_VUIParameters vui_parameters;

  unsigned char sps_extension_present_flag;
  unsigned char sps_range_extensions_flag;
  unsigned char transform_skip_rotation_enabled_flag;
  unsigned char transform_skip_context_enabled_flag;
  unsigned char implicit_rdpcm_enabled_flag;
  unsigned char explicit_rdpcm_enabled_flag;
  unsigned char extended_precision_processing_flag;
  unsigned char intra_smoothing_disabled_flag;
  unsigned char high_precision_offsets_enabled_flag;
  unsigned char persistent_rice_adaptation_enabled_flag;
  unsigned char cabac_bypass_alignment_enabled_flag;

} SpinHEVC_SPS;


typedef struct SpinHEVC_PPS
{
  unsigned      pps_pic_parameter_set_id;
  unsigned      pps_seq_parameter_set_id;
  unsigned char dependent_slice_segments_enabled_flag;
  unsigned char output_flag_present_flag;
  unsigned char num_extra_slice_header_bits;
  unsigned char sign_data_hiding_enabled_flag;
  unsigned char cabac_init_present_flag;
  unsigned      num_ref_idx_l0_default_active_minus1;
  unsigned      num_ref_idx_l1_default_active_minus1;
  int           init_qp_minus26;
  unsigned char constrained_intra_pred_flag;
  unsigned char transform_skip_enabled_flag;
  unsigned char cu_qp_delta_enabled_flag;
  unsigned      diff_cu_qp_delta_depth;
  int           pps_cb_qp_offset;
  int           pps_cr_qp_offset;
  unsigned char pps_slice_chroma_qp_offsets_present_flag;
  unsigned char weighted_pred_flag;
  unsigned char weighted_bipred_flag;
  unsigned char transquant_bypass_enabled_flag;

  unsigned char tiles_enabled_flag;
  unsigned char entropy_coding_sync_enabled_flag;
  unsigned      num_tile_columns_minus1;
  unsigned      num_tile_rows_minus1;
  unsigned char uniform_spacing_flag;
  unsigned short column_width_minus1[256];
  unsigned short row_height_minus1[512];
  unsigned char loop_filter_across_tiles_enabled_flag;

  unsigned char pps_loop_filter_across_slices_enabled_flag;
  unsigned char deblocking_filter_control_present_flag;
  unsigned char deblocking_filter_override_enabled_flag;
  unsigned char pps_deblocking_filter_disabled_flag;
  int           pps_beta_offset_div2;
  int           pps_tc_offset_div2;

  unsigned char pps_scaling_list_data_present_flag;
  int           scaling_list_4x4[6][16];
  int           scaling_list_8x8[6][64];
  int           scaling_list_16x16[6][64]; // list can have maximum of 64 unique elements.
  int           scaling_list_32x32[6][64];
  unsigned char lists_modification_present_flag;

  unsigned      log2_parallel_merge_level_minus2;
  unsigned char slice_segment_header_extension_present_flag;

  unsigned char pps_range_extension_flag;
  unsigned      log2_max_transform_skip_block_size_minus2;
  unsigned char cross_component_prediction_enabled_flag;
  unsigned char chroma_qp_offset_list_enabled_flag;
  unsigned      diff_cu_chroma_qp_offset_depth;
  unsigned      chroma_qp_offset_list_len_minus1;
  unsigned char cb_qp_offset_list[7];
  unsigned char cr_qp_offset_list[7];
  unsigned      log2_sao_offset_scale_luma;
  unsigned      log2_sao_offset_scale_chroma;

  unsigned char pps_multilayer_extension_flag;
  unsigned char pps_3d_extension_flag;
  unsigned char pps_scc_extension_flag;

} SpinHEVC_PPS;




#ifdef __cplusplus
}
#endif
#endif

/** @}*/

