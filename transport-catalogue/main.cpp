#include "transport_catalogue.h"
#include "input_reader.h"
#include "stat_reader.h"
#include <iostream>


int main() {
	transport_catalogue::TransportCatalogue catalogue;
	
	transport_catalogue::input_reader::ReadRequests(catalogue, std::cin);
	transport_catalogue::stat_reader::StatReader(catalogue, std::cin, std::cout);
	
	return 0;
}
