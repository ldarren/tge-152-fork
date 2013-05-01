#ifndef __glext_h_
#define __glext_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
** Copyright 1992-1999 Silicon Graphics, Inc.
** All Rights Reserved.
**
** This is UNPUBLISHED PROPRIETARY SOURCE CODE of Silicon Graphics, Inc.;
** the contents of this file may not be disclosed to third parties, copied or
** duplicated in any form, in whole or in part, without the prior written
** permission of Silicon Graphics, Inc.
**
** RESTRICTED RIGHTS LEGEND:
** Use, duplication or disclosure by the Government is subject to restrictions
** as set forth in subdivision (c)(1)(ii) of the Rights in Technical Data
** and Computer Software clause at DFARS 252.227-7013, and/or in similar or
** successor clauses in the FAR, DOD or NASA FAR Supplement. Unpublished -
** rights reserved under the Copyright Laws of the United States.
*/

#ifndef APIENTRY
#	ifdef _WIN32
#		define WIN32_LEAN_AND_MEAN 1
#		include <windows.h>
#	else
#		define APIENTRY
#	endif
#endif

//#define GL_GLEXT_PROTOTYPES 1

#define GL_EXT_abgr                         1
#define GL_EXT_bgra                         1
#define GL_EXT_clip_volume_hint             1
#define GL_EXT_compiled_vertex_array        1
#define GL_ARB_multisample					1
#define GL_EXT_packed_pixels                1
#define GL_EXT_rescale_normal				1
#define GL_EXT_separate_specular_color		1
#define GL_EXT_stencil_wrap                 1
#define GL_WIN_swap_hint                    1
#define GL_ARB_texture_cube_map				1
#define GL_EXT_texture_env_add              1
#define GL_EXT_texture_env_combine          1
#define GL_EXT_texture_filter_anisotropic	1
#define GL_NV_texgen_emboss					1
#define GL_NV_texgen_reflection             1
#define GL_NV_texture_env_combine4          1

#ifndef GL_VERSION_1_2
	#define GL_CLAMP_TO_EDGE					0x812F
#endif

/* EXT_abgr */
#ifndef GL_ABGR_EXT
	#define GL_ABGR_EXT                         0x8000
#endif

/* EXT_blend_color */
#ifndef GL_CONSTANT_COLOR
	#define GL_CONSTANT_COLOR                   0x8001
	#define GL_CONSTANT_COLOR_EXT               0x8001
	#define GL_ONE_MINUS_CONSTANT_COLOR         0x8002
	#define GL_ONE_MINUS_CONSTANT_COLOR_EXT     0x8002
	#define GL_CONSTANT_ALPHA                   0x8003
	#define GL_CONSTANT_ALPHA_EXT               0x8003
	#define GL_ONE_MINUS_CONSTANT_ALPHA         0x8004
	#define GL_ONE_MINUS_CONSTANT_ALPHA_EXT     0x8004
	#define GL_BLEND_COLOR                      0x8005
	#define GL_BLEND_COLOR_EXT                  0x8005
#endif

/* EXT_blend_minmax */
#ifndef GL_FUNC_ADD
	#define GL_FUNC_ADD                         0x8006
	#define GL_FUNC_ADD_EXT                     0x8006
	#define GL_MIN                              0x8007
	#define GL_MIN_EXT                          0x8007
	#define GL_MAX                              0x8008
	#define GL_MAX_EXT                          0x8008
	#define GL_BLEND_EQUATION                   0x8009
	#define GL_BLEND_EQUATION_EXT               0x8009
#endif

/* EXT_blend_subtract */
#ifndef GL_FUNC_SUBTRACT
	#define GL_FUNC_SUBTRACT                    0x800A
	#define GL_FUNC_SUBTRACT_EXT                0x800A
	#define GL_FUNC_REVERSE_SUBTRACT            0x800B
	#define GL_FUNC_REVERSE_SUBTRACT_EXT        0x800B
#endif

/* EXT_packed_pixels */
#ifndef GL_UNSIGNED_BYTE_3_3_2_EXT
	#define GL_UNSIGNED_BYTE_3_3_2_EXT          0x8032
	#define GL_UNSIGNED_SHORT_4_4_4_4_EXT       0x8033
	#define GL_UNSIGNED_SHORT_5_5_5_1_EXT       0x8034
	#define GL_UNSIGNED_INT_8_8_8_8_EXT         0x8035
	#define GL_UNSIGNED_INT_10_10_10_2_EXT      0x8036
#endif	

/* GL_EXT_rescale_normal */
#ifndef GL_RESCALE_NORMAL_EXT
	#define GL_RESCALE_NORMAL_EXT				0x803A
#endif

/* GL_EXT_texture3D */
#ifndef GL_PACK_SKIP_IMAGES               
	#define GL_PACK_SKIP_IMAGES						0x806B
	#define GL_PACK_SKIP_IMAGES_EXT					0x806B
	#define GL_PACK_IMAGE_HEIGHT					0x806C
	#define GL_PACK_IMAGE_HEIGHT_EXT				0x806C
	#define GL_UNPACK_SKIP_IMAGES					0x806D
	#define GL_UNPACK_SKIP_IMAGES_EXT				0x806D
	#define GL_UNPACK_IMAGE_HEIGHT					0x806E
	#define GL_UNPACK_IMAGE_HEIGHT_EXT				0x806E
	#define GL_TEXTURE_3D							0x806F
	#define GL_TEXTURE_3D_EXT						0x806F
	#define GL_PROXY_TEXTURE_3D						0x8070
	#define GL_PROXY_TEXTURE_3D_EXT					0x8070
	#define GL_TEXTURE_DEPTH						0x8071
	#define GL_TEXTURE_DEPTH_EXT					0x8071
	#define GL_TEXTURE_WRAP_R						0x8072
	#define GL_TEXTURE_WRAP_R_EXT					0x8072
	#define GL_MAX_3D_TEXTURE_SIZE					0x8073
	#define GL_MAX_3D_TEXTURE_SIZE_EXT				0x8073
#endif

/* EXT_vertex_array */
#ifndef GL_VERTEX_ARRAY_EXT
	#define GL_VERTEX_ARRAY_EXT                 0x8074
	#define GL_NORMAL_ARRAY_EXT                 0x8075
	#define GL_COLOR_ARRAY_EXT                  0x8076
	#define GL_INDEX_ARRAY_EXT                  0x8077
	#define GL_TEXTURE_COORD_ARRAY_EXT          0x8078
	#define GL_EDGE_FLAG_ARRAY_EXT              0x8079
	#define GL_VERTEX_ARRAY_SIZE_EXT            0x807A
	#define GL_VERTEX_ARRAY_TYPE_EXT            0x807B
	#define GL_VERTEX_ARRAY_STRIDE_EXT          0x807C
	#define GL_VERTEX_ARRAY_COUNT_EXT           0x807D
	#define GL_NORMAL_ARRAY_TYPE_EXT            0x807E
	#define GL_NORMAL_ARRAY_STRIDE_EXT          0x807F
	#define GL_NORMAL_ARRAY_COUNT_EXT           0x8080
	#define GL_COLOR_ARRAY_SIZE_EXT             0x8081
	#define GL_COLOR_ARRAY_TYPE_EXT             0x8082
	#define GL_COLOR_ARRAY_STRIDE_EXT           0x8083
	#define GL_COLOR_ARRAY_COUNT_EXT            0x8084
	#define GL_INDEX_ARRAY_TYPE_EXT             0x8085
	#define GL_INDEX_ARRAY_STRIDE_EXT           0x8086
	#define GL_INDEX_ARRAY_COUNT_EXT            0x8087
	#define GL_TEXTURE_COORD_ARRAY_SIZE_EXT     0x8088
	#define GL_TEXTURE_COORD_ARRAY_TYPE_EXT     0x8089
	#define GL_TEXTURE_COORD_ARRAY_STRIDE_EXT   0x808A
	#define GL_TEXTURE_COORD_ARRAY_COUNT_EXT    0x808B
	#define GL_EDGE_FLAG_ARRAY_STRIDE_EXT       0x808C
	#define GL_EDGE_FLAG_ARRAY_COUNT_EXT        0x808D
	#define GL_VERTEX_ARRAY_POINTER_EXT         0x808E
	#define GL_NORMAL_ARRAY_POINTER_EXT         0x808F
	#define GL_COLOR_ARRAY_POINTER_EXT          0x8090
	#define GL_INDEX_ARRAY_POINTER_EXT          0x8091
	#define GL_TEXTURE_COORD_ARRAY_POINTER_EXT  0x8092
	#define GL_EDGE_FLAG_ARRAY_POINTER_EXT      0x8093
#endif

/* GL_ARB_multisample */
#ifndef GL_ARB_multisample
	#define GL_MULTISAMPLE_ARB					0x809D
	#define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB		0x809E
	#define GL_SAMPLE_ALPHA_TO_ONE_ARB			0x809F
	#define GL_SAMPLE_COVERAGE_ARB				0x80A0
	#define GL_SAMPLE_BUFFERS_ARB				0x80A8
	#define GL_SAMPLES_ARB						0x80A9
	#define GL_SAMPLE_COVERAGE_VALUE_ARB		0x80AA
	#define GL_SAMPLE_COVERAGE_INVERT_ARB		0x80AB
	#define GL_MULTISAMPLE_BIT_ARB				0x20000000
#endif

/* EXT_bgra */
#ifndef GL_BGR_EXT
	#define GL_BGR_EXT                          0x80E0
	#define GL_BGRA_EXT                         0x80E1
#endif

/* EXT_clip_volume_hint */
#ifndef GL_CLIP_VOLUME_CLIPPING_HINT_EXT
	#define GL_CLIP_VOLUME_CLIPPING_HINT_EXT    0x80F0
#endif

/* EXT_point_parameters */
#ifndef GL_POINT_SIZE_MIN_EXT
	#define GL_POINT_SIZE_MIN_EXT               0x8126
	#define GL_POINT_SIZE_MAX_EXT               0x8127
	#define GL_POINT_FADE_THRESHOLD_SIZE_EXT    0x8128
	#define GL_DISTANCE_ATTENUATION_EXT         0x8129
#endif

/* SGIX_shadow - needs SGIX_depth_texture */
#ifndef GL_SGIX_shadow
	#define GL_SGIX_shadow						1
	#define GL_TEXTURE_COMPARE_SGIX				0x819A
	#define GL_TEXTURE_COMPARE_OPERATOR_SGIX	0x819B
	#define GL_TEXTURE_LEQUAL_R_SGIX			0x819C
	#define GL_TEXTURE_GEQUAL_R_SGIX			0x819D
#endif

/* SGIX_depth_texture */
#ifndef GL_SGIX_depth_texture
	#define GL_SGIX_depth_texture				1
	#define GL_DEPTH_COMPONENT16_SGIX			0x81A5
	#define GL_DEPTH_COMPONENT24_SGIX			0x81A6
	#define GL_DEPTH_COMPONENT32_SGIX			0x81A7
#endif

#ifndef GL_CLAMP_TO_BORDER_SGIS
	#define GL_CLAMP_TO_BORDER_SGIS            0x812D
#endif
/* GL_ARB_texture_border_clamp, replaces SGIS version */
#ifndef GL_CLAMP_TO_BORDER_ARB
	#define GL_CLAMP_TO_BORDER_ARB             0x812D
#endif

/* SGIS_texture_edge_clamp */
#ifndef GL_SGIS_texture_edge_clamp
	#define GL_SGIS_texture_edge_clamp			1
	#define GL_CLAMP_TO_EDGE_SGIS				0x812F
	#define GL_CLAMP_TO_EDGE_EXT				0x812F
#endif

/* EXT_compiled_vertex_array */
#ifndef GL_ARRAY_ELEMENT_LOCK_FIRST_EXT
	#define GL_ARRAY_ELEMENT_LOCK_FIRST_EXT     0x81A8
	#define GL_ARRAY_ELEMENT_LOCK_COUNT_EXT     0x81A9
#endif

/* EXT_cull_vertex */
#ifndef GL_CULL_VERTEX_EXT
	#define GL_CULL_VERTEX_EXT                  0x81AA
	#define GL_CULL_VERTEX_EYE_POSITION_EXT     0x81AB
	#define GL_CULL_VERTEX_OBJECT_POSITION_EXT  0x81AC
#endif

/* GL_EXT_separate_specular_color 

   For OGL 1.2, these are no longer extensions
   (no _EXT at the end) but the numbers are the same. 
*/
#ifndef GL_SEPARATE_SPECULAR_COLOR_EXT
	#define GL_LIGHT_MODEL_COLOR_CONTROL_EXT	0x81F8
	#define GL_SINGLE_COLOR_EXT					0x81F9
	#define GL_SEPARATE_SPECULAR_COLOR_EXT		0x81FA
#endif 

#ifndef GL_IBM_TEXTURE_MIRRORED_REPEAT
	#define GL_IBM_TEXTURE_MIRRORED_REPEAT	1
	#define GL_MIRRORED_REPEAT_IBM				0x8370	
#endif

/* GL_EXT_texture_compression_s3tc */
#ifndef GL_COMPRESSED_RGB_S3TC_DXT1_EXT
	#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
	#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
	#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
	#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#endif

/* GL_EXT_fog_coord */
#ifndef GL_FOG_COORDINATE_SOURCE_EXT		
	#define GL_FOG_COORDINATE_SOURCE_EXT		0x8450
	#define GL_FOG_COORDINATE_EXT				0x8451
	#define GL_FRAGMENT_DEPTH_EXT				0x8452
	#define GL_CURRENT_FOG_COORDINATE_EXT		0x8453
	#define GL_FOG_COORDINATE_ARRAY_TYPE_EXT	0x8454
	#define GL_FOG_COORDINATE_ARRAY_STRIDE_EXT	0x8455
	#define GL_FOG_COORDINATE_ARRAY_POINTER_EXT 0x8456
	#define GL_FOG_COORDINATE_ARRAY_EXT			0x8457
#endif

/* GL_EXT_secondary_color */
#ifndef GL_COLOR_SUM_EXT
	#define GL_COLOR_SUM_EXT						0x8458
	#define GL_CURRENT_SECONDARY_COLOR_EXT			0x8459
	#define GL_SECONDARY_COLOR_ARRAY_SIZE_EXT		0x845A
	#define GL_SECONDARY_COLOR_ARRAY_TYPE_EXT		0x845B
	#define GL_SECONDARY_COLOR_ARRAY_STRIDE_EXT		0x845C
	#define GL_SECONDARY_COLOR_ARRAY_POINTER_EXT	0x845D
	#define GL_SECONDARY_COLOR_ARRAY_EXT			0x845E
#endif

