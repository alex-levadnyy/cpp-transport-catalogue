#include "json_reader.h"
#include "map_renderer.h"
#include "request_handler.h"

#include <cassert>
#include <iostream>


int main() {

    transport_catalogue::TransportCatalogue catalogue;
    renderer::MapRenderer renderer;
       
    json_reader::JsonLoader json(std::cin);
    json.LoadData(catalogue);
    json.LoadRenderSettings(renderer);
    json.AnswerRequests(catalogue, renderer, std::cout);
    
    transport_catalogue::RequestHandler catalogue_handler(catalogue, renderer);
}
