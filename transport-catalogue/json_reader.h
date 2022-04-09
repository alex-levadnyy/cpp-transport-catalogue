#pragma once

#include "json.h"
#include "map_renderer.h"
#include "request_handler.h"
#include "transport_catalogue.h"

#include <iostream>


namespace json_reader {

// Класс для ввод/вывод данных TK в формате JSON
class JsonLoader final : public transport_catalogue::TransportCatalogueHandler {
public:

    // Читатет данные извходного потока
    JsonLoader(std::istream& data_in);

    // Загрузка данных из ТК, ннастроки ренедера, обработка запросов и запись в выходной поток 
    void LoadData(transport_catalogue::TransportCatalogue& catalogue) const override;
    void LoadRenderSettings(renderer::MapRenderer& renderer) const;
    void AnswerRequests(const transport_catalogue::TransportCatalogue& catalogue,
                        renderer::MapRenderer& renderer, std::ostream& requests_out) const override;

private:
    // Устанавливает настройки рендера
    renderer::RenderSettings LoadSettings(const json::Dict& data) const;

    static json::Array LoadAnswers(const json::Array& requests,
                            const transport_catalogue::TransportCatalogue& catalogue,
                            renderer::MapRenderer& renderer);

    // Загрузка данных из json в каталог
    static void LoadStops(const json::Array& data, transport_catalogue::TransportCatalogue& catalogue);
    static void LoadBuses(const json::Array& data, transport_catalogue::TransportCatalogue& catalogue);
    static void LoadDistances(const json::Array& data, transport_catalogue::TransportCatalogue& catalogue);

    static json::Dict LoadBusesAnswer(const json::Dict& request,
                               const transport_catalogue::TransportCatalogue& catalogue);

    static json::Dict LoadStopAnswer(const json::Dict& request,
                               const transport_catalogue::TransportCatalogue& catalogue);

    static json::Dict LoadMapAnswer(const json::Dict& request,
                             const transport_catalogue::TransportCatalogue& catalogue,
                             renderer::MapRenderer& renderer);

    // Проверка корректности данных
    static bool IsStop(const json::Node& node);

    static bool IsBuses(const json::Node& node);

    static bool IsBusesRequest(const json::Node& node);

    static bool IsStopRequest(const json::Node& node);

    static bool IsMapRequest(const json::Node& node);

 
    static svg::Color ReadColor(const json::Node& node);
    static svg::Point ReadOffset(const json::Array& node);

    json::Document data_;
};

} // namespace json_reader
