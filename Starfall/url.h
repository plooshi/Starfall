#pragma once
#include "pch.h"
#include "ue.h"

namespace Starfall {
    class URL {
    public:
        using StrType = FString;
        StrType Protocol, Seperator, Domain, Port, Path, Query;

        void Construct(StrType& url) {
            auto ProtoEnd = url.find(':');
            Protocol = url.substr(0, ProtoEnd);
            auto ProtoSize = (url[ProtoEnd + 1] == '/' && url[ProtoEnd + 2] == '/') ? 3 : 1;
            Seperator = url.substr(ProtoEnd, ProtoSize);
            auto DomainAndPortStart = url.substr(ProtoEnd + ProtoSize);
            auto PathEnd = DomainAndPortStart.find_first_of('/');
            auto DomainAndPort = DomainAndPortStart.substr(0, PathEnd);
            auto PathStart = DomainAndPortStart.substr(PathEnd);
            DomainAndPortStart.Dealloc();
            auto PortOff = DomainAndPort.find_first_of(':');
            Domain = DomainAndPort.substr(0, PortOff);
            if (PortOff != StrType::npos) Port = DomainAndPort.substr(PortOff);
            DomainAndPort.Dealloc();
            auto QueryOff = PathStart.find_first_of('?');
            Path = PathStart.substr(0, QueryOff);
            if (QueryOff != StrType::npos) Query = PathStart.substr(QueryOff);
            PathStart.Dealloc();
        }

        URL& SetHost(FString host) {
            auto ProtoEnd = host.find(':');
            Protocol = host.substr(0, ProtoEnd);
            auto ProtoSize = (host[ProtoEnd + 1] == '/' && host[ProtoEnd + 2] == '/') ? 3 : 1;
            Seperator.Dealloc();
            Seperator = host.substr(ProtoEnd, ProtoSize);
            auto DomainAndPortStart = host.substr(ProtoEnd + ProtoSize);
            auto PathEnd = DomainAndPortStart.find_first_of('/');
            auto DomainAndPort = DomainAndPortStart.substr(0, PathEnd);
            DomainAndPortStart.Dealloc();
            auto PortOff = DomainAndPort.find_first_of(':');
            Domain.Dealloc();
            Domain = DomainAndPort.substr(0, PortOff);
            if (PortOff != StrType::npos) {
                Port.Dealloc();
                Port = DomainAndPort.substr(PortOff);
            }
            DomainAndPort.Dealloc();
            return *this;
        }

        template <FString host>
        URL& SetHost() {
            constexpr auto ProtoEnd = FStringUtil::find_const(host, ':');
            Protocol.Dealloc();
            constexpr static auto ProtocolS = FStringUtil::substr<host, 0, ProtoEnd>();
            constexpr static FString ProtocolV = ProtocolS.data();
            Protocol = ProtocolV;
            constexpr auto ProtoSize = (host.String[ProtoEnd + 1] == '/' && host.String[ProtoEnd + 2] == '/') ? 3 : 1;
            Seperator.Dealloc();
            constexpr static auto SeperatorS = FStringUtil::substr<host, ProtoEnd, ProtoSize>();
            constexpr static FString SeperatorV = SeperatorS.data();
            Seperator = SeperatorV;
            constexpr auto DomainAndPortStart = FStringUtil::substr<host, ProtoEnd + ProtoSize>();
            constexpr auto PathEnd = CPPArrayUtil::find<DomainAndPortStart.size(), DomainAndPortStart>('/');
            constexpr auto DomainAndPort = CPPArrayUtil::substr<DomainAndPortStart.size(), DomainAndPortStart, 0, PathEnd>();
            constexpr auto PortOff = CPPArrayUtil::find<DomainAndPort.size(), DomainAndPort>(':');
            Domain.Dealloc();
            constexpr static auto DomainS = CPPArrayUtil::substr<DomainAndPort.size(), DomainAndPort, 0, PortOff>();
            constexpr static FString DomainV = DomainS.data();
            Domain = DomainV;
            if (PortOff != FString::npos) {
                Port.Dealloc();
                constexpr static auto PortS = CPPArrayUtil::substr<DomainAndPort.size(), DomainAndPort, PortOff>();
                constexpr static FString PortV = PortS.data();
                Port = PortV;
            }
            return *this;
        }

        StrType GetUrl();

        operator StrType();

        void Dealloc();
        void DeallocPathQuery();
    };

}
#ifdef __clang__
#define __URL_SetHost(url, host) url->SetHost(host)
#else
#define __URL_SetHost(url, host) url->SetHost<host>()
#endif