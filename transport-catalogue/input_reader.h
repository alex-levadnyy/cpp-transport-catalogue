#pragma once

#include "transport_catalogue.h"
#include <unordered_map>
#include <istream>
#include <string>

namespace transport_catalogue {
	namespace input_reader {
		void ReadRequests(TransportCatalogue& catalogue, std::istream& input);

		void ParseBus(TransportCatalogue& catalogue, std::string& bus);

		void ParseStop(TransportCatalogue& catalogue, std::string& stop_src_data, std::unordered_map<std::string, std::string>& stops);

		void ParseStopInfo(TransportCatalogue& catalogue, const std::string& first_stop, std::string& info);
		
		Coordinates ParseCoordinate(std::string& data);
		
	}
}
