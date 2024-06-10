#include "t3.h"
#include "t2.h"
#include "read_cache.h"


#include <chrono> // for execution time

std::vector<osmium::Location> crossed_nodes;
std::unordered_set<double> crossed_latitudes;

void get_final_closed_ways(const std::string &input_file_name,
                           std::vector<std::vector<osmium::object_id_type>> &final_closed_ways,
                           std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations)
{
    std::vector<std::vector<osmium::object_id_type>> ways_collection;
    std::unordered_set<osmium::object_id_type> node_ids;

    // read_coastline_ways(input_file_name, ways_collection);
    // std::cout << "Coastline Ways count: " << ways_collection.size() << std::endl;
    // extract_nodes_from_ways(ways_collection, node_ids);
    // std::cout << "Nodes extracted: " << node_ids.size() << std::endl;

    auto t1 = std::chrono::high_resolution_clock::now();
    read_coastline_ways_and_node_ids(input_file_name, ways_collection, node_ids);
    auto t2 = std::chrono::high_resolution_clock::now();
    auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);

    std::cout << "time for read_coastline_ways_and_node_ids: " << ms_int.count() << std::endl;

    t1 = std::chrono::high_resolution_clock::now();
    read_coastline_nodes(input_file_name, node_ids, node_locations);
    t2 = std::chrono::high_resolution_clock::now();
    ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    std::cout << "time for read_coastline_nodes: " << ms_int.count() << std::endl;

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
    merge_touching_ways(unclosed_ways, final_closed_ways, false);
    std::cout << "Unclosed -> Stitched Ways count: " << final_closed_ways.size() << std::endl;
    closed_ways.clear();
    unclosed_ways.clear();

    // Merge unstitched ways with stitched ways
    for(auto const & way : closed_ways)
        final_closed_ways.push_back(way);
    final_closed_ways.shrink_to_fit();
    std::cout << "\nStitched + Closed Ways final_closed_ways count: " << final_closed_ways.size() << std::endl;
}

// Find the bounding box for each of the closed ways
void getBoundingBox(const std::vector<std::vector<osmium::object_id_type>> &closed_ways,
                    const std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations,
                    std::vector<std::pair<osmium::Location, osmium::Location>> &bounding_boxes)
{
    for(auto const & way : closed_ways){
        float min_lat = 90.0, max_lat = -90.0, min_lon = 180.0, max_lon = -180.0;
        for(auto const & node : way){
            osmium::Location loc = node_locations.at(node);
            if (loc.lat() < min_lat) min_lat = loc.lat();
            if (loc.lat() > max_lat) max_lat = loc.lat();
            if (loc.lon() < min_lon) min_lon = loc.lon();
            if (loc.lon() > max_lon) max_lon = loc.lon();
        }
        bounding_boxes.push_back(std::make_pair(osmium::Location(min_lon, min_lat), osmium::Location(max_lon, max_lat)));
    }
    bounding_boxes.shrink_to_fit();
}

float latitude(float lambda, float lambda1, float lambda2, float phi1, float phi2){
    return phi1 + (lambda - lambda1) / (lambda2 - lambda1) * (phi2 - phi1);
}

// Intersection test for Lat-Lon Edges // Can be used for calculations in the check "X.lat[at Q.lon] > Q.lat"
float latitude_crossing_X(osmium::Location Q, float lambda1, float lambda2, float phi1, float phi2){
    return phi1 + (Q.lat() - lambda1) / (lambda2 - lambda1) * (phi2 - phi1);
}

bool isPointInBox(const osmium::Location &Q, const osmium::Location &A, const osmium::Location &B){
    return Q.lat() >= A.lat() && Q.lat() <= B.lat() && Q.lon() >= A.lon() && Q.lon() <= B.lon();
}

///////////////////////////////////////   By Darel Rex Finley
// bool isEdgeCrossedByHorizontalLine(osmium::Location Q, osmium::Location i, osmium::Location j){
//     if( (i.lat() < Q.lat && j.lat() >= Q.lat() ||   j.lat() < Q.lat() && i.lat() >= Q.lat()) // if Q is between edge lats
//         &&  (i.lon() <= Q.lon() || j.lon() <= Q.lon())) // if Q is to the right of the edge
//             return (
//             i.lon()
//             +
//             (Q.lat() - i.lat()) / (j.lat() - i.lat())
//             *
//             (j.lon() - i.lon())
//             <
//             Q.lon()
//             );
//     return false;
// }

