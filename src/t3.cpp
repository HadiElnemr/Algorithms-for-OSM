#include "t3.h"
#include "t2.h"

// // Great Circle Distance calculation    [http://www.movable-type.co.uk/scripts/latlong.html]
// // Haversine formula
// float great_circle_distance(float lat1, float lon1, float lat2, float lon2){
//     float R = 6371e3; // metres
//     float phi1 = lat1 * M_PI / 180; // φ, λ in radians
//     float phi2 = lat2 * M_PI / 180;
//     float delta_phi = (lat2-lat1) * M_PI / 180;
//     float delta_lambda = (lon2-lon1) * M_PI / 180;

//     float a = sin(delta_phi/2) * sin(delta_phi/2) +
//             cos(phi1) * cos(phi2) *
//             sin(delta_lambda/2) * sin(delta_lambda/2);
//     float c = 2 * atan2(sqrt(a), sqrt(1-a));

//     float d = R * c; // in metres
//     return d;
// }

// // Law of Cosines formula
// float great_circle_distance2(float lat1, float lon1, float lat2, float lon2){
//     float R = 6371e3; // metres
//     float phi1 = lat1 * M_PI / 180; // φ, λ in radians
//     float phi2 = lat2 * M_PI / 180;
//     float delta_lambda = (lon2-lon1) * M_PI / 180;

//     float d = acos( sin(phi1)*sin(phi2) + cos(phi1)*cos(phi2)*cos(delta_lambda) ) * R;
//     return d;
// }

// class NvectorSpherical {
//     public:
//         float x, y, z;
//         NvectorSpherical(float x, float y, float z) : x(x), y(y), z(z) {}
//         NvectorSpherical toNvector(float lon, float lat);


// };

void get_final_closed_ways(std::string &input_file_name,
                           std::vector<std::vector<osmium::object_id_type>> &final_closed_ways,
                           std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations)
{
    std::vector<std::vector<osmium::object_id_type>> ways_collection;
    std::unordered_set<osmium::object_id_type> node_ids;

    // std::vector<std::vector<osmium::object_id_type>> stitched_ways;

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
    merge_touching_ways(unclosed_ways, final_closed_ways, true);
    std::cout << "Unclosed -> Stitched Ways count: " << final_closed_ways.size() << std::endl;

    // Merge unstitched ways with stitched ways
    for(auto way : closed_ways)
        final_closed_ways.push_back(way);
    std::cout << "Stitched + Closed Ways count: " << final_closed_ways.size() << std::endl;
    // final_closed_ways = stitched_ways;
}

// Find the bounding box for each of the closed ways
void getBoundingBox(std::vector<std::vector<osmium::object_id_type>> &closed_ways, std::unordered_map<osmium::object_id_type, osmium::Location> &node_locations, std::vector<std::pair<osmium::Location, osmium::Location>> &bounding_boxes){
    for (auto way : closed_ways){
        float min_lat = 90, max_lat = -90, min_lon = 180, max_lon = -180;
        for (auto node : way){
            osmium::Location loc = node_locations[node];
            if (loc.lat() < min_lat) min_lat = loc.lat();
            if (loc.lat() > max_lat) max_lat = loc.lat();
            if (loc.lon() < min_lon) min_lon = loc.lon();
            if (loc.lon() > max_lon) max_lon = loc.lon();
        }
        bounding_boxes.push_back(std::make_pair(osmium::Location(min_lon, min_lat), osmium::Location(max_lon, max_lat)));
    }
}

float latitude(float lambda, float lambda1, float lambda2, float phi1, float phi2){
    return phi1 + (lambda - lambda1) / (lambda2 - lambda1) * (phi2 - phi1);
}

float latitude_crossing(float lambda, float lambda1, float lambda2, float phi1, float phi2){
    return phi1 + (lambda - lambda1) / (lambda2 - lambda1) * (phi2 - phi1);
}

