#include "t2.h"

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

// void merge_touching_ways(std::vector<std::vector<osmium::object_id_type>> &ways){
//     std::unordered_map<std::size_t, std::vector<osmium::object_id_type>> hash_map;
//     std::vector<std::vector<osmium::object_id_type>> unclosed_ways;
//     // Consider only ways that are not closed
//     for(std::size_t i = 0; i < ways.size(); i++)
//         if(ways[i].front() != ways[i].back()){
//             unclosed_ways.push_back(ways[i]);
//             hash_map[ways[i].front()] = ways[i]; // hash_map has all unclosed ways
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

//     // std::cout << "Found: " << count_found << std::endl;
//     // std::cout << "Not found: " << count_not_found << std::endl;

//     // std::cout << "Hash map size after merging: " << hash_map.size() << std::endl;

//     }
// }

void merge_touching_ways(std::vector<std::vector<osmium::object_id_type>> &unclosed_ways,
                         std::vector<std::vector<osmium::object_id_type>> &stitched_ways,
                         bool closed_ways_only)
{
    std::unordered_map<osmium::object_id_type, std::vector<osmium::object_id_type>> hash_map;
    hash_map.reserve(unclosed_ways.size());

    for(auto way : unclosed_ways){
        hash_map[way.front()] = way;
    }
    for(auto& way : unclosed_ways){
        while(hash_map.find(way.back()) != hash_map.end()){
            way.insert(way.end(), hash_map[way.back()].begin()+1, hash_map[way.back()].end());
            hash_map[way.front()] = way;
            if(way.front() == way.back()){
                break;
            }
            hash_map.erase(way.back());
        }
    }
    stitched_ways.reserve(hash_map.size());
    if(closed_ways_only)
        for(auto way : hash_map){
            if(way.second.front() == way.second.back())
                stitched_ways.push_back(way.second);
        }
    else
        for(auto way : hash_map)
            stitched_ways.push_back(way.second);
    hash_map.clear();
}

