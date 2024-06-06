#pragma once
#include "pch.h"
#include "url.h"

namespace Starfall {
    namespace Redirection {
        bool shouldRedirect(URL* uri);
    }
    using namespace Redirection;
}