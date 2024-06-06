#include "pch.h"
#include "url.h"

namespace Starfall {
    URL::StrType URL::GetUrl() {
        StrType v1 = Protocol.substr(0, Protocol.Length - 1);
        v1 += Seperator;
        v1 += Domain;
        v1 += Port;
        v1 += Path;
        v1 += Query;
        return v1;
    }

    URL::operator StrType() {
        return GetUrl();
    }

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