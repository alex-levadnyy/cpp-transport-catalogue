/*
1 Есть идея сделать вариант отображения карты не "вид сверху", а в перспективе, как в навигаторах. Есть смысл? (кроме опыта)
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

// TransportCatalogue основной класс транспортного каталога
class TransportCatalogue final {
    
public:
    // Создает маршрут из остановок
    void AddBus(const std::string& route_name, domain::RouteType route_type, const std::vector<std::string>& stops);
    // добавляет остановку в каталог
    void AddStop(const std::string &stop_name, geo::Coordinates coordinate);
    
    // Добавляет расстояние между остановками
    void SetDistanceStops(const std::string &stop_from, const std::string &stop_to, int distance);
    // Возвращет расстояние между остановками в прям или обратном направлении
    int GetDistance(const std::string& stop_from, const std::string& stop_to) const;
    // Возвращает структуру RouteInfo по названию автобуса
    domain::RouteInfo GetRouteInfo(const std::string &route_name) const;

    // Возвращает автобусы, проходящие через остановку
    std::optional<std::reference_wrapper<const std::set<std::string_view>>>
    GetBusesOnStop(const std::string &stop_name) const;
    
    const std::unordered_map<std::string_view, const domain::Bus*>& GetRoutes() const;
    const std::unordered_map<std::string_view, const domain::Stop*>& GetStops() const;
    const std::unordered_map<std::string_view, std::set<std::string_view>>& GetBusesOnStops() const;

    const std::unordered_map<std::string_view, std::unordered_map<std::string_view, int>>& GetDistances() const;


private:
    // Добавляет остановку
    void AddStop(domain::Stop stop) noexcept;
    // Добавляет автобус
    void AddBus(domain::Bus bus) noexcept;
    // Возвращает указатель на остановку по её имени
    const domain::Stop* GetStop(const std::string& stop_name) const;
    // Возвращает указатель на автобус по его имени
    const domain::Bus* GetBus(const std::string& route_name) const;
    // Возвращает расстояние между остановками в прямом направлении
    int GetForwardDistance(const std::string& stop_from, const std::string& stop_to) const;
    // Считает общее расстояние по маршруту
    int CalculateRealRouteLength(const domain::Bus* bus) const;

    // Остановки
    std::deque<domain::Stop> stops_;
    std::unordered_map<std::string_view, const domain::Stop*> stop_by_name_;
    // Автобусы на каждой остановке
    std::unordered_map<std::string_view, std::set<std::string_view>> buses_by_stop_name_;
    // Автобусы
    std::deque<domain::Bus> bus_stops_;
    std::unordered_map<std::string_view, const domain::Bus*> bus_by_name_;
    // Расстояния между остановками
    std::unordered_map<std::string_view, std::unordered_map<std::string_view, int>> stops_to_dist_;

};

// Считает количество остановок на маршруте
int CalculateStops(const domain::Bus* bus) noexcept;
// считает количество уникальных остановок на маршруте
int CalculateUniqueStops(const domain::Bus* bus) noexcept;
// Считает расстояние на маршруте по прямой между координатами остановок
double CalculateRouteLength(const domain::Bus* bus) noexcept;

} // namespace transport_catalogue
