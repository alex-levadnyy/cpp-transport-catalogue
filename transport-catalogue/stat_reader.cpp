#include "stat_reader.h"
#include "input_reader.h"
#include <iostream>

namespace transport_catalogue::stat_reader {

    //Перегрузка оператора для вывода структуры BusRoute
    std::ostream& operator<<(std::ostream& out, const BusRoute& route_info)
    {
        using namespace std::string_literals;
        if (!route_info.is_found) {
            out << "Bus "s << route_info.bus_name << ": not found"s << std::endl;
        }
        else {
            out << "Bus "s << route_info.bus_name << ": " << route_info.stops << " stops on route, "s << route_info.unique_stops <<
                " unique stops, "s << route_info.true_length << " route length, "s << route_info.curvature << " curvature"s << std::endl;
        }
        return out;
    }

    //Перегрузка оператора для вывода структуры StopRoutes
    std::ostream& operator<<(std::ostream& out, const StopRoutes& route_info)
    {
        using namespace std::string_literals;
        if (!route_info.is_found) {
            out << "Stop "s << route_info.stop_name << ": not found"s << std::endl;
        }
        else if (route_info.is_found && route_info.routes.empty()) {
            out << "Stop "s << route_info.stop_name << ": no buses"s << std::endl;
        }
        else {
            out << "Stop "s << route_info.stop_name << ": buses"s;
            for (auto &i : route_info.routes) {
                out << " "s << i;
            }
            out << std::endl;
        }
        return out;
    }

    //Чтение и обрабортка запросов на вывод
    void StatReader(TransportCatalogue data) {
        using namespace std::string_literals;
        int num;
        std::cin >> num;
        std::cin.ignore();
        for (int i = 0; i < num; ++i) {
            std::string query;
            std::getline(std::cin, query);
            detail::RemoveBeginSpace(query);
            if (query.substr(0, 3) == "Bus"s) {
                query.erase(0, 3);
                PrintBus(data.RouteInformation(query));
            }
            if (query.substr(0, 4) == "Stop"s) {
                query.erase(0, 4);
                PrintStop(data.StopInformation(query));
            }
        }
    }

    void PrintBus(const BusRoute data) {
        std::cout << data;
    }

    void PrintStop(const StopRoutes &data) {
        std::cout << data;
    }    
}
