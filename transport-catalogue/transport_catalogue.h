/*
1 Про std::variant и Color не совсем понял..
2 Еще есть идея сделать вариат отображения карты не "вид сверху", а в перспективе, как в навигаторах. Есть смысл? (кроме опыта)
*/

#pragma once

#include "domain.h"

#include <deque>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>


namespace transport_catalogue {

// Основной класс ТК
class TransportCatalogue final {
    

public:
    void AddBus(const std::string& bus_name, bool bus_type, const std::vector<std::string>& stops);
    void AddStop(const std::string& stop_name, const geo::Coordinates coordinate);

    void SetDistanceStops(const std::string &stop_from, const std::string &stop_to, int distance);
    int GetDistance(const std::string& stop_from, const std::string& stop_to) const;

    domain::RouteInfo GetRouteInfo(const std::string& bus_name) const;
    std::optional<std::reference_wrapper<const std::set<std::string_view>>>
        GetBusesThroughStop(const std::string& stop_name) const;
    const std::unordered_map<std::string_view, const domain::Bus*>& GetBuses() const;
    const std::unordered_map<std::string_view, const domain::Stop*>& GetStops() const;
    const std::unordered_map<std::string_view, std::set<std::string_view>>& GetBusesOnStops() const;


private:
    void AddStop(domain::Stop stop) noexcept;
    void AddBus(domain::Bus bus) noexcept;

    const domain::Stop* GetStop(const std::string& stop_name) const;
    const domain::Bus* GetBus(const std::string& route_name) const;

    int GetForwardDistance(const std::string& stop_from, const std::string& stop_to) const;
    int CalculateRealRouteLength(const domain::Bus* route) const;

    std::deque<domain::Stop> stops_;
    std::unordered_map<std::string_view, const domain::Stop*> stop_by_name_;
    std::unordered_map<std::string_view, std::set<std::string_view>> buses_by_stop_name_;
    std::deque<domain::Bus> bus_stops_;
    std::unordered_map<std::string_view, const domain::Bus*> bus_by_name_;
    std::unordered_map<std::string_view, std::unordered_map<std::string_view, int>> stops_to_dist_;
};

int CalculateStops(const domain::Bus* bus) noexcept;
int CalculateUniqueStops(const domain::Bus* bus) noexcept;
double CalculateRouteLength(const domain::Bus* bus) noexcept;

} // namespace transport_catalogue