// isEdgeCrossed by northerly ray to the North Pole
bool isEdgeCrossed(const osmium::Location& Q, const osmium::Location& i, const osmium::Location& j){
    if( ( (i.lon() < Q.lon() && j.lon() >= Q.lon()) || (j.lon() < Q.lon() && i.lon() >= Q.lon()) ) // if Q is between edge longitudes    i    Q     j
        &&  (i.lat() >= Q.lat() || j.lat() >= Q.lat())
        && i.lat() + (j.lat() - i.lat()) / (j.lon() - i.lon()) * (Q.lon() - i.lon()) > Q.lat())
        // std::cout <<  osmium::Location(Q.lon(), i.lat() + (j.lat() - i.lat()) / (j.lon() - i.lon()) * (Q.lon() - i.lon())) << std::endl;
        {
            if(crossed_latitudes.find(i.lat() + (j.lat() - i.lat()) / (j.lon() - i.lon()) * (Q.lon() - i.lon())) == crossed_latitudes.end())
                crossed_latitudes.insert(i.lat() + (j.lat() - i.lat()) / (j.lon() - i.lon()) * (Q.lon() - i.lon()));
            else
                return false;
            // crossed_nodes.push_back(osmium::Location(Q.lon(), i.lat() + (j.lat() - i.lat()) / (j.lon() - i.lon()) * (Q.lon() - i.lon())));
            std::cout << osmium::Location(Q.lon(), i.lat() + (j.lat() - i.lat()) / (j.lon() - i.lon()) * (Q.lon() - i.lon())) << std::endl;
            // std::cout << i << " , " << j << std::endl;
        }
    if( ( (i.lon() < Q.lon() && j.lon() >= Q.lon()) || (j.lon() < Q.lon() && i.lon() >= Q.lon()) ) // if Q is between edge longitudes    i    Q     j
        &&  (i.lat() >= Q.lat() || j.lat() >= Q.lat())) // if the edge is above Q
            return (
            i.lat()
            +
            (j.lat() - i.lat()) / (j.lon() - i.lon())
            *
            (Q.lon() - i.lon())
            >
            Q.lat()
            );
    return false;
}

bool isPointInPolygon(const osmium::Location &Q,
                      const std::vector<osmium::object_id_type> &way,
                      const std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations)
{
//   auto j = way.size()-1; // If you want to forcely close the road, incorrect for several reasons
  auto j = 0;
  bool parity_odd = 0;

    for(std::size_t i = 1; i < way.size(); i++) {
        parity_odd ^= isEdgeCrossed(Q, node_locations.at(way[i]), node_locations.at(way[j]));
        j = i;
}
  return parity_odd;
}

int get_column_index(const float& column_width, const double& longitude){
    return (int)((longitude + 180) / column_width) % int(360/column_width);
}

