#include "stat_reader.h"
#include "input_reader.h"
#include <vector>
#include <string>
#include <iomanip>

using namespace std::string_literals;

namespace transport_catalogue::stat_reader {

	std::ostream& operator<<(std::ostream& out, const RouteInfo& route_info) {
		if (route_info.count_bus_stop == 0) {
			out << "not found"s << std::endl;
		} else {
			out << route_info.count_bus_stop << " stops on route, "s << route_info.count_unique_bus_stop 
				<< " unique stops, "s << std::setprecision(6) << route_info.lenght_route 
				<< " route length, "s << std::setprecision(6) << route_info.curvature << " curvature"s 
				<< std::endl;
		}
		return out;
	}

	void PrintBus(const TransportCatalogue& catalogue, const std::string& bus, std::ostream& out) {
		const std::string bus_name = bus.substr(4);
		out << "Bus "s << bus_name << ": "s;
		out << catalogue.GetRouteInfo(bus_name);
	}
		
	void PrintStop(const TransportCatalogue& catalogue, const std::string& stop, std::ostream& out) {
		const std::string name = stop.substr(5);
		out << "Stop "s << name << ": "s;
		const auto buses_by_stop = catalogue.GetBusesThroughStop(name);
		if (catalogue.GetStop(name) == nullptr) {
			out << "not found"s << std::endl;
		} else if (buses_by_stop.empty()) {
			out << "no buses"s << std::endl;
		} else { 
			out << "buses"s;
			for (const auto bus : buses_by_stop) {
				out << " "s << bus;
			}
			out << std::endl;
		}
	}
		
	void StatReader(const TransportCatalogue& catalogue, std::istream& in, std::ostream& out) {
		size_t count_requests = 0;
		in >> count_requests;
		std::string str;
		size_t count = 0;
		while (std::getline(in, str)) {
			if (str.find("Bus"s) != std::string::npos) {
				PrintBus(catalogue, str, out);
				++count;
			} else if (str.find("Stop"s) != std::string::npos) {
				PrintStop(catalogue, str, out);
				++count;
			} else if (!str.empty()) {
				++count;
			} 
			if (count >= count_requests) {
				break;
			}
		}
	}
	
}
