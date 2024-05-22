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

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <utility>

void merge_touching_ways_nestedloop(std::vector<std::vector<osmium::object_id_type>> &ways){
    std::vector<std::vector<osmium::object_id_type>> unclosed_ways;
    // Consider only ways that are not closed
    for(std::size_t i = 0; i < ways.size(); i++)
        if(ways[i].front() != ways[i].back())
            unclosed_ways.push_back(ways[i]);
    std::cout << "Unclosed ways: " << unclosed_ways.size() << ", out of Ways: " << ways.size() << std::endl;


    // std::vector<std::vector<osmium::object_id_type>> merged_way;
    int removed_count = 0;
    std::vector<bool> removed_way_flags;
    removed_way_flags.resize(unclosed_ways.size(), false);

    for(std::size_t i = 0; i < unclosed_ways.size(); i++)
    {
        if(removed_way_flags[i])
            continue;
        for(std::size_t j = i+1; j < unclosed_ways.size(); j++){
            if (removed_way_flags[j])
                continue;
            if(unclosed_ways[i].back() == unclosed_ways[j].front()){
                unclosed_ways[i].insert(unclosed_ways[i].end(), unclosed_ways[j].begin(), unclosed_ways[j].end());
                // merged_way.push_back(ways[i]);
                // ways.erase(ways.begin() + j);
                removed_way_flags[j] = true;
                removed_count++;
                // j = i;
            } else if(unclosed_ways[i].front() == unclosed_ways[j].back()){
                unclosed_ways[j].insert(unclosed_ways[j].end(), unclosed_ways[i].begin(), unclosed_ways[i].end());
                // ways.erase(ways.begin() + i);
                removed_way_flags[i] = true;
                removed_count++;

                // if(i==0)
                //     continue;
                // i -= 1;
            }
            printf("%ld and %ld\n", i, j);
        }
    }
    std::cout << "Removed ways: " << removed_count << std::endl;
}

// void merge_touching_ways_pair(std::vector<std::vector<osmium::object_id_type>> &ways){
//     std::unordered_map<std::pair<std::size_t, std::size_t>, std::vector<osmium::object_id_type>> hash_map;
//     std::vector<std::vector<osmium::object_id_type>> unclosed_ways;
//     // Consider only ways that are not closed
//     for(std::size_t i = 0; i < ways.size(); i++)
//         if(ways[i].front() != ways[i].back()){
//             unclosed_ways.push_back(ways[i]);
//             hash_map[std::make_pair(ways[i].front(), ways[i].back())] = ways[i]; // hash_map has all unclosed ways
//         }
//     int count_found = 0;
//     int count_not_found = 0;
//     // std::cout << "Unclosed ways: " << unclosed_ways.size() << std::endl;
//     // std::cout << "Hash map size before merging: " << hash_map.size() << std::endl;

//     for(std::size_t i = 0; i < unclosed_ways.size(); i++)
//     {
//         if(hash_map.find(unclosed_ways[i].back()) != hash_map.end()){ // if the way is touching another way
//             count_found++; // unclosed way has the front and we check for its end in the hash_map
//             unclosed_ways[i].insert(unclosed_ways[i].end(), hash_map[unclosed_ways[i].back()].begin()+1, hash_map[unclosed_ways[i].back()].end());
//             // hash_map[unclosed_ways[i].front()] = unclosed_ways[i]; // update hash_map with the new way
//             auto end = hash_map[unclosed_ways[i].front()].end();
//             auto front_way = hash_map[unclosed_ways[i].back()];
//             // hash_map[unclosed_ways[i].back()].insert(begin, unclosed_ways[i].begin(), unclosed_ways[i].end());
//             hash_map[unclosed_ways[i].front()] = unclosed_ways[i];
//             hash_map.erase(unclosed_ways[i].back());
//             // hash_map[unclosed_ways[i].front()].insert(end, front_way.begin(), front_way.end());
//         }
//         else
//             count_not_found++;

//     std::cout << "Found: " << count_found << std::endl;
//     std::cout << "Not found: " << count_not_found << std::endl;

//     std::cout << "Hash map size after merging: " << hash_map.size() << std::endl;

//     }
// }