// Intuitive for one axis, to be updated for grid implementation
void bresenham_algorithm_horizontally(const osmium::object_id_type &A,
                                      const osmium::object_id_type &B,
                                      const std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations,
                                      std::vector<std::vector<std::pair<osmium::object_id_type, osmium::object_id_type>>> &column_edges,
                                      const float& column_width)
{
    const osmium::Location& loc_A = node_locations.at(A);
    const osmium::Location& loc_B = node_locations.at(B);
    // double lat = loc_A.lat();
    double lon = loc_A.lon();
    // double lat2 = loc_B.lat();
    double lon2 = loc_B.lon();

    if((lon > lon2 && !(lon > 160 && lon2 < -160)) || (lon2 > 160 && lon < -160))
    // if(lon > lon2)
        std::swap(lon, lon2);

    ///////////////////////////////////////////// Not considering at 180 and -180 longitude
    // std::cout << "begin: " << get_column_index(column_width, lon) << ", end: " << get_column_index(column_width, lon2) << std::endl;
    // getchar();
    // std::cout << lon << "   " << get_column_index(column_width, lon) << std::endl;

    // std::cout << "Start: " << get_column_index(column_width, lon) << " , End: " << get_column_index(column_width, lon2) << std::endl;


    int i = get_column_index(column_width, lon);
    int j = get_column_index(column_width, lon2);
    column_edges[i].push_back(std::make_pair(A, B));
    while(i != j)
    {
        i = (i+1)%(int)(360/column_width);
        column_edges[i].push_back(std::make_pair(A, B));
        column_edges[i].shrink_to_fit();
    }
    // for(int i = get_column_index(column_width, lon); ; i++){
    //     column_edges[i%(360/column_width)].push_back(std::make_pair(A, B));
    //     // if((int)(node_locations.at(A).lon()) == -23)
    //         // std::cout << A << " , " << B << std::endl;
    //     if(i%(360/column_width) == get_column_index(column_width, lon2))
    //         break;
    // }
}

void get_columns_edges(const std::vector<std::vector<osmium::object_id_type>> &ways,
                       const std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations,
                       std::vector<std::vector<std::pair<osmium::object_id_type, osmium::object_id_type>>> &column_edges,
                       const float& column_width)
{
    int number_of_columns = 360/column_width; // number of bins
    column_edges.reserve(number_of_columns);
    for(int i = 0; i < number_of_columns; i++)
        column_edges.push_back(std::vector<std::pair<osmium::object_id_type, osmium::object_id_type>>());
    std::cout << "Length of column edges vector: " << column_edges.size() << std::endl;
    for(auto const & way : ways){
        for(std::size_t i = 1; i < way.size(); i++) { // Take each way segment and assign corresponding columns
                bresenham_algorithm_horizontally(way[i-1], way[i], node_locations, column_edges, column_width);
        }
    }
}

bool isLand(const osmium::Location &Q,
            // std::vector<std::pair<osmium::Location, osmium::Location>> &bounding_boxes,
            const std::vector<std::vector<std::pair<osmium::object_id_type, osmium::object_id_type>>>& column_edges,
            const std::vector<std::vector<osmium::object_id_type>> &ways_polygons,
            const std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations,
            const float& column_width)
{
    // std::vector<long> matched_ways_bounding_boxes_indices;
    // std::vector<std::pair<osmium::Location, osmium::Location>> crossed_bounding_boxes;
    // std::vector<unsigned int> crossed_ways_bounding_boxes_indices;
    // Get bounding boxes in which Q is located                                                     --------------------------------
    // for(std::size_t i = 0; i<bounding_boxes.size(); i++){
    //     if(isPointInBox(Q, bounding_boxes[i].first, bounding_boxes[i].second))
    //         crossed_ways_bounding_boxes_indices.push_back(i);
    // }

    // std::cout << "\nMatched bounding boxes: " << crossed_ways_bounding_boxes_indices.size() << std::endl; --------------------------------

    // Bresenham's algorithm
    int column_index = get_column_index(column_width, Q.lon());
    // std::cout << "Column index of Q: " << column_index << std::endl;
    bool parity_odd = 0;
    for(const std::pair<osmium::object_id_type, osmium::object_id_type>& edge : column_edges[column_index]){
        // std::cout << "Edge: " << edge.first << " , " << edge.second << std::endl;
        parity_odd ^= isEdgeCrossed(Q, node_locations.at(edge.first), node_locations.at(edge.second));
    }

    // // Comment
    // bool parity_odd = 0;
    // for(std::size_t i = 0; i<ways_polygons.size(); i++){
    //     if(isPointInPolygon(Q, ways_polygons[i], node_locations))                                // if Q is in the closed way "land", then naturlich Land
    //         return true;
        // parity_odd ^= isPointInPolygon(Q, closed_ways[i], node_locations);                                // if Q is in the closed way "land", then naturlich Land

    // }

    //Uncomment // but change to boxes bounding the test point
    // for(std::size_t i = 0; i<crossed_ways_bounding_boxes_indices.size(); i++){
    //     // std::vector<osmium::object_id_type> way = closed_ways[crossed_ways_bounding_boxes_indices[i]];
    //     if(isPointInPolygon(Q,
    //                         closed_ways[crossed_ways_bounding_boxes_indices[i]],
    //                         node_locations))                                // if Q is in the closed way "land", then naturlich Land
    //         return false;
    // }

    // std::cout << "\nNot inside any Land" << std::endl;

    return parity_odd;
}