/* ARB_multitexture */
#ifndef GL_ARB_multitexture
	#define GL_ACTIVE_TEXTURE_ARB               0x84E0
	#define GL_CLIENT_ACTIVE_TEXTURE_ARB        0x84E1
	#define GL_MAX_TEXTURE_UNITS_ARB            0x84E2
	#define GL_TEXTURE0_ARB                     0x84C0
	#define GL_TEXTURE1_ARB                     0x84C1
	#define GL_TEXTURE2_ARB                     0x84C2
	#define GL_TEXTURE3_ARB                     0x84C3
	#define GL_TEXTURE4_ARB                     0x84C4
	#define GL_TEXTURE5_ARB                     0x84C5
	#define GL_TEXTURE6_ARB                     0x84C6
	#define GL_TEXTURE7_ARB                     0x84C7
	#define GL_TEXTURE8_ARB                     0x84C8
	#define GL_TEXTURE9_ARB                     0x84C9
	#define GL_TEXTURE10_ARB                    0x84CA
	#define GL_TEXTURE11_ARB                    0x84CB
	#define GL_TEXTURE12_ARB                    0x84CC
	#define GL_TEXTURE13_ARB                    0x84CD
	#define GL_TEXTURE14_ARB                    0x84CE
	#define GL_TEXTURE15_ARB                    0x84CF
	#define GL_TEXTURE16_ARB                    0x84D0
	#define GL_TEXTURE17_ARB                    0x84D1
	#define GL_TEXTURE18_ARB                    0x84D2
	#define GL_TEXTURE19_ARB                    0x84D3
	#define GL_TEXTURE20_ARB                    0x84D4
	#define GL_TEXTURE21_ARB                    0x84D5
	#define GL_TEXTURE22_ARB                    0x84D6
	#define GL_TEXTURE23_ARB                    0x84D7
	#define GL_TEXTURE24_ARB                    0x84D8
	#define GL_TEXTURE25_ARB                    0x84D9
	#define GL_TEXTURE26_ARB                    0x84DA
	#define GL_TEXTURE27_ARB                    0x84DB
	#define GL_TEXTURE28_ARB                    0x84DC
	#define GL_TEXTURE29_ARB                    0x84DD
	#define GL_TEXTURE30_ARB                    0x84DE
	#define GL_TEXTURE31_ARB                    0x84DF
#endif

/* GL_ARB_transpose_matrix */
#ifndef GL_TRANSPOSE_MODELVIEW_MATRIX_ARB
	#define GL_TRANSPOSE_MODELVIEW_MATRIX_ARB	0x84E3
	#define GL_TRANSPOSE_PROJECTION_MATRIX_ARB	0x84E4
	#define GL_TRANSPOSE_TEXTURE_MATRIX_ARB		0x84E5
	#define GL_TRANSPOSE_COLOR_MATRIX_ARB		0x84E6
#endif

/* GL_NV_fence */
#ifndef GL_NV_fence
	#define GL_ALL_COMPLETED_NV               0x84F2
	#define GL_FENCE_STATUS_NV                0x84F3
	#define GL_FENCE_CONDITION_NV             0x84F4
#endif

/* GL_EXT_texture_filter_anisotropic */
#ifndef GL_TEXTURE_MAX_ANISOTROPY_EXT		
	#define GL_TEXTURE_MAX_ANISOTROPY_EXT		0x84FE
	#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT	0x84FF
#endif

/* GL_EXT_vertex_weighting */
#ifndef GL_MODELVIEW0_STACK_DEPTH_EXT	
	#define GL_MODELVIEW0_STACK_DEPTH_EXT		GL_MODELVIEW_STACK_DEPTH
	#define GL_MODELVIEW1_STACK_DEPTH_EXT		0x8502
	#define GL_MODELVIEW0_MATRIX_EXT			GL_MODELVIEW_MATRIX
	#define GL_MODELVIEW_MATRIX1_EXT			0x8506
	#define GL_VERTEX_WEIGHTING_EXT				0x8509
	#define GL_MODELVIEW0_EXT					GL_MODELVIEW
	#define GL_MODELVIEW1_EXT					0x850A
	#define GL_CURRENT_VERTEX_WEIGHT_EXT		0x850B
	#define GL_VERTEX_WEIGHT_ARRAY_EXT			0x850C
	#define GL_VERTEX_WEIGHT_ARRAY_SIZE_EXT		0x850D
	#define GL_VERTEX_WEIGHT_ARRAY_TYPE_EXT		0x850E
	#define GL_VERTEX_WEIGHT_ARRAY_STRIDE_EXT	0x850F
	#define GL_VERTEX_WEIGHT_ARRAY_POINTER_EXT	0x8510
#endif

/* GL_ARB_texture_compression */
#ifndef GL_COMPRESSED_ALPHA_ARB
	#define GL_COMPRESSED_ALPHA_ARB					0x84E9
	#define GL_COMPRESSED_LUMINANCE_ARB				0x84EA
	#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB		0x84EB
	#define GL_COMPRESSED_INTENSITY_ARB				0x84EC
	#define GL_COMPRESSED_RGB_ARB					0x84ED
	#define GL_COMPRESSED_RGBA_ARB					0x84EE
	#define GL_TEXTURE_COMPRESSION_HINT_ARB			0x84EF
	#define GL_TEXTURE_IMAGE_SIZE_ARB				0x86A0
	#define GL_TEXTURE_COMPRESSED_ARB				0x86A1
	#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB	0x86A2
	#define GL_COMPRESSED_TEXTURE_FORMATS_ARB		0x86A3
#endif

/* ARB_texture_env_dot3 */
#ifndef GL_ARB_texture_env_dot3
	#define GL_ARB_texture_env_dot3			  1
	#define GL_DOT3_RGB_ARB                   0x86AE
	#define GL_DOT3_RGB_EXT                   0x86AE
	#define GL_DOT3_RGBA_ARB                  0x86AF
	#define GL_DOT3_RGBA_EXT                  0x86AF
#endif

/* EXT_stencil_wrap */
#ifndef GL_INCR_WRAP_EXT
	#define GL_INCR_WRAP_EXT                    0x8507
	#define GL_DECR_WRAP_EXT                    0x8508
#endif

/* NV_texgen_reflection, or EXT_texgen_reflection or ARB_texgen_reflection */
#ifndef GL_NORMAL_MAP_NV
	#define GL_NORMAL_MAP_NV                    0x8511
	#define GL_REFLECTION_MAP_NV                0x8512
#endif
#ifndef GL_NORMAL_MAP_ARB
	#define GL_NORMAL_MAP_ARB						0x8511 /* NV == ARB */
	#define GL_REFLECTION_MAP_ARB					0x8512 /* NV == ARB */
#endif

/* GL_ARB_texture_cube_map */
#ifndef GL_TEXTURE_CUBE_MAP_ARB
	#define GL_TEXTURE_CUBE_MAP_ARB					0x8513
	#define GL_TEXTURE_BINDING_CUBE_MAP_ARB			0x8514
	#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB		0x8515
	#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB		0x8516
	#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB		0x8517
	#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB		0x8518
	#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB		0x8519
	#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB		0x851A
	#define GL_PROXY_TEXTURE_CUBE_MAP_ARB			0x851B
	#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB		0x851C
#endif

/* GL_NV_vertex_array_range */
#ifndef GL_VERTEX_ARRAY_RANGE_NV			
	#define GL_VERTEX_ARRAY_RANGE_NV			0x851D
	#define GL_VERTEX_ARRAY_RANGE_LENGTH_NV		0x851E
	#define GL_VERTEX_ARRAY_RANGE_VALID_NV		0x851F
	#define GL_MAX_VERTEX_ARRAY_RANGE_ELEMENT_NV 0x8520
	#define GL_VERTEX_ARRAY_RANGE_POINTER_NV	0x8521
#endif

#ifndef GL_NV_vertex_array_range2
	#define GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV 0x8533
#endif

/* GL_NV_register_combiners */
#ifndef GL_REGISTER_COMBINERS_NV
	#define GL_REGISTER_COMBINERS_NV          0x8522
	#define GL_VARIABLE_A_NV                  0x8523
	#define GL_VARIABLE_B_NV                  0x8524
	#define GL_VARIABLE_C_NV                  0x8525
	#define GL_VARIABLE_D_NV                  0x8526
	#define GL_VARIABLE_E_NV                  0x8527
	#define GL_VARIABLE_F_NV                  0x8528
	#define GL_VARIABLE_G_NV                  0x8529
	#define GL_CONSTANT_COLOR0_NV             0x852A
	#define GL_CONSTANT_COLOR1_NV             0x852B
	#define GL_PRIMARY_COLOR_NV               0x852C
	#define GL_SECONDARY_COLOR_NV             0x852D
	#define GL_SPARE0_NV                      0x852E
	#define GL_SPARE1_NV                      0x852F
	#define GL_DISCARD_NV                     0x8530
	#define GL_E_TIMES_F_NV                   0x8531
	#define GL_SPARE0_PLUS_SECONDARY_COLOR_NV 0x8532
	#define GL_UNSIGNED_IDENTITY_NV           0x8536
	#define GL_UNSIGNED_INVERT_NV             0x8537
	#define GL_EXPAND_NORMAL_NV               0x8538
	#define GL_EXPAND_NEGATE_NV               0x8539
	#define GL_HALF_BIAS_NORMAL_NV            0x853A
	#define GL_HALF_BIAS_NEGATE_NV            0x853B
	#define GL_SIGNED_IDENTITY_NV             0x853C
	#define GL_SIGNED_NEGATE_NV               0x853D
	#define GL_SCALE_BY_TWO_NV                0x853E
	#define GL_SCALE_BY_FOUR_NV               0x853F
	#define GL_SCALE_BY_ONE_HALF_NV           0x8540
	#define GL_BIAS_BY_NEGATIVE_ONE_HALF_NV   0x8541
	#define GL_COMBINER_INPUT_NV              0x8542
	#define GL_COMBINER_MAPPING_NV            0x8543
	#define GL_COMBINER_COMPONENT_USAGE_NV    0x8544
	#define GL_COMBINER_AB_DOT_PRODUCT_NV     0x8545
	#define GL_COMBINER_CD_DOT_PRODUCT_NV     0x8546
	#define GL_COMBINER_MUX_SUM_NV            0x8547
	#define GL_COMBINER_SCALE_NV              0x8548
	#define GL_COMBINER_BIAS_NV               0x8549
	#define GL_COMBINER_AB_OUTPUT_NV          0x854A
	#define GL_COMBINER_CD_OUTPUT_NV          0x854B
	#define GL_COMBINER_SUM_OUTPUT_NV         0x854C
	#define GL_MAX_GENERAL_COMBINERS_NV       0x854D
	#define GL_NUM_GENERAL_COMBINERS_NV       0x854E
	#define GL_COLOR_SUM_CLAMP_NV             0x854F
	#define GL_COMBINER0_NV                   0x8550
	#define GL_COMBINER1_NV                   0x8551
	#define GL_COMBINER2_NV                   0x8552
	#define GL_COMBINER3_NV                   0x8553
	#define GL_COMBINER4_NV                   0x8554
	#define GL_COMBINER5_NV                   0x8555
	#define GL_COMBINER6_NV                   0x8556
	#define GL_COMBINER7_NV                   0x8557
	/* reuse GL_TEXTURE0_ARB */
	/* reuse GL_TEXTURE1_ARB */
	/* reuse GL_ZERO */
	/* reuse GL_NONE */
	/* reuse GL_FOG */
#endif

/* GL_NV_texgen_emboss */
#ifndef GL_EMBOSS_LIGHT_NV
	#define GL_EMBOSS_LIGHT_NV					0x855D
	#define GL_EMBOSS_CONSTANT_NV				0x855E
	#define GL_EMBOSS_MAP_NV					0x855F
#endif

/* EXT_texture_env_combine */
#ifndef GL_COMBINE_EXT
	#define GL_COMBINE_EXT                      0x8570
	#define GL_COMBINE_RGB_EXT                  0x8571
	#define GL_COMBINE_ALPHA_EXT                0x8572
	#define GL_RGB_SCALE_EXT                    0x8573
	#define GL_ADD_SIGNED_EXT                   0x8574
	#define GL_INTERPOLATE_EXT                  0x8575
	#define GL_CONSTANT_EXT                     0x8576
	#define GL_PRIMARY_COLOR_EXT                0x8577
	#define GL_PREVIOUS_EXT                     0x8578
	#define GL_SOURCE0_RGB_EXT                  0x8580
	#define GL_SOURCE1_RGB_EXT                  0x8581
	#define GL_SOURCE2_RGB_EXT                  0x8582
	#define GL_SOURCE0_ALPHA_EXT                0x8588
	#define GL_SOURCE1_ALPHA_EXT                0x8589
	#define GL_SOURCE2_ALPHA_EXT                0x858A
	#define GL_OPERAND0_RGB_EXT                 0x8590
	#define GL_OPERAND1_RGB_EXT                 0x8591
	#define GL_OPERAND2_RGB_EXT                 0x8592
	#define GL_OPERAND0_ALPHA_EXT               0x8598
	#define GL_OPERAND1_ALPHA_EXT               0x8599
	#define GL_OPERAND2_ALPHA_EXT               0x859A
#endif

/* NV_texture_env_combine4 */
#ifndef GL_COMBINE4_NV
	#define GL_COMBINE4_NV                      0x8503
	#define GL_SOURCE3_RGB_NV                   0x8583
	#define GL_SOURCE3_ALPHA_NV                 0x858B
	#define GL_OPERAND3_RGB_NV                  0x8593
	#define GL_OPERAND3_ALPHA_NV                0x859B
#endif

