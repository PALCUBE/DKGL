//
//  File: DKAudioStreamWave.cpp
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2004-2015 Hongtae Kim. All rights reserved.
//

#include <memory.h>
#include "DKAudioStreamWave.h"


#ifdef _WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif


using namespace DKFoundation;

#pragma pack(push, 4)

namespace DKFramework
{
	namespace Private
	{
		enum WaveFormatType
		{
			WaveFormatTypeUnknown = 0,
			WaveFormatTypePCM = 1,			// WAVE_FORMAT_PCM
			WaveFormatTypeEXT = 0xFFFE,		// WAVE_FORMAT_EXTENSIBLE
		};
		struct WaveFileHeader
		{
			char riff[4];
			unsigned int riffSize;
			char wave[4];
		};
		struct RiffChunk
		{
			char name[4];
			unsigned int size;
		};
		struct WaveFormat
		{
			unsigned short formatTag;
			unsigned short channels;
			unsigned int samplesPerSec;
			unsigned int avgBytesPerSec;
			unsigned short blockAlign;
			unsigned short bitsPerSample;
			unsigned short size;
			unsigned short reserved;
			unsigned int channelMask;
			unsigned char subformatGuid[16];
		};
		struct WaveFormatPCM
		{
			unsigned short tag;
			unsigned short channels;
			unsigned int samplesPerSec;
			unsigned int avgBytesPerSec;
			unsigned short blockAlign;
			unsigned short bitsPerSample;
		};
		struct WaveFormatEX
		{
			unsigned short tag;
			unsigned short channels;
			unsigned int samplesPerSec;
			unsigned int avgBytesPerSec;
			unsigned short blockAlign;
			unsigned short bitsPerSample;
			unsigned short size;
			char unused[2];
		};
		struct WaveFormatExt
		{
			WaveFormatEX format;
			union
			{
				unsigned short validBitsPerSample;
				unsigned short samplesPerBlock;
				unsigned short reserved;
			} samples;
			unsigned int channelMask;

			struct
			{
				unsigned int	data1;
				unsigned short	data2;
				unsigned short	data3;
				unsigned char	data4[8];
			} subFormatGUID;
		};
		struct WaveFileContext
		{
			DKObject<DKStream>	stream;
			WaveFormatType		formatType;
			WaveFormatExt		formatExt;

			char* data;
			size_t dataSize;
			size_t dataOffset;
		};
	}
}

#pragma pack(pop)

using namespace DKFramework;
using namespace DKFramework::Private;

static_assert(sizeof(WaveFileHeader) == 12, "sizeof(WaveFileHeader) == 12");
static_assert(sizeof(RiffChunk) == 8, "sizeof(RiffChunk) == 8");
static_assert(sizeof(WaveFormat) == 40, "sizeof(WaveFormat) == 40");
static_assert(sizeof(WaveFormatPCM) == 16, "sizeof(WaveFormatPCM) == 16");
static_assert(sizeof(WaveFormatEX) == 20, "sizeof(WaveFormatEX) == 20");
static_assert(sizeof(WaveFormatExt) == 44, "sizeof(WaveFormatEX) == 44");


DKAudioStreamWave::DKAudioStreamWave(void)
	: DKAudioStream(DKAudioStream::FileTypeWave)
	, context(new WaveFileContext)
{
}

DKAudioStreamWave::~DKAudioStreamWave(void)
{
	context->stream = NULL;
	delete context;
}

bool DKAudioStreamWave::Open(const DKString& file)
{
	DKObject<DKFile> f = DKFile::Create(file, DKFile::ModeOpenReadOnly, DKFile::ModeShareAll);
	if (f)
	{
		return Open(f.SafeCast<DKStream>());
	}
	return false;
}

