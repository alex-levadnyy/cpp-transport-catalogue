#pragma once

#include <iostream>
#include <memory>

#include "json_builder.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"
#include "transport_router.h"

namespace json_reader {

// Класс для ввод/вывод данных TK в формате JSON
class JsonLoader final {
public:

    // При создании считывает все данные из входного потока
    JsonLoader(std::istream &data_in);

    // Загрузка данных из ТК, настроки ренедера, обработка запросов и запись в выходной поток 
    void LoadData(transport_catalogue::TransportCatalogue &catalogue) const;
    void LoadRenderSettings(renderer::MapRenderer &renderer) const;
    void AnswerRequests(const transport_catalogue::TransportCatalogue &catalogue,
                        renderer::MapRenderer &renderer, std::ostream &requests_out) const;

private:
    // Устанавливает настройки рендера
    renderer::RenderSettings LoadSettings(const json::Dict &data) const;

    // формирует и возвращает ответы на запросы
    json::Array LoadAnswers(const json::Array& requests,
        const transport_catalogue::TransportCatalogue& catalogue,
        renderer::MapRenderer& renderer) const;

    transport_router::TransportRouter::RoutingSettings LoadRoutingSettings() const;

    void InitRouter(const transport_catalogue::TransportCatalogue &catalogue) const;

    // Загрузка данных из json в каталог
    static void LoadStops(const json::Array &data, transport_catalogue::TransportCatalogue &catalogue);
    static void LoadBuses(const json::Array &data, transport_catalogue::TransportCatalogue &catalogue);
    static void LoadDistances(const json::Array &data, transport_catalogue::TransportCatalogue &catalogue);

    static json::Dict LoadRouteAnswer(const json::Dict &request,
                                      const transport_catalogue::TransportCatalogue &catalogue);
    static json::Dict LoadStopAnswer(const json::Dict &request,
                                     const transport_catalogue::TransportCatalogue &catalogue);
    static json::Dict LoadMapAnswer(const json::Dict &request,
                                    const transport_catalogue::TransportCatalogue &catalogue,
                                    renderer::MapRenderer &renderer);
    json::Dict LoadRouteBuildAnswer(const json::Dict &request,
                                    const transport_catalogue::TransportCatalogue &catalogue) const;

    static json::Dict ErrorMessage(int id);
    
    // Проверка корректности данных
    static bool IsStop(const json::Node &node);
    static bool IsRoute(const json::Node &node);
    static bool IsRouteRequest(const json::Node &node);
    static bool IsStopRequest(const json::Node &node);
    static bool IsMapRequest(const json::Node &node);
    static bool IsRouteBuildRequest(const json::Node &node);

    static svg::Color ReadColor(const json::Node &node);
    static svg::Point ReadOffset(const json::Array &node);

    json::Document data_;
    mutable std::unique_ptr<transport_router::TransportRouter> router_;
};

} // namespace json_reader