/* NV_vertex_program */
#ifndef GL_VERTEX_PROGRAM_NV
	#define GL_VERTEX_PROGRAM_NV              0x8620
	#define GL_VERTEX_STATE_PROGRAM_NV        0x8621
	#define GL_ATTRIB_ARRAY_SIZE_NV           0x8623
	#define GL_ATTRIB_ARRAY_STRIDE_NV         0x8624
	#define GL_ATTRIB_ARRAY_TYPE_NV           0x8625
	#define GL_CURRENT_ATTRIB_NV              0x8626
	#define GL_PROGRAM_LENGTH_NV              0x8627
	#define GL_PROGRAM_STRING_NV              0x8628
	#define GL_MODELVIEW_PROJECTION_NV        0x8629
	#define GL_IDENTITY_NV                    0x862A
	#define GL_INVERSE_NV                     0x862B
	#define GL_TRANSPOSE_NV                   0x862C
	#define GL_INVERSE_TRANSPOSE_NV           0x862D
	#define GL_MAX_TRACK_MATRIX_STACK_DEPTH_NV 0x862E
	#define GL_MAX_TRACK_MATRICES_NV          0x862F
	#define GL_MATRIX0_NV                     0x8630
	#define GL_MATRIX1_NV                     0x8631
	#define GL_MATRIX2_NV                     0x8632
	#define GL_MATRIX3_NV                     0x8633
	#define GL_MATRIX4_NV                     0x8634
	#define GL_MATRIX5_NV                     0x8635
	#define GL_MATRIX6_NV                     0x8636
	#define GL_MATRIX7_NV                     0x8637
	#define GL_CURRENT_MATRIX_STACK_DEPTH_NV  0x8640
	#define GL_CURRENT_MATRIX_NV              0x8641
	#define GL_VERTEX_PROGRAM_POINT_SIZE_NV   0x8642
	#define GL_VERTEX_PROGRAM_TWO_SIDE_NV     0x8643
	#define GL_PROGRAM_PARAMETER_NV           0x8644
	#define GL_ATTRIB_ARRAY_POINTER_NV        0x8645
	#define GL_PROGRAM_TARGET_NV              0x8646
	#define GL_PROGRAM_RESIDENT_NV            0x8647
	#define GL_TRACK_MATRIX_NV                0x8648
	#define GL_TRACK_MATRIX_TRANSFORM_NV      0x8649
	#define GL_VERTEX_PROGRAM_BINDING_NV      0x864A
	#define GL_PROGRAM_ERROR_POSITION_NV      0x864B
	#define GL_VERTEX_ATTRIB_ARRAY0_NV        0x8650
	#define GL_VERTEX_ATTRIB_ARRAY1_NV        0x8651
	#define GL_VERTEX_ATTRIB_ARRAY2_NV        0x8652
	#define GL_VERTEX_ATTRIB_ARRAY3_NV        0x8653
	#define GL_VERTEX_ATTRIB_ARRAY4_NV        0x8654
	#define GL_VERTEX_ATTRIB_ARRAY5_NV        0x8655
	#define GL_VERTEX_ATTRIB_ARRAY6_NV        0x8656
	#define GL_VERTEX_ATTRIB_ARRAY7_NV        0x8657
	#define GL_VERTEX_ATTRIB_ARRAY8_NV        0x8658
	#define GL_VERTEX_ATTRIB_ARRAY9_NV        0x8659
	#define GL_VERTEX_ATTRIB_ARRAY10_NV       0x865A
	#define GL_VERTEX_ATTRIB_ARRAY11_NV       0x865B
	#define GL_VERTEX_ATTRIB_ARRAY12_NV       0x865C
	#define GL_VERTEX_ATTRIB_ARRAY13_NV       0x865D
	#define GL_VERTEX_ATTRIB_ARRAY14_NV       0x865E
	#define GL_VERTEX_ATTRIB_ARRAY15_NV       0x865F
	#define GL_MAP1_VERTEX_ATTRIB0_4_NV       0x8660
	#define GL_MAP1_VERTEX_ATTRIB1_4_NV       0x8661
	#define GL_MAP1_VERTEX_ATTRIB2_4_NV       0x8662
	#define GL_MAP1_VERTEX_ATTRIB3_4_NV       0x8663
	#define GL_MAP1_VERTEX_ATTRIB4_4_NV       0x8664
	#define GL_MAP1_VERTEX_ATTRIB5_4_NV       0x8665
	#define GL_MAP1_VERTEX_ATTRIB6_4_NV       0x8666
	#define GL_MAP1_VERTEX_ATTRIB7_4_NV       0x8667
	#define GL_MAP1_VERTEX_ATTRIB8_4_NV       0x8668
	#define GL_MAP1_VERTEX_ATTRIB9_4_NV       0x8669
	#define GL_MAP1_VERTEX_ATTRIB10_4_NV      0x866A
	#define GL_MAP1_VERTEX_ATTRIB11_4_NV      0x866B
	#define GL_MAP1_VERTEX_ATTRIB12_4_NV      0x866C
	#define GL_MAP1_VERTEX_ATTRIB13_4_NV      0x866D
	#define GL_MAP1_VERTEX_ATTRIB14_4_NV      0x866E
	#define GL_MAP1_VERTEX_ATTRIB15_4_NV      0x866F
	#define GL_MAP2_VERTEX_ATTRIB0_4_NV       0x8670
	#define GL_MAP2_VERTEX_ATTRIB1_4_NV       0x8671
	#define GL_MAP2_VERTEX_ATTRIB2_4_NV       0x8672
	#define GL_MAP2_VERTEX_ATTRIB3_4_NV       0x8673
	#define GL_MAP2_VERTEX_ATTRIB4_4_NV       0x8674
	#define GL_MAP2_VERTEX_ATTRIB5_4_NV       0x8675
	#define GL_MAP2_VERTEX_ATTRIB6_4_NV       0x8676
	#define GL_MAP2_VERTEX_ATTRIB7_4_NV       0x8677
	#define GL_MAP2_VERTEX_ATTRIB8_4_NV       0x8678
	#define GL_MAP2_VERTEX_ATTRIB9_4_NV       0x8679
	#define GL_MAP2_VERTEX_ATTRIB10_4_NV      0x867A
	#define GL_MAP2_VERTEX_ATTRIB11_4_NV      0x867B
	#define GL_MAP2_VERTEX_ATTRIB12_4_NV      0x867C
	#define GL_MAP2_VERTEX_ATTRIB13_4_NV      0x867D
	#define GL_MAP2_VERTEX_ATTRIB14_4_NV      0x867E
	#define GL_MAP2_VERTEX_ATTRIB15_4_NV      0x867F
#endif

/* NV_texture_shader */
#ifndef GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV 
	#define GL_RGBA_UNSIGNED_DOT_PRODUCT_MAPPING_NV 0x86D9
	#define GL_UNSIGNED_INT_S8_S8_8_8_NV      0x86DA
	#define GL_UNSIGNED_INT_S8_S8_8_8_REV_NV  0x86DB
	#define GL_DSDT_MAG_INTENSITY_NV          0x86DC
	#define GL_SHADER_CONSISTENT_NV           0x86DD
	#define GL_TEXTURE_SHADER_NV              0x86DE
	#define GL_SHADER_OPERATION_NV            0x86DF
	#define GL_CULL_MODES_NV                  0x86E0
	#define GL_OFFSET_TEXTURE_2D_MATRIX_NV    0x86E1
	#define GL_OFFSET_TEXTURE_2D_SCALE_NV     0x86E2
	#define GL_OFFSET_TEXTURE_2D_BIAS_NV      0x86E3
	#define GL_PREVIOUS_TEXTURE_INPUT_NV      0x86E4
	#define GL_CONST_EYE_NV                   0x86E5
	#define GL_PASS_THROUGH_NV                0x86E6
	#define GL_CULL_FRAGMENT_NV               0x86E7
	#define GL_OFFSET_TEXTURE_2D_NV           0x86E8
	#define GL_DEPENDENT_AR_TEXTURE_2D_NV     0x86E9
	#define GL_DEPENDENT_GB_TEXTURE_2D_NV     0x86EA
	#define GL_ISOTROPIC_BRDF_NV              0x86EB
	#define GL_DOT_PRODUCT_NV                 0x86EC
	#define GL_DOT_PRODUCT_DEPTH_REPLACE_NV   0x86ED
	#define GL_DOT_PRODUCT_TEXTURE_2D_NV      0x86EE
	#define GL_DOT_PRODUCT_TEXTURE_3D_NV      0x86EF
	#define GL_DOT_PRODUCT_TEXTURE_CUBE_MAP_NV 0x86F0
	#define GL_DOT_PRODUCT_DIFFUSE_CUBE_MAP_NV 0x86F1
	#define GL_DOT_PRODUCT_REFLECT_CUBE_MAP_NV 0x86F2
	#define GL_DOT_PRODUCT_CONST_EYE_REFLECT_CUBE_MAP_NV 0x86F3
	#define GL_HILO_NV                        0x86F4
	#define GL_DSDT_NV                        0x86F5
	#define GL_DSDT_MAG_NV                    0x86F6
	#define GL_DSDT_MAG_VIB_NV                0x86F7
	#define GL_HILO16_NV                      0x86F8
	#define GL_SIGNED_HILO_NV                 0x86F9
	#define GL_SIGNED_HILO16_NV               0x86FA
	#define GL_SIGNED_RGBA_NV                 0x86FB
	#define GL_SIGNED_RGBA8_NV                0x86FC
	#define GL_SIGNED_RGB_NV                  0x86FE
	#define GL_SIGNED_RGB8_NV                 0x86FF
	#define GL_SIGNED_LUMINANCE_NV            0x8701
	#define GL_SIGNED_LUMINANCE8_NV           0x8702
	#define GL_SIGNED_LUMINANCE_ALPHA_NV      0x8703
	#define GL_SIGNED_LUMINANCE8_ALPHA8_NV    0x8704
	#define GL_SIGNED_ALPHA_NV                0x8705
	#define GL_SIGNED_ALPHA8_NV               0x8706
	#define GL_SIGNED_INTENSITY_NV            0x8707
	#define GL_SIGNED_INTENSITY8_NV           0x8708
	#define GL_DSDT8_NV                       0x8709
	#define GL_DSDT8_MAG8_NV                  0x870A
	#define GL_DSDT8_MAG8_INTENSITY8_NV       0x870B
	#define GL_SIGNED_RGB_UNSIGNED_ALPHA_NV   0x870C
	#define GL_SIGNED_RGB8_UNSIGNED_ALPHA8_NV 0x870D
	#define GL_HI_SCALE_NV                    0x870E
	#define GL_LO_SCALE_NV                    0x870F
	#define GL_DS_SCALE_NV                    0x8710
	#define GL_DT_SCALE_NV                    0x8711
	#define GL_MAGNITUDE_SCALE_NV             0x8712
	#define GL_VIBRANCE_SCALE_NV              0x8713
	#define GL_HI_BIAS_NV                     0x8714
	#define GL_LO_BIAS_NV                     0x8715
	#define GL_DS_BIAS_NV                     0x8716
	#define GL_DT_BIAS_NV                     0x8717
	#define GL_MAGNITUDE_BIAS_NV              0x8718
	#define GL_VIBRANCE_BIAS_NV               0x8719
	#define GL_TEXTURE_BORDER_VALUES_NV       0x871A
	#define GL_TEXTURE_HI_SIZE_NV             0x871B
	#define GL_TEXTURE_LO_SIZE_NV             0x871C
	#define GL_TEXTURE_DS_SIZE_NV             0x871D
	#define GL_TEXTURE_DT_SIZE_NV             0x871E
	#define GL_TEXTURE_MAG_SIZE_NV            0x871F
#endif

#ifndef GL_ATI_texture_mirror_once
	#define GL_ATI_texture_mirror_once 1
	#define GL_MIRROR_CLAMP_ATI					0x8742
	#define GL_MIRROR_CLAMP_TO_EDGE_ATI			0x8743
#endif

/*************************************************************/

/* EXT_vertex_array */
#ifndef GL_EXT_vertex_array 
	#define GL_EXT_vertex_array                 1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glArrayElementEXT (GLint);
		extern void APIENTRY glColorPointerEXT (GLint, GLenum, GLsizei, GLsizei, const GLvoid *);
		extern void APIENTRY glDrawArraysEXT (GLenum, GLint, GLsizei);
		extern void APIENTRY glEdgeFlagPointerEXT (GLsizei, GLsizei, const GLboolean *);
		extern void APIENTRY glGetPointervEXT (GLenum, GLvoid* *);
		extern void APIENTRY glIndexPointerEXT (GLenum, GLsizei, GLsizei, const GLvoid *);
		extern void APIENTRY glNormalPointerEXT (GLenum, GLsizei, GLsizei, const GLvoid *);
		extern void APIENTRY glTexCoordPointerEXT (GLint, GLenum, GLsizei, GLsizei, const GLvoid *);
		extern void APIENTRY glVertexPointerEXT (GLint, GLenum, GLsizei, GLsizei, const GLvoid *);
	#else
		#define MAYA_BIND_VERTEX_ARRAY 1
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLARRAYELEMENTEXTPROC) (GLint i);
	typedef void (APIENTRY * PFNGLCOLORPOINTEREXTPROC) (GLint GLsizei, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
	typedef void (APIENTRY * PFNGLDRAWARRAYSEXTPROC) (GLenum mode, GLint first, GLsizei count);
	typedef void (APIENTRY * PFNGLEDGEFLAGPOINTEREXTPROC) (GLsizei stride, GLsizei count, const GLboolean *pointer);
	typedef void (APIENTRY * PFNGLGETPOINTERVEXTPROC) (GLenum pname, GLvoid* *params);
	typedef void (APIENTRY * PFNGLINDEXPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
	typedef void (APIENTRY * PFNGLNORMALPOINTEREXTPROC) (GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
	typedef void (APIENTRY * PFNGLTEXCOORDPOINTEREXTPROC) (GLint GLsizei, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
	typedef void (APIENTRY * PFNGLVERTEXPOINTEREXTPROC) (GLint GLsizei, GLenum type, GLsizei stride, GLsizei count, const GLvoid *pointer);
#endif	

/* ARB_multitexture */
#ifndef GL_ARB_multitexture
	#define GL_ARB_multitexture 1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glActiveTextureARB (GLenum);
		extern void APIENTRY glClientActiveTextureARB (GLenum);
		extern void APIENTRY glMultiTexCoord1dARB (GLenum, GLdouble);
		extern void APIENTRY glMultiTexCoord1dvARB (GLenum, const GLdouble *);
		extern void APIENTRY glMultiTexCoord1fARB (GLenum, GLfloat);
		extern void APIENTRY glMultiTexCoord1fvARB (GLenum, const GLfloat *);
		extern void APIENTRY glMultiTexCoord1iARB (GLenum, GLint);
		extern void APIENTRY glMultiTexCoord1ivARB (GLenum, const GLint *);
		extern void APIENTRY glMultiTexCoord1sARB (GLenum, GLshort);
		extern void APIENTRY glMultiTexCoord1svARB (GLenum, const GLshort *);
		extern void APIENTRY glMultiTexCoord2dARB (GLenum, GLdouble, GLdouble);
		extern void APIENTRY glMultiTexCoord2dvARB (GLenum, const GLdouble *);
		extern void APIENTRY glMultiTexCoord2fARB (GLenum, GLfloat, GLfloat);
		extern void APIENTRY glMultiTexCoord2fvARB (GLenum, const GLfloat *);
		extern void APIENTRY glMultiTexCoord2iARB (GLenum, GLint, GLint);
		extern void APIENTRY glMultiTexCoord2ivARB (GLenum, const GLint *);
		extern void APIENTRY glMultiTexCoord2sARB (GLenum, GLshort, GLshort);
		extern void APIENTRY glMultiTexCoord2svARB (GLenum, const GLshort *);
		extern void APIENTRY glMultiTexCoord3dARB (GLenum, GLdouble, GLdouble, GLdouble);
		extern void APIENTRY glMultiTexCoord3dvARB (GLenum, const GLdouble *);
		extern void APIENTRY glMultiTexCoord3fARB (GLenum, GLfloat, GLfloat, GLfloat);
		extern void APIENTRY glMultiTexCoord3fvARB (GLenum, const GLfloat *);
		extern void APIENTRY glMultiTexCoord3iARB (GLenum, GLint, GLint, GLint);
		extern void APIENTRY glMultiTexCoord3ivARB (GLenum, const GLint *);
		extern void APIENTRY glMultiTexCoord3sARB (GLenum, GLshort, GLshort, GLshort);
		extern void APIENTRY glMultiTexCoord3svARB (GLenum, const GLshort *);
		extern void APIENTRY glMultiTexCoord4dARB (GLenum, GLdouble, GLdouble, GLdouble, GLdouble);
		extern void APIENTRY glMultiTexCoord4dvARB (GLenum, const GLdouble *);
		extern void APIENTRY glMultiTexCoord4fARB (GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
		extern void APIENTRY glMultiTexCoord4fvARB (GLenum, const GLfloat *);
		extern void APIENTRY glMultiTexCoord4iARB (GLenum, GLint, GLint, GLint, GLint);
		extern void APIENTRY glMultiTexCoord4ivARB (GLenum, const GLint *);
		extern void APIENTRY glMultiTexCoord4sARB (GLenum, GLshort, GLshort, GLshort, GLshort);
		extern void APIENTRY glMultiTexCoord4svARB (GLenum, const GLshort *);
	#else
		#define MAYA_BIND_MULTITEXTURE 1
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLMULTITEXCOORD1DARBPROC) (GLenum target, GLdouble s);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD1DVARBPROC) (GLenum target, const GLdouble *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD1FARBPROC) (GLenum target, GLfloat s);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD1FVARBPROC) (GLenum target, const GLfloat *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD1IARBPROC) (GLenum target, GLint s);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD1IVARBPROC) (GLenum target, const GLint *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD1SARBPROC) (GLenum target, GLshort s);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD1SVARBPROC) (GLenum target, const GLshort *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD2DARBPROC) (GLenum target, GLdouble s, GLdouble t);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD2DVARBPROC) (GLenum target, const GLdouble *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD2FVARBPROC) (GLenum target, const GLfloat *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD2IARBPROC) (GLenum target, GLint s, GLint t);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD2IVARBPROC) (GLenum target, const GLint *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD2SARBPROC) (GLenum target, GLshort s, GLshort t);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD2SVARBPROC) (GLenum target, const GLshort *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD3DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD3DVARBPROC) (GLenum target, const GLdouble *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD3FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD3FVARBPROC) (GLenum target, const GLfloat *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD3IARBPROC) (GLenum target, GLint s, GLint t, GLint r);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD3IVARBPROC) (GLenum target, const GLint *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD3SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD3SVARBPROC) (GLenum target, const GLshort *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD4DARBPROC) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD4DVARBPROC) (GLenum target, const GLdouble *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD4FARBPROC) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD4FVARBPROC) (GLenum target, const GLfloat *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD4IARBPROC) (GLenum target, GLint s, GLint t, GLint r, GLint q);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD4IVARBPROC) (GLenum target, const GLint *v);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD4SARBPROC) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
	typedef void (APIENTRY * PFNGLMULTITEXCOORD4SVARBPROC) (GLenum target, const GLshort *v);
	typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);
	typedef void (APIENTRY * PFNGLCLIENTACTIVETEXTUREARBPROC) (GLenum target);
