#pragma once
#include "pch.h"

namespace Starfall {
    namespace Callbacks {
        bool RequestExitWithStatusCallback(struct pf_patch_t* patch, void* stream);

        bool PushWidgetCallback(struct pf_patch_t* patch, void* stream);
    }
    using namespace Callbacks;

    namespace Finders {
        void FindExit();

        void FindPushWidget();
    }
    using namespace Finders;
}