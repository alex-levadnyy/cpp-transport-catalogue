#pragma once

#include "json_reader.h"
#include "map_renderer.h"
#include "serialization.h"
#include "transport_catalogue.h"
#include "transport_router.h"

#include "filesystem"
#include "optional"

namespace transport_catalogue {

// Фасад реализующий основные запросы к системе
class RequestHandler final {
public:

    using Bus = transport_router::TransportRouter::TransportRoute;
    using RoutingSettings = transport_router::TransportRouter::RoutingSettings;

    // конструктор, принимает транспортный каталог по ссылке
    explicit RequestHandler(TransportCatalogue& catalogue);

    // возвращает информацию о маршруе по его имени
    // если маршрута нет в каталоге - выбрасывает исключение std::out_of_range
    domain::RouteInfo GetRouteInfo(const std::string& route_name) const;

    // возвращает список автобусов, проходящих через остановку
    // если остановки нет в каталоге - выбрасывает исключение std::out_of_range
    std::optional<std::reference_wrapper<const std::set<std::string_view>>>
        GetBusesOnStop(const std::string& stop_name) const;

    // возвращает сформированную "карту" маршрутов в формате svg-документа
    svg::Document RenderMap() const;

    // Возвращает маршрут между двумя остановками
    std::optional<Bus> BuildRoute(const std::string& from, const std::string& to);

    // загружает все доступные данные из JSON
    void LoadDataFromJson(const json_reader::JsonLoader& json);
    void LoadDataFronJson(const std::filesystem::path& file_path);

    // загружает запросы из Json и выводит ответы в поток out
    void LoadRequestsAndAnswer(const json_reader::JsonLoader& json, std::ostream& out);

    // Сериализует доступные данные
    bool SerializeData();
    // Десериализует доступные данные
    bool DeserializeData();

    // Принудительно переинициализирует маршрутизатор
    bool ReInitRouter();

    // методы для ручного выставления настроек
    void SetRenderSettings(const renderer::RenderSettings& render_settings);
    void SetRoutingSettings(const RoutingSettings& routing_settings);
    void SetSerializeSettings(const serialize::Serializator::Settings& serialize_setings);

private:
    // Инициализирует маршрутизатор
    bool InitRouter();

    TransportCatalogue& catalogue_;
    std::unique_ptr<transport_router::TransportRouter> router_;

    std::optional<renderer::RenderSettings> render_settings_;
    std::optional<RoutingSettings> routing_settings_;
    std::optional<serialize::Serializator::Settings> serialize_settings_;

};

} // namespace transport_catalogue
