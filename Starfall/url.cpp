 #include "pch.h"
#include "url.h"
#include "base.h"

namespace Starfall {

    void URL::DeallocPathQuery() {
        Path.Dealloc();
        Query.Dealloc();
    }

    void URL::Dealloc() {
        Protocol.Dealloc();
        Seperator.Dealloc();
        Domain.Dealloc();
        Port.Dealloc();
        Path.Dealloc();
        Query.Dealloc();
    }
}