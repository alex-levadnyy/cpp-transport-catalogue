#include "json_reader.h"

#include <sstream>
#include <stdexcept>
#include <string>

using namespace std::literals;

namespace json_reader {

    JsonLoader::JsonLoader(std::istream& data_in)
        : data_(json::Load(data_in)) {
    }

    void JsonLoader::LoadData(transport_catalogue::TransportCatalogue& catalogue) const {

        // Загружаем данные в каталог, если они есть
        if (data_.GetRoot().IsMap() && data_.GetRoot().AsMap().count("base_requests"s) > 0) {
            auto& base_requests = data_.GetRoot().AsMap().at("base_requests"s);
            // проверяем, что данные для загрузки хранятся в нужном формате
            if (base_requests.IsArray()) {
                LoadStops(base_requests.AsArray(), catalogue);
                LoadBuses(base_requests.AsArray(), catalogue);
                LoadDistances(base_requests.AsArray(), catalogue);
            }
        }
    }

    void JsonLoader::LoadRenderSettings(renderer::MapRenderer& renderer) const {
        // загружаем параметры рендеринга, если они есть
        if (data_.GetRoot().IsMap() && data_.GetRoot().AsMap().count("render_settings"s) > 0) {
            auto& render_settings = data_.GetRoot().AsMap().at("render_settings"s);
            if (render_settings.IsMap()) {
                renderer.SetSettings(LoadSettings(render_settings.AsMap()));
            }
        }
    }

    void JsonLoader::AnswerRequests(const transport_catalogue::TransportCatalogue& catalogue,
        renderer::MapRenderer& renderer, std::ostream& requests_out) const {

        // Загружаем запросы, если они есть и формируем ответы
        if (data_.GetRoot().IsMap() && data_.GetRoot().AsMap().count("stat_requests"s) > 0) {
            auto& requests = data_.GetRoot().AsMap().at("stat_requests"s);
            // проверяем, что запросы хранятся в нужном формате
            if (requests.IsArray()) {
                json::Array answers = LoadAnswers(requests.AsArray(), catalogue, renderer);
                // выводим результат в поток
                json::Print(json::Document(json::Node{ answers }), requests_out);
            }
        }
    }

    void JsonLoader::LoadStops(const json::Array& data, transport_catalogue::TransportCatalogue& catalogue) {
        for (const auto& elem : data) {
            if (IsStop(elem)) {
                const auto& name = elem.AsMap().at("name"s).AsString();
                const auto lat = elem.AsMap().at("latitude"s).AsDouble();
                const auto lng = elem.AsMap().at("longitude"s).AsDouble();
                catalogue.AddStop(name, { lat, lng });
            }
        }
    }

    void JsonLoader::LoadBuses(const json::Array& data, transport_catalogue::TransportCatalogue& catalogue) {
        for (const auto& elem : data) {
            if (IsBuses(elem)) {
                const auto& name = elem.AsMap().at("name"s).AsString();
                const auto is_roundtrip = elem.AsMap().at("is_roundtrip"s).AsBool();
                const auto stops = elem.AsMap().at("stops"s).AsArray();
                std::vector<std::string> stops_names;
                for (const auto& stop_name : stops) {
                    if (stop_name.IsString()) {
                        stops_names.push_back(stop_name.AsString());
                    }
                }
                catalogue.AddBus(name, is_roundtrip, stops_names);
            }
        }
    }

    void JsonLoader::LoadDistances(const json::Array& data,
        transport_catalogue::TransportCatalogue& catalogue) {
        for (const auto& elem : data) {
            if (IsStop(elem)) {
                const auto& name_from = elem.AsMap().at("name"s).AsString();
                const auto distances = elem.AsMap().at("road_distances"s).AsMap();
                for (const auto& [name_to, distance] : distances) {
                    if (distance.IsInt()) {
                        catalogue.SetDistanceStops(name_from, name_to, distance.AsInt());
                    }
                }
            }
        }
    }

