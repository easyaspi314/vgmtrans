#include "stdafx.h"
#include "common.h"
#include "VGMMultiSectionSeq.h"
#include "VGMSeqSection.h"
#include "SeqTrack.h"

VGMSeqSection::VGMSeqSection(VGMMultiSectionSeq* parentFile, uint32_t theOffset, uint32_t theLength, const std::wstring theName, uint8_t color)
	: VGMContainerItem(parentFile, theOffset, theLength, theName, color),
	parentSeq(parentFile)
{
	AddContainer<SeqTrack>(aTracks);
}

VGMSeqSection::~VGMSeqSection(void)
{
	DeleteVect<SeqTrack>(aTracks);
}

bool VGMSeqSection::Load()
{
	return GetTrackPointers();
}

bool VGMSeqSection::GetTrackPointers()
{
	return true;
}
