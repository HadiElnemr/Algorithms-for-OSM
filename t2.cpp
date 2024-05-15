#include <iostream>
#include <osmium/io/pbf_input.hpp>

// Get access to isatty utility function and progress bar utility class.
#include <osmium/util/file.hpp>
#include <osmium/util/progress_bar.hpp>

#include <osmium/geom/coordinates.hpp>
#include <osmium/geom/geojson.hpp>
#include <osmium/geom/mercator_projection.hpp> // Include MercatorProjection
#include <osmium/geom/wkt.hpp> // Include WKTFactory

#include <fstream>
#include <iostream>


void merge_touching_ways(std::vector<std::vector<osmium::object_id_type>> &ways){
    for(std::size_t i = 0; i < ways.size(); i++)
        for(std::size_t j = i+1; j < ways.size(); j++)
            if(ways[i].back() == ways[j].front()){
                ways[i].insert(ways[i].end(), ways[j].begin(), ways[j].end());
                ways.erase(ways.begin() + j);
                j = i;
            } else if(ways[i].front() == ways[j].back()){
                ways[j].insert(ways[j].end(), ways[i].begin(), ways[i].end());
                ways.erase(ways.begin() + i);
                if(i==0)
                    continue;
                i -= 1;
            } 
}

int main() {
// -o3
// parity of crossing lines
// Lines 180
// Grid
    try {
        // std::string inputFile = "planet-coastlinespbf-cleaned.osm.pbf";
        std::string inputFile = "antarctica-latest.osm.pbf";
        
        // std::ofstream outputFile("t2.out");
        std::ofstream outputFile("t2_delete.out");
        
        osmium::io::File input_file{inputFile};
        osmium::io::Reader reader{input_file, osmium::osm_entity_bits::node | osmium::osm_entity_bits::way};
        
        // Initialize progress bar, enable it only if STDERR is a TTY.
        // osmium::ProgressBar progress{reader.file_size(), osmium::isatty(2)};
        
        // Create a GeoJSONFactory for converting geometries to GeoJSON
        osmium::geom::GeoJSONFactory<> geojson_factory;
        std::vector<osmium::object_id_type> node_ids;
        std::vector<std::vector<osmium::object_id_type>> way_nodes;

        while (osmium::memory::Buffer buffer = reader.read()) {
            // make an empty list of node ids that we will gather if its part of a way of coastline

            for (const auto& item : buffer) {

                if (item.type() == osmium::item_type::way) {
                    const auto& way = static_cast<const osmium::Way&>(item);
                                    
                    if (way.tags().has_tag("natural", "coastline")) {
                        way_nodes.push_back(std::vector<osmium::object_id_type>());
                        for (const auto& node : way.nodes()) {
                            node_ids.push_back(node.ref());
                            way_nodes.back().push_back(node.ref());
                            // std::cout << node.ref() << std::endl;
                        }
                        // std::cout << geojson_factory.create_point(node.location()) << std::endl;
                        
                        // geojson_factory.create_linestring(way);
                        // std::cout << geojson_factory.create_linestring(way) << std::endl;
                    }
                }
            }
        }
        // reader.close();

        std::cout << "nodes count: " << node_ids.size() << std::endl;
        std::cout << "ways count: " << way_nodes.size() << std::endl;
        // 689471
        // 56418325 in planet-coastlines

        merge_touching_ways(way_nodes);
        std::cout << "ways count after merging: " << way_nodes.size() << std::endl;
        
        // Open output file and write the json data
        if (outputFile.is_open()) 
        {   
            outputFile << "{\n\t\"type\": \"FeatureCollection\",\n\t\"features\": [\n";

            // reread pbf file
            // osmium::io::Reader reader2{input_file};
            // osmium::io::Reader reader2{input_file, osmium::osm_entity_bits::node | osmium::osm_entity_bits::way | osmium::osm_entity_bits::relation};
            reader.close();
            osmium::io::Reader reader{input_file, osmium::osm_entity_bits::node | osmium::osm_entity_bits::way | osmium::osm_entity_bits::relation};
            
            while (const auto& buffer = reader.read()) {
                for (const auto& item : buffer) {
                    if (item.type() == osmium::item_type::node) {
                        const auto& node = static_cast<const osmium::Node&>(item);
                        // check if node id is in the list of node ids
                        if (std::find(node_ids.begin(), node_ids.end(), node.id()) != node_ids.end()){
                            outputFile << "\t{" << std::endl; 
                            outputFile << "\t\t" << "\"type\": \"Feature\"," << std::endl;
                            outputFile << "\t\t" << "\"properties\": {}," << std::endl;
                            outputFile << "\t\t" << "\"geometry\": " << std::endl;
                            
                            outputFile << "\t\t" << geojson_factory.create_point(node) << std::endl; 

                            outputFile << "\t}," << std::endl;
                        }
                    }
                }
            }
            reader.close();

            outputFile << "\t]\n}";

        }
        else
        {
            std::cerr << "Error opening file\n";
        }

        reader.close();
        return 0;
    } catch (const std::exception& e) {
        // All exceptions used by the Osmium library derive from std::exception.
        std::cerr << e.what() << '\n';
        return 1;
    }   
}