#pragma once
#include "Earth.h"
#include <random>

namespace trajectorysim {
//Provides methods for dispersion of inputs
class Dispersion
{
public:
    Dispersion();
    static const double k_PI;

    // Returns a random 3D unit vector. Random distribution is designed to be uniformly
    // distributed around the surface of a sphere, and avoids uneven distribution around poles
    static Earth::Coords Random3DUnitVector(std::mt19937 &rng);

    // Returns a set of 3D coordinates, where each coordinate is normally distributed around the
    // corresponding input coordinate's mean and standard deviation
    // Separate methods are provided for output of Earth::Coords and Earth::LatLonAlt struts
    static Earth::Coords Disperse3DVector(std::mt19937 &rng, Earth::Coords mean, Earth::Coords stdDeviation);
    static Earth::LatLonAlt Disperse3DVector(std::mt19937 &rng, Earth::LatLonAlt mean, Earth::LatLonAlt stdDeviation);

    // Returns a double normally distributed around the provided mean and standard deviation.
    // Setting allowNegative to false will set negative numbers to 1e-9 before returning.
    static double DisperseInput(std::mt19937 &rng, double mean, double stdDeviation, bool allowNegative = true);
};
} // namespace trajectorysim