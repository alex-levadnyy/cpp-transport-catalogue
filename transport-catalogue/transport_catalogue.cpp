#include "transport_catalogue.h"

#include <algorithm>
#include <cassert>
#include <stdexcept>
#include <unordered_set>

using namespace std;

namespace transport_catalogue {

void TransportCatalogue::AddBus(domain::Bus route) noexcept {
    bus_stops_.push_back(move(route));
    string_view route_name = bus_stops_.back().name;
    bus_by_name_.insert({route_name, &bus_stops_.back()});
    for (auto stop : bus_stops_.back().stops) {
        buses_by_stop_name_[stop->name].insert(route_name);
    }
}

void TransportCatalogue::
AddBus(const string &route_name, domain::RouteType route_type, const vector<string> &stops) {
    if (route_type == domain::RouteType::CIRCLE) {
        if (stops.front() != stops.back()) {
            throw std::invalid_argument("In circle route first and last stops must be equal!"s);
        }
    }
    domain::Bus route;
    route.name = route_name;
    route.route_type = route_type;
    for (auto &stop_name : stops) {
        route.stops.push_back(GetStop(stop_name));
    }
    AddBus(move(route));
}

void TransportCatalogue::AddStop(domain::Stop stop) noexcept {
    stops_.push_back(move(stop));
    stop_by_name_.insert({ stops_.back().name, &stops_.back() });
}

void TransportCatalogue::AddStop(const std::string& stop_name, geo::Coordinates coordinate) {
    domain::Stop stop;
    stop.name = stop_name;
    stop.coordinate = coordinate;
    AddStop(stop);
}

void TransportCatalogue::SetDistanceStops(const std::string &stop_from, const std::string &stop_to, int distance) {
    auto Stop_from = GetStop(stop_from);
    auto Stop_to = GetStop(stop_to);
    stops_to_dist_[Stop_from->name][Stop_to->name] = distance;
}

const domain::Stop* TransportCatalogue::GetStop(const string &stop_name) const {
    if (stop_by_name_.count(stop_name) == 0) {
        throw std::out_of_range("Stop "s + stop_name + " does not exist in catalogue"s);
    }
    return stop_by_name_.at(stop_name);
}

const domain::Bus* TransportCatalogue::GetBus(const string &route_name) const {
    if (bus_by_name_.count(route_name) == 0) {
        throw std::out_of_range("Route "s + route_name + " does not exist in catalogue"s);
    }
    return bus_by_name_.at(route_name);
}

domain::RouteInfo TransportCatalogue::GetRouteInfo(const string &route_name) const {
    domain::RouteInfo result;
    auto route = GetBus(route_name);
    result.name = route->name;
    result.route_type = route->route_type;
    result.num_of_stops = CalculateStops(route);
    result.num_of_unique_stops = CalculateUniqueStops(route);
    result.route_length = CalculateRealRouteLength(route);
    result.curvature = result.route_length / CalculateRouteLength(route);
    return result;
}

std::optional<std::reference_wrapper<const std::set<std::string_view>>>
TransportCatalogue::GetBusesOnStop(const std::string &stop_name) const {
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

const std::unordered_map<string_view, const domain::Bus*>
&TransportCatalogue::GetRoutes() const {
    return bus_by_name_;
}

const std::unordered_map<string_view, const domain::Stop*>
&TransportCatalogue::GetStops() const {
    return stop_by_name_;
}

const std::unordered_map<string_view, std::set<string_view>>
&TransportCatalogue::GetBusesOnStops() const {
    return buses_by_stop_name_;
}

const std::unordered_map<string_view, std::unordered_map<string_view, int> >
&TransportCatalogue::GetDistances() const {
    return stops_to_dist_;
}

int TransportCatalogue::CalculateRealRouteLength(const domain::Bus *route) const {
    int result = 0;
    if (route != nullptr) {
        for (auto iter1 = route->stops.begin(), iter2 = iter1+1;
             iter2 < route->stops.end();
             ++iter1, ++iter2) {
            result += GetDistance((*iter1)->name, (*iter2)->name);
        }
        if (route->route_type == domain::RouteType::LINEAR) {
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
        if (route->route_type == domain::RouteType::LINEAR) {
            result = result  * 2 - 1;
        }
    }
    return result;
}

int CalculateUniqueStops(const domain::Bus *route) noexcept {
    int result = 0;
    if (route != nullptr) {
        unordered_set<string_view> uniques;
        for (auto stop : route->stops) {
            uniques.insert(stop->name);
        }
        result = static_cast<int>(uniques.size());
    }
    return result;
}

double CalculateRouteLength(const domain::Bus *route) noexcept {
    double result = 0.0;
    if (route != nullptr) {
        for (auto iter1 = route->stops.begin(), iter2 = iter1+1;
             iter2 < route->stops.end();
             ++iter1, ++iter2) {
            result += ComputeDistance((*iter1)->coordinate, (*iter2)->coordinate);
        }
        if (route->route_type == domain::RouteType::LINEAR) {
            result *= 2;
        }
    }
    return result;
}

} // namespace transport_catalogue
