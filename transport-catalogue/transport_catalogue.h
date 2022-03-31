#pragma once

#include "geo.h"

#include <string>
#include <string_view>
#include <deque>
#include <unordered_map>
#include <vector>
#include <set>

namespace transport_catalogue {


		struct Bus {
			std::string name;
			bool is_circular;
			std::vector<std::string_view> stops;
		};

		struct Stop {
			std::string name;
			Coordinates coordinates;
		};

		struct RouteInfo {
			size_t count_bus_stop;
			size_t count_unique_bus_stop;
			double lenght_route;
			double curvature;
		};

	
	namespace detail {
		struct StopsPairHasher {
			size_t operator()(const std::pair<const Stop*, const Stop*>& stops_pair) const;

			std::hash<const void*> hasher;
		};
	}

	class TransportCatalogue {
	private:
		std::deque<Bus> buses_;
		std::deque<Stop> bus_stops_;
		std::unordered_map<std::string_view, const Stop*> stop_by_name_;
		std::unordered_map<std::string_view, const Bus*> bus_by_name_;
		std::unordered_map<std::string_view, std::set<std::string_view>> buses_by_stop_name_;
		std::unordered_map<std::pair<const Stop*, const Stop*>, double, detail::StopsPairHasher> stops_to_dist_;

	public:
		void AddBus(const std::string& name, const std::vector<std::string_view>& bus_stops, bool is_circular);

		void AddStop(const std::string& name, const Coordinates coords);

		void SetDistanceStops(std::string_view from, std::string_view to, const int distance);

		const Stop* GetStop(std::string_view name) const;

		const Bus* GetBus(std::string_view name) const;

		std::set<std::string_view> GetBusesThroughStop(std::string_view stop_name) const;

		double GetRealDistance(std::string_view from, std::string_view to) const;

		double GetGeoDistance(std::string_view from, std::string_view to) const;

		RouteInfo GetRouteInfo(std::string_view name) const;
	};
}
