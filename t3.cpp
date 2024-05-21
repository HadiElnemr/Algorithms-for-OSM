#include <iostream>
#include <osmium/io/pbf_input.hpp>

#include <osmium/geom/coordinates.hpp>
#include <osmium/geom/geojson.hpp>
#include <osmium/geom/mercator_projection.hpp> // Include MercatorProjection

#include <cmath>

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
// double toNvector(double lon, double lat) { 
//         double φ = lat.toRadians();
//         double λ = this.lon.toRadians();

//         double sinφ = Math.sin(φ), cosφ = Math.cos(φ);
//         double sinλ = Math.sin(λ), cosλ = Math.cos(λ);

//         // right-handed vector: x -> 0°E,0°N; y -> 90°E,0°N, z -> 90°N
//         const x = cosφ * cosλ;
//         const y = cosφ * sinλ;
//         const z = sinφ;

//         // return new NvectorSpherical(x, y, z);
//         return x, y, z;
// }






