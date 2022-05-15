#pragma once

#include "map_renderer.h"
#include "transport_catalogue.h"
#include "transport_router.h"

namespace transport_catalogue {

    // Фасад реализующий основные запросы к системе
    class RequestHandler final {
    public:
        using Route = transport_router::TransportRoute;
        using RoutingSettings = transport_router::TransportRouter::RoutingSettings;

        explicit RequestHandler(const TransportCatalogue& catalogue, renderer::MapRenderer& renderer)
            : catalogue_(catalogue), renderer_(renderer) 
        {
        }

        domain::RouteInfo GetRouteInfo(const std::string& route_name) const;

        std::optional<std::reference_wrapper<const std::set<std::string_view>>>
            GetBusesThroughStop(const std::string& stop_name) const;

        svg::Document RenderMap() const;

        // Инициализирует маршрутизатор
        void InitRouter(RoutingSettings settings) const;

        // Возвращает маршрут между двумя остановками
        std::optional<Route> BuildRoute(const std::string &from, const std::string &to);

    private:
        const TransportCatalogue& catalogue_;
        renderer::MapRenderer& renderer_;
        mutable std::unique_ptr<transport_router::TransportRouter> router_;
    };

} // namespace transport_catalogue

