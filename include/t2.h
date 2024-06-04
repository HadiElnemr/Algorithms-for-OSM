#ifndef T2_H
#define T2_H

#include <iostream>
#include <osmium/io/pbf_input.hpp>

#include <osmium/util/file.hpp>
#include <osmium/util/progress_bar.hpp>

#include <osmium/osm.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>

#include <osmium/geom/coordinates.hpp>
#include <osmium/geom/geojson.hpp>
#include <osmium/geom/mercator_projection.hpp> // Include MercatorProjection
#include <osmium/geom/wkt.hpp> // Include WKTFactory


#include <fstream>
#include <iostream>

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <utility>


void merge_touching_ways_nestedloop(std::vector<std::vector<osmium::object_id_type>> &ways);
void merge_touching_ways(std::vector<std::vector<osmium::object_id_type>> &unclosed_ways,
                         std::vector<std::vector<osmium::object_id_type>> &stitched_ways,
                         bool closed_ways_only);
int read_coastline_ways(std::string &inputFile,
                        std::vector<std::vector<osmium::object_id_type>> &ways_collection);
void extract_nodes_from_ways(std::vector<std::vector<osmium::object_id_type>> &ways_collection,
                             std::unordered_set<osmium::object_id_type> &node_ids);
int read_coastline_ways_and_node_ids(const std::string &inputFile,
                                     std::vector<std::vector<osmium::object_id_type>> &ways_collection,
                                     std::unordered_set<osmium::object_id_type> &node_ids);
int read_coastline_nodes(const std::string &inputFile,
                         const std::unordered_set<osmium::object_id_type> &node_ids,
                         std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations);
void print_geojson_nodes(std::unordered_map<osmium::object_id_type, osmium::Location> &nodes);
int write_geojson_nodes(std::string &outputFilePath,
                        std::unordered_map<osmium::object_id_type, osmium::Location> &nodes);
void get_closed_ways(std::vector<std::vector<osmium::object_id_type>> &ways_collection,
                     std::vector<std::vector<osmium::object_id_type>> &closed_ways);
void get_unclosed_ways(std::vector<std::vector<osmium::object_id_type>> &ways_collection,
                       std::vector<std::vector<osmium::object_id_type>> &unclosed_ways);
int main_task2(int argc, char* argv[]);

#endif // T2_H