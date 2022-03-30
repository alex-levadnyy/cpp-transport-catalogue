#include "input_reader.h"
#include "geo.h"

#include <unordered_set>
#include <string_view>

using namespace std::string_literals;

namespace transport_catalogue {
	namespace input_reader {
		//Удаляем пробелы в начале string
		void RemoveBeginSpace(std::string& data) {
			const size_t first_not_space = data.find_first_not_of(' ');
			(first_not_space == data.npos) ? data.erase(0, data.size()) : data.erase(0, first_not_space);
		}

		//Удаляем пробелы в конце string
		void RemoveEndSpace(std::string& data) {
			const size_t last_not_space = data.find_last_not_of(' ');
			(last_not_space == data.npos) ? data.erase(0, data.size()) : data.erase(last_not_space + 1, data.size());
		}

		//Парсим остановки и координаты
		void ParseStop(TransportCatalogue& catalogue, std::string& stop_src_data, std::unordered_map<std::string, std::string>& stops) {
			Coordinates stop;
			size_t position = stop_src_data.find(":"s);
			std::string stop_name = stop_src_data.substr(5, position - 5);
			RemoveBeginSpace(stop_name);
			RemoveEndSpace(stop_name);
			stop_src_data = stop_src_data.substr(position + 2);
			stop = ParseCoordinate(stop_src_data);
			position = stop_src_data.find(","s);
			if (position != std::string::npos) {
				stops[stop_name] = stop_src_data.substr(position + 2);
			}
			catalogue.AddStop(stop_name, stop);
		}

		//Парсим автобусы
		void ParseBus(TransportCatalogue& catalogue, std::string& bus) {
			bool is_circle = false;
			std::vector<std::string_view> route;
			auto position = bus.find(":"s);
			std::string bus_name = bus.substr(4, position - 4);
			bus = bus.substr(position + 2);
			std::string name;
			std::string spliter = ">"s;
			if (bus.find("-"s) != std::string::npos) {
				spliter = "-"s;
				is_circle = true;
			}
			while (!bus.empty()) {
				position = bus.find(spliter);
				if (position == std::string::npos) {
					name = bus;
					RemoveBeginSpace(name);
					RemoveEndSpace(name);
					break;
				}
				name = bus.substr(0, position - 1);
				RemoveBeginSpace(name);
				RemoveEndSpace(name);
				const auto stop = catalogue.GetStop(name);
				if (stop != nullptr) {
					route.push_back(stop->name);
				}
				bus = bus.substr(position + 2);
			}
			const auto stop = catalogue.GetStop(name);
			if (stop != nullptr) {
				route.push_back(stop->name);
			}
			if (is_circle) {
				for (int i = route.size() - 2; i >= 0; --i) {
					route.push_back(route[i]);
				}
			}
			catalogue.AddBus(bus_name, route, is_circle);
		}
		
		//Парсим информацию об остановках
		void ParseStopInfo(TransportCatalogue& catalogue, const std::string& first_stop, std::string& info) {
			while (!info.empty()) {
				auto position = info.find("m"s);
				const std::string distance = info.substr(0, position);
				info = info.substr(position + 5);
				position = info.find(","s);
				const std::string second_stop = info.substr(0, position);
				const auto stop_first = catalogue.GetStop(first_stop);
				const auto stop_second = catalogue.GetStop(second_stop);
				catalogue.SetDistanceStops(stop_first->name, stop_second->name, atoi(distance.c_str()));
				if (position != std::string::npos) {
					info = info.substr(position + 2);
				} else {
					break;
				}
			}
		}

		//Парсим координаты
		Coordinates ParseCoordinate(std::string& data)
		{
			Coordinates stop_coordinates;
			size_t position = data.find(","s);
			double lat = std::stod(data.substr(0, position));
			data = data.substr(position + 2);
			position = data.find(","s);
			double lng;
			if (position != std::string::npos) {
				lng = std::stod(data.substr(0, position));
			}
			else {
				lng = std::stod(data);
			}
			stop_coordinates = { lat,lng };
			return stop_coordinates;
		}
		
		//Парсим запросы
		void ReadRequests(TransportCatalogue& catalogue, std::istream& input) {
			size_t count_requests;
			input >> count_requests;
			std::string line;
			size_t count = 0;
			std::unordered_set<std::string> buses;
			std::unordered_map<std::string, std::string> stops;
			while (getline(input, line)) {
				if (line.find("Stop"s) != std::string::npos) {
					ParseStop(catalogue, line, stops);
					++count;
				} else if (line.find("Bus"s) != std::string::npos) {
					buses.insert(line);
					++count;
				}
				if (count >= count_requests) {
					break;
				}
			}

			for (auto [first_stop, distances] : stops) {
				ParseStopInfo(catalogue, first_stop, distances);
			}

			for (std::string bus : buses) {
				ParseBus(catalogue, bus);
			}
		}
	}
}
