#pragma once

#include <iostream>
#include "transport_catalogue.h"

namespace transport_catalogue::stat_reader {

	std::ostream& operator<<(std::ostream& out, const RouteInfo& info);

	void PrintBus(const TransportCatalogue& catalogue, const std::string& bus, std::ostream& out);

	void PrintStop(const TransportCatalogue& catalogue, const std::string& stop, std::ostream& out);

	void StatReader(const TransportCatalogue& catalogue, std::istream& in, std::ostream& out);

}
