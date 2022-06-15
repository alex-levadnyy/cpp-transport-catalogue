#include "request_handler.h"

#include <fstream>
#include <memory>

using namespace std::literals;

namespace transport_catalogue {

    RequestHandler::RequestHandler(TransportCatalogue& catalogue)
        : catalogue_(catalogue) {}

    domain::RouteInfo RequestHandler::GetRouteInfo(const std::string& route_name) const {
        return catalogue_.GetRouteInfo(route_name);
    }

    std::optional<std::reference_wrapper<const std::set<std::string_view>>>
        RequestHandler::GetBusesOnStop(const std::string& stop_name) const {
        return catalogue_.GetBusesOnStop(stop_name);
    }

    svg::Document RequestHandler::RenderMap() const {
        if (render_settings_) {
            renderer::MapRenderer renderer;
            renderer.SetSettings(render_settings_.value());
            return renderer.RenderMap(catalogue_);
        }
        else {
            std::cerr << "Can't find rendering settings"s << std::endl;
            return {};
        }
    }
    
    bool RequestHandler::InitRouter() {
        if (!router_) {
            return ReInitRouter();
        }
        return true;
    }
    
    
    std::optional<RequestHandler::Bus>
    RequestHandler::BuildRoute(const std::string &from, const std::string &to) {
        if (!InitRouter()) {
            std::cerr << "Can't init Transport Router"s << std::endl;
            return std::nullopt;
        } else {
            return router_->BuildRoute(from, to);
        }
    }
    
    void RequestHandler::LoadDataFromJson(const json_reader::JsonLoader& json) {
        json.LoadData(catalogue_);
        render_settings_ = json.LoadRenderSettings();
        serialize_settings_ = json.LoadSerializeSettings();
        routing_settings_ = json.LoadRoutingSettings();
    }

    void RequestHandler::LoadDataFronJson(const std::filesystem::path& file_path) {
        std::ifstream in(file_path);
        if (in.is_open()) {
            LoadDataFromJson(json_reader::JsonLoader(in));
        }
        else {
            std::cerr << "Error opening file : "s + file_path.filename().string() << std::endl;
        }
    }

    void RequestHandler::LoadRequestsAndAnswer(const json_reader::JsonLoader& json, std::ostream& out) {

        
        if (!InitRouter()) {
            std::cerr << "Can't init Transport Router"s << std::endl;
            return;
        }
        
        json.AnswerRequests(catalogue_, render_settings_.value_or(renderer::RenderSettings{}), *router_, out);
    }

    bool RequestHandler::SerializeData() {
        if (!serialize_settings_) {
            std::cerr << "Can't find Serialize Settings : "s << std::endl;
            return false;
        }
        serialize::Serializator serializator(serialize_settings_.value());
        serializator.AddTransportCatalogue(catalogue_);
        
        if (render_settings_) {
            serializator.AddRenderSettings(render_settings_.value());
        }
        
        if (routing_settings_) {
            InitRouter();
            router_->InitRouter();
            serializator.AddTransportRouter(*router_.get());
        }
        
        return serializator.Serialize();
    }

    bool RequestHandler::DeserializeData() {
        if (!serialize_settings_) {
            std::cerr << "Can't find Serialize Settings : "s << std::endl;
            return false;
        }
        serialize::Serializator serializator(serialize_settings_.value());
        if (serializator.Deserialize(catalogue_, render_settings_, router_)) {
            if (router_) {
                routing_settings_ = router_->GetSettings();
            }
            return true;
        }
        return false;
    }
    
    bool RequestHandler::ReInitRouter() {
        if (routing_settings_) {
            router_ = std::make_unique<transport_router::TransportRouter>(catalogue_, routing_settings_.value());
            return true;
        } else {
            std::cerr << "Can't find routing settings"s << std::endl;
            return false;
        }
    }
    
    void RequestHandler::SetRenderSettings(const renderer::RenderSettings& render_settings) {
        render_settings_ = render_settings;
    }

    void RequestHandler::SetRoutingSettings(const RoutingSettings& routing_settings) {
        routing_settings_ = routing_settings;
    }

    void RequestHandler::SetSerializeSettings(const serialize::Serializator::Settings& serialize_setings) {
        serialize_settings_ = serialize_setings;
    }

} // namespace transport_catalogue

