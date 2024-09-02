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
struct ConstexprString {
    char _St[_Sz];

public:
    consteval ConstexprString(const char(&_Ps)[_Sz])
    {
        std::copy_n(_Ps, _Sz, _St);
    }

    operator const char* () {
        return _St;
    }

    constexpr std::string_view StringView() const {
        return _St;
    }

    constexpr int PatternCount() const {
        int c = 0;
        for (int i = 0; i < _Sz; i++) {
            if (_St[i] == ' ') c++;
        }
        return c + 1; // last i think
    }
};

template <typename _Ft>
struct ConstexprFunc {
    _Ft _Fn;

public:
    consteval ConstexprFunc(_Ft _Pf)
    {
        _Fn = _Pf;
    }

    constexpr _Ft Get() const {
        return _Fn;
    }
};

template <size_t _Sz>
struct ConstexprArray {
    uint8_t _Ar[_Sz];
public:
    consteval ConstexprArray(std::array<uint8_t, _Sz> _Pa) {
        std::copy_n(_Pa.data(), _Sz, _Ar);
    }

    constexpr void* Get() const {
        return (void*)_Ar;
    }
};

namespace Plooshfinder {
    #include "../plooshfinder/include/plooshfinder.h"
}

template <ConstexprString _St, ConstexprFunc _Cb, ConstexprArray _Ma, ConstexprArray _Mk>
class ConstexprPatch {
public:
    constexpr Plooshfinder::pf_patch_t Create() {
        return pf_construct_patch((void*)_Ma.Get(), (void*)_Mk.Get(), _St.PatternCount(), _Cb.Get());
    }
};

namespace Plooshfinder {
	#include "../plooshfinder/include/plooshfinder_sig.h"
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