void merge_touching_ways(std::vector<std::vector<osmium::object_id_type>> &ways){
    std::unordered_map<std::size_t, std::vector<osmium::object_id_type>> hash_map;
    std::vector<std::vector<osmium::object_id_type>> unclosed_ways;
    // Consider only ways that are not closed
    for(std::size_t i = 0; i < ways.size(); i++)
        if(ways[i].front() != ways[i].back()){
            unclosed_ways.push_back(ways[i]);
            hash_map[ways[i].front()] = ways[i]; // hash_map has all unclosed ways
        }
    int count_found = 0;
    int count_not_found = 0;
    // std::cout << "Unclosed ways: " << unclosed_ways.size() << std::endl;
    // std::cout << "Hash map size before merging: " << hash_map.size() << std::endl;

    for(std::size_t i = 0; i < unclosed_ways.size(); i++)
    {
        if(hash_map.find(unclosed_ways[i].back()) != hash_map.end()){ // if the way is touching another way
            count_found++; // unclosed way has the front and we check for its end in the hash_map
            unclosed_ways[i].insert(unclosed_ways[i].end(), hash_map[unclosed_ways[i].back()].begin()+1, hash_map[unclosed_ways[i].back()].end());
            // hash_map[unclosed_ways[i].front()] = unclosed_ways[i]; // update hash_map with the new way
            auto end = hash_map[unclosed_ways[i].front()].end();
            auto front_way = hash_map[unclosed_ways[i].back()];
            // hash_map[unclosed_ways[i].back()].insert(begin, unclosed_ways[i].begin(), unclosed_ways[i].end());
            hash_map[unclosed_ways[i].front()] = unclosed_ways[i];
            hash_map.erase(unclosed_ways[i].back());
            // hash_map[unclosed_ways[i].front()].insert(end, front_way.begin(), front_way.end());
        }
        else
            count_not_found++;

    // std::cout << "Found: " << count_found << std::endl;
    // std::cout << "Not found: " << count_not_found << std::endl;

    // std::cout << "Hash map size after merging: " << hash_map.size() << std::endl;

    }
}

int read_coastline_ways(std::string inputFile, std::vector<std::vector<osmium::object_id_type>> &ways_collection)
{
    try {
        osmium::io::File input_file{inputFile};
        osmium::io::Reader reader{input_file, osmium::osm_entity_bits::node | osmium::osm_entity_bits::way};

        while (osmium::memory::Buffer buffer = reader.read()) {
            // make an empty list of node ids that we will gather if its part of a way of coastline

            for (const auto& item : buffer) {

                if (item.type() == osmium::item_type::way) {
                    const auto& way = static_cast<const osmium::Way&>(item);

                    if (way.tags().has_tag("natural", "coastline")) {
                        ways_collection.push_back(std::vector<osmium::object_id_type>());
                        for (const auto& node : way.nodes()) {
                            ways_collection.back().push_back(node.ref());
                        }
                    }
                }
            }
        }
        reader.close();

        return 0;
    } catch (const std::exception& e) {
        // All exceptions used by the Osmium library derive from std::exception.
        std::cerr << e.what() << '\n';
        return 1;
    }
}

void extract_nodes_from_ways(std::vector<std::vector<osmium::object_id_type>> &ways_collection, std::unordered_set<osmium::object_id_type> &node_ids)
{
    for (auto way : ways_collection)
        for (auto node_id : way)
            node_ids.insert(node_id);
}

int read_coastline_nodes(std::string inputFile, std::unordered_set<osmium::object_id_type> &node_ids,
                         std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations)
{
    try {
        osmium::io::File input_file{inputFile};
        osmium::io::Reader reader{input_file, osmium::osm_entity_bits::node | osmium::osm_entity_bits::way};

        while (osmium::memory::Buffer buffer = reader.read()) {
            // make an empty list of node ids that we will gather if its part of a way of coastline

            for (const auto& item : buffer) {

                if (item.type() == osmium::item_type::node) {
                    const auto& node = static_cast<const osmium::Node&>(item);
                    if(node_ids.find(node.id()) != node_ids.end())
                        node_locations[node.id()] = node.location();
                }
            }
        }
        reader.close();
        return 0;
    } catch (const std::exception& e) {
        // All exceptions used by the Osmium library derive from std::exception.
        std::cerr << e.what() << '\n';
        return 1;
    }
}

int write_geojson_nodes(std::string outputFilePath, std::unordered_map<osmium::object_id_type, osmium::Location> &nodes){
    std::ofstream outputFile(outputFilePath);
    osmium::geom::GeoJSONFactory<> geojson_factory;
    if (outputFile.is_open())
    {
        outputFile << "{\n\t\"type\": \"FeatureCollection\",\n\t\"features\": [\n";

        for (auto& node : nodes){
            outputFile << "{\"type\": \"Feature\", \"properties\": {}, \"geometry\": " << geojson_factory.create_point(node.second) << "},\n";
        }

        outputFile << "\t]\n}";

    }
    else
    {
        std::cerr << "Error opening file\n";
    }

    return 0;
}


