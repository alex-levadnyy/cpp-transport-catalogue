#pragma once

#include "transport_catalogue.h"

namespace transport_catalogue::stat_reader {
    void StatReader(TransportCatalogue t);

    void PrintBus(const BusRoute r);

    void PrintStop(const StopRoutes &s);

    std::ostream& operator<<(std::ostream& out, const BusRoute& route_info);

    std::ostream& operator<<(std::ostream& out, const StopRoutes& route_info);
}
