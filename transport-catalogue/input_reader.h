#pragma once
#include <string>
#include <deque>

namespace transport_catalogue::input_reader {
    std::deque<std::string> ReadRequests();
}

namespace detail {
    void RemoveBeginEndSpaces(std::string_view& str);

    std::string_view SeparatorString(std::string_view& str, char separator);

    void RemoveBeginSpace(std::string& data);
}
