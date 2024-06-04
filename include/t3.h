#ifndef T3_H
#define T3_H

#include <iostream>
#include <osmium/io/pbf_input.hpp>

#include <osmium/geom/coordinates.hpp>
#include <osmium/geom/geojson.hpp>
#include <osmium/geom/mercator_projection.hpp> // Include MercatorProjection

#include <cmath>

#include <unordered_map>


void get_final_closed_ways(std::string &input_file_name,
                           std::vector<std::vector<osmium::object_id_type>> &final_closed_ways,
                           std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations);
void getBoundingBox(std::vector<std::vector<osmium::object_id_type>> &closed_ways,
                    std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations,
                    std::vector<std::pair<osmium::Location, osmium::Location>> &bounding_boxes);
double latitude(double lambda, double lambda1, double lambda2, double phi1, double phi2);
double latitude_crossing(double lambda, double lambda1, double lambda2, double phi1, double phi2);
bool isWater(osmium::Location &Q,
            std::vector<std::pair<osmium::Location, osmium::Location>> &bounding_boxes,
            std::unordered_map<osmium::object_id_type,
            osmium::Location> &node_locations);

int main_task3(int argc, char* argv[]);

#endif // T3_H