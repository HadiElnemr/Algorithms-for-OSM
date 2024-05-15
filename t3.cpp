#include <iostream>
#include <osmium/io/pbf_input.hpp>

#include <osmium/geom/coordinates.hpp>
#include <osmium/geom/geojson.hpp>
#include <osmium/geom/mercator_projection.hpp> // Include MercatorProjection

#include <cmath>

// Great Circle Distance calculation    [http://www.movable-type.co.uk/scripts/latlong.html]
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






