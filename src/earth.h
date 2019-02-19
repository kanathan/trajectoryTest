#pragma once
#include <vector>

namespace trajectorysim {

class Earth
{
public:
    struct Coords {
        double x;
        double y;
        double z;
    };

    struct LatLonAlt {
        double lat;
        double lon;
        double alt;
    };

    struct Properties {
        double temp;
        double press;
        double density;
        double speed_of_sound;
        double dyn_viscosity;
    };

    static const double a;
    static const double f;

    std::vector<std::vector<double>> atmo_properties;
    Properties current_properties;

    Earth();

    // Outputs LatLonAlt coords to ECEF coords using WGS85
    static Coords LatLonAltToECEF(LatLonAlt pos_LLA);

    // Outputs altitude at current ECEF coords, using WGS85
    static double ECEFToAlt(Coords Pos_ECEF);

    // Provides acceleration of gravity in ECEF frame for given position
    static Earth::Coords Gravity_Accel(Coords Pos_ECEF);

    // Returns reynolds number for a given velocity. Must update aero
    // properties using setProperties() before running
    double GetReynoldsNumber(double velocity, double charLength);

    // Updates current aero properties from atmo_properties, based on current altitude
    Properties setProperties(double altitude);

private:
    // Pulls data from atmosphere.csv into atmo_properties
    std::vector<std::vector<double>> getAtmoTable();
};
} // namespace trajectorysim