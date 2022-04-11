#pragma once

#include "geo.h"

#include <string>
#include <vector>


namespace domain {

struct RouteInfo {
    std::string name;
    bool route_type;
    int num_of_stops = 0;
    int num_of_unique_stops = 0;
    int route_length = 0;
    double curvature = 0.0;
};


struct Stop {
    std::string name;
    geo::Coordinates coordinate;
    friend bool operator==(const Stop &lhs, const Stop &rhs);
};


struct Bus {
    std::string name;
    bool route_type = false;
    std::vector<const Stop*> stops;
    friend bool operator==(const Bus &lhs, const Bus &rhs);
};

} // namespace domain
