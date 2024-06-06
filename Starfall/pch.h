#ifndef PCH_H
#define PCH_H

#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <array>
#include <psapi.h>
#include <string_view>

namespace Plooshfinder {
	#include "../plooshfinder/include/plooshfinder_sig.h"
	#include "../plooshfinder/include/plooshfinder.h"
	#include "../plooshfinder/include/formats/pe.h"
};
using namespace Plooshfinder;

namespace Starfall {
	namespace Types {
		enum StarfallURLSet {
			Default, // default, private server
			Hybrid, // redirect profile, version, and content pages to private server, otherwise use official
			Dev, // redirect profile & content pages to private server, otherwise use official
			All, // redirect every single request to private server
		};
		enum UEGame {
			Generic,
			Fortnite
		};
	}
	using namespace Types;
}
using namespace Starfall;

#endif
