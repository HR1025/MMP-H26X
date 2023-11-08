//
// H264Common.h
//
// Library: Coedec
// Package: H264
// Module:  H264
// 

#pragma once

#include <iostream>
#include <cassert>

#include "H264Common.h"

namespace Mmp
{
namespace Codec
{

#ifdef MMP_H264_DEBUG_MODE
#define MPP_H264_SYNTAXT_STRICT_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        std::cerr << "[H264] " << "[" << __FILE__ << ":" << __LINE__ << "]" << msg << std::endl;\
                                                                        assert(false);\
                                                                        exp;\
                                                                    }
#define MPP_H264_SYNTAXT_NORMAL_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        std::cout << "[H264] " << "[" << __FILE__ << ":" << __LINE__ << "]" << msg << std::endl;\
                                                                        exp;\
                                                                    }
#else
#define MPP_H264_SYNTAXT_STRICT_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        exp;\
                                                                    }
#define MPP_H264_SYNTAXT_NORMAL_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        exp;\
                                                                    }
#endif /* MMP_H264_DEBUG_MODE */

/**
 * @sa ISO 14496/10(2020) - 8.2.1 Decoding process for picture order count
 */
void DecodeH264PictureOrderCount(H264SpsSyntax::ptr sps, H264SliceHeaderSyntax::ptr prevSlice, H264SliceHeaderSyntax::ptr curSlice, uint8_t nal_ref_idc, H264PocContext::ptr poc);

} // namespace Codec
} // namespace Mmp