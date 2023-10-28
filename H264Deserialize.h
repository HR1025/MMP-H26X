//
// H264Deserialize.h
//
// Library: Coedec
// Package: H264
// Module:  H264
// 

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "H264Common.h"
#include "H264BinaryReader.h"

namespace Mmp
{
namespace Codec
{

class H264Deserialize
{
public:
    using ptr = std::shared_ptr<H264Deserialize>;
public:
    H264Deserialize();
    ~H264Deserialize();
public:
    bool DeserializeNalSyntax(H264BinaryReader::ptr br, H264NalSyntax::ptr nal, uint64_t NumBytesInNALuint);
    bool DeserializeHrdSyntax(H264BinaryReader::ptr br, H264HrdSyntax::ptr hrd);
    bool DeserializeVuiSyntax(H264BinaryReader::ptr br, H264VuiSyntax::ptr vui);
    bool DeserializeSeiSyntax(H264BinaryReader::ptr br, H264VuiSyntax::ptr vui, H264SeiSyntax::ptr sei);
    bool DeserializeSpsSyntax(H264BinaryReader::ptr br, H264SpsSyntax::ptr sps);
    bool DeserializeSliceSyntax(H264BinaryReader::ptr br, H264SliceSyntax::ptr slice);
    bool DeserializeDecodedReferencePictureMarkingSyntax(H264BinaryReader::ptr br, H264NalSyntax::ptr nal, H264DecodedReferencePictureMarkingSyntax::ptr drpm);
    bool DeserializeSubSpsSyntax(H264BinaryReader::ptr br, H264SpsSyntax::ptr sps, H264SubSpsSyntax::ptr subSps);
    bool DeserializeSpsMvcSyntax(H264BinaryReader::ptr br, H264SpsMvcSyntax::ptr mvc);
    bool DeserializeMvcVuiSyntax(H264BinaryReader::ptr br, H264MvcVuiSyntax::ptr mvcVui);
    bool DeserializePpsSyntax(H264BinaryReader::ptr br, H264PpsSyntax::ptr pps);
private:
    bool DeserializeNalSvcSyntax(H264BinaryReader::ptr br, H264NalSvcSyntax::ptr svc);
    bool DeserializeNal3dAvcSyntax(H264BinaryReader::ptr br, H264Nal3dAvcSyntax::ptr avc);
    bool DeserializeNalMvcSyntax(H264BinaryReader::ptr br, H264NalMvcSyntax::ptr mvc);
    bool DeserializeScalingListSyntax(H264BinaryReader::ptr br, std::vector<int32_t>& scalingList, int32_t sizeOfScalingList, int32_t& useDefaultScalingMatrixFlag);
    bool DeserializeReferencePictureListModificationSyntax(H264BinaryReader::ptr br, H264SliceSyntax::ptr slice, H264ReferencePictureListModificationSyntax::ptr rplm);
    bool DeserializePredictionWeightTableSyntax(H264BinaryReader::ptr br, H264SpsSyntax::ptr sps, H264SliceSyntax::ptr slice, H264PredictionWeightTableSyntax::ptr pwt);
private: /* SEI */
    bool DeserializeSeiBufferPeriodSyntax(H264BinaryReader::ptr br, H264SeiBufferPeriodSyntax::ptr bp);
    bool DeserializeSeiPictureTimingSyntax(H264BinaryReader::ptr br, H264VuiSyntax::ptr vui, H264SeiPictureTimingSyntax::ptr pt);
    bool DeserializeSeiRecoveryPointSyntax(H264BinaryReader::ptr br, H264SeiRecoveryPointSyntax::ptr pt);
    bool DeserializeSeiContentLigntLevelInfoSyntax(H264BinaryReader::ptr br, H264SeiContentLigntLevelInfoSyntax::ptr clli);
    bool DeserializeSeiDisplayOrientationSyntax(H264BinaryReader::ptr br, H264SeiDisplayOrientation::ptr dot);
    bool DeserializeSeiFilmGrainSyntax(H264BinaryReader::ptr br, H264SeiFilmGrainSyntax::ptr fg);
    bool DeserializeSeiFramePackingArrangementSyntax(H264BinaryReader::ptr br, H264SeiFramePackingArrangement::ptr fpa);
private:
    H264ContextSyntax::ptr _contex;
};

} // namespace Codec
} // namespace Mmp