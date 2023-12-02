//
// H26xUltis.h
//
// Library: Coedec
// Package: H26x
// Module:  H26x
// 

#pragma once

#include <iostream>
#include <cassert>

namespace Mmp
{
namespace Codec
{

#ifdef MMP_H26X_DEBUG_MODE
#define MPP_H26X_SYNTAXT_STRICT_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        std::cerr << "[H264] " << "[" << __FILE__ << ":" << __LINE__ << "]" << msg << std::endl;\
                                                                        assert(false);\
                                                                        exp;\
                                                                    }
#define MPP_H26X_SYNTAXT_NORMAL_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        std::cout << "[H264] " << "[" << __FILE__ << ":" << __LINE__ << "]" << msg << std::endl;\
                                                                        exp;\
                                                                    }
#else
#define MPP_H26X_SYNTAXT_STRICT_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        exp;\
                                                                    }
#define MPP_H26X_SYNTAXT_NORMAL_CHECK(cond, msg, exp)               if (!(cond))\
                                                                    {\
                                                                        exp;\
                                                                    }
#endif /* MMP_H26X_DEBUG_MODE */

} // namespace Codec
} // namespace Mmp