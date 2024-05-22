#include <iostream>
#include <osmium/io/pbf_input.hpp>

#include <osmium/geom/coordinates.hpp>
#include <osmium/geom/geojson.hpp>
#include <osmium/geom/mercator_projection.hpp> // Include MercatorProjection

#include <cmath>

#include <unordered_map>

// Great Circle Distance calculation    [http://www.movable-type.co.uk/scripts/latlong.html]
// Haversine formula
double great_circle_distance(double lat1, double lon1, double lat2, double lon2){
    double R = 6371e3; // metres
    double phi1 = lat1 * M_PI / 180; // φ, λ in radians
    double phi2 = lat2 * M_PI / 180;
    double delta_phi = (lat2-lat1) * M_PI / 180;
    double delta_lambda = (lon2-lon1) * M_PI / 180;

    double a = sin(delta_phi/2) * sin(delta_phi/2) +
            cos(phi1) * cos(phi2) *
            sin(delta_lambda/2) * sin(delta_lambda/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    double d = R * c; // in metres
    return d;
}

// Law of Cosines formula
double great_circle_distance2(double lat1, double lon1, double lat2, double lon2){
    double R = 6371e3; // metres
    double phi1 = lat1 * M_PI / 180; // φ, λ in radians
    double phi2 = lat2 * M_PI / 180;
    double delta_lambda = (lon2-lon1) * M_PI / 180;

    double d = acos( sin(phi1)*sin(phi2) + cos(phi1)*cos(phi2)*cos(delta_lambda) ) * R;
    return d;
}

class NvectorSpherical {
    public:
        double x, y, z;
        NvectorSpherical(double x, double y, double z) : x(x), y(y), z(z) {}
        NvectorSpherical toNvector(double lon, double lat);


};

int main() {

    return 0;
}


// Find the bounding box for each of the closed ways
void boundingBox(std::vector<std::vector<osmium::object_id_type>> closed_ways, std::unordered_map<osmium::object_id_type, osmium::Location> node_locations){
    for (auto way : closed_ways){
        double min_lat = 90, max_lat = -90, min_lon = 180, max_lon = -180;
        for (auto node : way){
            osmium::Location loc = node_locations[node];
            if (loc.lat() < min_lat) min_lat = loc.lat();
            if (loc.lat() > max_lat) max_lat = loc.lat();
            if (loc.lon() < min_lon) min_lon = loc.lon();
            if (loc.lon() > max_lon) max_lon = loc.lon();
        }
        std::cout << "Bounding box: " << min_lat << ", " << min_lon << ", " << max_lat << ", " << max_lon << std::endl;
    }
}




