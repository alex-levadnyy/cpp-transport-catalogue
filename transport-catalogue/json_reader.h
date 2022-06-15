#pragma once

#include "json_builder.h"
#include "map_renderer.h"
#include "serialization.h"
#include "transport_catalogue.h"
#include "transport_router.h"

#include <iostream>
#include <memory>
#include <optional>

namespace json_reader {

// Класс для ввод/вывод данных TK в формате JSON
class JsonLoader final {
public:

    // При создании считывает все данные из входного потока
    JsonLoader(std::istream &data_in);

    // Загрузка данных из ТК, настроки ренедера, обработка запросов и запись в выходной поток 
    bool LoadData(transport_catalogue::TransportCatalogue &catalogue) const;
    std::optional<renderer::RenderSettings> LoadRenderSettings() const;
    std::optional<serialize::Serializator::Settings> LoadSerializeSettings () const;
    std::optional<transport_router::TransportRouter::RoutingSettings> LoadRoutingSettings() const;

    // Отрабатывает запросы и записывает ответы в выходной поток
    void AnswerRequests(const transport_catalogue::TransportCatalogue &catalogue,
                        const renderer::RenderSettings &render_settings,
                        transport_router::TransportRouter &router,
                        std::ostream &requests_out) const;

private:
    // Устанавливает настройки рендера
    renderer::RenderSettings LoadSettings(const json::Dict &data) const;

    // формирует и возвращает ответы на запросы
    json::Array LoadAnswers(const json::Array &requests,
                            const transport_catalogue::TransportCatalogue &catalogue,
                            const renderer::RenderSettings &render_settings,
                            transport_router::TransportRouter &router) const;

    // загрузка данных из json в каталог
    static void LoadStops(const json::Array &data, transport_catalogue::TransportCatalogue &catalogue);
    static void LoadBuses(const json::Array &data, transport_catalogue::TransportCatalogue &catalogue);
    static void LoadDistances(const json::Array &data, transport_catalogue::TransportCatalogue &catalogue);

    // возвращает ответ на запрос инфромации о маршруте, остановке, карте маршрута
    static json::Dict LoadRouteAnswer(const json::Dict &request,
                               const transport_catalogue::TransportCatalogue &catalogue);
    static json::Dict LoadStopAnswer(const json::Dict &request,
                               const transport_catalogue::TransportCatalogue &catalogue);
    static json::Dict LoadMapAnswer(const json::Dict &request,
                             const transport_catalogue::TransportCatalogue &catalogue,
                             const renderer::RenderSettings &render_settings);

    json::Dict LoadRouteBuildAnswer(const json::Dict &request,
                                    const transport_catalogue::TransportCatalogue &catalogue,
                                    transport_router::TransportRouter &router) const;

    // возвращает сообщение с ошибкой о запросе с некорректным именем автобуса или маршрута
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
};

} // namespace json_reader