int main_task3(int argc, char* argv[])
{
    std::string input_file_name;

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " INPUT-FILE\n";
        // return 1;
        input_file_name = "antarctica-latest.osm.pbf";
    }
    else
        input_file_name = argv[1];

    std::vector<std::vector<osmium::object_id_type>> final_closed_ways;
    std::unordered_map<osmium::object_id_type, osmium::Location> node_locations;

    get_final_closed_ways(input_file_name, final_closed_ways, node_locations);

    // std::cout << node_locations.size() << std::endl;
    // std::cout << "test in bounding boxes" << std::endl;

    // std::vector<std::pair<osmium::Location, osmium::Location>> bounding_boxes;
    // getBoundingBox(final_closed_ways, node_locations, bounding_boxes);
    // std::cout << "number of bounding boxes: " << bounding_boxes.size() << std::endl;

    // Bresenham's algorithm
    std::vector<std::vector<std::pair<osmium::object_id_type, osmium::object_id_type>>> column_edges;
    int column_width = 10;
    get_columns_edges(final_closed_ways, node_locations, column_edges, column_width);

    std::vector<osmium::Location> test_points;
    // test_points.push_back(osmium::Location(-26.732548749488956, 10.671596281947856));
    // test_points.push_back(osmium::Location(11.939290077225365, 12.162589122776751));
    // test_points.push_back(osmium::Location(16.193990227322075, -80.31058318376975));
    test_points.push_back(osmium::Location(-23.30866463537828, -85.05112877980659)); // In Antarctica
    test_points.push_back(osmium::Location(122.32121620632046, -72.40337730584133)); // In Antarctica other side Shows water always
    test_points.push_back(osmium::Location(122.32120204453742, -72.40337785754292)); // In Antarctica also near so shows water
    test_points.push_back(osmium::Location(123.74663350794424, -71.266113575238)); // In Antarctica
    // test_points.push_back(osmium::Location(61.6524702239368, 76.2930632732388)); // In Russia
    // test_points.push_back(osmium::Location(-38.48528429954149, 77.52013957622279)); // In Greenland
    // test_points.push_back(osmium::Location(-89.46894116718542, 85.05112877980659)); // In Water near north pole
    // test_points.push_back(osmium::Location(176.7141430662184, 81.88365705783642)); // In Water near north pole
    // test_points.push_back(osmium::Location(161.17794752875415, 5.768768463425289)); // In Water Pacific Ocean
    // test_points.push_back(osmium::Location(169.81630171686493, 82.69225385345266)); // In Water North pole

    int Q_i = 0;
    for(auto const & Q : test_points){
        Q_i++;
        std::cout << "Q" << Q_i << " : " << Q << std::endl;
        std::cout << (isLand(Q, column_edges, final_closed_ways, node_locations, column_width)? "----Land----" : "----Water----") << std::endl << '\n';
        // std::cout << (isLand(Q, bounding_boxes, final_closed_ways, node_locations)? "----Land----" : "----Water----") << std::endl;
        crossed_latitudes.clear();
    }
    // std::cout << "Crossed Nodes:" << std::endl;
    for (auto location : crossed_nodes)
        std::cout << (float)location.lat() << std::endl;
    // std::cout << isEdgeCrossed(osmium::Location(3.0,2.0), osmium::Location(2,3), osmium::Location(4,3)) << std::endl;
    // for(auto const & edge : column_edges[30])
    //     // if(node_locations.at(edge.first).lon() == test_points[1].lon() || node_locations.at(edge.second).lon() == test_points[1].lon())
    //         std::cout << node_locations.at(edge.first) << " , " << node_locations.at(edge.second) << std::endl;
    return 0;
}

// Problem of precision?