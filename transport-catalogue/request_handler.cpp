#include "request_handler.h"

namespace transport_catalogue {

    domain::RouteInfo RequestHandler::GetRouteInfo(const std::string& route_name) const {
        return catalogue_.GetRouteInfo(route_name);
    }

    std::optional<std::reference_wrapper<const std::set<std::string_view>>>
        RequestHandler::GetBusesThroughStop(const std::string& stop_name) const {
        return catalogue_.GetBusesThroughStop(stop_name);
    }

    svg::Document RequestHandler::RenderMap() const {

        return renderer_.RenderMap(catalogue_);
    }

    void RequestHandler::InitRouter(RoutingSettings settings) const {
    router_ = std::make_unique<transport_router::TransportRouter>
            (catalogue_, settings.wait_time, settings.velocity);
    }

    std::optional<RequestHandler::Route> RequestHandler::BuildRoute(const std::string &from, const std::string &to) {
        if (!router_) {
            throw std::runtime_error("You should init router before use it.\nUse \"InitRouter\" member function");
        }
        return router_->BuildRoute(from, to);
    }
} // namespace transport_catalogue