#endif

/* EXT_compiled_vertex_array */
#ifndef GL_EXT_compiled_vertex_array
	#define GL_EXT_compiled_vertex_array 1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glLockArraysEXT (GLint, GLsizei);
		extern void APIENTRY glUnlockArraysEXT (void);
	#else
		#define MAYA_BIND_COMPILED_VERTEX_ARRAY 1
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLLOCKARRAYSEXTPROC) (GLint first, GLsizei count);
	typedef void (APIENTRY * PFNGLUNLOCKARRAYSEXTPROC) (void);
#endif

/* EXT_cull_vertex */
#ifndef GL_EXT_cull_vertex
	#define GL_EXT_cull_vertex 1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glCullParameterdvEXT (GLenum, GLdouble *);
		extern void APIENTRY glCullParameterfvEXT (GLenum, GLfloat *);
	#else
		#define MAYA_BIND_CULL_VERTEX 1
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLCULLPARAMETERDVEXTPROC) (GLenum pname, GLdouble* params);
	typedef void (APIENTRY * PFNGLCULLPARAMETERFVEXTPROC) (GLenum pname, GLfloat* params);
#endif

/* WIN_swap_hint */
typedef void (APIENTRY * PFNGLADDSWAPHINTRECTWINPROC) (GLint x, GLint y, GLsizei width, GLsizei height);

/* EXT_point_parameter */
#ifndef GL_EXT_point_parameters
	#define GL_EXT_point_parameters 1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glPointParameterfEXT (GLenum, GLfloat);
		extern void APIENTRY glPointParameterfvEXT (GLenum, const GLfloat *);
		extern void APIENTRY glPointParameterfSGIS (GLenum, GLfloat);
		extern void APIENTRY glPointParameterfvSGIS (GLenum, const GLfloat *);
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLPOINTPARAMETERFEXTPROC) (GLenum pname, GLfloat param);
	typedef void (APIENTRY * PFNGLPOINTPARAMETERFVEXTPROC) (GLenum pname, const GLfloat *params);
#endif

/* GL_EXT_blend_minmax */
#ifndef GL_EXT_blend_minmax
	#define GL_EXT_blend_minmax 1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glBlendEquationEXT (GLenum);
	#else
		#define MAYA_BIND_BLEND_MINMAX 1
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLBLENDEQUATIONEXTPROC) (GLenum mode);
#endif

/* GL_ARB_transpose_matrix */
#ifndef GL_ARB_transpose_matrix
	#define GL_ARB_transpose_matrix 1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glLoadTransposeMatrixfARB (const GLfloat *);
		extern void APIENTRY glLoadTransposeMatrixdARB (const GLdouble *);
		extern void APIENTRY glMultTransposeMatrixfARB (const GLfloat *);
		extern void APIENTRY glMultTransposeMatrixdARB (const GLdouble *);
	#else
		#define MAYA_BIND_TRANSPOSE_MATRIX 1
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXFARBPROC) (const GLfloat *m);
	typedef void (APIENTRY * PFNGLLOADTRANSPOSEMATRIXDARBPROC) (const GLdouble *m);
	typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXFARBPROC) (const GLfloat *m);
	typedef void (APIENTRY * PFNGLMULTTRANSPOSEMATRIXDARBPROC) (const GLdouble *m);
#endif

/* GL_ARB_texture_compression */
#ifndef GL_ARB_texture_compression
	#define GL_ARB_texture_compression 1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glCompressedTexImage3DARB (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
		extern void APIENTRY glCompressedTexImage2DARB (GLenum, GLint, GLenum, GLsizei, GLsizei, GLint, GLsizei, const GLvoid *);
		extern void APIENTRY glCompressedTexImage1DARB (GLenum, GLint, GLenum, GLsizei, GLint, GLsizei, const GLvoid *);
		extern void APIENTRY glCompressedTexSubImage3DARB (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
		extern void APIENTRY glCompressedTexSubImage2DARB (GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLsizei, const GLvoid *);
		extern void APIENTRY glCompressedTexSubImage1DARB (GLenum, GLint, GLint, GLsizei, GLenum, GLsizei, const GLvoid *);
		extern void APIENTRY glGetCompressedTexImageARB (GLenum, GLint, void *);
	#else
		#define MAYA_BIND_TEXTURE_COMPRESSION 1
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE3DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *data);
	typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE2DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *data);
	typedef void (APIENTRY * PFNGLCOMPRESSEDTEXIMAGE1DARBPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data);
	typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data);
	typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data);
	typedef void (APIENTRY * PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC) (GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data);
	typedef void (APIENTRY * PFNGLGETCOMPRESSEDTEXIMAGEARBPROC) (GLenum target, GLint level, void *img);
#endif

/* GL_EXT_secondary_color */
#ifndef GL_EXT_secondary_color
	#define GL_EXT_secondary_color 1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glSecondaryColor3bEXT (GLbyte, GLbyte, GLbyte);
		extern void APIENTRY glSecondaryColor3bvEXT (const GLbyte *);
		extern void APIENTRY glSecondaryColor3dEXT (GLdouble, GLdouble, GLdouble);
		extern void APIENTRY glSecondaryColor3dvEXT (const GLdouble *);
		extern void APIENTRY glSecondaryColor3fEXT (GLfloat, GLfloat, GLfloat);
		extern void APIENTRY glSecondaryColor3fvEXT (const GLfloat *);
		extern void APIENTRY glSecondaryColor3iEXT (GLint, GLint, GLint);
		extern void APIENTRY glSecondaryColor3ivEXT (const GLint *);
		extern void APIENTRY glSecondaryColor3sEXT (GLshort, GLshort, GLshort);
		extern void APIENTRY glSecondaryColor3svEXT (const GLshort *);
		extern void APIENTRY glSecondaryColor3ubEXT (GLubyte, GLubyte, GLubyte);
		extern void APIENTRY glSecondaryColor3ubvEXT (const GLubyte *);
		extern void APIENTRY glSecondaryColor3uiEXT (GLuint, GLuint, GLuint);
		extern void APIENTRY glSecondaryColor3uivEXT (const GLuint *);
		extern void APIENTRY glSecondaryColor3usEXT (GLushort, GLushort, GLushort);
		extern void APIENTRY glSecondaryColor3usvEXT (const GLushort *);
		extern void APIENTRY glSecondaryColorPointerEXT (GLint, GLenum, GLsizei, GLvoid *);
	#else
		#define MAYA_BIND_SECONDARY_COLOR 1
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BEXTPROC) (GLbyte red, GLbyte green, GLbyte blue);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3BVEXTPROC) (const GLbyte *v);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DEXTPROC) (GLdouble red, GLdouble green, GLdouble blue);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3DVEXTPROC) (const GLdouble *v);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FEXTPROC) (GLfloat red, GLfloat green, GLfloat blue);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3FVEXTPROC) (const GLfloat *v);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IEXTPROC) (GLint red, GLint green, GLint blue);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3IVEXTPROC) (const GLint *v);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SEXTPROC) (GLshort red, GLshort green, GLshort blue);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3SVEXTPROC) (const GLshort *v);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBEXTPROC) (GLubyte red, GLubyte green, GLubyte blue);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UBVEXTPROC) (const GLubyte *v);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIEXTPROC) (GLuint red, GLuint green, GLuint blue);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3UIVEXTPROC) (const GLuint *v);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USEXTPROC) (GLushort red, GLushort green, GLushort blue);
	typedef void (APIENTRY * PFNGLSECONDARYCOLOR3USVEXTPROC) (const GLushort *v);
	typedef void (APIENTRY * PFNGLSECONDARYCOLORPOINTEREXTPROC) (GLint sizei, GLenum type, GLsizei stride, GLvoid *pointer);
#endif

/* GL_EXT_fog_coord */
#ifndef GL_EXT_fog_coord
	#define GL_EXT_fog_coord 1
	#define GL_EXT_fog_coord_pFunc				1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glFogCoordfEXT (GLfloat);
		extern void APIENTRY glFogCoordfvEXT (const GLfloat *);
		extern void APIENTRY glFogCoorddEXT (GLdouble);
		extern void APIENTRY glFogCoorddvEXT (const GLdouble *);
		extern void APIENTRY glFogCoordPointerEXT (GLenum, GLsizei, const GLvoid *);
	#else
		#define MAYA_BIND_FOG_COORD 1
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLFOGCOORDFEXTPROC) (GLfloat coord);
	typedef void (APIENTRY * PFNGLFOGCOORDFVEXTPROC) (const GLfloat *coord);
	typedef void (APIENTRY * PFNGLFOGCOORDDEXTPROC) (GLdouble coord);
	typedef void (APIENTRY * PFNGLFOGCOORDDVEXTPROC) (const GLdouble *coord);
	typedef void (APIENTRY * PFNGLFOGCOORDPOINTEREXTPROC) (GLenum type, GLsizei stride, const GLvoid *pointer);
#endif

/* GL_EXT_vertex_weighting */
#ifndef GL_EXT_vertex_weighting
	#define GL_EXT_vertex_weighting			1
	#define GL_EXT_vertex_weighting_pFunc	1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glVertexWeightfEXT (GLfloat);
		extern void APIENTRY glVertexWeightfvEXT (const GLfloat *);
		extern void APIENTRY glVertexWeightPointerEXT (GLsizei, GLenum, GLsizei, const GLvoid *);
	#else
		#define MAYA_BIND_VERTEX_WEIGHTING 1
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLVERTEXWEIGHTFEXTPROC) (GLfloat weight);
	typedef void (APIENTRY * PFNGLVERTEXWEIGHTFVEXTPROC) (const GLfloat *weight);
	typedef void (APIENTRY * PFNGLVERTEXWEIGHTPOINTEREXTPROC) (GLsizei sizei, GLenum type, GLsizei stride, const GLvoid *pointer);
#endif

/* GL_NV_vertex_array_range */
#ifndef GL_NV_vertex_array_range
	#define GL_NV_vertex_array_range 1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glFlushVertexArrayRangeNV (void);
		extern void APIENTRY glVertexArrayRangeNV (GLsizei, const GLvoid *);
	#else
		#define MAYA_BIND_NV_ARRAY_RANGE 1
	#endif 
	typedef void (APIENTRY * PFNGLFLUSHVERTEXARRAYRANGENVPROC) (void);
	typedef void (APIENTRY * PFNGLVERTEXARRAYRANGENVPROC) (GLsizei GLsizei, const GLvoid *pointer);
#endif

