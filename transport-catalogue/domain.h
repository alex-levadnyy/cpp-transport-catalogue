#pragma once

#include "geo.h"

#include <string>
#include <vector>

namespace domain {

// Тип маршрута
enum class RouteType {
    LINEAR,
    CIRCLE,
    UNKNOWN,
};

// Информация о маршруте
struct RouteInfo {
    std::string name;
    RouteType route_type;
    int num_of_stops = 0;
    int num_of_unique_stops = 0;
    int route_length = 0;
    double curvature = 0.0;
};

// Остановка: имя и координаты
struct Stop {
    std::string name;
    geo::Coordinates coordinate;
    friend bool operator==(const Stop& lhs, const Stop& rhs);
};

// Автобус: имя (номера автобуса), тип и список остановок
struct Bus {
    std::string name;
    RouteType route_type = RouteType::UNKNOWN;
    std::vector<const Stop*> stops;
    friend bool operator==(const Bus& lhs, const Bus& rhs);
};

} // namespace domain
