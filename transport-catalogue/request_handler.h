#pragma once

#include "map_renderer.h"
#include "transport_catalogue.h"

namespace transport_catalogue {

class RequestHandler;

// Интерфейс взаимодействия ТК с интерфейсами ввода-вывода
class TransportCatalogueHandler {
public:
    virtual void LoadData(TransportCatalogue& catalogue) const = 0;

    virtual void AnswerRequests(const TransportCatalogue& catalogue,
                                renderer::MapRenderer& renderer, std::ostream& requests_out) const = 0;
protected:
    ~TransportCatalogueHandler() = default;
};


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