bool isWater(osmium::Location &Q,
            std::vector<std::pair<osmium::Location, osmium::Location>> &bounding_boxes,
            std::unordered_map<osmium::object_id_type,
            osmium::Location> &node_locations)
{
    int count = 0;
    std::vector<long> matched_ways_bounding_boxes_indices;
    for(int i = 0; i<bounding_boxes.size(); i++){
        osmium::Location A = bounding_boxes[i].first; // SW    ......B
        osmium::Location B = bounding_boxes[i].second; // NE   A......
        std::vector<std::pair<osmium::Location, osmium::Location>> Q_bounding_boxes;
        if(Q.lat() < A.lat() || Q.lat() > B.lat() || Q.lon() < A.lon() || Q.lon() > B.lon())
            continue; // if Q is outside the bounding box, skip, else, save bounding box
        Q_bounding_boxes.push_back(std::make_pair(A, B));
        matched_ways_bounding_boxes_indices.push_back(i);
    }
    if(matched_ways_bounding_boxes_indices.size() == 0)
        return true;
    // further investigation
    //
    // for(int i = 0; i<bounding_boxes.size(); i++){
    //     osmium::Location A = bounding_boxes[i].first;
    //     osmium::Location B = bounding_boxes[i].second;
    //     osmium::Location C = bounding_boxes[(i+1)%bounding_boxes.size()].first;
    //     osmium::Location D = bounding_boxes[(i+1)%bounding_boxes.size()].second;
    //     if(A.lat() == B.lat() && C.lat() == D.lat()){
    //         if(A.lat() == Q.lat() && Q.lon() >= A.lon() && Q.lon() <= B.lon())
    //             return true;
    //         if(A.lat() == Q.lat() && Q.lon() >= B.lon() && Q.lon() <= A.lon())
    //             return true;
    //     }
    //     if(A.lon() == B.lon() && C.lon() == D.lon()){
    //         if(A.lon() == Q.lon() && Q.lat() >= A.lat() && Q.lat() <= B.lat())
    //             return true;
    //         if(A.lon() == Q.lon() && Q.lat() >= B.lat() && Q.lat() <= A.lat())
    //             return true;
    //     }
    //     if(A.lat() == B.lat() && C.lon() == D.lon()){
    //         if(A.lat() == Q.lat() && Q.lon() >= A.lon() && Q.lon() <= B.lon())
    //             return true;
    //         if(A.lat() == Q.lat() && Q.lon() >= B.lon() && Q.lon() <= A.lon())
    //             return true;
    //     }
    //     if(A.lon() == B.lon() && C.lat() == D.lat()){
    //         if(A.lon() == Q.lon() && Q.lat() >= A.lat() && Q.lat() <= B.lat())
    //             return true;
    //         if(A.lon() == Q.lon() && Q.lat() >= B.lat() && Q.lat() <= A.lat())
    //             return true;
    //     }
    //     if(A.lat() == B.lat() && C.lat() == D.lat()){
    //         if(A.lat() == Q.lat() && Q.lon() >= A.lon() && Q.lon() <= B.lon())
    //             return true;
    //         if(A.lat() == Q.lat() && Q.lon() >= B.lon() && Q.lon() <= A.lon())
    //             return true;
    //     }
    //     if(A.lon() == B.lon() && C.lon() == D.lon()){
    //         if(A.lon() == Q.lon() && Q.lat() >= A.lat() && Q.lat() <= B.lat())
    //             return true;
    //     }
    // }
    return false;
}

int main_task3(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " INPUT-FILE\n";
        return 1;
    }

    std::string input_file_name = argv[1];
    std::vector<std::vector<osmium::object_id_type>> final_closed_ways;
    std::unordered_map<osmium::object_id_type, osmium::Location> node_locations;
    get_final_closed_ways(input_file_name, final_closed_ways, node_locations);

    std::vector<std::pair<osmium::Location, osmium::Location>> bounding_boxes;
    getBoundingBox(final_closed_ways, node_locations, bounding_boxes);
    std::cout << "number of bounding boxes: " << bounding_boxes.size() << std::endl;
    final_closed_ways.clear();

    std::vector<osmium::Location> test_points;
    // osmium::Location Q(-26.732548749488956,
    //       10.671596281947856);
    // test_points.push_back(Q);
    test_points.push_back(osmium::Location(-26.732548749488956, 10.671596281947856));
    test_points.push_back(osmium::Location(11.939290077225365, 12.162589122776751));
    test_points.push_back(osmium::Location(16.193990227322075, -80.31058318376975));

    for(auto Q : test_points){
        std::cout << Q << std::endl;
        if(isWater(Q, bounding_boxes, node_locations))
            std::cout << "----Water----" << std::endl;
        else
            std::cout << "----Land----" << std::endl;
    }

    return 0;
}