    renderer::RenderSettings JsonLoader::LoadSettings(const json::Dict& data) const {
        renderer::RenderSettings result;

        // считываем все нужные параметры при их наличии
        if (data.count("width"s) != 0 && data.at("width"s).IsDouble()) {
            result.size.x = data.at("width"s).AsDouble();
        }
        if (data.count("height"s) != 0 && data.at("height"s).IsDouble()) {
            result.size.y = data.at("height"s).AsDouble();
        }
        if (data.count("padding"s) != 0 && data.at("padding"s).IsDouble()) {
            result.padding = data.at("padding"s).AsDouble();
        }
        if (data.count("line_width"s) != 0 && data.at("line_width"s).IsDouble()) {
            result.line_width = data.at("line_width"s).AsDouble();
        }
        if (data.count("stop_radius"s) != 0 && data.at("stop_radius"s).IsDouble()) {
            result.stop_radius = data.at("stop_radius"s).AsDouble();
        }
        if (data.count("bus_label_font_size"s) != 0 && data.at("bus_label_font_size"s).IsInt()) {
            result.bus_label_font_size = data.at("bus_label_font_size"s).AsInt();
        }
        if (data.count("bus_label_offset"s) != 0 && data.at("bus_label_offset"s).IsArray()) {
            result.bus_label_offset = ReadOffset(data.at("bus_label_offset"s).AsArray());
        }
        if (data.count("stop_label_font_size"s) != 0 && data.at("stop_label_font_size"s).IsInt()) {
            result.stop_label_font_size = data.at("stop_label_font_size"s).AsInt();
        }
        if (data.count("stop_label_offset"s) != 0 && data.at("stop_label_offset"s).IsArray()) {
            result.stop_label_offset = ReadOffset(data.at("stop_label_offset"s).AsArray());
        }
        if (data.count("underlayer_color"s) != 0) {
            result.underlayer_color = ReadColor(data.at("underlayer_color"s));
        }
        if (data.count("underlayer_width"s) != 0 && data.at("underlayer_width"s).IsDouble()) {
            result.underlayer_width = data.at("underlayer_width"s).AsDouble();
        }
        if (data.count("color_palette"s) != 0 && data.at("color_palette"s).IsArray()) {
            for (auto& color : data.at("color_palette"s).AsArray()) {
                result.color_palette.push_back(ReadColor(color));
            }
        }
        return result;
    }

    json::Array JsonLoader::LoadAnswers(const json::Array& requests,
        const transport_catalogue::TransportCatalogue& catalogue,
        renderer::MapRenderer& renderer) {
        json::Array result;
        for (const auto& request : requests) {
            if (IsBusesRequest(request)) {
                result.push_back(LoadBusesAnswer(request.AsMap(), catalogue));
            }
            else if (IsStopRequest(request)) {
                result.push_back(LoadStopAnswer(request.AsMap(), catalogue));
            }
            else if (IsMapRequest(request)) {
                result.push_back(LoadMapAnswer(request.AsMap(), catalogue, renderer));
            }
        }
        return result;
    }

    json::Dict JsonLoader::LoadBusesAnswer(const json::Dict& request,
        const transport_catalogue::TransportCatalogue& catalogue) {
        json::Dict result;
        int id = request.at("id"s).AsInt();
        result.insert({ "request_id"s, id });
        const auto& name = request.at("name"s).AsString();
        try {
            auto answer = catalogue.GetRouteInfo(name);
            // если маршрут существует - возвращаем данные о нём
            result.insert({ "curvature", answer.curvature });
            result.insert({ "route_length", answer.route_length });
            result.insert({ "stop_count", answer.num_of_stops });
            result.insert({ "unique_stop_count", answer.num_of_unique_stops });
        }
        catch (std::out_of_range&) {
            // если маршрута нет - возвращаем сообщение с ошибкой
            result.insert({ "error_message"s, "not found"s });
        }
        return result;
    }

    json::Dict JsonLoader::LoadStopAnswer(const json::Dict& request,
        const transport_catalogue::TransportCatalogue& catalogue) {
        json::Dict result;
        int id = request.at("id"s).AsInt();
        result.insert({ "request_id"s, id });
        const auto& name = request.at("name"s).AsString();
        try {
            auto answer = catalogue.GetBusesThroughStop(name);
            //  если остановка существует возвращаем список автобусов через неё проходящих
            if (!answer) {
                result.insert({ "buses"s, json::Array() });
            }
            else {
                json::Array buses;
                for (auto name : answer.value().get()) {
                    buses.push_back(std::string(name));
                }
                result.insert({ "buses"s, buses });
            }
        }
        catch (std::out_of_range&) {
            // если остановки нет - возвращаем сообщение с ошибкой
            result.insert({ "error_message"s, "not found"s });
        }
        return result;
    }

