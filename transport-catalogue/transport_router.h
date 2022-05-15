#pragma once

#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

namespace transport_router {

struct RouteEdge {
    std::string_view bus_name;
    std::string_view stop_from;
    std::string_view stop_to;
    double total_time = 0;
    int span_count = 0;
};

bool operator<(const RouteEdge &left, const RouteEdge &right);
bool operator>(const RouteEdge &left, const RouteEdge &right);
RouteEdge operator+(const RouteEdge &left, const RouteEdge &right);

using TransportRoute = std::vector<RouteEdge>;

class TransportRouter {
public:
    struct RoutingSettings {
        int wait_time = 0;        // мин.
        double velocity = 100;    // м/с
    };

    TransportRouter(const transport_catalogue::TransportCatalogue &catalogue,
                    int wait_time, int velocity);

    std::optional<TransportRoute> BuildRoute(const std::string &from, const std::string &to);

    const RoutingSettings& GetSettings() const;

private:
    // коэффициент перевода км/ч в м/мин
    constexpr static double KMH_TO_MMIN = 1000.0 / 60.0;

    RoutingSettings settings_;
    const transport_catalogue::TransportCatalogue &catalogue_;

    std::unordered_map<size_t, const domain::Stop*> stops_by_id_;
    std::unordered_map<std::string_view, size_t> id_by_stop_name_;

    graph::DirectedWeightedGraph<RouteEdge> graph_;
    std::unique_ptr<graph::Router<RouteEdge>> router_;

    void BuildEdges();
    size_t CountStops();
    graph::Edge<RouteEdge> MakeEdge(const domain::Bus* route, int stop_from_index, int stop_to_index);
    double ComputeRouteTime(const domain::Bus* route, int stop_from_index, int stop_to_index);
};

} // namespace transport_router

