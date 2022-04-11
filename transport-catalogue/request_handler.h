#pragma once

#include "map_renderer.h"
#include "transport_catalogue.h"

namespace transport_catalogue {

class RequestHandler;

// Фасад реализующий основные запросы к системе
class RequestHandler final {
public:

    explicit RequestHandler(const TransportCatalogue& catalogue, renderer::MapRenderer& renderer)
        : catalogue_(catalogue), renderer_(renderer) {}

    domain::RouteInfo GetRouteInfo(const std::string& route_name) const;

    std::optional<std::reference_wrapper<const std::set<std::string_view>>>
    GetBusesThroughStop(const std::string& stop_name) const;

    svg::Document RenderMap() const;

private:
    const TransportCatalogue& catalogue_;
    renderer::MapRenderer& renderer_;
};

} // namespace transport_catalogue