int read_coastline_ways(std::string &inputFile, std::vector<std::vector<osmium::object_id_type>> &ways_collection)
{
    try {
        osmium::io::File input_file{inputFile};
        osmium::io::Reader reader{input_file, osmium::osm_entity_bits::way};

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
        ways_collection.shrink_to_fit();
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

int read_coastline_nodes(std::string &inputFile,
                         std::unordered_set<osmium::object_id_type> &node_ids,
                         std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations)
{
    try {
        node_locations.reserve(node_ids.size());

        osmium::io::File input_file{inputFile};
        osmium::io::Reader reader{input_file, osmium::osm_entity_bits::node};

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

void print_geojson_nodes(std::unordered_map<osmium::object_id_type, osmium::Location> &nodes)
{
    osmium::geom::GeoJSONFactory<> geojson_factory;
    for (auto& node : nodes)
        std::cout << "{\"type\": \"Feature\", \"properties\": {}, \"geometry\": " << geojson_factory.create_point(node.second) << "},\n";
}

int write_geojson_nodes(std::string &outputFilePath, std::unordered_map<osmium::object_id_type, osmium::Location> &nodes){
    std::ofstream outputFile(outputFilePath);
    osmium::geom::GeoJSONFactory<> geojson_factory;
    if (outputFile.is_open())
    {
        outputFile << "{\n\t\"type\": \"FeatureCollection\",\n\t\"features\": [\n";
        bool first = true;
        for (auto& node : nodes){
            if (first)
                first = false;
            else
                outputFile << ",\n";
            outputFile << "{\"type\": \"Feature\", \"properties\": {}, \"geometry\": " << geojson_factory.create_point(node.second) << "}";
        }

        outputFile << "\t]\n}";

    }
    else
    {
        std::cerr << "Error opening file\n";
    }

    return 0;
}

void get_closed_ways(std::vector<std::vector<osmium::object_id_type>> &ways_collection, std::vector<std::vector<osmium::object_id_type>> &closed_ways)
{
    for (auto way : ways_collection)
        if (way.front() == way.back())
            closed_ways.push_back(way);
    closed_ways.shrink_to_fit();
}

void get_unclosed_ways(std::vector<std::vector<osmium::object_id_type>> &ways_collection, std::vector<std::vector<osmium::object_id_type>> &unclosed_ways)
{
    for (auto way : ways_collection)
        if (way.front() != way.back())
            unclosed_ways.push_back(way);
    unclosed_ways.shrink_to_fit();
}

// void stitch_ways(std::vector<std::vector<osmium::object_id_type>> &ways_collection, std::vector<std::vector<osmium::object_id_type>> &closed_ways)
// int main_old() {
//         .....
//         std::cout << "nodes count: " << node_ids.size() << std::endl;
//         // 689471
//         // 56418325 in planet-coastlines
//         std::cout << "ways count: " << ways_collection.size() << std::endl;
//         // 19224
//         // 798 after merging
//         .....
// }

// main method with file input and output from args
int main_task2(int argc, char* argv[]) {
    if (argc != 3 && argc != 2) {
        std::cerr << "Usage: " << argv[0] << " INPUT-FILE or INPUT-FILE OUTPUT-FILE\n";
        return 1;
    }

    std::string input_file_name = argv[1];

    std::vector<std::vector<osmium::object_id_type>> ways_collection;
    std::unordered_set<osmium::object_id_type> node_ids;
    std::unordered_map<osmium::object_id_type, osmium::Location> node_locations;
    std::vector<std::vector<osmium::object_id_type>> stitched_ways;

    read_coastline_ways(input_file_name, ways_collection);
    std::cout << "Coastline Ways count: " << ways_collection.size() << std::endl;
    extract_nodes_from_ways(ways_collection, node_ids);
    std::cout << "Nodes extracted: " << node_ids.size() << std::endl;

    read_coastline_nodes(input_file_name, node_ids, node_locations);
    std::cout << "Coastline Nodes count: " << node_locations.size() << std::endl;

    node_ids.clear();

    // Before stitching
    std::vector<std::vector<osmium::object_id_type>> closed_ways;
    std::vector<std::vector<osmium::object_id_type>> unclosed_ways;
    get_closed_ways(ways_collection, closed_ways);
    get_unclosed_ways(ways_collection, unclosed_ways);
    std::cout << "Closed Ways count (before stitching): " << closed_ways.size() << std::endl;
    std::cout << "Unclosed Ways count (before stitching): " << unclosed_ways.size() << std::endl;

    ways_collection.clear();

    // Stitching
    merge_touching_ways(unclosed_ways, stitched_ways, false);
    std::cout << "Unclosed -> Stitched Ways count: " << stitched_ways.size() << std::endl;

    // Merge unstitched ways with stitched ways
    for(auto way : closed_ways)
        stitched_ways.push_back(way);
    std::cout << "Stitched + Closed Ways count: " << stitched_ways.size() << std::endl;
    // After stitching
    std::vector<std::vector<osmium::object_id_type>> closed_ways_after_stitching;
    std::vector<std::vector<osmium::object_id_type>> unclosed_ways_after_stitching;
    get_closed_ways(stitched_ways, closed_ways_after_stitching);
    get_unclosed_ways(stitched_ways, unclosed_ways_after_stitching);
    std::cout << "Closed Ways count (after stitching): " << closed_ways_after_stitching.size() << std::endl;
    std::cout << "Unclosed Ways count (after stitching): " << unclosed_ways_after_stitching.size() << std::endl;

    if(argc == 3){
        std::string output_file_name = argv[2];
        if (write_geojson_nodes(output_file_name, node_locations) == 0)
            std::cout << "GeoJSON file written successfully\n";
        else
            std::cerr << "Error writing GeoJSON file\n";
    }

    return 0;
}