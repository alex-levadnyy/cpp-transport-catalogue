#include "input_reader.h"
#include <iostream>

//читаем запрос и заполняем контейнер
//для последующей обработки
//в начало Stop
//в конец Bus
namespace transport_catalogue::input_reader {
    std::deque<std::string> ReadRequests() {
        using namespace std::string_literals;
        int count;
        std::deque<std::string> requests;

        std::cin >> count;
        std::cin.ignore();
        for (int i = 0; i < count; ++i) {
            std::string query;
            std::getline(std::cin, query);
            detail::RemoveBeginSpace(query);
            if (query.substr(0, 3) == "Bus"s) {
                requests.push_back(query);
            }
            else if (query.substr(0, 4) == "Stop"s) {
                requests.push_front(query);
            }
        }
        return requests;
    }
}

namespace detail {
    
    //Удаляем пробелы в начале string
    void RemoveBeginSpace(std::string& data) {
        size_t first_not_space = data.find_first_not_of(' ');
        (first_not_space == data.npos) ? data.erase(0, data.size()) : data.erase(0, first_not_space);
    }

    //Удаляем пробелы в начале и конце string_view
    void RemoveBeginEndSpaces(std::string_view& str) {
        size_t first_not_space = str.find_first_not_of(' ');
        (first_not_space == str.npos) ? str.remove_prefix(str.size()) : str.remove_prefix(first_not_space);

        size_t last_not_spase = str.find_last_not_of(' ');
        if (last_not_spase != str.npos) {
            str.remove_suffix(str.size() - last_not_spase - 1);
        }
    }

    //Отрезаем строку по separator
    std::string_view SeparatorString(std::string_view& str, char separator) {
        std::string_view name;
        std::size_t sep_pos;
        //определяем конец строки
        (str.find(separator) != str.npos) ? sep_pos = str.find(separator) : sep_pos = str.npos;
        name = str.substr(0, sep_pos);
        //удаляем пробелы в начале и в конце
        RemoveBeginEndSpaces(name);
        (sep_pos != str.npos) ? str.remove_prefix(sep_pos + 1) : str.remove_prefix(str.size());
        return name;
    }
}
