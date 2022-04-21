#include "request_handler.h"

namespace transport_catalogue {

domain::RouteInfo RequestHandler::GetRouteInfo(const std::string &route_name) const {
    return catalogue_.GetRouteInfo(route_name);
}

std::optional<std::reference_wrapper<const std::set<std::string_view>>>
RequestHandler::GetBusesThroughStop(const std::string &stop_name) const {
    return catalogue_.GetBusesThroughStop(stop_name);
}

svg::Document RequestHandler::RenderMap() const {

    return renderer_.RenderMap(catalogue_);
}

} // namespace transport_catalogue