    json::Dict JsonLoader::LoadMapAnswer(const json::Dict& request,
        const transport_catalogue::TransportCatalogue& catalogue,
        renderer::MapRenderer& renderer) {
        json::Dict result;
        int id = request.at("id"s).AsInt();
        result.insert({ "request_id"s, id });
        // формируем карту и выводим её в виде строки
        auto map = renderer.RenderMap(catalogue);
        std::ostringstream out;
        map.Render(out);
        result.insert({ "map", out.str() });
        return result;
    }

    bool JsonLoader::IsStop(const json::Node& node) {
        if (!node.IsMap()) {
            return false;
        }
        const auto& stop = node.AsMap();
        if (stop.count("type"s) == 0 || stop.at("type"s) != "Stop"s) {
            return false;
        }
        if (stop.count("name"s) == 0 || !(stop.at("name"s).IsString())) {
            return false;
        }
        if (stop.count("latitude"s) == 0 || !(stop.at("latitude"s).IsDouble())) {
            return false;
        }
        if (stop.count("longitude"s) == 0 || !(stop.at("longitude"s).IsDouble())) {
            return false;
        }
        if (stop.count("road_distances"s) == 0 || (stop.at("longitude"s).IsMap())) {
            return false;
        }
        return true;
    }

    bool JsonLoader::IsBuses(const json::Node& node) {
        if (!node.IsMap()) {
            return false;
        }
        const auto& bus = node.AsMap();
        if (bus.count("type"s) == 0 || bus.at("type"s) != "Bus"s) {
            return false;
        }
        if (bus.count("name"s) == 0 || !(bus.at("name"s).IsString())) {
            return false;
        }
        if (bus.count("is_roundtrip"s) == 0 || !(bus.at("is_roundtrip"s).IsBool())) {
            return false;
        }
        if (bus.count("stops"s) == 0 || !(bus.at("stops"s).IsArray())) {
            return false;
        }
        return true;
    }

    bool JsonLoader::IsBusesRequest(const json::Node& node) {
        if (!node.IsMap()) {
            return false;
        }
        const auto& request = node.AsMap();
        if (request.count("type"s) == 0 || request.at("type"s) != "Bus"s) {
            return false;
        }
        if (request.count("id"s) == 0 || !(request.at("id"s).IsInt())) {
            return false;
        }
        if (request.count("name"s) == 0 || !(request.at("name"s).IsString())) {
            return false;
        }
        return true;
    }

    bool JsonLoader::IsStopRequest(const json::Node& node) {
        if (!node.IsMap()) {
            return false;
        }
        const auto& request = node.AsMap();
        if (request.count("type"s) == 0 || request.at("type"s) != "Stop"s) {
            return false;
        }
        if (request.count("id"s) == 0 || !(request.at("id"s).IsInt())) {
            return false;
        }
        if (request.count("name"s) == 0 || !(request.at("name"s).IsString())) {
            return false;
        }
        return true;
    }

    bool JsonLoader::IsMapRequest(const json::Node& node) {
        if (!node.IsMap()) {
            return false;
        }
        const auto& request = node.AsMap();
        if (request.count("type"s) == 0 || request.at("type"s) != "Map"s) {
            return false;
        }
        if (request.count("id"s) == 0 || !(request.at("id"s).IsInt())) {
            return false;
        }
        return true;
    }

    svg::Color JsonLoader::ReadColor(const json::Node& color) {
        if (color.IsString()) {
            return color.AsString();
        }
        else if (color.IsArray() && color.AsArray().size() == 3) {
            auto result_color = svg::Rgb(static_cast<uint8_t>(color.AsArray().at(0).AsInt()),
                static_cast<uint8_t>(color.AsArray().at(1).AsInt()),
                static_cast<uint8_t>(color.AsArray().at(2).AsInt()));
            return result_color;
        }
        else if (color.IsArray() && color.AsArray().size() == 4) {
            auto result_color = svg::Rgba(static_cast<uint8_t>(color.AsArray().at(0).AsInt()),
                static_cast<uint8_t>(color.AsArray().at(1).AsInt()),
                static_cast<uint8_t>(color.AsArray().at(2).AsInt()),
                color.AsArray().at(3).AsDouble());
            return result_color;
        }
        else {
            return svg::NoneColor;
        }
    }

    svg::Point JsonLoader::ReadOffset(const json::Array& offset) {
        svg::Point result;
        if (offset.size() > 1) {
            if (offset.at(0).IsDouble()) {
                result.x = offset.at(0).AsDouble();
            }
            if (offset.at(1).IsDouble()) {
                result.y = offset.at(1).AsDouble();
            }
        }
        return result;
    }

} // namespace json_reader
