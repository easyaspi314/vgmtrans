#include "stdafx.h"
#include "RareSnesInstr.h"
#include "Format.h"
#include "SNESDSP.h"
#include "RareSnesFormat.h"

// ****************
// RareSnesInstrSet
// ****************

RareSnesInstrSet::RareSnesInstrSet(RawFile* file, ULONG offset, U32 spcDirAddr, const std::wstring & name) :
	VGMInstrSet(RareSnesFormat::name, file, offset, 0, name),
	spcDirAddr(spcDirAddr)
{
	unLength = 0x100;
	if (dwOffset + unLength > file->size())
	{
		unLength = file->size() - dwOffset;
	}
	ScanAvailableInstruments();
}

RareSnesInstrSet::~RareSnesInstrSet()
{
}


void RareSnesInstrSet::ScanAvailableInstruments()
{
	availInstruments.clear();

	bool firstZero = true;
	for (UINT inst = 0; inst < unLength; inst++)
	{
		BYTE srcn = GetByte(dwOffset + inst);

		if (srcn == 0 && !firstZero)
		{
			continue;
		}
		if (srcn == 0)
		{
			firstZero = false;
		}

		UINT offDirEnt = spcDirAddr + (srcn * 4);
		if (offDirEnt + 4 > 0x10000)
		{
			continue;
		}

		uint16_t addrSampStart = GetShort(offDirEnt);
		uint16_t addrSampLoop = GetShort(offDirEnt + 2);
		// valid loop?
		if (addrSampStart > addrSampLoop)
		{
			continue;
		}
		// not in DIR table
		if (addrSampStart < spcDirAddr + (256 * 4))
		{
			continue;
		}
		// address 0 is probably legit, but it should not be used
		if (addrSampStart == 0)
		{
			continue;
		}
		// Rare engine does not break the following rule... perhaps
		if (addrSampStart < spcDirAddr)
		{
			continue;
		}

		availInstruments.push_back(inst);
	}
}

bool RareSnesInstrSet::GetHeaderInfo()
{
	return true;
}

bool RareSnesInstrSet::GetInstrPointers()
{
	for (std::vector<BYTE>::iterator itr = availInstruments.begin(); itr != availInstruments.end(); ++itr)
	{
		BYTE inst = (*itr);
		BYTE srcn = GetByte(dwOffset + inst);
		RareSnesInstr * newInstr = new RareSnesInstr(this, dwOffset + inst, inst >> 7, inst & 0x7f, spcDirAddr);
		aInstrs.push_back(newInstr);
	}
	return aInstrs.size() != 0;
}

const std::vector<BYTE>& RareSnesInstrSet::GetAvailableInstruments()
{
	return availInstruments;
}

// *************
// RareSnesInstr
// *************

RareSnesInstr::RareSnesInstr(VGMInstrSet* instrSet, ULONG offset, ULONG theBank, ULONG theInstrNum, U32 spcDirAddr, const wstring& name) :
	VGMInstr(instrSet, offset, 1, theBank, theInstrNum, name),
	spcDirAddr(spcDirAddr)
{
}

RareSnesInstr::~RareSnesInstr()
{
}

bool RareSnesInstr::LoadInstr()
{
	BYTE srcn = GetByte(dwOffset);
	UINT offDirEnt = spcDirAddr + (srcn * 4);
	if (offDirEnt + 4 > 0x10000)
	{
		return false;
	}

	uint16_t addrSampStart = GetShort(offDirEnt);

	RareSnesRgn * rgn = new RareSnesRgn(this, dwOffset);
	rgn->sampOffset = addrSampStart;
	aRgns.push_back(rgn);
	return true;
}

// ***********
// RareSnesRgn
// ***********

RareSnesRgn::RareSnesRgn(RareSnesInstr* instr, ULONG offset) :
	VGMRgn(instr, offset)
{
}

RareSnesRgn::~RareSnesRgn()
{
}

bool RareSnesRgn::LoadRgn()
{
	return true;
}