int main_old() {
// -o3
// parity of crossing lines
// Lines 180
// Grid
    try {
        std::string inputFile = "planet-coastlinespbf-cleaned.osm.pbf";
        // std::string inputFile = "antarctica-latest.osm.pbf";

        // std::ofstream outputFile("t2.out");
        std::ofstream outputFile("t2_delete.out");

        osmium::io::File input_file{inputFile};
        osmium::io::Reader reader{input_file, osmium::osm_entity_bits::node | osmium::osm_entity_bits::way};

        // Initialize progress bar, enable it only if STDERR is a TTY.
        // osmium::ProgressBar progress{reader.file_size(), osmium::isatty(2)};

        // Create a GeoJSONFactory for converting geometries to GeoJSON
        osmium::geom::GeoJSONFactory<> geojson_factory;
        std::vector<osmium::object_id_type> node_ids;
        std::vector<std::vector<osmium::object_id_type>> ways_collection;

        while (osmium::memory::Buffer buffer = reader.read()) {
            // make an empty list of node ids that we will gather if its part of a way of coastline

            for (const auto& item : buffer) {

                if (item.type() == osmium::item_type::way) {
                    const auto& way = static_cast<const osmium::Way&>(item);

                    if (way.tags().has_tag("natural", "coastline")) {
                        ways_collection.push_back(std::vector<osmium::object_id_type>());
                        for (const auto& node : way.nodes()) {
                            node_ids.push_back(node.ref());
                            ways_collection.back().push_back(node.ref());
                            // std::cout << node.ref() << std::endl;
                        }
                        // std::cout << geojson_factory.create_point(node.location()) << std::endl;

                        // geojson_factory.create_linestring(way);
                        // std::cout << geojson_factory.create_linestring(way) << std::endl;
                    }
                }
            }
        }
        reader.close();

        std::cout << "nodes count: " << node_ids.size() << std::endl;
        // 689471
        // 56418325 in planet-coastlines
        std::cout << "ways count: " << ways_collection.size() << std::endl;
        // 19224
        // 798 after merging

        merge_touching_ways(ways_collection);
        // merge_touching_ways_nestedloop(ways_collection);
        // std::cout << "ways count after merging: " << ways_collection.size() << std::endl;

        // return 0;

        // write_to_file(outputFile):
        // Open output file and write the json data
        if (outputFile.is_open())
        {
            outputFile << "{\n\t\"type\": \"FeatureCollection\",\n\t\"features\": [\n";

            // reread pbf file
            // osmium::io::Reader reader2{input_file};
            // osmium::io::Reader reader2{input_file, osmium::osm_entity_bits::node | osmium::osm_entity_bits::way | osmium::osm_entity_bits::relation};
            osmium::io::Reader reader{input_file, osmium::osm_entity_bits::node | osmium::osm_entity_bits::way};

            while (const auto& buffer = reader.read()) {
                for (const auto& item : buffer) {
                    if (item.type() == osmium::item_type::node) {
                        const auto& node = static_cast<const osmium::Node&>(item);
                        // check if node id is in the list of node ids
                        if (std::find(node_ids.begin(), node_ids.end(), node.id()) != node_ids.end()){
                            outputFile << "{\"type\": \"Feature\", \"properties\": {}, \"geometry\": " << geojson_factory.create_point(node) << "},\n";
                            // outputFile << "\t{" << std::endl;
                            // outputFile << "\t\t" << "\"type\": \"Feature\"," << std::endl;
                            // outputFile << "\t\t" << "\"properties\": {}," << std::endl;
                            // outputFile << "\t\t" << "\"geometry\": " << std::endl;

                            // outputFile << "\t\t" << geojson_factory.create_point(node) << std::endl;

                            // outputFile << "\t}," << std::endl;
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

// main method with file input and output from args
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " INPUT-FILE OUTPUT-FILE\n";
        return 1;
    }

    std::string input_file_name = argv[1];
    std::string output_file_name = argv[2];

    std::vector<std::vector<osmium::object_id_type>> ways_collection;
    std::unordered_set<osmium::object_id_type> node_ids;
    std::unordered_map<osmium::object_id_type, osmium::Location> node_locations;

    read_coastline_ways(input_file_name, ways_collection);
    std::cout << "Coastline Ways count: " << ways_collection.size() << std::endl;
    extract_nodes_from_ways(ways_collection, node_ids);
    std::cout << "Nodes extracted: " << node_ids.size() << std::endl;
    read_coastline_nodes(input_file_name, node_ids, node_locations);
    std::cout << "Coastline Nodes count: " << node_locations.size() << std::endl;

    // merge_touching_ways(ways_collection);

    // if (write_geojson_nodes(output_file_name, node_locations) == 0)
    //     std::cout << "GeoJSON file written successfully\n";
    // else
    //     std::cerr << "Error writing GeoJSON file\n";

    return 0;
}
