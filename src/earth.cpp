#include "stdafx.h"
#include "Earth.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace trajectorysim {

static double LinearInterpolation(double x, double x1, double x2, double y1, double y2) {
    return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
}

Earth::Earth()
{
    atmo_properties = getAtmoTable();
}

//From WGS84
const double Earth::a = 6378137.0; //m
const double Earth::f = 1.0 / 298.257223563;

std::vector<std::vector<double>> Earth::getAtmoTable() {
    std::ifstream filestream;
    std::string output;
    std::vector<std::vector<double>> atmo_properties;

    filestream.open("atmosphere.csv");

    if (!filestream) {
        throw 10; //Will be caught in main()
    }

    std::getline(filestream, output);
    std::getline(filestream, output);

    int line = 0;
    int col = 0;
    while (std::getline(filestream, output)) {
        std::vector<double> vecLine;
        while (output.length() > 0) {
            int index = output.find(",");
            std::string valueStr = output.substr(0, index);
            output = output.substr(index + 1, output.length() - index);
            vecLine.push_back(std::stod(valueStr));
            if (index == -1) {
                break;
            }
        }
        atmo_properties.push_back(vecLine);
    }
    return atmo_properties;
}

Earth::Coords Earth::LatLonAltToECEF(LatLonAlt pos_LLA) {
    const double k_PI = 3.14159265359;
    Coords posECEF;

    double rad_lat = pos_LLA.lat * k_PI / 180.0;
    double rad_lon = pos_LLA.lon * k_PI / 180.0;

    double gc_lat = atan(pow((1 - f), 2)*tan(rad_lat));
    double r = sqrt(pow(a, 2) / (1 + (1 / pow((1 - f), 2) - 1)*pow(sin(gc_lat), 2)));

    posECEF.x = r * cos(gc_lat)*cos(rad_lon) + pos_LLA.alt * cos(rad_lat)*cos(rad_lon);
    posECEF.y = r * cos(gc_lat)*sin(rad_lon) + pos_LLA.alt * cos(rad_lat)*sin(rad_lon);
    posECEF.z = r * sin(gc_lat) + pos_LLA.alt * sin(rad_lat);

    return posECEF;
}

double Earth::ECEFToAlt(Coords Pos_ECEF) {
    //LatLonAlt Pos_LLA;
    double altitude;

    double s = sqrt(pow(Pos_ECEF.x, 2) + pow(Pos_ECEF.y, 2));
    double e = sqrt(1 - pow((1 - f), 2));

    //Pos_LLA.lon = atan(Pos_ECEF.y / Pos_ECEF.x);

    double reduced_lat = atan(Pos_ECEF.z / ((1 - f)*s));
    double geo_lat = atan((Pos_ECEF.z + ((pow(e, 2)*(1 - f)) / (1 - pow(e, 2)))*a*pow(sin(reduced_lat), 3)) / (s - pow(e, 2)*a*pow(cos(reduced_lat), 3)));
    double prev_geo_lat;
    int count = 0;

    do {
        if (count > 50) {
            throw 20;
        }
        prev_geo_lat = geo_lat;
        reduced_lat = atan(((1 - f)*sin(geo_lat)) / (cos(geo_lat)));
        geo_lat = atan((Pos_ECEF.z + ((pow(e, 2)*(1 - f)) / (1 - pow(e, 2)))*a*pow(sin(reduced_lat), 3)) / (s - pow(e, 2)*a*pow(cos(reduced_lat), 3)));
    } while (abs(geo_lat - prev_geo_lat) > 10.0);

    //Pos_LLA.lat = atan(tan(geoLat) / pow((1 - f), 2));

    double N = a / sqrt(1 - pow(e, 2)*pow(sin(geo_lat), 2));
    altitude = s * cos(geo_lat) + (Pos_ECEF.z + pow(e, 2)*N*sin(geo_lat))*sin(geo_lat) - N;

    return altitude;
}

Earth::Coords Earth::Gravity_Accel(Earth::Coords pos_ECEF)
{
    const double grav_const = -9.80665; // m/s^2
	double altitude = ECEFToAlt(pos_ECEF);
	double grav_current = grav_const * pow((a / (a + altitude)), 2);
    Earth::Coords a_gravity;
    double alpha, beta, gamma;
    double abs_pos;

    abs_pos = sqrt(pow(pos_ECEF.x, 2) + pow(pos_ECEF.y, 2) + pow(pos_ECEF.z, 2));
    alpha = acos(pos_ECEF.x / abs_pos);
    beta = acos(pos_ECEF.y / abs_pos);
    gamma = acos(pos_ECEF.z / abs_pos);

    a_gravity.x = cos(alpha) * grav_current;
    a_gravity.y = cos(beta) * grav_current;
    a_gravity.z = cos(gamma) * grav_current;

    return a_gravity;
}

double Earth::GetReynoldsNumber(double velocity, double charLength) {
    return velocity * charLength / (current_properties.dyn_viscosity / current_properties.density);
}

Earth::Properties Earth::setProperties(double altitude) {
    //Find and interpolate from atmo table
    int upperindex, lowerindex;
    for (upperindex = 0; upperindex < atmo_properties.size(); ++upperindex) {
        if (atmo_properties[upperindex][0] > altitude) break;
    }
    if (upperindex == atmo_properties.size()) {
        //Higher than table. Return highest value
        upperindex = atmo_properties.size() - 1;
        lowerindex = upperindex;
    }
    else if (upperindex == 0) {
        //Low altitude. Just use lowest table entry
        upperindex = 0;
        lowerindex = 0;
    }
    else lowerindex = upperindex - 1;
    if (upperindex == lowerindex) {
        //Don't bother interpolating
        return Properties{
            atmo_properties[upperindex][1],
            atmo_properties[upperindex][2],
            atmo_properties[upperindex][3],
            atmo_properties[upperindex][4],
            atmo_properties[upperindex][5]
        };
    }
    return Properties{
        LinearInterpolation(altitude, atmo_properties[lowerindex][0], atmo_properties[upperindex][0], atmo_properties[lowerindex][1], atmo_properties[upperindex][1]),
        LinearInterpolation(altitude, atmo_properties[lowerindex][0], atmo_properties[upperindex][0], atmo_properties[lowerindex][2], atmo_properties[upperindex][2]),
        LinearInterpolation(altitude, atmo_properties[lowerindex][0], atmo_properties[upperindex][0], atmo_properties[lowerindex][3], atmo_properties[upperindex][3]),
        LinearInterpolation(altitude, atmo_properties[lowerindex][0], atmo_properties[upperindex][0], atmo_properties[lowerindex][4], atmo_properties[upperindex][4]),
        LinearInterpolation(altitude, atmo_properties[lowerindex][0], atmo_properties[upperindex][0], atmo_properties[lowerindex][5], atmo_properties[upperindex][5])
    };
}
} // namespace trajectorysim