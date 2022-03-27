#include "geo.h"
#include "input_reader.h"
#include "transport_catalogue.h"
#include "stat_reader.h"

int main(){
    using namespace transport_catalogue;
    TransportCatalogue t(input_reader::ReadRequests());
    stat_reader::StatReader(t);
    return 0;
}