bool DKAudioStreamWave::Open(DKStream* stream)
{
	if (stream == NULL || !stream->IsReadable() || !stream->IsSeekable())
		return false;

	context->stream = NULL;
	context->dataSize = 0;
	context->dataOffset = 0;
	context->formatType = WaveFormatTypeUnknown;

	WaveFileHeader header;
	memset(&header, 0, sizeof(WaveFileHeader));
	if (stream->Read(&header, sizeof(WaveFileHeader)) == sizeof(WaveFileHeader))
	{
		if (strncasecmp(header.riff, "RIFF", 4) == 0 && strncasecmp(header.wave, "WAVE", 4) == 0)
		{
			// swap byte order (from little-endian to system)
			header.riffSize = DKLittleEndianToSystem(header.riffSize);

			// read all chunk
			RiffChunk chunk;
			while (stream->Read(&chunk, sizeof(RiffChunk)) == sizeof(RiffChunk))
			{
				chunk.size = DKLittleEndianToSystem(chunk.size);

				if (strncasecmp(chunk.name, "fmt ", 4) == 0)
				{
					if (chunk.size <= sizeof(WaveFormat))
					{
						WaveFormat format;
						memset(&format, 0, sizeof(WaveFormat));
						if (stream->Read(&format, chunk.size) == chunk.size)
						{
							// swap byte order
							format.formatTag = DKLittleEndianToSystem(format.formatTag);
							format.channels = DKLittleEndianToSystem(format.channels);
							format.samplesPerSec = DKLittleEndianToSystem(format.samplesPerSec);
							format.avgBytesPerSec = DKLittleEndianToSystem(format.avgBytesPerSec);
							format.blockAlign = DKLittleEndianToSystem(format.blockAlign);
							format.bitsPerSample = DKLittleEndianToSystem(format.bitsPerSample);
							format.size = DKLittleEndianToSystem(format.size);
							format.reserved = DKLittleEndianToSystem(format.reserved);
							format.channelMask = DKLittleEndianToSystem(format.channelMask);

							if (format.formatTag == WaveFormatTypePCM)
							{
								context->formatType = WaveFormatTypePCM;
								memcpy(&context->formatExt.format, &format, sizeof(WaveFormatPCM));
							}
							else if (format.formatTag == WaveFormatTypeEXT)
							{
								context->formatType = WaveFormatTypeEXT;
								memcpy(&context->formatExt, &format, sizeof(WaveFormatExt));
							}
							else
							{
							//	DKLog("DKAudioStreamWave: Unknown format! (0x%x)", format.formatTag);
							}
						}
						else
						{
							DKLog("DKAudioStreamWave: Read error!\n");
							return false;
						}
					}
					else
					{
						stream->SetPos(stream->GetPos() + chunk.size);
					}
				}
				else if (strncasecmp(chunk.name, "data", 4) == 0)
				{
					context->dataSize = chunk.size;
					context->dataOffset = stream->GetPos();
					stream->SetPos(stream->GetPos() + chunk.size);
				}
				else
				{
					stream->SetPos(stream->GetPos() + chunk.size);
				}

				if (chunk.size & 1)	// byte align
					stream->SetPos(stream->GetPos() + 1);
			}

			//DKLog("DKAudioStreamWave: dataSize:%d", (int)context->dataSize);
			//DKLog("DKAudioStreamWave: dataOffset:%d", (int)context->dataOffset);
			//DKLog("DKAudioStreamWave: formatType:%d", (int)context->formatType);

			if (context->dataSize && context->dataOffset &&
				(context->formatType == WaveFormatTypePCM || context->formatType == WaveFormatTypeEXT))
			{
				context->stream = stream;
				SetChannels(context->formatExt.format.channels);
				SetFrequency(context->formatExt.format.samplesPerSec);
				SetBits(context->formatExt.format.bitsPerSample);
				SetSeekable(true);
				return true;
			}
		}
	}

	context->stream = NULL;
	context->dataSize = 0;
	context->dataOffset = 0;
	context->formatType = WaveFormatTypeUnknown;
	return false;
}

size_t DKAudioStreamWave::Read(void* buffer, size_t size)
{
	if (context->stream)
	{
		size_t pos = context->stream->GetPos();
		if (pos + size > context->dataSize)
			size = context->dataSize - pos;

		if (size > context->formatExt.format.blockAlign)
		{
			// buffer should be aligned with format.blockAlign
			if (context->formatExt.format.blockAlign > 0)
				size = size - (size % context->formatExt.format.blockAlign);

			if (size > 0)
				return context->stream->Read(buffer, size);
		}
	}
	return 0;
}

DKAudioStream::Position DKAudioStreamWave::SeekRaw(Position pos)
{
	if (context->stream)
	{
		// alignment
		if (context->formatExt.format.blockAlign > 0)
			pos = pos - (pos % context->formatExt.format.blockAlign);

		pos = context->stream->SetPos(static_cast<Position>(context->dataOffset) + Clamp(pos, 0, context->dataSize));
		return Clamp<Position>(pos - context->dataOffset, 0, context->dataSize);
	}
	return 0;
}

DKAudioStream::Position DKAudioStreamWave::SeekPcm(Position pos)
{
	return SeekRaw(pos);
}

double DKAudioStreamWave::SeekTime(double s)
{
	if (context->stream)
	{
		Position pos = SeekRaw(static_cast<Position>(s * static_cast<double>(context->formatExt.format.avgBytesPerSec)));
		return static_cast<double>(pos) / static_cast<double>(context->formatExt.format.avgBytesPerSec);
	}
	return 0;
}

DKAudioStream::Position DKAudioStreamWave::RawPos(void) const
{
	if (context->stream)
	{
		return context->stream->GetPos() - static_cast<Position>(context->dataOffset);
	}
	return 0;
}

DKAudioStream::Position DKAudioStreamWave::PcmPos(void) const
{
	return RawPos();
}

double DKAudioStreamWave::TimePos(void) const
{
	if (context->stream)
	{
		Position pos = context->stream->GetPos() - static_cast<Position>(context->dataOffset);
		return static_cast<double>(pos) / static_cast<double>(context->formatExt.format.avgBytesPerSec);
	}
	return 0;
}

DKAudioStream::Position DKAudioStreamWave::RawTotal(void) const
{
	if (context->stream)
	{
		return context->dataSize;
	}
	return 0;
}

DKAudioStream::Position DKAudioStreamWave::PcmTotal(void) const
{
	return RawTotal();
}

double DKAudioStreamWave::TimeTotal(void) const
{
	if (context->stream)
	{
		return static_cast<double>(context->dataSize) / static_cast<double>(context->formatExt.format.avgBytesPerSec);
	}
	return 0;
}
