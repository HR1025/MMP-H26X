cmake_minimum_required(VERSION 3.8)
project(MMP_H264 DESCRIPTION "Multi Media Plugin - H264 Deserialize and Serialize")

option(MMP_H264_DEBUG_MODE "Enable debug mode" ON)
option(ENBALE_MMP_H264_SAMPLE "Enbale MMP H264 Sampele" ON)

set(MMP_H264_SRCS)
set(MMP_H264_INCS)

list(APPEND MMP_H264_INCS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# H264
list(APPEND MMP_H264_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/H264Common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264Common.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AbstractH264ByteReader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264Ultis.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264Ultis.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/H264BinaryReader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264BinaryReader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/H264Deserialize.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H264Deserialize.cpp
)

# H265
list(APPEND MMP_H264_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/H265Common.h
    ${CMAKE_CURRENT_SOURCE_DIR}/H265Common.cpp
)

add_library(MMP_H264 STATIC ${MMP_H264_SRCS})
add_library(MMP::H264 ALIAS MMP_H264)
target_include_directories(MMP_H264 PUBLIC ${MMP_H264_INCS})
if (MMP_H264_DEBUG_MODE)
    target_compile_definitions(MMP_H264 PUBLIC MMP_H264_DEBUG_MODE)
endif()

if (ENBALE_MMP_H264_SAMPLE)
    add_executable(Sample ${MMP_H264_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    target_include_directories(Sample PUBLIC ${MMP_H264_INCS})
    if (MMP_H264_DEBUG_MODE)
        target_compile_definitions(Sample PUBLIC MMP_H264_DEBUG_MODE)
    endif()
    if (UNIX)
        target_link_libraries(Sample asan)
        target_compile_options(Sample PUBLIC -fsanitize=address)
    endif()
endif()