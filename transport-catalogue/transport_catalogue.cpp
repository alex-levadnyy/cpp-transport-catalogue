#include "transport_catalogue.h"

#include <unordered_set>


namespace transport_catalogue {
	namespace detail {
		size_t StopsPairHasher::operator()(const std::pair<const Stop*, const Stop*>& stops_pair) const {
			return hasher(stops_pair.first) + hasher(stops_pair.second) * 37;
		} 
	}
	
	void TransportCatalogue::AddBus(std::string& name, const std::vector<std::string_view>& bus_stops, bool is_circular) {
		buses_.push_back({ std::move(name), is_circular, bus_stops });
		bus_by_name_[buses_.back().name] = &buses_.back();

		for (const std::string_view stop : bus_stops) {
			buses_by_stop_name_[stop].insert(buses_.back().name);
		}
	}

	void TransportCatalogue::AddStop(std::string& name, const Coordinates coords) {
		bus_stops_.push_back({ std::move(name), coords });
		stop_by_name_[bus_stops_.back().name] = &bus_stops_.back();
	}

	void TransportCatalogue::SetDistanceStops(std::string_view from, std::string_view to, const int distance) {
		const auto from_stop = GetStop(from);
		const auto to_stop = GetStop(to);
		if (from_stop == nullptr || to_stop == nullptr) {
			return;
		}

		stops_to_dist_[{from_stop, to_stop}] = distance;
	}

	double TransportCatalogue::GetRealDistance(std::string_view from, std::string_view to) const {
		const auto from_stop = GetStop(from);
		const auto to_stop = GetStop(to);
		if (from_stop == nullptr || to_stop == nullptr) {
			return 0.0;
		}

		auto stops_pair = stops_to_dist_.find({from_stop, to_stop});
		if (stops_pair != stops_to_dist_.end()) {
			return stops_pair->second;
		} 
		else {
			stops_pair = stops_to_dist_.find({to_stop, from_stop});
			if (stops_pair != stops_to_dist_.end()) {
				return stops_pair->second;
			} 
			else {
				return 0.0;
			}
		}
	}

	double TransportCatalogue::GetGeoDistance(std::string_view from, std::string_view to) const {
		const auto from_stop = GetStop(from);
		const auto to_stop = GetStop(to);
		if (from_stop == nullptr || to_stop == nullptr) {
			return 0.0;
		}

		return ComputeDistance(from_stop->coordinates, to_stop->coordinates);
	}

	const Stop* TransportCatalogue::GetStop(std::string_view name) const {
		const auto stop = stop_by_name_.find(name);
		if (stop != stop_by_name_.end()) {
			return stop->second;
		} 
		else {
			return nullptr;
		}
	}

	const Bus* TransportCatalogue::GetBus(std::string_view name) const {
		const auto bus = bus_by_name_.find(name);
		if (bus != bus_by_name_.end()) {
			return bus->second;
		} 
		else {
			return nullptr;
		}
	}

	std::set<std::string_view> TransportCatalogue::GetBusesThroughStop(std::string_view stop_name) const {
		const auto it = buses_by_stop_name_.find(stop_name);
		if (it != buses_by_stop_name_.end()) {
			return it->second;
		} 
		else {
			return {};
		}
	}

	RouteInfo TransportCatalogue::GetRouteInfo(std::string_view name) const {
		RouteInfo result_route_info;
		const auto bus = GetBus(name);
		if (bus == nullptr) {
			return {};
		}

		std::unordered_set<std::string_view> seen_stops;
		size_t count_uniq_stops = 0;
		double real_lenght_route = 0.0;
		double geo_lenght_route = 0.0;

		for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
			real_lenght_route += GetRealDistance(bus->stops[i], bus->stops[i + 1]);  
			geo_lenght_route += GetGeoDistance(bus->stops[i], bus->stops[i + 1]);
			if (seen_stops.count(bus->stops[i]) == 0) {
				++count_uniq_stops;
				seen_stops.insert(bus->stops[i]);
			}
		}
		const double curvature = real_lenght_route / geo_lenght_route;
		result_route_info = { bus->stops.size(), count_uniq_stops, real_lenght_route, curvature };

		return result_route_info;
	}
}
