#pragma once
#include "opts.h"
#include "pch.h"

namespace Starfall {
	namespace Globals {
		extern void* buf;
		//extern size_t size;
		extern void* EOSBuf;

		extern void* tbuf;
		extern size_t tsize;

		extern void* rbuf;
		extern size_t rsize;


		extern void* EOSTextBuf;
		extern size_t EOSTextSize;

		extern void* EOSRDataBuf;
		extern size_t EOSRDataSize;
	}
	using namespace Globals;
};
using namespace Starfall;

#define Log(type, ...) if (Console) printf("LogStarfall: " #type ": " __VA_ARGS__)