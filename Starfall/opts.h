#pragma once
#include "ue.h"

constexpr bool Console = false; // if the console window should be created
constexpr StarfallURLSet URLSet = Default;
constexpr inline FString Backend = L"http://127.0.0.1:3551"; // your backend url
constexpr bool bHasPushWidget = false; // enable if you have gs closing after a couple seconds of listening. breaks closing the client (so don't build with this enabled for usage in a launcher.)

// misc options, don't change unless you know what you're doing
constexpr bool UseBackendParam = false; // for phoenix/paradise launcher
constexpr bool ManualMapping = false; // if you're using EAC & a manual mapper, then enable this for the dll to work
constexpr bool FixMemLeak = true; // memory leak fix