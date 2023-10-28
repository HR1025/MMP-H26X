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

#define MMP_H264_DEBUG_MODE

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
                                                                        exp;
                                                                    }
#define MPP_H264_SYNTAXT_NORMAL_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        exp;
                                                                    }
#endif /* MMP_H264_DEBUG_MODE */

} // namespace Codec
} // namespace Mmp