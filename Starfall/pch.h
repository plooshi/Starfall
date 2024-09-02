#ifndef PCH_H
#define PCH_H

#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <array>
#include <psapi.h>
#include <string_view>
#include <string>

template <size_t _Sz>
struct String {
    char _St[_Sz];

    consteval String(const char(&_Ps)[_Sz])
    {
        std::copy_n(_Ps, _Sz, _St);
    }

    operator const char* () {
        return _St;
    }

    constexpr std::string_view StringView() const {
        return _St;
    }
};

template <typename _Ft>
struct Func {
    _Ft _Fn;

    consteval Func(_Ft _Pf)
    {
        _Fn = _Pf;
    }

    constexpr _Ft Get() const {
        return _Fn;
    }
};
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
			Fortnite,
			Generic427 // found in latest EGL
		};
	}
	using namespace Types;
}
using namespace Starfall;

#endif
