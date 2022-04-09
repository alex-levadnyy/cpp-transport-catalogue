#include "domain.h"

namespace domain {

bool operator==(const Stop &lhs, const Stop &rhs) {
    return (lhs.name == rhs.name && lhs.coordinate == rhs.coordinate);
}

bool operator==(const Bus &lhs, const Bus &rhs) {
    return (lhs.name == rhs.name);
}

} // namespace domain