/* GL_EXT_texture3D */
#ifndef GL_EXT_texture3D 
	#define GL_EXT_texture3D 1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glTexImage3DEXT (GLenum, GLint, GLenum, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *);
		extern void APIENTRY glTexSubImage3DEXT (GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, const GLvoid *);
	#else
		#define MAYA_BIND_TEXTURE3D 1
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLTEXIMAGE3DEXTPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
	typedef void (APIENTRY * PFNGLTEXSUBIMAGE3DEXTPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
#endif

/* GL_ARB_multisample */
#ifndef GL_ARB_multisample
	#define GL_ARB_multisample 1
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glSampleCoverageARB (GLclampf, GLboolean);
		extern void APIENTRY glSamplePassARB (GLenum);
	#else
		#define MAYA_BIND_MULTISAMPLE 1
	#endif /* GL_GLEXT_PROTOTYPES */
	typedef void (APIENTRY * PFNGLSAMPLECOVERAGEARBPROC) (GLclampf value, GLboolean invert);
	typedef void (APIENTRY * PFNGLSAMPLEPASSARBPROC) (GLenum pass);
#endif

/* NV_vertex_program */
#ifndef GL_NV_vertex_program
	#define GL_NV_vertex_program 1
	typedef GLboolean (APIENTRY * PFNGLAREPROGRAMSRESIDENTNVPROC) (GLsizei n, const GLuint *programs, GLboolean *residences);
	typedef void (APIENTRY * PFNGLBINDPROGRAMNVPROC) (GLenum target, GLuint id);
	typedef void (APIENTRY * PFNGLDELETEPROGRAMSNVPROC) (GLsizei n, const GLuint *programs);
	typedef void (APIENTRY * PFNGLEXECUTEPROGRAMNVPROC) (GLenum target, GLuint id, const GLfloat *params);
	typedef void (APIENTRY * PFNGLGENPROGRAMSNVPROC) (GLsizei n, GLuint *programs);
	typedef void (APIENTRY * PFNGLGETPROGRAMPARAMETERDVNVPROC) (GLenum target, GLuint index, GLenum pname, GLdouble *params);
	typedef void (APIENTRY * PFNGLGETPROGRAMPARAMETERFVNVPROC) (GLenum target, GLuint index, GLenum pname, GLfloat *params);
	typedef void (APIENTRY * PFNGLGETPROGRAMIVNVPROC) (GLuint id, GLenum pname, GLint *params);
	typedef void (APIENTRY * PFNGLGETPROGRAMSTRINGNVPROC) (GLuint id, GLenum pname, GLubyte *program);
	typedef void (APIENTRY * PFNGLGETTRACKMATRIXNVPROC) (GLenum target, GLuint address, GLenum pname, GLint *params);
	typedef void (APIENTRY * PFNGLGETVERTEXATTRIBDVNVPROC) (GLuint index, GLenum pname, GLdouble *params);
	typedef void (APIENTRY * PFNGLGETVERTEXATTRIBFVNVPROC) (GLuint index, GLenum pname, GLfloat *params);
	typedef void (APIENTRY * PFNGLGETVERTEXATTRIBIVNVPROC) (GLuint index, GLenum pname, GLint *params);
	typedef void (APIENTRY * PFNGLGETVERTEXATTRIBPOINTERNVPROC) (GLuint index, GLenum pname, GLvoid* *pointer);
	typedef GLboolean (APIENTRY * PFNGLISPROGRAMNVPROC) (GLuint id);
	typedef void (APIENTRY * PFNGLLOADPROGRAMNVPROC) (GLenum target, GLuint id, GLsizei len, const GLubyte *program);
	typedef void (APIENTRY * PFNGLPROGRAMPARAMETER4DNVPROC) (GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	typedef void (APIENTRY * PFNGLPROGRAMPARAMETER4DVNVPROC) (GLenum target, GLuint index, const GLdouble *v);
	typedef void (APIENTRY * PFNGLPROGRAMPARAMETER4FNVPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	typedef void (APIENTRY * PFNGLPROGRAMPARAMETER4FVNVPROC) (GLenum target, GLuint index, const GLfloat *v);
	typedef void (APIENTRY * PFNGLPROGRAMPARAMETERS4DVNVPROC) (GLenum target, GLuint index, GLsizei count, const GLdouble *v);
	typedef void (APIENTRY * PFNGLPROGRAMPARAMETERS4FVNVPROC) (GLenum target, GLuint index, GLsizei count, const GLfloat *v);
	typedef void (APIENTRY * PFNGLREQUESTRESIDENTPROGRAMSNVPROC) (GLsizei n, const GLuint *programs);
	typedef void (APIENTRY * PFNGLTRACKMATRIXNVPROC) (GLenum target, GLuint address, GLenum matrix, GLenum transform);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBPOINTERNVPROC) (GLuint index, GLint fsize, GLenum type, GLsizei stride, const GLvoid *pointer);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB1DNVPROC) (GLuint index, GLdouble x);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB1DVNVPROC) (GLuint index, const GLdouble *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB1FNVPROC) (GLuint index, GLfloat x);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB1FVNVPROC) (GLuint index, const GLfloat *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB1SNVPROC) (GLuint index, GLshort x);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB1SVNVPROC) (GLuint index, const GLshort *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB2DNVPROC) (GLuint index, GLdouble x, GLdouble y);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB2DVNVPROC) (GLuint index, const GLdouble *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB2FNVPROC) (GLuint index, GLfloat x, GLfloat y);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB2FVNVPROC) (GLuint index, const GLfloat *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB2SNVPROC) (GLuint index, GLshort x, GLshort y);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB2SVNVPROC) (GLuint index, const GLshort *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB3DNVPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB3DVNVPROC) (GLuint index, const GLdouble *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB3FNVPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB3FVNVPROC) (GLuint index, const GLfloat *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB3SNVPROC) (GLuint index, GLshort x, GLshort y, GLshort z);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB3SVNVPROC) (GLuint index, const GLshort *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB4DNVPROC) (GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB4DVNVPROC) (GLuint index, const GLdouble *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB4FNVPROC) (GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB4FVNVPROC) (GLuint index, const GLfloat *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB4SNVPROC) (GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB4SVNVPROC) (GLuint index, const GLshort *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIB4UBVNVPROC) (GLuint index, const GLubyte *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS1DVNVPROC) (GLuint index, GLsizei count, const GLdouble *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS1FVNVPROC) (GLuint index, GLsizei count, const GLfloat *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS1SVNVPROC) (GLuint index, GLsizei count, const GLshort *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS2DVNVPROC) (GLuint index, GLsizei count, const GLdouble *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS2FVNVPROC) (GLuint index, GLsizei count, const GLfloat *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS2SVNVPROC) (GLuint index, GLsizei count, const GLshort *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS3DVNVPROC) (GLuint index, GLsizei count, const GLdouble *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS3FVNVPROC) (GLuint index, GLsizei count, const GLfloat *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS3SVNVPROC) (GLuint index, GLsizei count, const GLshort *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS4DVNVPROC) (GLuint index, GLsizei count, const GLdouble *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS4FVNVPROC) (GLuint index, GLsizei count, const GLfloat *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS4SVNVPROC) (GLuint index, GLsizei count, const GLshort *v);
	typedef void (APIENTRY * PFNGLVERTEXATTRIBS4UBVNVPROC) (GLuint index, GLsizei count, const GLubyte *v);
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glBindProgramNV(GLenum target,GLuint id);
		extern void APIENTRY glDeleteProgramsNV(GLsizei n, const GLuint *ids);
		extern void APIENTRY glExecuteProgramNV(GLenum target,GLuint id, const GLfloat *params);
		extern void APIENTRY glGenProgramsNV(GLsizei n, GLuint *ids); 
		extern GLboolean APIENTRY glAreProgramsResidentNV(GLsizei n, const GLuint *ids, boolean *residences);
		extern void APIENTRY glRequestResidentProgramsNV(GLsizei n, GLuint *ids);
		extern void APIENTRY glGetProgramParameterfvNV(GLenum target, GLuint index, GLenum pname, GLfloat *params);
		extern void APIENTRY glGetProgramParameterdvNV(GLenum target, GLuint index, GLenum pname, GLdouble *params);
		extern void APIENTRY glGetProgramivNV(GLuint id, GLenum pname, int *params);
		extern void APIENTRY glGetProgramStringNV(GLuint id, GLenum pname, GLubyte *program);
		extern void APIENTRY glGetTrackMatrixivNV(GLenum target,GLuint address, GLenum pname, int *params);
		extern void APIENTRY glGetVertexAttribdvNV(GLuint index, GLenum pname, GLdouble *params);
		extern void APIENTRY glGetVertexAttribfvNV(GLuint index, GLenum pname, GLfloat *params);
		extern void APIENTRY glGetVertexAttribivNV(GLuint index, GLenum pname, int *params);
		extern void APIENTRY glGetVertexAttribPointervNV(GLuint index, GLenum pname, void **pointer); boolean IsProgramNV(GLuint id);
		extern void APIENTRY glLoadProgramNV(GLenum target,GLuint id, GLsizei len, const GLubyte *program);
		extern void APIENTRY glProgramParameter4fNV(GLenum target,GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		extern void APIENTRY glProgramParameter4dNV(GLenum target,GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
		extern void APIENTRY glProgramParameter4dvNV(GLenum target,GLuint index, const GLdouble *params);
		extern void APIENTRY glProgramParameter4fvNV(GLenum target,GLuint index, const GLfloat *params);
		extern void APIENTRY glProgramParameters4dvNV(GLenum target,GLuint index,GLuint num, const GLdouble *params);
		extern void APIENTRY glProgramParameters4fvNV(GLenum target,GLuint index,GLuint num, const GLfloat *params);
		extern void APIENTRY glTrackMatrixNV(GLenum target,GLuint address, GLenum matrix, GLenum transform);
		extern void APIENTRY glVertexAttribPointerNV(GLuint index, GLint sizei, GLenum type, GLsizei stride, const void *pointer);
		extern void APIENTRY glVertexAttrib1sNV(GLuint index, GLshort x);
		extern void APIENTRY glVertexAttrib1fNV(GLuint index, GLfloat x);
		extern void APIENTRY glVertexAttrib1dNV(GLuint index, GLdouble x);
		extern void APIENTRY glVertexAttrib2sNV(GLuint index, GLshort x, GLshort y);
		extern void APIENTRY glVertexAttrib2fNV(GLuint index, GLfloat x, GLfloat y);
		extern void APIENTRY glVertexAttrib2dNV(GLuint index, GLdouble x, GLdouble y);
		extern void APIENTRY glVertexAttrib3sNV(GLuint index, GLshort x, GLshort y, GLshort z);
		extern void APIENTRY glVertexAttrib3fNV(GLuint index, GLfloat x, GLfloat y, GLfloat z);
		extern void APIENTRY glVertexAttrib3dNV(GLuint index, GLdouble x, GLdouble y, GLdouble z);
		extern void APIENTRY glVertexAttrib4sNV(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
		extern void APIENTRY glVertexAttrib4fNV(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		extern void APIENTRY glVertexAttrib4dNV(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
		extern void APIENTRY glVertexAttrib4ubNV(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
		extern void APIENTRY glVertexAttrib1svNV(GLuint index, const GLshort *v);
		extern void APIENTRY glVertexAttrib1fvNV(GLuint index, const GLfloat *v);
		extern void APIENTRY glVertexAttrib1dvNV(GLuint index, const GLdouble *v);
		extern void APIENTRY glVertexAttrib2svNV(GLuint index, const GLshort *v);
		extern void APIENTRY glVertexAttrib2fvNV(GLuint index, const GLfloat *v);
		extern void APIENTRY glVertexAttrib2dvNV(GLuint index, const GLdouble *v);
		extern void APIENTRY glVertexAttrib3svNV(GLuint index, const GLshort *v);
		extern void APIENTRY glVertexAttrib3fvNV(GLuint index, const GLfloat *v);
		extern void APIENTRY glVertexAttrib3dvNV(GLuint index, const GLdouble *v);
		extern void APIENTRY glVertexAttrib4svNV(GLuint index, const GLshort *v);
		extern void APIENTRY glVertexAttrib4fvNV(GLuint index, const GLfloat *v);
		extern void APIENTRY glVertexAttrib4dvNV(GLuint index, const GLdouble *v);
		extern void APIENTRY glVertexAttrib4ubvNV(GLuint index, const GLubyte *v);
		extern void APIENTRY glVertexAttribs1svNV(GLuint index, GLsizei n, const GLshort *v);
		extern void APIENTRY glVertexAttribs1fvNV(GLuint index, GLsizei n, const GLfloat *v);
		extern void APIENTRY glVertexAttribs1dvNV(GLuint index, GLsizei n, const GLdouble *v);
		extern void APIENTRY glVertexAttribs2svNV(GLuint index, GLsizei n, const GLshort *v);
		extern void APIENTRY glVertexAttribs2fvNV(GLuint index, GLsizei n, const GLfloat *v);
		extern void APIENTRY glVertexAttribs2dvNV(GLuint index, GLsizei n, const GLdouble *v);
		extern void APIENTRY glVertexAttribs3svNV(GLuint index, GLsizei n, const GLshort *v);
		extern void APIENTRY glVertexAttribs3fvNV(GLuint index, GLsizei n, const GLfloat *v);
		extern void APIENTRY glVertexAttribs3dvNV(GLuint index, GLsizei n, const GLdouble *v);
		extern void APIENTRY glVertexAttribs4svNV(GLuint index, GLsizei n, const GLshort *v);
		extern void APIENTRY glVertexAttribs4fvNV(GLuint index, GLsizei n, const GLfloat *v);
		extern void APIENTRY glVertexAttribs4dvNV(GLuint index, GLsizei n, const GLdouble *v);
		extern void APIENTRY glVertexAttribs4ubvNV(GLuint index, GLsizei n, const GLubyte *v);
	#else
		#define MAYA_BIND_VERTEXPROGRAM_NV 1
		extern PFNGLAREPROGRAMSRESIDENTNVPROC glAreProgramsResidentNV;
		extern PFNGLBINDPROGRAMNVPROC glBindProgramNV;
		extern PFNGLDELETEPROGRAMSNVPROC glDeleteProgramsNV;
		extern PFNGLEXECUTEPROGRAMNVPROC glExecuteProgramNV;
		extern PFNGLGENPROGRAMSNVPROC glGenProgramsNV;
		extern PFNGLGETPROGRAMPARAMETERDVNVPROC glGetProgramParameterdvNV;
		extern PFNGLGETPROGRAMPARAMETERFVNVPROC glGetProgramParameterfvNV;
		extern PFNGLGETPROGRAMIVNVPROC glGetProgramivNV;
		extern PFNGLGETPROGRAMSTRINGNVPROC glGetProgramStringNV;
		extern PFNGLGETTRACKMATRIXNVPROC glGetTrackMatrixNV;
		extern PFNGLGETVERTEXATTRIBDVNVPROC glGetVertexAttribdvNV;
		extern PFNGLGETVERTEXATTRIBFVNVPROC glGetVertexAttribfvNV;
		extern PFNGLGETVERTEXATTRIBIVNVPROC glGetVertexAttribivNV;
		extern PFNGLGETVERTEXATTRIBPOINTERNVPROC glGetVertexAttribPointerNV;
		extern PFNGLISPROGRAMNVPROC glIsProgramNV;
		extern PFNGLLOADPROGRAMNVPROC glLoadProgramNV;
		extern PFNGLPROGRAMPARAMETER4DNVPROC glProgramParameter4dNV;
		extern PFNGLPROGRAMPARAMETER4DVNVPROC glProgramParameter4dvNV;
		extern PFNGLPROGRAMPARAMETER4FNVPROC glProgramParameter4fNV;
		extern PFNGLPROGRAMPARAMETER4FVNVPROC glProgramParameter4fvNV;
		extern PFNGLPROGRAMPARAMETERS4DVNVPROC glProgramParameters4dvNV;
		extern PFNGLPROGRAMPARAMETERS4FVNVPROC glProgramParameters4fvNV;
		extern PFNGLREQUESTRESIDENTPROGRAMSNVPROC glRequestResidentProgramsNV;
		extern PFNGLTRACKMATRIXNVPROC glTrackMatrixNV;
		extern PFNGLVERTEXATTRIBPOINTERNVPROC glVertexAttribPointerNV;
		extern PFNGLVERTEXATTRIB1DNVPROC glVertexAttrib1dNV;
		extern PFNGLVERTEXATTRIB1DVNVPROC glVertexAttrib1dvNV;
		extern PFNGLVERTEXATTRIB1FNVPROC glVertexAttrib1fNV;
		extern PFNGLVERTEXATTRIB1FVNVPROC glVertexAttrib1fvNV;
		extern PFNGLVERTEXATTRIB1SNVPROC glVertexAttrib1sNV;
		extern PFNGLVERTEXATTRIB1SVNVPROC glVertexAttrib1svNV;
		extern PFNGLVERTEXATTRIB2DNVPROC glVertexAttrib2dNV;
		extern PFNGLVERTEXATTRIB2DVNVPROC glVertexAttrib2dvNV;
		extern PFNGLVERTEXATTRIB2FNVPROC glVertexAttrib2fNV;
		extern PFNGLVERTEXATTRIB2FVNVPROC glVertexAttrib2fvNV;
		extern PFNGLVERTEXATTRIB2SNVPROC glVertexAttrib2sNV;
		extern PFNGLVERTEXATTRIB2SVNVPROC glVertexAttrib2svNV;
		extern PFNGLVERTEXATTRIB3DNVPROC glVertexAttrib3dNV;
		extern PFNGLVERTEXATTRIB3DVNVPROC glVertexAttrib3dvNV;
		extern PFNGLVERTEXATTRIB3FNVPROC glVertexAttrib3fNV;
		extern PFNGLVERTEXATTRIB3FVNVPROC glVertexAttrib3fvNV;
		extern PFNGLVERTEXATTRIB3SNVPROC glVertexAttrib3sNV;
		extern PFNGLVERTEXATTRIB3SVNVPROC glVertexAttrib3svNV;
		extern PFNGLVERTEXATTRIB4DNVPROC glVertexAttrib4dNV;
		extern PFNGLVERTEXATTRIB4DVNVPROC glVertexAttrib4dvNV;
		extern PFNGLVERTEXATTRIB4FNVPROC glVertexAttrib4fNV;
		extern PFNGLVERTEXATTRIB4FVNVPROC glVertexAttrib4fvNV;
		extern PFNGLVERTEXATTRIB4SNVPROC glVertexAttrib4sNV;
		extern PFNGLVERTEXATTRIB4SVNVPROC glVertexAttrib4svNV;
		extern PFNGLVERTEXATTRIB4UBVNVPROC glVertexAttrib4ubvNV;
		extern PFNGLVERTEXATTRIBS1DVNVPROC glVertexAttribs1dvNV;
		extern PFNGLVERTEXATTRIBS1FVNVPROC glVertexAttribs1fvNV;
		extern PFNGLVERTEXATTRIBS1SVNVPROC glVertexAttribs1svNV;
		extern PFNGLVERTEXATTRIBS2DVNVPROC glVertexAttribs2dvNV;
		extern PFNGLVERTEXATTRIBS2FVNVPROC glVertexAttribs2fvNV;
		extern PFNGLVERTEXATTRIBS2SVNVPROC glVertexAttribs2svNV;
		extern PFNGLVERTEXATTRIBS3DVNVPROC glVertexAttribs3dvNV;
		extern PFNGLVERTEXATTRIBS3FVNVPROC glVertexAttribs3fvNV;
		extern PFNGLVERTEXATTRIBS3SVNVPROC glVertexAttribs3svNV;
		extern PFNGLVERTEXATTRIBS4DVNVPROC glVertexAttribs4dvNV;
		extern PFNGLVERTEXATTRIBS4FVNVPROC glVertexAttribs4fvNV;
		extern PFNGLVERTEXATTRIBS4SVNVPROC glVertexAttribs4svNV;
		extern PFNGLVERTEXATTRIBS4UBVNVPROC glVertexAttribs4ubvNV;
	#endif
#endif

// Register combiner API
/* GL_NV_register_combiners */
#ifndef GL_NV_register_combiners
	#define GL_NV_register_combiners 1
	typedef void (APIENTRY * PFNGLCOMBINERPARAMETERFVNVPROC) (GLenum pname, const GLfloat *params);
	typedef void (APIENTRY * PFNGLCOMBINERPARAMETERFNVPROC) (GLenum pname, GLfloat param);
	typedef void (APIENTRY * PFNGLCOMBINERPARAMETERIVNVPROC) (GLenum pname, const GLint *params);
	typedef void (APIENTRY * PFNGLCOMBINERPARAMETERINVPROC) (GLenum pname, GLint param);
	typedef void (APIENTRY * PFNGLCOMBINERINPUTNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage);
	typedef void (APIENTRY * PFNGLCOMBINEROUTPUTNVPROC) (GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct, GLboolean cdDotProduct, GLboolean muxSum);
	typedef void (APIENTRY * PFNGLFINALCOMBINERINPUTNVPROC) (GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage);
	typedef void (APIENTRY * PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat *params);
	typedef void (APIENTRY * PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC) (GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint *params);
	typedef void (APIENTRY * PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC) (GLenum stage, GLenum portion, GLenum pname, GLfloat *params);
	typedef void (APIENTRY * PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC) (GLenum stage, GLenum portion, GLenum pname, GLint *params);
	typedef void (APIENTRY * PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC) (GLenum variable, GLenum pname, GLfloat *params);
	typedef void (APIENTRY * PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC) (GLenum variable, GLenum pname, GLint *params);
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glCombinerParameterfvNV (GLenum, const GLfloat *);
		extern void APIENTRY glCombinerParameterfNV (GLenum, GLfloat);
		extern void APIENTRY glCombinerParameterivNV (GLenum, const GLint *);
		extern void APIENTRY glCombinerParameteriNV (GLenum, GLint);
		extern void APIENTRY glCombinerInputNV (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum);
		extern void APIENTRY glCombinerOutputNV (GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLenum, GLboolean, GLboolean, GLboolean);
		extern void APIENTRY glFinalCombinerInputNV (GLenum, GLenum, GLenum, GLenum);
		extern void APIENTRY glGetCombinerInputParameterfvNV (GLenum, GLenum, GLenum, GLenum, GLfloat *);
		extern void APIENTRY glGetCombinerInputParameterivNV (GLenum, GLenum, GLenum, GLenum, GLint *);
		extern void APIENTRY glGetCombinerOutputParameterfvNV (GLenum, GLenum, GLenum, GLfloat *);
		extern void APIENTRY glGetCombinerOutputParameterivNV (GLenum, GLenum, GLenum, GLint *);
		extern void APIENTRY glGetFinalCombinerInputParameterfvNV (GLenum, GLenum, GLfloat *);
		extern void APIENTRY glGetFinalCombinerInputParameterivNV (GLenum, GLenum, GLint *);
	#else
		#define MAYA_BIND_REGISTER_COMBINERS_NV 1
		extern PFNGLCOMBINERPARAMETERFVNVPROC glCombinerParameterfvNV;
		extern PFNGLCOMBINERPARAMETERIVNVPROC glCombinerParameterivNV;
		extern PFNGLCOMBINERPARAMETERFNVPROC glCombinerParameterfNV;
		extern PFNGLCOMBINERPARAMETERINVPROC glCombinerParameteriNV;
		extern PFNGLCOMBINERINPUTNVPROC glCombinerInputNV;
		extern PFNGLCOMBINEROUTPUTNVPROC glCombinerOutputNV;
		extern PFNGLFINALCOMBINERINPUTNVPROC glFinalCombinerInputNV;
		extern PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC glGetCombinerInputParameterfvNV;
		extern PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC glGetCombinerInputParameterivNV;
		extern PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC glGetCombinerOutputParameterfvNV;
		extern PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC glGetCombinerOutputParameterivNV;
		extern PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfvNV;
		extern PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameterivNV;
	#endif /* GL_GLEXT_PROTOTYPES */
#endif

/* Nvidia Fence API */
#ifndef GL_NV_fence
#define GL_NV_fence 1
	typedef void (APIENTRY * PFNGLGENFENCESNVPROC) (GLsizei n, GLuint *fences);
	typedef void (APIENTRY * PFNGLDELETEFENCESNVPROC) (GLsizei n, const GLuint *fences);
	typedef void (APIENTRY * PFNGLSETFENCENVPROC) (GLuint fence, GLenum condition);
	typedef GLboolean (APIENTRY * PFNGLTESTFENCENVPROC) (GLuint fence);
	typedef void (APIENTRY * PFNGLFINISHFENCENVPROC) (GLuint fence);
	typedef GLboolean (APIENTRY * PFNGLISFENCENVPROC) (GLuint fence);
	typedef void (APIENTRY * PFNGLGETFENCEIVNVPROC) (GLuint fence, GLenum pname, GLint *params);
	#ifdef GL_GLEXT_PROTOTYPES
		extern void APIENTRY glGenFencesNV (GLsizei, GLuint *);
		extern void APIENTRY glDeleteFencesNV (GLsizei, const GLuint *);
		extern void APIENTRY glSetFenceNV (GLuint, GLenum);
		extern GLboolean APIENTRY glTestFenceNV (GLuint);
		extern void APIENTRY glFinishFenceNV (GLuint);
		extern GLboolean APIENTRY glIsFenceNV (GLuint);
		extern void APIENTRY glGetFenceivNV (GLuint, GLenum, GLint *);
	#else
		#define MAYA_BIND_NV_FENCE 1
	#endif 
	typedef void (APIENTRY * PFNGLGENFENCESNVPROC) (GLsizei n, GLuint *fences);
	typedef void (APIENTRY * PFNGLDELETEFENCESNVPROC) (GLsizei n, const GLuint *fences);
	typedef void (APIENTRY * PFNGLSETFENCENVPROC) (GLuint fence, GLenum condition);
	typedef GLboolean (APIENTRY * PFNGLTESTFENCENVPROC) (GLuint fence);
	typedef void (APIENTRY * PFNGLFINISHFENCENVPROC) (GLuint fence);
	typedef GLboolean (APIENTRY * PFNGLISFENCENVPROC) (GLuint fence);
	typedef void (APIENTRY * PFNGLGETFENCEIVNVPROC) (GLuint fence, GLenum pname, GLint *params);
#endif


#ifndef WGL_NV_allocate_memory
#define WGL_NV_allocate_memory 1
	#ifdef WGL_WGLEXT_PROTOTYPES
		extern void * wglAllocateMemoryNV(int size, float readfreq, float writefreq, float priority);
		extern void wglFreeMemoryNV(void * pointer); 
	#else
		#define MAYA_BIND_NV_ALLOCATE_MEMORY 1
	#endif
	typedef void * (APIENTRY * PFNWGLALLOCATEMEMORYNVPROC) (int size, float readfreq, float writefreq, float priority);
	typedef void (APIENTRY * PFNWGLFREEMEMORYNVPROC) (void *pointer);
#endif

/// ATI Extensions ///
//
// ATI PN triangles extension
//
#ifndef GL_ATI_pn_triangles
	#define GL_ATI_pn_triangles 1

	#define GL_PN_TRIANGLES_ATI                             0x87F0
	#define GL_MAX_PN_TRIANGLES_TESSELATION_LEVEL_ATI       0x87F1
	#define GL_PN_TRIANGLES_POINT_MODE_ATI                  0x87F2
	#define GL_PN_TRIANGLES_NORMAL_MODE_ATI                 0x87F3
	#define GL_PN_TRIANGLES_TESSELATION_LEVEL_ATI           0x87F4
	#define GL_PN_TRIANGLES_POINT_MODE_LINEAR_ATI           0x87F5
	#define GL_PN_TRIANGLES_POINT_MODE_CUBIC_ATI            0x87F6
	#define GL_PN_TRIANGLES_NORMAL_MODE_LINEAR_ATI          0x87F7
	#define GL_PN_TRIANGLES_NORMAL_MODE_QUADRATIC_ATI		0x87F8

#ifdef GL_GLEXT_PROTOTYPES
	extern void glPNTrianglesiATI( GLenum pname, GLint param );
	extern void glPNTrianglesfATI( GLenum pname, GLfloat param );
#else
	#define MAYA_BIND_PN_TRIANGLES 1
	typedef void (APIENTRY *PFNGLPNTRIANGLESIATIPROC)(GLenum pname, GLint param);
	typedef void (APIENTRY *PFNGLPNTRIANGLESFATIPROC)(GLenum pname, GLfloat param);
	extern PFNGLPNTRIANGLESIATIPROC glPNTrianglesiATI;
	extern PFNGLPNTRIANGLESFATIPROC glPNTrianglesfATI;
#endif

#endif // GL_ATI_pn_triangles

//
// Programmable vertex shading extension
//
#ifndef GL_EXT_vertex_shader
	#define GL_EXT_vertex_shader 1
	#define GL_VERTEX_SHADER_EXT								0x8780
	#define GL_VERTEX_SHADER_BINDING_EXT						0x8781
	#define GL_OP_INDEX_EXT										0x8782
	#define GL_OP_NEGATE_EXT									0x8783
	#define GL_OP_DOT3_EXT										0x8784
	#define GL_OP_DOT4_EXT										0x8785
	#define GL_OP_MUL_EXT										0x8786
	#define GL_OP_ADD_EXT										0x8787
	#define GL_OP_MADD_EXT										0x8788
	#define GL_OP_FRAC_EXT										0x8789
	#define GL_OP_MAX_EXT										0x878A
	#define GL_OP_MIN_EXT										0x878B
	#define GL_OP_SET_GE_EXT									0x878C
	#define GL_OP_SET_LT_EXT									0x878D
	#define GL_OP_CLAMP_EXT										0x878E
	#define GL_OP_FLOOR_EXT										0x878F
	#define GL_OP_ROUND_EXT										0x8790
	#define GL_OP_EXP_BASE_2_EXT								0x8791
	#define GL_OP_LOG_BASE_2_EXT								0x8792
	#define GL_OP_POWER_EXT										0x8793
	#define GL_OP_RECIP_EXT										0x8794
	#define GL_OP_RECIP_SQRT_EXT								0x8795
	#define GL_OP_SUB_EXT										0x8796
	#define GL_OP_CROSS_PRODUCT_EXT								0x8797
	#define GL_OP_MULTIPLY_MATRIX_EXT							0x8798
	#define GL_OP_MOV_EXT										0x8799
	#define GL_OUTPUT_VERTEX_EXT								0x879A
	#define GL_OUTPUT_COLOR0_EXT								0x879B
	#define GL_OUTPUT_COLOR1_EXT								0x879C
	#define GL_OUTPUT_TEXTURE_COORD0_EXT						0x879D
	#define GL_OUTPUT_TEXTURE_COORD1_EXT						0x879E
	#define GL_OUTPUT_TEXTURE_COORD2_EXT						0x879F
	#define GL_OUTPUT_TEXTURE_COORD3_EXT						0x87A0
	#define GL_OUTPUT_TEXTURE_COORD4_EXT						0x87A1
	#define GL_OUTPUT_TEXTURE_COORD5_EXT						0x87A2
	#define GL_OUTPUT_TEXTURE_COORD6_EXT						0x87A3
	#define GL_OUTPUT_TEXTURE_COORD7_EXT						0x87A4
	#define GL_OUTPUT_TEXTURE_COORD8_EXT						0x87A5
	#define GL_OUTPUT_TEXTURE_COORD9_EXT						0x87A6
	#define GL_OUTPUT_TEXTURE_COORD10_EXT						0x87A7
	#define GL_OUTPUT_TEXTURE_COORD11_EXT						0x87A8
	#define GL_OUTPUT_TEXTURE_COORD12_EXT						0x87A9
	#define GL_OUTPUT_TEXTURE_COORD13_EXT						0x87AA
	#define GL_OUTPUT_TEXTURE_COORD14_EXT						0x87AB
	#define GL_OUTPUT_TEXTURE_COORD15_EXT						0x87AC
	#define GL_OUTPUT_TEXTURE_COORD16_EXT						0x87AD
	#define GL_OUTPUT_TEXTURE_COORD17_EXT						0x87AE
	#define GL_OUTPUT_TEXTURE_COORD18_EXT						0x87AF
	#define GL_OUTPUT_TEXTURE_COORD19_EXT						0x87B0
	#define GL_OUTPUT_TEXTURE_COORD20_EXT						0x87B1
	#define GL_OUTPUT_TEXTURE_COORD21_EXT						0x87B2
	#define GL_OUTPUT_TEXTURE_COORD22_EXT						0x87B3
	#define GL_OUTPUT_TEXTURE_COORD23_EXT						0x87B4
	#define GL_OUTPUT_TEXTURE_COORD24_EXT						0x87B5
	#define GL_OUTPUT_TEXTURE_COORD25_EXT						0x87B6
	#define GL_OUTPUT_TEXTURE_COORD26_EXT						0x87B7
	#define GL_OUTPUT_TEXTURE_COORD27_EXT						0x87B8
	#define GL_OUTPUT_TEXTURE_COORD28_EXT						0x87B9
	#define GL_OUTPUT_TEXTURE_COORD29_EXT						0x87BA
	#define GL_OUTPUT_TEXTURE_COORD30_EXT						0x87BB
	#define GL_OUTPUT_TEXTURE_COORD31_EXT						0x87BC
	#define GL_OUTPUT_FOG_EXT									0x87BD
	#define GL_SCALAR_EXT										0x87BE
	#define GL_VECTOR_EXT										0x87BF
	#define GL_MATRIX_EXT										0x87C0
	#define GL_VARIANT_EXT										0x87C1
	#define GL_INVARIANT_EXT									0x87C2
	#define GL_LOCAL_CONSTANT_EXT								0x87C3
	#define GL_LOCAL_EXT										0x87C4
	#define GL_MAX_VERTEX_SHADER_INSTRUCTIONS_EXT				0x87C5
	#define GL_MAX_VERTEX_SHADER_VARIANTS_EXT					0x87C6
	#define GL_MAX_VERTEX_SHADER_INVARIANTS_EXT					0x87C7
	#define GL_MAX_VERTEX_SHADER_LOCAL_CONSTANTS_EXT			0x87C8
	#define GL_MAX_VERTEX_SHADER_LOCALS_EXT						0x87C9
	#define GL_MAX_OPTIMIZED_VERTEX_SHADER_INSTRUCTIONS_EXT		0x87CA
	#define GL_MAX_OPTIMIZED_VERTEX_SHADER_VARIANTS_EXT			0x87CB
	#define GL_MAX_OPTIMIZED_VERTEX_SHADER_INVARIANTS_EXT		0x87CC
	#define GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCAL_CONSTANTS_EXT	0x87CD
	#define GL_MAX_OPTIMIZED_VERTEX_SHADER_LOCALS_EXT			0x87CE
	#define GL_VERTEX_SHADER_INSTRUCTIONS_EXT					0x87CF
	#define GL_VERTEX_SHADER_VARIANTS_EXT						0x87D0
	#define GL_VERTEX_SHADER_INVARIANTS_EXT						0x87D1
	#define GL_VERTEX_SHADER_LOCAL_CONSTANTS_EXT				0x87D2
	#define GL_VERTEX_SHADER_LOCALS_EXT							0x87D3
	#define GL_VERTEX_SHADER_OPTIMIZED_EXT						0x87D4
	#define GL_X_EXT											0x87D5
	#define GL_Y_EXT											0x87D6
	#define GL_Z_EXT											0x87D7
	#define GL_W_EXT											0x87D8
	#define GL_NEGATIVE_X_EXT									0x87D9
	#define GL_NEGATIVE_Y_EXT									0x87DA
	#define GL_NEGATIVE_Z_EXT									0x87DB
	#define GL_NEGATIVE_W_EXT									0x87DC
	#define GL_ZERO_EXT											0x87DD
	#define GL_ONE_EXT											0x87DE
	#define GL_NEGATIVE_ONE_EXT									0x87DF
	#define GL_NORMALIZED_RANGE_EXT								0x87E0
	#define GL_FULL_RANGE_EXT									0x87E1
	#define GL_CURRENT_VERTEX_EXT								0x87E2
	#define GL_MVP_MATRIX_EXT									0x87E3
	#define GL_VARIANT_VALUE_EXT								0x87E4
	#define GL_VARIANT_DATATYPE_EXT								0x87E5
	#define GL_VARIANT_ARRAY_STRIDE_EXT							0x87E6
	#define GL_VARIANT_ARRAY_TYPE_EXT							0x87E7
	#define GL_VARIANT_ARRAY_EXT								0x87E8
	#define GL_VARIANT_ARRAY_POINTER_EXT						0x87E9
	#define GL_INVARIANT_VALUE_EXT								0x87EA
	#define GL_INVARIANT_DATATYPE_EXT							0x87EB
	#define GL_LOCAL_CONSTANT_VALUE_EXT							0x87EC
	#define GL_LOCAL_CONSTANT_DATATYPE_EXT						0x87ED

#ifdef GL_GLEXT_PROTOTYPES
	extern void glBeginVertexShaderEXT( void );
    extern void glEndVertexShaderEXT( void );
    extern void glBindVertexShaderEXT( GLuint id );
    extern GLuint glGenVertexShadersEXT( GLuint range );
    extern void glDeleteVertexShaderEXT( GLuint id );
    extern void glShaderOp1EXT( GLenum op, GLuint res, GLuint arg1 );
    extern void glShaderOp2EXT( GLenum op, GLuint res, GLuint arg1, GLuint arg2 );
    extern void glShaderOp3EXT( GLenum op, GLuint res, GLuint arg1, GLuint arg2, GLuint arg3 );
    extern void glSwizzleEXT( GLuint res, GLuint in, GLenum outX, GLenum outY, GLenum outZ, 
                     GLenum outW );
    extern void glWriteMaskEXT( GLuint res, GLuint in, GLenum outX, GLenum outY, GLenum outZ,
                      GLenum outW );
    extern void glInsertComponentEXT( GLuint res, GLuint src, GLuint num );
    extern void glExtractComponentEXT( GLuint res, GLuint src, GLuint num );
    extern GLuint glGenSymbolsEXT(GLenum datatype,GLenum storagetype,GLenum range,
                     GLuint components ); 
    extern void glSetInvariantEXT( GLuint id, GLenum type, void *addr );
    extern void glSetLocalConstantEXT( GLuint id, GLenum type, void *addr );
    extern void glVariantbvEXT( GLuint id, GLbyte *addr );
    extern void glVariantsvEXT( GLuint id, GLshort *addr );
    extern void glVariantivEXT( GLuint id, GLint *addr );
    extern void glVariantfvEXT( GLuint id, GLfloat *addr );
    extern void glVariantdvEXT( GLuint id, GLdouble *addr );
    extern void glVariantubvEXT( GLuint id, GLubyte *addr );
    extern void glVariantusvEXT( GLuint id, GLshort *addr );
    extern void glVariantuivEXT( GLuint id, GLuint *addr );
    extern void glVariantPointerEXT( GLuint id, GLenum type, GLuint stride, void *addr );
    extern void glEnableVariantClientStateEXT( GLuint id);
    extern void glDisableVariantClientStateEXT( GLuint id);
    extern GLuint glBindLightParameterEXT( GLenum light, GLenum value);
    extern GLuint glBindMaterialParameterEXT( GLenum face, GLenum value);
    extern GLuint glBindTexGenParameterEXT( GLenum unit, GLenum coord, GLenum value);
    extern GLuint glBindTextureUnitParameterEXT( GLenum unit, GLenum value);
    extern GLuint glBindParameterEXT( GLenum value);
    extern GLboolean glIsVariantEnabledEXT( GLuint id, GLenum cap);
    extern void glGetVariantBooleanvEXT( GLuint id, GLenum value, boolean *data);
    extern void glGetVariantIntegervEXT( GLuint id, GLenum value, GLint *data);
    extern void glGetVariantFloatvEXT( GLuint id, GLenum value, GLfloat *data);
    extern void glGetVariantPointervEXT( GLuint id, GLenum value, void **data);
    extern void glGetInvariantBooleanvEXT( GLuint id, GLenum value, boolean *data);
    extern void glGetInvariantIntegervEXT( GLuint id, GLenum value, GLint *data);
    extern void glGetInvariantFloatvEXT( GLuint id, GLenum value, GLfloat *data);
    extern void glGetLocalConstantBooleanvEXT( GLuint id, GLenum value, boolean *data);
    extern void glGetLocalConstantIntegervEXT( GLuint id, GLenum value, GLint *data);
    extern void glGetLocalConstantFloatvEXT( GLuint id, GLenum value, GLfloat *data);
#else
	#define MAYA_BIND_EXT_VERTEX_SHADER 1
	typedef GLvoid    (APIENTRY * PFNGLBEGINVERTEXSHADEREXTPROC) (void);
	typedef GLvoid    (APIENTRY * PFNGLENDVERTEXSHADEREXTPROC) (void);
	typedef GLvoid    (APIENTRY * PFNGLBINDVERTEXSHADEREXTPROC) (GLuint id);
	typedef GLuint    (APIENTRY * PFNGLGENVERTEXSHADERSEXTPROC) (GLuint range);
	typedef GLvoid    (APIENTRY * PFNGLDELETEVERTEXSHADEREXTPROC) (GLuint id);
	typedef GLvoid    (APIENTRY * PFNGLSHADEROP1EXTPROC) (GLenum op, GLuint res, GLuint arg1);
	typedef GLvoid    (APIENTRY * PFNGLSHADEROP2EXTPROC) (GLenum op, GLuint res, GLuint arg1,
														  GLuint arg2);
	typedef GLvoid    (APIENTRY * PFNGLSHADEROP3EXTPROC) (GLenum op, GLuint res, GLuint arg1,
														  GLuint arg2, GLuint arg3);
	typedef GLvoid    (APIENTRY * PFNGLSWIZZLEEXTPROC) (GLuint res, GLuint in, GLenum outX,
														GLenum outY, GLenum outZ, GLenum outW);
	typedef GLvoid    (APIENTRY * PFNGLWRITEMASKEXTPROC) (GLuint res, GLuint in, GLenum outX,
														  GLenum outY, GLenum outZ, GLenum outW);
	typedef GLvoid    (APIENTRY * PFNGLINSERTCOMPONENTEXTPROC) (GLuint res, GLuint src, GLuint num);
	typedef GLvoid    (APIENTRY * PFNGLEXTRACTCOMPONENTEXTPROC) (GLuint res, GLuint src, GLuint num);
	typedef GLuint    (APIENTRY * PFNGLGENSYMBOLSEXTPROC) (GLenum dataType, GLenum storageType,
														   GLenum range, GLuint components);
	typedef GLvoid    (APIENTRY * PFNGLSETINVARIANTEXTPROC) (GLuint id, GLenum type, GLvoid *addr);
	typedef GLvoid    (APIENTRY * PFNGLSETLOCALCONSTANTEXTPROC) (GLuint id, GLenum type, GLvoid *addr);
	typedef GLvoid    (APIENTRY * PFNGLVARIANTBVEXTPROC) (GLuint id, GLbyte *addr);
	typedef GLvoid    (APIENTRY * PFNGLVARIANTSVEXTPROC) (GLuint id, GLshort *addr);
	typedef GLvoid    (APIENTRY * PFNGLVARIANTIVEXTPROC) (GLuint id, GLint *addr);
	typedef GLvoid    (APIENTRY * PFNGLVARIANTFVEXTPROC) (GLuint id, GLfloat *addr);
	typedef GLvoid    (APIENTRY * PFNGLVARIANTDVEXTPROC) (GLuint id, GLdouble *addr);
	typedef GLvoid    (APIENTRY * PFNGLVARIANTUBVEXTPROC) (GLuint id, GLubyte *addr);
	typedef GLvoid    (APIENTRY * PFNGLVARIANTUSVEXTPROC) (GLuint id, GLushort *addr);
	typedef GLvoid    (APIENTRY * PFNGLVARIANTUIVEXTPROC) (GLuint id, GLuint *addr);
	typedef GLvoid    (APIENTRY * PFNGLVARIANTPOINTEREXTPROC) (GLuint id, GLenum type,
															   GLuint stride, GLvoid *addr);
	typedef GLvoid    (APIENTRY * PFNGLENABLEVARIANTCLIENTSTATEEXTPROC) (GLuint id);
	typedef GLvoid    (APIENTRY * PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC) (GLuint id);
	typedef GLuint    (APIENTRY * PFNGLBINDLIGHTPARAMETEREXTPROC) (GLenum light, GLenum value);
	typedef GLuint    (APIENTRY * PFNGLBINDMATERIALPARAMETEREXTPROC) (GLenum face, GLenum value);
	typedef GLuint    (APIENTRY * PFNGLBINDTEXGENPARAMETEREXTPROC) (GLenum unit, GLenum coord,
																	GLenum value);
	typedef GLuint    (APIENTRY * PFNGLBINDTEXTUREUNITPARAMETEREXTPROC) (GLenum unit, GLenum value);
	typedef GLuint    (APIENTRY * PFNGLBINDPARAMETEREXTPROC) (GLenum value);
	typedef GLboolean (APIENTRY * PFNGLISVARIANTENABLEDEXTPROC) (GLuint id, GLenum cap);
	typedef GLvoid    (APIENTRY * PFNGLGETVARIANTBOOLEANVEXTPROC) (GLuint id, GLenum value,
																   GLboolean *data);
	typedef GLvoid    (APIENTRY * PFNGLGETVARIANTINTEGERVEXTPROC) (GLuint id, GLenum value,
																   GLint *data);
	typedef GLvoid    (APIENTRY * PFNGLGETVARIANTFLOATVEXTPROC) (GLuint id, GLenum value,
																   GLfloat *data);
	typedef GLvoid    (APIENTRY * PFNGLGETVARIANTPOINTERVEXTPROC) (GLuint id, GLenum value,
																   GLvoid **data);
	typedef GLvoid    (APIENTRY * PFNGLGETINVARIANTBOOLEANVEXTPROC) (GLuint id, GLenum value,
																	 GLboolean *data);
	typedef GLvoid    (APIENTRY * PFNGLGETINVARIANTINTEGERVEXTPROC) (GLuint id, GLenum value,
																	 GLint *data);
	typedef GLvoid    (APIENTRY * PFNGLGETINVARIANTFLOATVEXTPROC) (GLuint id, GLenum value,
																   GLfloat *data);
	typedef GLvoid    (APIENTRY * PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC) (GLuint id, GLenum value,
																		 GLboolean *data);
	typedef GLvoid    (APIENTRY * PFNGLGETLOCALCONSTANTINTEGERVEXTPROC) (GLuint id, GLenum value,
																		 GLint *data);
	typedef GLvoid    (APIENTRY * PFNGLGETLOCALCONSTANTFLOATVEXTPROC) (GLuint id, GLenum value,
																	   GLfloat *data);
	extern PFNGLBEGINVERTEXSHADEREXTPROC glBeginVertexShaderEXT;
    extern PFNGLENDVERTEXSHADEREXTPROC glEndVertexShaderEXT;
    extern PFNGLBINDVERTEXSHADEREXTPROC glBindVertexShaderEXT;
    extern PFNGLGENVERTEXSHADERSEXTPROC glGenVertexShadersEXT;
    extern PFNGLDELETEVERTEXSHADEREXTPROC glDeleteVertexShaderEXT;
    extern PFNGLSHADEROP1EXTPROC glShaderOp1EXT;
    extern PFNGLSHADEROP2EXTPROC glShaderOp2EXT;
    extern PFNGLSHADEROP3EXTPROC glShaderOp3EXT;
    extern PFNGLSWIZZLEEXTPROC glSwizzleEXT;
    extern PFNGLWRITEMASKEXTPROC glWriteMaskEXT;
    extern PFNGLINSERTCOMPONENTEXTPROC glInsertComponentEXT;
    extern PFNGLEXTRACTCOMPONENTEXTPROC glExtractComponentEXT;
    extern PFNGLGENSYMBOLSEXTPROC glGenSymbolsEXT;
    extern PFNGLSETINVARIANTEXTPROC glSetInvariantEXT;
    extern PFNGLSETLOCALCONSTANTEXTPROC glSetLocalConstantEXT;
    extern PFNGLVARIANTBVEXTPROC glVariantbvEXT;
    extern PFNGLVARIANTSVEXTPROC glVariantsvEXT;
    extern PFNGLVARIANTIVEXTPROC glVariantivEXT;
    extern PFNGLVARIANTFVEXTPROC glVariantfvEXT;
    extern PFNGLVARIANTDVEXTPROC glVariantdvEXT;
    extern PFNGLVARIANTUBVEXTPROC glVariantubvEXT;
    extern PFNGLVARIANTUSVEXTPROC glVariantusvEXT;
    extern PFNGLVARIANTUIVEXTPROC glVariantuivEXT;
    extern PFNGLVARIANTPOINTEREXTPROC glVariantPointerEXT;
    extern PFNGLENABLEVARIANTCLIENTSTATEEXTPROC glEnableVariantClientStateEXT;
    extern PFNGLDISABLEVARIANTCLIENTSTATEEXTPROC glDisableVariantClientStateEXT;
    extern PFNGLBINDLIGHTPARAMETEREXTPROC glBindLightParameterEXT;
    extern PFNGLBINDMATERIALPARAMETEREXTPROC glBindMaterialParameterEXT;
    extern PFNGLBINDTEXGENPARAMETEREXTPROC glBindTexGenParameterEXT;
    extern PFNGLBINDTEXTUREUNITPARAMETEREXTPROC glBindTextureUnitParameterEXT;
	extern PFNGLBINDPARAMETEREXTPROC glBindParameterEXT;
    extern PFNGLISVARIANTENABLEDEXTPROC glIsVariantEnabledEXT;
    extern PFNGLGETVARIANTBOOLEANVEXTPROC glGetVariantBooleanvEXT;
    extern PFNGLGETVARIANTINTEGERVEXTPROC glGetVariantIntegervEXT;
    extern PFNGLGETVARIANTFLOATVEXTPROC glGetVariantFloatvEXT;
    extern PFNGLGETVARIANTPOINTERVEXTPROC glGetVariantPointervEXT;
	extern PFNGLGETINVARIANTBOOLEANVEXTPROC glGetInvariantBooleanvEXT;
    extern PFNGLGETINVARIANTINTEGERVEXTPROC glGetInvariantIntegervEXT;
    extern PFNGLGETINVARIANTFLOATVEXTPROC glGetInvariantFloatvEXT;
	extern PFNGLGETLOCALCONSTANTBOOLEANVEXTPROC glGetLocalConstantBooleanvEXT;
    extern PFNGLGETLOCALCONSTANTINTEGERVEXTPROC glGetLocalConstantIntegervEXT;
    extern PFNGLGETLOCALCONSTANTFLOATVEXTPROC glGetLocalConstantFloatvEXT;
#endif
#endif /* GL_EXT_vertex_shader */

//
// Fragment / pixel shading extension
#ifndef GL_ATI_fragment_shader
	#define GL_ATI_fragment_shader 1

	#define GL_FRAGMENT_SHADER_ATI						0x8920
	#define GL_REG_0_ATI								0x8921
	#define GL_REG_1_ATI								0x8922
	#define GL_REG_2_ATI								0x8923
	#define GL_REG_3_ATI								0x8924
	#define GL_REG_4_ATI								0x8925
	#define GL_REG_5_ATI								0x8926
	#define GL_REG_6_ATI								0x8927
	#define GL_REG_7_ATI								0x8928
	#define GL_REG_8_ATI								0x8929
	#define GL_REG_9_ATI								0x892A
	#define GL_REG_10_ATI								0x892B
	#define GL_REG_11_ATI								0x892C
	#define GL_REG_12_ATI								0x892D
	#define GL_REG_13_ATI								0x892E
	#define GL_REG_14_ATI								0x892F
	#define GL_REG_15_ATI								0x8930
	#define GL_REG_16_ATI								0x8931
	#define GL_REG_17_ATI								0x8932
	#define GL_REG_18_ATI								0x8933
	#define GL_REG_19_ATI								0x8934
	#define GL_REG_20_ATI								0x8935
	#define GL_REG_21_ATI								0x8936
	#define GL_REG_22_ATI								0x8937
	#define GL_REG_23_ATI								0x8938
	#define GL_REG_24_ATI								0x8939
	#define GL_REG_25_ATI								0x893A
	#define GL_REG_26_ATI								0x893B
	#define GL_REG_27_ATI								0x893C
	#define GL_REG_28_ATI								0x893D
	#define GL_REG_29_ATI								0x893E
	#define GL_REG_30_ATI								0x893F
	#define GL_REG_31_ATI								0x8940
	#define GL_CON_0_ATI								0x8941
	#define GL_CON_1_ATI								0x8942
	#define GL_CON_2_ATI								0x8943
	#define GL_CON_3_ATI								0x8944
	#define GL_CON_4_ATI								0x8945
	#define GL_CON_5_ATI								0x8946
	#define GL_CON_6_ATI								0x8947
	#define GL_CON_7_ATI								0x8948
	#define GL_CON_8_ATI								0x8949
	#define GL_CON_9_ATI								0x894A
	#define GL_CON_10_ATI								0x894B
	#define GL_CON_11_ATI								0x894C
	#define GL_CON_12_ATI								0x894D
	#define GL_CON_13_ATI								0x894E
	#define GL_CON_14_ATI								0x894F
	#define GL_CON_15_ATI								0x8950
	#define GL_CON_16_ATI								0x8951
	#define GL_CON_17_ATI								0x8952
	#define GL_CON_18_ATI								0x8953
	#define GL_CON_19_ATI								0x8954
	#define GL_CON_20_ATI								0x8955
	#define GL_CON_21_ATI								0x8956
	#define GL_CON_22_ATI								0x8957
	#define GL_CON_23_ATI								0x8958
	#define GL_CON_24_ATI								0x8959
	#define GL_CON_25_ATI								0x895A
	#define GL_CON_26_ATI								0x895B
	#define GL_CON_27_ATI								0x895C
	#define GL_CON_28_ATI								0x895D
	#define GL_CON_29_ATI								0x895E
	#define GL_CON_30_ATI								0x895F
	#define GL_CON_31_ATI								0x8960
	#define GL_MOV_ATI									0x8961
	#define GL_ADD_ATI									0x8963
	#define GL_MUL_ATI									0x8964
	#define GL_SUB_ATI									0x8965
	#define GL_DOT3_ATI									0x8966
	#define GL_DOT4_ATI									0x8967
	#define GL_MAD_ATI									0x8968
	#define GL_LERP_ATI									0x8969
	#define GL_CND_ATI									0x896A
	#define GL_CND0_ATI									0x896B
	#define GL_DOT2_ADD_ATI								0x896C
	#define GL_SECONDARY_INTERPOLATOR_ATI				0x896D
	#define GL_NUM_FRAGMENT_REGISTERS_ATI				0x896E
	#define GL_NUM_FRAGMENT_CONSTANTS_ATI				0x896F
	#define GL_NUM_PASSES_ATI							0x8970
	#define GL_NUM_INSTRUCTIONS_PER_PASS_ATI			0x8971
	#define GL_NUM_INSTRUCTIONS_TOTAL_ATI				0x8972
	#define GL_NUM_INPUT_INTERPOLATOR_COMPONENTS_ATI	0x8973
	#define GL_NUM_LOOPBACK_COMPONENTS_ATI				0x8974
	#define GL_COLOR_ALPHA_PAIRING_ATI					0x8975
	#define GL_SWIZZLE_STR_ATI							0x8976
	#define GL_SWIZZLE_STQ_ATI							0x8977
	#define GL_SWIZZLE_STR_DR_ATI						0x8978
	#define GL_SWIZZLE_STQ_DQ_ATI						0x8979
	#define GL_SWIZZLE_STRQ_ATI							0x897A
	#define GL_SWIZZLE_STRQ_DQ_ATI						0x897B
	#define GL_RED_BIT_ATI								0x00000001
	#define GL_GREEN_BIT_ATI							0x00000002
	#define GL_BLUE_BIT_ATI								0x00000004
	#define GL_2X_BIT_ATI								0x00000001
	#define GL_4X_BIT_ATI								0x00000002
	#define GL_8X_BIT_ATI								0x00000004
	#define GL_HALF_BIT_ATI								0x00000008
	#define GL_QUARTER_BIT_ATI							0x00000010
	#define GL_EIGHTH_BIT_ATI							0x00000020
	#define GL_SATURATE_BIT_ATI							0x00000040
	#define GL_COMP_BIT_ATI								0x00000002
	#define GL_NEGATE_BIT_ATI							0x00000004
	#define GL_BIAS_BIT_ATI								0x00000008

#ifdef GL_GLEXT_PROTOTYPES
	extern GLuint glGenFragmentShadersATI ( GLuint range );
    extern void glBindFragmentShaderATI ( GLuint id );
    extern void glDeleteFragmentShaderATI ( GLuint id );
    extern void glBeginFragmentShaderATI ( GLvoid );
    extern void glEndFragmentShaderATI ( GLvoid );
    extern void glPassTexCoordATI ( GLuint dst, GLuint coord, GLenum swizzle );
    extern void glSampleMapATI ( GLuint dst, GLuint interp, GLenum swizzle );
    extern void glColorFragmentOp1ATI ( GLenum op, GLuint dst, GLuint dstMask, 
                                GLuint dstMod, GLuint arg1, GLuint arg1Rep, 
                                GLuint arg1Mod );
    extern void glColorFragmentOp2ATI ( GLenum op, GLuint dst, GLuint dstMask, 
                                GLuint dstMod, GLuint arg1, GLuint arg1Rep, 
                                GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, 
                                GLuint arg2Mod );
    extern void glColorFragmentOp3ATI ( GLenum op, GLuint dst, GLuint dstMask, 
                                GLuint dstMod, GLuint arg1, GLuint arg1Rep, 
                                GLuint arg1Mod, GLuint arg2, GLuint arg2Rep, 
                                GLuint arg2Mod, GLuint arg3, GLuint arg3Rep, 
                                GLuint arg3Mod );
    extern void glAlphaFragmentOp1ATI ( GLenum op, GLuint dst, GLuint dstMod, 
                                GLuint arg1, GLuint arg1Rep, GLuint arg1Mod );
    extern void glAlphaFragmentOp2ATI ( GLenum op, GLuint dst, GLuint dstMod, 
                                GLuint arg1, GLuint arg1Rep, GLuint arg1Mod, 
                                GLuint arg2, GLuint arg2Rep, GLuint arg2Mod );
    extern void glAlphaFragmentOp3ATI ( GLenum op, GLuint dst, GLuint dstMod, 
                                GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
                                GLuint arg2, GLuint arg2Rep, GLuint arg2Mod,
                                GLuint arg3, GLuint arg3Rep, GLuint arg3Mod );
    extern void glSetFragmentShaderConstantATI ( GLuint dst, const float *value );
#else
	#define MAYA_BIND_ATI_FRAGMENT_SHADER 1
	typedef GLuint (APIENTRY *PFNGLGENFRAGMENTSHADERSATIPROC)(GLuint range);
	typedef GLvoid (APIENTRY *PFNGLBINDFRAGMENTSHADERATIPROC)(GLuint id);
	typedef GLvoid (APIENTRY *PFNGLDELETEFRAGMENTSHADERATIPROC)(GLuint id);
	typedef GLvoid (APIENTRY *PFNGLBEGINFRAGMENTSHADERATIPROC)(GLvoid);
	typedef GLvoid (APIENTRY *PFNGLENDFRAGMENTSHADERATIPROC)(GLvoid);
	typedef GLvoid (APIENTRY *PFNGLPASSTEXCOORDATIPROC)(GLuint dst, GLuint coord, GLenum swizzle);
	typedef GLvoid (APIENTRY *PFNGLSAMPLEMAPATIPROC)(GLuint dst, GLuint interp, GLenum swizzle);
	typedef GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP1ATIPROC)(GLenum op, GLuint dst, GLuint dstMask,
										   GLuint dstMod, GLuint arg1, GLuint arg1Rep,
										   GLuint arg1Mod);
	typedef GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP2ATIPROC)(GLenum op, GLuint dst, GLuint dstMask,
										   GLuint dstMod, GLuint arg1, GLuint arg1Rep,
										   GLuint arg1Mod, GLuint arg2, GLuint arg2Rep,
										   GLuint arg2Mod);
	typedef GLvoid (APIENTRY *PFNGLCOLORFRAGMENTOP3ATIPROC)(GLenum op, GLuint dst, GLuint dstMask,
										   GLuint dstMod, GLuint arg1, GLuint arg1Rep,
										   GLuint arg1Mod, GLuint arg2, GLuint arg2Rep,
										   GLuint arg2Mod, GLuint arg3, GLuint arg3Rep,
										   GLuint arg3Mod);
	typedef GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP1ATIPROC)(GLenum op, GLuint dst, GLuint dstMod,
										   GLuint arg1, GLuint arg1Rep, GLuint arg1Mod);
	typedef GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP2ATIPROC)(GLenum op, GLuint dst, GLuint dstMod,
										   GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
										   GLuint arg2, GLuint arg2Rep, GLuint arg2Mod);
	typedef GLvoid (APIENTRY *PFNGLALPHAFRAGMENTOP3ATIPROC)(GLenum op, GLuint dst, GLuint dstMod,
										   GLuint arg1, GLuint arg1Rep, GLuint arg1Mod,
										   GLuint arg2, GLuint arg2Rep, GLuint arg2Mod,
										   GLuint arg3, GLuint arg3Rep, GLuint arg3Mod);
	typedef GLvoid (APIENTRY *PFNGLSETFRAGMENTSHADERCONSTANTATIPROC)(GLuint dst, const GLfloat *value);

	extern PFNGLGENFRAGMENTSHADERSATIPROC glGenFragmentShadersATI;
    extern PFNGLBINDFRAGMENTSHADERATIPROC glBindFragmentShaderATI;
    extern PFNGLDELETEFRAGMENTSHADERATIPROC glDeleteFragmentShaderATI;
    extern PFNGLBEGINFRAGMENTSHADERATIPROC glBeginFragmentShaderATI;
    extern PFNGLENDFRAGMENTSHADERATIPROC glEndFragmentShaderATI;
    extern PFNGLPASSTEXCOORDATIPROC glPassTexCoordATI;
    extern PFNGLSAMPLEMAPATIPROC glSampleMapATI;
    extern PFNGLCOLORFRAGMENTOP1ATIPROC glColorFragmentOp1ATI;
    extern PFNGLCOLORFRAGMENTOP2ATIPROC glColorFragmentOp2ATI;
    extern PFNGLCOLORFRAGMENTOP3ATIPROC glColorFragmentOp3ATI;
    extern PFNGLALPHAFRAGMENTOP1ATIPROC glAlphaFragmentOp1ATI;
    extern PFNGLALPHAFRAGMENTOP2ATIPROC glAlphaFragmentOp2ATI;
    extern PFNGLALPHAFRAGMENTOP3ATIPROC glAlphaFragmentOp3ATI;
    extern PFNGLSETFRAGMENTSHADERCONSTANTATIPROC glSetFragmentShaderConstantATI;
#endif
#endif /* GL_ATI_fragment_shader */

#ifdef __cplusplus
}
#endif

#endif /* __glext_h_ */
