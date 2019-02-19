#include "stdafx.h"
#include "Dispersion.h"

namespace trajectorysim {

Dispersion::Dispersion()
{
}

const double Dispersion::k_PI = 3.14159265359;

Earth::Coords Dispersion::Random3DUnitVector(std::mt19937 &rng) {
    std::uniform_real_distribution<double> distr(0, 1);

    double theta = 2 * k_PI * distr(rng);
    double phi = acos(2 * distr(rng) - 1);

    Earth::Coords UnitVector{
        sin(theta)*cos(phi),
        sin(theta)*sin(phi),
        cos(theta)
    };
    return UnitVector;
}

Earth::Coords Dispersion::Disperse3DVector(std::mt19937 &rng, Earth::Coords mean, Earth::Coords stdDeviation) {
    //Generate normal distribution for 3d coords
    Earth::Coords output;
    if (stdDeviation.x <= 0) output.x = mean.x;
    else {
        std::normal_distribution<double> distr(mean.x, stdDeviation.x);
        output.x = distr(rng);
    }
    if (stdDeviation.y <= 0) output.y = mean.y;
    else {
        std::normal_distribution<double> distr(mean.y, stdDeviation.y);
        output.y = distr(rng);
    }
    if (stdDeviation.z <= 0) output.z = mean.z;
    else {
        std::normal_distribution<double> distr(mean.z, stdDeviation.z);
        output.z = distr(rng);
    }
    return output;
}

Earth::LatLonAlt Dispersion::Disperse3DVector(std::mt19937 &rng, Earth::LatLonAlt mean, Earth::LatLonAlt stdDeviation) {
    //Generate normal distribution for 3d coords
    Earth::LatLonAlt output;
    if (stdDeviation.lat <= 0) output.lat = mean.lat;
    else {
        std::normal_distribution<double> distr(mean.lat, stdDeviation.lat);
        output.lat = distr(rng);
    }
    if (stdDeviation.lon <= 0) output.lon = mean.lon;
    else {
        std::normal_distribution<double> distr(mean.lon, stdDeviation.lon);
        output.lon = distr(rng);
    }
    if (stdDeviation.alt <= 0) output.alt = mean.alt;
    else {
        std::normal_distribution<double> distr(mean.alt, stdDeviation.alt);
        output.alt = distr(rng);
    }
    return output;
}

double Dispersion::DisperseInput(std::mt19937 &rng, double mean, double stdDeviation, bool allowNegative) {
    //Generate normal distribution
    double output;
    if (stdDeviation <= 0) return mean;
    std::normal_distribution<double> distr(mean, stdDeviation);
    output = distr(rng);
    if (!allowNegative && (output < 0)) output = 1e-9; //return a very small number
    return output;
}
} // namespace trajectorysim