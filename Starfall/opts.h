#pragma once
#include "ue.h"

constexpr bool Console = true; // if the console window should be created
constexpr StarfallURLSet URLSet = Default;
constexpr inline FString Backend = L"http://127.0.0.1:3551"; // your backend url
constexpr UEGame Game = Fortnite; // use Generic for any UE game other than fortnite

// misc options, don't change unless you know what you're doing
constexpr bool UseBackendParam = false; // for phoenix/paradise launcher
constexpr bool ManualMapping = false; // if you're using EAC & a manual mapper, then enable this for the dll to work
constexpr bool FixMemLeak = true; // memory leak fix