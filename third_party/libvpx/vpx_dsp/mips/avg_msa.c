/*
 *  Copyright (c) 2015 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "./vpx_dsp_rtcd.h"
#include "vpx_dsp/mips/macros_msa.h"

uint32_t vpx_avg_8x8_msa(const uint8_t *src, int32_t src_stride) {
  uint32_t sum_out;
  v16u8 src0, src1, src2, src3, src4, src5, src6, src7;
  v8u16 sum0, sum1, sum2, sum3, sum4, sum5, sum6, sum7;
  v4u32 sum = { 0 };

  LD_UB8(src, src_stride, src0, src1, src2, src3, src4, src5, src6, src7);
  HADD_UB4_UH(src0, src1, src2, src3, sum0, sum1, sum2, sum3);
  HADD_UB4_UH(src4, src5, src6, src7, sum4, sum5, sum6, sum7);
  ADD4(sum0, sum1, sum2, sum3, sum4, sum5, sum6, sum7, sum0, sum2, sum4, sum6);
  ADD2(sum0, sum2, sum4, sum6, sum0, sum4);
  sum0 += sum4;

  sum = __msa_hadd_u_w(sum0, sum0);
  sum0 = (v8u16)__msa_pckev_h((v8i16)sum, (v8i16)sum);
  sum = __msa_hadd_u_w(sum0, sum0);
  sum = (v4u32)__msa_srari_w((v4i32)sum, 6);
  sum_out = __msa_copy_u_w((v4i32)sum, 0);

  return sum_out;
}

uint32_t vpx_avg_4x4_msa(const uint8_t *src, int32_t src_stride) {
  uint32_t sum_out;
  uint32_t src0, src1, src2, src3;
  v16u8 vec = { 0 };
  v8u16 sum0;
  v4u32 sum1;
  v2u64 sum2;

  LW4(src, src_stride, src0, src1, src2, src3);
  INSERT_W4_UB(src0, src1, src2, src3, vec);

  sum0 = __msa_hadd_u_h(vec, vec);
  sum1 = __msa_hadd_u_w(sum0, sum0);
  sum0 = (v8u16)__msa_pckev_h((v8i16)sum1, (v8i16)sum1);
  sum1 = __msa_hadd_u_w(sum0, sum0);
  sum2 = __msa_hadd_u_d(sum1, sum1);
  sum1 = (v4u32)__msa_srari_w((v4i32)sum2, 4);
  sum_out = __msa_copy_u_w((v4i32)sum1, 0);

  return sum_out;
}

void vpx_hadamard_8x8_msa(const int16_t *src, int src_stride, int16_t *dst) {
  v8i16 src0, src1, src2, src3, src4, src5, src6, src7;
  v8i16 tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;

  LD_SH8(src, src_stride, src0, src1, src2, src3, src4, src5, src6, src7);
  BUTTERFLY_8(src0, src2, src4, src6, src7, src5, src3, src1, tmp0, tmp2, tmp4,
              tmp6, tmp7, tmp5, tmp3, tmp1);
  BUTTERFLY_8(tmp0, tmp1, tmp4, tmp5, tmp7, tmp6, tmp3, tmp2, src0, src1, src4,
              src5, src7, src6, src3, src2);
  BUTTERFLY_8(src0, src1, src2, src3, src7, src6, src5, src4, tmp0, tmp7, tmp3,
              tmp4, tmp5, tmp1, tmp6, tmp2);
  TRANSPOSE8x8_SH_SH(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, src0, src1,
                     src2, src3, src4, src5, src6, src7);
  BUTTERFLY_8(src0, src2, src4, src6, src7, src5, src3, src1, tmp0, tmp2, tmp4,
              tmp6, tmp7, tmp5, tmp3, tmp1);
  BUTTERFLY_8(tmp0, tmp1, tmp4, tmp5, tmp7, tmp6, tmp3, tmp2, src0, src1, src4,
              src5, src7, src6, src3, src2);
  BUTTERFLY_8(src0, src1, src2, src3, src7, src6, src5, src4, tmp0, tmp7, tmp3,
              tmp4, tmp5, tmp1, tmp6, tmp2);
  TRANSPOSE8x8_SH_SH(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, src0, src1,
                     src2, src3, src4, src5, src6, src7);
  ST_SH8(src0, src1, src2, src3, src4, src5, src6, src7, dst, 8);
}

void vpx_hadamard_16x16_msa(const int16_t *src, int src_stride, int16_t *dst) {
  v8i16 src0, src1, src2, src3, src4, src5, src6, src7, src8, src9, src10;
  v8i16 src11, src12, src13, src14, src15, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5;
  v8i16 tmp6, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15;
  v8i16 res0, res1, res2, res3, res4, res5, res6, res7;

  LD_SH2(src, 8, src0, src8);
  src += src_stride;
  LD_SH2(src, 8, src1, src9);
  src += src_stride;
  LD_SH2(src, 8, src2, src10);
  src += src_stride;
  LD_SH2(src, 8, src3, src11);
  src += src_stride;
  LD_SH2(src, 8, src4, src12);
  src += src_stride;
  LD_SH2(src, 8, src5, src13);
  src += src_stride;
  LD_SH2(src, 8, src6, src14);
  src += src_stride;
  LD_SH2(src, 8, src7, src15);
  src += src_stride;

  BUTTERFLY_8(src0, src2, src4, src6, src7, src5, src3, src1, tmp0, tmp2, tmp4,
              tmp6, tmp7, tmp5, tmp3, tmp1);
  BUTTERFLY_8(src8, src10, src12, src14, src15, src13, src11, src9, tmp8, tmp10,
              tmp12, tmp14, tmp15, tmp13, tmp11, tmp9);

  BUTTERFLY_8(tmp0, tmp1, tmp4, tmp5, tmp7, tmp6, tmp3, tmp2, src0, src1, src4,
              src5, src7, src6, src3, src2);
  BUTTERFLY_8(src0, src1, src2, src3, src7, src6, src5, src4, tmp0, tmp7, tmp3,
              tmp4, tmp5, tmp1, tmp6, tmp2);
  TRANSPOSE8x8_SH_SH(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, src0, src1,
                     src2, src3, src4, src5, src6, src7);
  BUTTERFLY_8(src0, src2, src4, src6, src7, src5, src3, src1, tmp0, tmp2, tmp4,
              tmp6, tmp7, tmp5, tmp3, tmp1);
  BUTTERFLY_8(tmp0, tmp1, tmp4, tmp5, tmp7, tmp6, tmp3, tmp2, src0, src1, src4,
              src5, src7, src6, src3, src2);
  BUTTERFLY_8(src0, src1, src2, src3, src7, src6, src5, src4, tmp0, tmp7, tmp3,
              tmp4, tmp5, tmp1, tmp6, tmp2);
  TRANSPOSE8x8_SH_SH(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, src0, src1,
                     src2, src11, src4, src5, src6, src7);
  ST_SH8(src0, src1, src2, src11, src4, src5, src6, src7, dst, 8);

  BUTTERFLY_8(tmp8, tmp9, tmp12, tmp13, tmp15, tmp14, tmp11, tmp10, src8, src9,
              src12, src13, src15, src14, src11, src10);
  BUTTERFLY_8(src8, src9, src10, src11, src15, src14, src13, src12, tmp8, tmp15,
              tmp11, tmp12, tmp13, tmp9, tmp14, tmp10);
  TRANSPOSE8x8_SH_SH(tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, src8,
                     src9, src10, src11, src12, src13, src14, src15);
  BUTTERFLY_8(src8, src10, src12, src14, src15, src13, src11, src9, tmp8, tmp10,
              tmp12, tmp14, tmp15, tmp13, tmp11, tmp9);
  BUTTERFLY_8(tmp8, tmp9, tmp12, tmp13, tmp15, tmp14, tmp11, tmp10, src8, src9,
              src12, src13, src15, src14, src11, src10);
  BUTTERFLY_8(src8, src9, src10, src11, src15, src14, src13, src12, tmp8, tmp15,
              tmp11, tmp12, tmp13, tmp9, tmp14, tmp10);
  TRANSPOSE8x8_SH_SH(tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, res0,
                     res1, res2, res3, res4, res5, res6, res7);

  LD_SH2(src, 8, src0, src8);
  src += src_stride;
  LD_SH2(src, 8, src1, src9);
  src += src_stride;
  LD_SH2(src, 8, src2, src10);
  src += src_stride;
  LD_SH2(src, 8, src3, src11);
  src += src_stride;

  ST_SH8(res0, res1, res2, res3, res4, res5, res6, res7, dst + 64, 8);

  LD_SH2(src, 8, src4, src12);
  src += src_stride;
  LD_SH2(src, 8, src5, src13);
  src += src_stride;
  LD_SH2(src, 8, src6, src14);
  src += src_stride;
  LD_SH2(src, 8, src7, src15);
  src += src_stride;

  BUTTERFLY_8(src0, src2, src4, src6, src7, src5, src3, src1, tmp0, tmp2, tmp4,
              tmp6, tmp7, tmp5, tmp3, tmp1);
  BUTTERFLY_8(src8, src10, src12, src14, src15, src13, src11, src9, tmp8, tmp10,
              tmp12, tmp14, tmp15, tmp13, tmp11, tmp9);

  BUTTERFLY_8(tmp0, tmp1, tmp4, tmp5, tmp7, tmp6, tmp3, tmp2, src0, src1, src4,
              src5, src7, src6, src3, src2);
  BUTTERFLY_8(src0, src1, src2, src3, src7, src6, src5, src4, tmp0, tmp7, tmp3,
              tmp4, tmp5, tmp1, tmp6, tmp2);
  TRANSPOSE8x8_SH_SH(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, src0, src1,
                     src2, src3, src4, src5, src6, src7);
  BUTTERFLY_8(src0, src2, src4, src6, src7, src5, src3, src1, tmp0, tmp2, tmp4,
              tmp6, tmp7, tmp5, tmp3, tmp1);
  BUTTERFLY_8(tmp0, tmp1, tmp4, tmp5, tmp7, tmp6, tmp3, tmp2, src0, src1, src4,
              src5, src7, src6, src3, src2);
  BUTTERFLY_8(src0, src1, src2, src3, src7, src6, src5, src4, tmp0, tmp7, tmp3,
              tmp4, tmp5, tmp1, tmp6, tmp2);
  TRANSPOSE8x8_SH_SH(tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, src0, src1,
                     src2, src3, src4, src5, src6, src7);
  ST_SH8(src0, src1, src2, src3, src4, src5, src6, src7, dst + 2 * 64, 8);

  BUTTERFLY_8(tmp8, tmp9, tmp12, tmp13, tmp15, tmp14, tmp11, tmp10, src8, src9,
              src12, src13, src15, src14, src11, src10);
  BUTTERFLY_8(src8, src9, src10, src11, src15, src14, src13, src12, tmp8, tmp15,
              tmp11, tmp12, tmp13, tmp9, tmp14, tmp10);
  TRANSPOSE8x8_SH_SH(tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, src8,
                     src9, src10, src11, src12, src13, src14, src15);
  BUTTERFLY_8(src8, src10, src12, src14, src15, src13, src11, src9, tmp8, tmp10,
              tmp12, tmp14, tmp15, tmp13, tmp11, tmp9);
  BUTTERFLY_8(tmp8, tmp9, tmp12, tmp13, tmp15, tmp14, tmp11, tmp10, src8, src9,
              src12, src13, src15, src14, src11, src10);
  BUTTERFLY_8(src8, src9, src10, src11, src15, src14, src13, src12, tmp8, tmp15,
              tmp11, tmp12, tmp13, tmp9, tmp14, tmp10);
  TRANSPOSE8x8_SH_SH(tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, res0,
                     res1, res2, res3, res4, res5, res6, res7);
  ST_SH8(res0, res1, res2, res3, res4, res5, res6, res7, dst + 3 * 64, 8);

  LD_SH4(dst, 64, src0, src1, src2, src3);
  LD_SH4(dst + 8, 64, src4, src5, src6, src7);

  BUTTERFLY_8(src0, src2, src4, src6, src7, src5, src3, src1, tmp0, tmp2, tmp4,
              tmp6, tmp7, tmp5, tmp3, tmp1);
  SRA_4V(tmp0, tmp1, tmp2, tmp3, 1);
  SRA_4V(tmp4, tmp5, tmp6, tmp7, 1);
  BUTTERFLY_8(tmp0, tmp1, tmp4, tmp5, tmp7, tmp6, tmp3, tmp2, src0, src1, src4,
              src5, src7, src6, src3, src2);

  ST_SH4(src0, src1, src2, src3, dst, 64);
  ST_SH4(src4, src5, src6, src7, dst + 8, 64);
  dst += 16;

  LD_SH4(dst, 64, src0, src1, src2, src3);
  LD_SH4(dst + 8, 64, src4, src5, src6, src7);

  BUTTERFLY_8(src0, src2, src4, src6, src7, src5, src3, src1, tmp0, tmp2, tmp4,
              tmp6, tmp7, tmp5, tmp3, tmp1);
  SRA_4V(tmp0, tmp1, tmp2, tmp3, 1);
  SRA_4V(tmp4, tmp5, tmp6, tmp7, 1);
  BUTTERFLY_8(tmp0, tmp1, tmp4, tmp5, tmp7, tmp6, tmp3, tmp2, src0, src1, src4,
              src5, src7, src6, src3, src2);

  ST_SH4(src0, src1, src2, src3, dst, 64);
  ST_SH4(src4, src5, src6, src7, dst + 8, 64);
  dst += 16;

  LD_SH4(dst, 64, src0, src1, src2, src3);
  LD_SH4(dst + 8, 64, src4, src5, src6, src7);

  BUTTERFLY_8(src0, src2, src4, src6, src7, src5, src3, src1, tmp0, tmp2, tmp4,
              tmp6, tmp7, tmp5, tmp3, tmp1);
  SRA_4V(tmp0, tmp1, tmp2, tmp3, 1);
  SRA_4V(tmp4, tmp5, tmp6, tmp7, 1);
  BUTTERFLY_8(tmp0, tmp1, tmp4, tmp5, tmp7, tmp6, tmp3, tmp2, src0, src1, src4,
              src5, src7, src6, src3, src2);

  ST_SH4(src0, src1, src2, src3, dst, 64);
  ST_SH4(src4, src5, src6, src7, dst + 8, 64);
  dst += 16;

  LD_SH4(dst, 64, src0, src1, src2, src3);
  LD_SH4(dst + 8, 64, src4, src5, src6, src7);

  BUTTERFLY_8(src0, src2, src4, src6, src7, src5, src3, src1, tmp0, tmp2, tmp4,
              tmp6, tmp7, tmp5, tmp3, tmp1);
  SRA_4V(tmp0, tmp1, tmp2, tmp3, 1);
  SRA_4V(tmp4, tmp5, tmp6, tmp7, 1);
  BUTTERFLY_8(tmp0, tmp1, tmp4, tmp5, tmp7, tmp6, tmp3, tmp2, src0, src1, src4,
              src5, src7, src6, src3, src2);

  ST_SH4(src0, src1, src2, src3, dst, 64);
  ST_SH4(src4, src5, src6, src7, dst + 8, 64);
}
