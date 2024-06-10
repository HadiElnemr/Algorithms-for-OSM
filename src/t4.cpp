#include "t4.h"
#include "t3.h"
#include "t2.h"
#include <random>
#include <cmath>

std::vector<std::vector<osmium::object_id_type>> final_closed_ways;
std::unordered_map<osmium::object_id_type, osmium::Location> node_locations;
std::vector<std::vector<std::pair<osmium::object_id_type, osmium::object_id_type>>> column_edges;
float column_width;

int write_geojson_nodes(const std::string &outputFilePath, const std::vector<osmium::Location> &nodes){
    std::ofstream outputFile(outputFilePath);
    osmium::geom::GeoJSONFactory<> geojson_factory;
    if (outputFile.is_open())
    {
        outputFile << "{\n\t\"type\": \"FeatureCollection\",\n\t\"features\": [\n";
        bool first = true;
        for (auto const & node : nodes){
            if (first)
                first = false;
            else
                outputFile << ",\n";
            outputFile << "{\"type\": \"Feature\", \"properties\": {}, \"geometry\": " << geojson_factory.create_point(node) << "}";
        }

        outputFile << "\t]\n}";

    }
    else
    {
        std::cerr << "Error opening file\n";
    }

    return 0;
}


// haversine formula
// double haversine(osmium::Location &loc1, osmium::Location &loc2){
//     double lat1 = loc1.lat();
//     double lon1 = loc1.lon();
//     double lat2 = loc2.lat();
//     double lon2 = loc2.lon();
//     double dlat = lat2 - lat1;
//     double dlon = lon2 - lon1;
//     double a = pow(sin(dlat/2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon/2), 2);
//     double c = 2 * atan2(sqrt(a), sqrt(1-a));
//     return 6371 * c;
// }

osmium::Location generate_random_location(){ // using cos(latitute) to generate uniform random points on the sphere
    // Initialize random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0); // Uniform distribution for -1 to 1
    std::uniform_real_distribution<> dis2(0.0, 1.0); // Uniform distribution for -1 to 1

    // Generate random latitude and longitude
    double longitude = (dis2(gen) * 360) - 180; // Longitude [-180, 180]
    // Correctly distribute latitude points
    double latitude = asin(dis(gen)) * 180 / M_PI; // Latitude [-90, 90] with correct distribution

    // double latitude = 2 * M_PI * (rand() / (double)RAND_MAX) - M_PI;
    // double longitude = acos(2 * (rand() / (double)RAND_MAX) - 1) - M_PI/2;
    return osmium::Location(longitude, latitude);
}

// Function to generate random nodes
std::vector<osmium::Location> generate_random_nodes(size_t count){
    std::vector<osmium::Location> nodes;
    nodes.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        if(i%10)
            std::cout << "Generated " << i << " nodes" << std::endl;
        osmium::Location Q = generate_random_location();
        if(isLand(Q, column_edges, final_closed_ways, node_locations, column_width))
        {
            i--;
            continue;
        }
        nodes.push_back(Q);
    }
    column_edges.clear();
    final_closed_ways.clear();
    node_locations.clear();

    return nodes;
}


int main_task4(int argc, char* argv[])
{
    std::string input_file_name;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " INPUT-FILE\n";
        // return 1;
        input_file_name = "antarctica-latest.osm.pbf";
    }
    else
        input_file_name = argv[1];



    get_final_closed_ways(input_file_name, final_closed_ways, node_locations);

    // Bresenham's algorithm
    column_width = 0.001;
    get_columns_edges(final_closed_ways, node_locations, column_edges, column_width);

    // std::vector<osmium::Location> test_points;
    // // test_points.push_back(osmium::Location(-26.732548749488956, 10.671596281947856));
    // // test_points.push_back(osmium::Location(11.939290077225365, 12.162589122776751));
    // // test_points.push_back(osmium::Location(16.193990227322075, -80.31058318376975));
    // // test_points.push_back(osmium::Location(-23.30866463537828, -85.05112877980659)); // In Antarctica
    // // test_points.push_back(osmium::Location(122.32121620632046, -72.40337730584133)); // In Antarctica other side Shows water always
    // // test_points.push_back(osmium::Location(122.32120204453742, -72.40337785754292)); // In Antarctica also near so shows water
    // // test_points.push_back(osmium::Location(123.74663350794424, -71.266113575238)); // In Antarctica
    // test_points.push_back(osmium::Location(61.6524702239368, 76.2930632732388)); // In Russia
    // test_points.push_back(osmium::Location(-38.48528429954149, 77.52013957622279)); // In Greenland
    // test_points.push_back(osmium::Location(-89.46894116718542, 85.05112877980659)); // In Water near north pole
    // test_points.push_back(osmium::Location(176.7141430662184, 81.88365705783642)); // In Water near north pole
    // test_points.push_back(osmium::Location(161.17794752875415, 5.768768463425289)); // In Water Pacific Ocean
    // // test_points.push_back(osmium::Location(169.81630171686493, 82.69225385345266)); // In Water North pole

    // int Q_i = 0;
    // for(auto const & Q : test_points){
    //     Q_i++;
    //     std::cout << "Q" << Q_i << " : " << Q << std::endl;
    //     std::cout << (isLand(Q, column_edges, final_closed_ways, node_locations, column_width)? "----Land----" : "----Water----") << std::endl << '\n';
    //     // std::cout << (isLand(Q, bounding_boxes, final_closed_ways, node_locations)? "----Land----" : "----Water----") << std::endl;
    // }
    std::vector<osmium::Location> nodes =  generate_random_nodes(4e6); // 4 million nodes
    std::cout << "finished generating" << std::endl;
    write_geojson_nodes("../random_nodes.geojson", nodes);


    return 0;
}