#pragma once
#include "Projectile.h"
#include <fstream>

namespace trajectorysim {
class Simulation
{
public:
    // Create a Simulation object. Required before running sim, using run(). Will output results to [prefix]solution.csv, and if
    // fullout is true, [prefix]run_[run_num].csv as well.
    // Projectile must be a pointer, and can include child classes
    // dT is the simulation time step in secs
    // fulloutput if set to true will generate separate .csv files for each simulation run with full pos/vel outputs
    // run_num is used to identify the run in output files
    // fileprefix adds a prefix to output files if further separation of simulation outputs is required
    Simulation(Projectile* projectile, double dT, bool fulloutput = false, int run_num = 1, std::string fileprefix = "");

    // Runs the simulation
    void run();

    // Function to output sim results to [prefix]run_[run_num].csv on each timestep
    void stepoutput();

    // Function to output sim results to [prefix]solution.csv after run completion
    void solutionoutput();

private:
    Projectile* projectile;
    double time;
    double dT;
    int run_num;
    bool fulloutput;
    std::string fileprefix;
    std::ofstream solutionfile;
    std::ofstream runfile;
    Earth earth;
    Earth::Coords initpos;
    Earth::Coords initvel;
};
} // namespace trajectorysim