#include "transport_catalogue.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <unordered_set>

using namespace std::literals;

namespace transport_catalogue {

void TransportCatalogue::AddBus(domain::Bus bus) noexcept {
    bus_stops_.push_back(std::move(bus));
    std::string_view bus_name = bus_stops_.back().name;
    bus_by_name_.insert({ bus_name, &bus_stops_.back()});
    for (auto stop : bus_stops_.back().stops) {
        buses_by_stop_name_[stop->name].insert(bus_name);
    }
}

void TransportCatalogue::AddBus(const std::string& bus_name,
                                bool bus_type, const std::vector<std::string>& stops) {
    if (bus_type) {
        if (stops.front() != stops.back()) {
            throw std::invalid_argument("The first and last stops must be the same!"s);
        }
    }
    domain::Bus bus;
    bus.name = bus_name;
    bus.route_type = bus_type;
    for (auto& stop_name : stops) {
        bus.stops.push_back(GetStop(stop_name));
    }
    AddBus(std::move(bus));
}

void TransportCatalogue::AddStop(const std::string &stop_name, const geo::Coordinates coordinate) {
    domain::Stop stop;
    stop.name = stop_name;
    stop.coordinate = coordinate;
    AddStop(stop);
}

void TransportCatalogue::AddStop(domain::Stop stop) noexcept {
    stops_.push_back(std::move(stop));
    stop_by_name_.insert({ stops_.back().name, &stops_.back() });
}

void TransportCatalogue::SetDistanceStops(const std::string &stop_from, const std::string &stop_to, int distance) {
    const auto stop_from_ = GetStop(stop_from);
    const auto stop_to_ = GetStop(stop_to);
    stops_to_dist_[stop_from_->name][stop_to_->name] = distance;
}

const domain::Stop* TransportCatalogue::GetStop(const std::string &stop_name) const {
    if (stop_by_name_.count(stop_name) == 0) {
        throw std::out_of_range("Stop "s + stop_name + " does not exist in catalogue"s);
    }
    return stop_by_name_.at(stop_name);
}

const domain::Bus* TransportCatalogue::GetBus(const std::string &bus_name) const {
    if (bus_by_name_.count(bus_name) == 0) {
        throw std::out_of_range("Route "s + bus_name + " does not exist in catalogue"s);
    }
    return bus_by_name_.at(bus_name);
}

domain::RouteInfo TransportCatalogue::GetRouteInfo(const std::string &route_name) const {
    domain::RouteInfo result_route_info;
    const auto bus = GetBus(route_name);
    result_route_info.name = bus->name;
    result_route_info.route_type = bus->route_type;
    result_route_info.num_of_stops = CalculateStops(bus);
    result_route_info.num_of_unique_stops = CalculateUniqueStops(bus);
    result_route_info.route_length = CalculateRealRouteLength(bus);
    result_route_info.curvature = result_route_info.route_length / CalculateRouteLength(bus);
    return result_route_info;
}

std::optional<std::reference_wrapper<const std::set<std::string_view>>>
TransportCatalogue::GetBusesThroughStop(const std::string &stop_name) const {
    auto found = buses_by_stop_name_.find(GetStop(stop_name)->name);
    if (found == buses_by_stop_name_.end()) {
        return std::nullopt;
    } else {
        return std::cref(found->second);
    }
}

int TransportCatalogue::GetForwardDistance(const std::string &stop_from,
                                           const std::string &stop_to) const {
    if (stops_to_dist_.count(stop_from) == 0 ||
        stops_to_dist_.at(stop_from).count(stop_to) == 0) {
        throw std::out_of_range("No information about distance from "s
                                    + stop_from + " to "s + stop_to);
    }
    return stops_to_dist_.at(stop_from).at(stop_to);
}

int TransportCatalogue::GetDistance(const std::string &stop_from, 
                                    const std::string &stop_to) const {
    int result = 0;
    try {
        result = GetForwardDistance(stop_from, stop_to);
    } catch (std::out_of_range&) {
        try {
            result = GetForwardDistance(stop_to, stop_from);
        } catch (std::out_of_range&) {
            throw std::out_of_range("No information about distance between stops "s
                                                + stop_from + " and "s + stop_to);
        }
    }
    return result;
}

const std::unordered_map<std::string_view, const domain::Bus*> &TransportCatalogue::GetBuses() const {
    return bus_by_name_;
}

const std::unordered_map<std::string_view, const domain::Stop*> &TransportCatalogue::GetStops() const {
    return stop_by_name_;
}

const std::unordered_map<std::string_view, std::set<std::string_view>> &TransportCatalogue::GetBusesOnStops() const {
    return buses_by_stop_name_;
}

int TransportCatalogue::CalculateRealRouteLength(const domain::Bus *route) const {
    int result = 0;
    if (route != nullptr) {
        for (auto iter1 = route->stops.begin(), iter2 = iter1+1;
             iter2 < route->stops.end();
             ++iter1, ++iter2) {
            result += GetDistance((*iter1)->name, (*iter2)->name);
        }
        if (!route->route_type) {
            for (auto iter1 = route->stops.rbegin(), iter2 = iter1+1;
                 iter2 < route->stops.rend();
                 ++iter1, ++iter2) {
                result += GetDistance((*iter1)->name, (*iter2)->name);
            }
        }
    }
    return result;
}

int CalculateStops(const domain::Bus *route) noexcept {
    int result = 0;
    if (route != nullptr) {
        result = static_cast<int>(route->stops.size());
        if (!route->route_type) {
            result = result  * 2 - 1;
        }
    }
    return result;
}

int CalculateUniqueStops(const domain::Bus *route) noexcept {
    int result = 0;
    if (route != nullptr) {
        std::unordered_set<std::string_view> uniques;
        for (auto stop : route->stops) {
            uniques.insert(stop->name);
        }
        result = static_cast<int>(uniques.size());
    }
    return result;
}

double CalculateRouteLength(const domain::Bus *bus) noexcept {
    double result = 0.0;
    if (bus != nullptr) {
        for (auto it1 = bus->stops.begin(), it2 = std::next(it1);
            it2 < bus->stops.end(); ++it1, ++it2) {
            result += ComputeDistance((*it1)->coordinate, (*it2)->coordinate);
        }
        if (!bus->route_type) {
            result *= 2;
        }
    }
    return result;
}

} // namespace transport_catalogue
