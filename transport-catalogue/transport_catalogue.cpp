#include "transport_catalogue.h"
#include "input_reader.h"
#include <functional>
#include <charconv>

namespace transport_catalogue {

    using detail::SeparatorString;

    TransportCatalogue::TransportCatalogue(std::deque<std::string> data)
            : queries_(std::move(data)) {
        using namespace std::string_literals;
        for (const auto& query : queries_) {
            if (query.substr(0, 4) == "Stop"s) {
                AddStop(query);
            }
            if (query.substr(0, 3) == "Bus"s) {
                AddBus(query);
            }
        }
        for (auto& stop : stops_) {
            AddNextStops(stop.second);
        }
        for (auto& stop : buses_) {
            ComputeRealRouteLength(stop.second);
        }
    }

    void TransportCatalogue::AddStop(std::string_view stop_str) {
        Stop stop;
        //Выделяем имя остановки из строки
        //Вырезаем подстроку Stop
        stop_str.remove_prefix(4);
        stop.stop_name = SeparatorString(stop_str, ':');

        //Перевод широты и долготы в double
        std::string_view lat_ = SeparatorString(stop_str, ',');
        stop.latitude = std::stod({lat_.data(), lat_.size()});
        //std::from_chars(lat_.data(), lat_.data() + lat_.size(), stop.latitude); //не работает в тренажере
        std::string_view lng_ = SeparatorString(stop_str, ',');
        stop.longitude = std::stod({lng_.data(), lng_.size()});
        //std::from_chars(lng_.data(), lng_.data() + lng_.size(), stop.longitude); //не работает в тренажере

        stop.next_stops = stop_str;
        stops_.insert({stop.stop_name, stop});
        buses_for_stops_.insert({stop.stop_name, {}});
    }

    void TransportCatalogue::AddNextStops(Stop &stop) {
        using namespace std::string_literals;
        while (!stop.next_stops.empty()) {
            std::string_view distance = SeparatorString(stop.next_stops, 'm');
            stop.next_stops.remove_prefix(stop.next_stops.find("to"s) + 2);
            std::string_view next_name = SeparatorString(stop.next_stops, ',');
            if (stops_.count(next_name)) {
                stop.dist_to_next.insert(
                        {stops_.at(next_name).stop_name, std::stoi({distance.data(), distance.size()})});
            }
        }
    }

    void TransportCatalogue::AddBus(std::string_view bus_str) {
        Bus bus;
        char sep;

        bus_str.remove_prefix(3);
        bus.bus_name = SeparatorString(bus_str, ':');
        
        if (bus_str.find('>') != std::string_view::npos) {
            bus.is_circle = true;
            sep = '>';
        }
        else {
            sep = '-';
        }

        while (!bus_str.empty()) {
            auto stop_name = SeparatorString(bus_str, sep);
            if (stops_.count(stop_name)) {
                Stop *stop_ptr = &stops_.at(stop_name);
                bus.route.push_back(stop_ptr);
                buses_for_stops_[stop_ptr->stop_name].insert(bus.bus_name);
            }
        }
        for (size_t i = 1; i < bus.route.size(); ++i) {
            if (bus.route[i - 1] == bus.route[i]) {
                continue;
            } 
            else {
                bus.r_length += geography::ComputeDistance({bus.route[i - 1]->latitude, bus.route[i - 1]->longitude},
                                                {bus.route[i]->latitude, bus.route[i]->longitude});
            }
        }
        if (!bus.is_circle) {
            bus.r_length *= 2;
        }
        buses_.insert({bus.bus_name, bus});
    }

    void TransportCatalogue::ComputeRealRouteLength(Bus &bus) {
        for (size_t i = 1; i < bus.route.size(); ++i) {
            if (bus.route[i - 1]->dist_to_next.count(bus.route[i]->stop_name)) {
                bus.true_length += bus.route[i - 1]->dist_to_next.at(bus.route[i]->stop_name);
            } else if (bus.route[i]->dist_to_next.count(bus.route[i - 1]->stop_name)) {
                bus.true_length += bus.route[i]->dist_to_next.at(bus.route[i - 1]->stop_name);
            }
        }
        if (!bus.is_circle) {
            for (size_t i = 1; i < bus.route.size(); ++i) {
                if (bus.route[i]->dist_to_next.count(bus.route[i - 1]->stop_name)) {
                    bus.true_length += bus.route[i]->dist_to_next.at(bus.route[i - 1]->stop_name);
                } else if (bus.route[i - 1]->dist_to_next.count(bus.route[i]->stop_name)) {
                    bus.true_length += bus.route[i - 1]->dist_to_next.at(bus.route[i]->stop_name);
                }
            }
        }
        bus.curvature = bus.true_length / bus.r_length;
    }

    Stop TransportCatalogue::FindStop(std::string_view stop) {
        return stops_.at(stop);
    }

    Bus TransportCatalogue::FindBus(std::string_view bus) {
        return buses_.at(bus);
    }

    BusRoute TransportCatalogue::RouteInformation(std::string_view bus) {
        detail::RemoveBeginEndSpaces(bus);
        BusRoute route;
        if (buses_.count(bus)) {
            if (buses_.at(bus).is_circle) {
                route.stops = (buses_.at(bus).route.size());
            }
            else {
                route.stops = buses_.at(bus).route.size() * 2 - 1;
            }
            route.is_found = true;
            std::set<const Stop*> unique_stops(buses_.at(bus).route.begin(), buses_.at(bus).route.end());
            route.bus_name = buses_.at(bus).bus_name;
            route.unique_stops = unique_stops.size();
            route.r_length = buses_.at(bus).r_length;
            route.curvature = buses_.at(bus).curvature;
            route.true_length = buses_.at(bus).true_length;
        } else {
            route.bus_name = bus;
        }
        return route;
    }

    StopRoutes TransportCatalogue::StopInformation(std::string_view stop) {
        detail::RemoveBeginEndSpaces(stop);
        StopRoutes buses_for_stop;
        if (buses_for_stops_.count(stop)) {
            buses_for_stop.is_found = true;
            buses_for_stop.routes = buses_for_stops_.at(stop);
        }
        buses_for_stop.stop_name = stop;
        return buses_for_stop;
    }
}
