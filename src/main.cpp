// main.cpp : Handles program initilization, takes user input, and iterates through multiple simulations.
// By providing --seed argument, can ensure that simulation results for given inputs are indentical each time the program runs
//

#include "stdafx.h"
#include "Projectile.h"
#include "Earth.h"
#include "Simulation.h"
#include "Dispersion.h"
#include "cxxopts.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <string>

namespace trajectorysim {

struct RunParms {
    Earth::LatLonAlt pos_LLA;
    Earth::Coords vel_ECEF;
    double mass;
    double length;
    double diameter;
    double impulse;
    double Cd_subsonic;
    double Cd_supersonic;
    std::string shape;
};

int main(int argc, char *argv[])
{
    //Default parameters
    RunParms init_run_parms;
    init_run_parms.pos_LLA = {
        -39.91144,
        177.96056,
        74625.5
    };
    init_run_parms.vel_ECEF = {
        196.9,
        -193.5,
        -2149.0
    };
    init_run_parms.mass = 1068.0;
    init_run_parms.length = 12.6;
    init_run_parms.diameter = 1.2;
    init_run_parms.impulse = 73.7;
    init_run_parms.Cd_subsonic = 0.8;
    init_run_parms.Cd_supersonic = 1.6;
    init_run_parms.shape = "cylinder";

    //Default dispersions
    RunParms parm_std_deviation{
        Earth::LatLonAlt {0.0,0.0,0.0},
        Earth::Coords {0.0,0.0,0.0},
        0.0,0.0,0.0,0.0,0.0,0.0,
        ""
    };

    //Default Sim settings
    double seed = NULL;
    double dT = 0.1;
    int simCount = 10;
    std::string fileprefix = "";
	bool full_output = false;

    //Default bools
    bool rand_pos = false;
    bool rand_vel = false;
    bool rand_mass = false;
    bool rand_length = false;
    bool rand_diameter = false;
    bool rand_impulse = false;
    bool rand_Cd_subsonic = false;
    bool rand_Cd_supersonic = false;

    //Get arguments using cxxopts library
    try {
        cxxopts::Options options("TrajectorySim", "Trajectory Simulation");
        options.add_options()
            ("help", "Help")
            ("lat", "Latitude", cxxopts::value<double>()->default_value(std::to_string(init_run_parms.pos_LLA.lat)))
            ("lon", "Longitude", cxxopts::value<double>()->default_value(std::to_string(init_run_parms.pos_LLA.lon)))
            ("alt", "Altitude", cxxopts::value<double>()->default_value(std::to_string(init_run_parms.pos_LLA.alt)))
            ("Vx", "Velocity_x", cxxopts::value<double>()->default_value(std::to_string(init_run_parms.vel_ECEF.x)))
            ("Vy", "Velocity_y", cxxopts::value<double>()->default_value(std::to_string(init_run_parms.vel_ECEF.y)))
            ("Vz", "Velocity_z", cxxopts::value<double>()->default_value(std::to_string(init_run_parms.vel_ECEF.z)))
            ("mass", "Mass", cxxopts::value<double>()->default_value(std::to_string(init_run_parms.mass)))
            ("length", "Length", cxxopts::value<double>()->default_value(std::to_string(init_run_parms.length)))
            ("diameter", "Diameter", cxxopts::value<double>()->default_value(std::to_string(init_run_parms.diameter)))
            ("impulse", "ExpImpulse", cxxopts::value<double>()->default_value(std::to_string(init_run_parms.impulse)))
            ("cd_subsonic", "Cd Subsonic", cxxopts::value<double>()->default_value(std::to_string(init_run_parms.Cd_subsonic)))
            ("cd_supersonic", "Cd Supersonic", cxxopts::value<double>()->default_value(std::to_string(init_run_parms.Cd_supersonic)))
            ("lat_stddev", "Latitude_stddev", cxxopts::value<double>()->default_value(std::to_string(0.0)))
            ("lon_stddev", "Longitude_stddev", cxxopts::value<double>()->default_value(std::to_string(0.0)))
            ("alt_stddev", "Altitude_stddev", cxxopts::value<double>()->default_value(std::to_string(0.0)))
            ("Vx_stddev", "Velocity_x_stddev", cxxopts::value<double>()->default_value(std::to_string(0.0)))
            ("Vy_stddev", "Velocity_y_stddev", cxxopts::value<double>()->default_value(std::to_string(0.0)))
            ("Vz_stddev", "Velocity_z_stddev", cxxopts::value<double>()->default_value(std::to_string(0.0)))
            ("mass_stddev", "Mass_stddev", cxxopts::value<double>()->default_value(std::to_string(0.0)))
            ("length_stddev", "Length_stddev", cxxopts::value<double>()->default_value(std::to_string(0.0)))
            ("diameter_stddev", "Diameter_stddev", cxxopts::value<double>()->default_value(std::to_string(0.0)))
            ("impulse_stddev", "ExpImpulse_stddev", cxxopts::value<double>()->default_value(std::to_string(0.0)))
            ("cd_subsonic_stddev", "Cd Subsonic_stddev", cxxopts::value<double>()->default_value(std::to_string(0.0)))
            ("cd_supersonic_stddev", "Cd Supersonic_stddev", cxxopts::value<double>()->default_value(std::to_string(0.0)))
            ("shape", "Shape", cxxopts::value<std::string>()->default_value(init_run_parms.shape))
            ("seed", "Seed", cxxopts::value<std::string>()->default_value("null"))
            ("dT", "Simulation dT", cxxopts::value<double>()->default_value(std::to_string(dT)))
            ("simcount", "# of Simulations", cxxopts::value<int>()->default_value(std::to_string(simCount)))
            ("filePrefix", "Output File Prefix", cxxopts::value<std::string>()->default_value(""))
			("fullOutput", "Output Full Run Info")
            ("randPos", "Disperse Position")
            ("randVel", "Disperse Velocity")
            ("randMass", "Disperse Mass")
            ("randLength", "Disperse Length")
            ("randDiameter", "Disperse Diameter")
            ("randImpulse", "Disperse ExpImpulse")
            ("randCd_subsonic", "Disperse Subsonic Cd")
            ("randCd_supersonic", "Disperse Supersonic Cd")
            ;
        auto result = options.parse(argc, argv);

        if (result["help"].as<bool>()) {
            std::cout << options.help() << std::endl;
            return 0;
        }

        init_run_parms.pos_LLA = {
            result["lat"].as<double>(),
            result["lon"].as<double>(),
            result["alt"].as<double>()
        };
        init_run_parms.vel_ECEF = {
            result["Vx"].as<double>(),
            result["Vy"].as<double>(),
            result["Vz"].as<double>()
        };
        init_run_parms.mass = result["mass"].as<double>();
        init_run_parms.length = result["length"].as<double>();
        init_run_parms.diameter = result["diameter"].as<double>();
        init_run_parms.impulse = result["impulse"].as<double>();
        init_run_parms.Cd_subsonic = result["cd_subsonic"].as<double>();
        init_run_parms.Cd_supersonic = result["cd_supersonic"].as<double>();

        parm_std_deviation.pos_LLA = {
            result["lat_stddev"].as<double>(),
            result["lon_stddev"].as<double>(),
            result["alt_stddev"].as<double>()
        };
        parm_std_deviation.vel_ECEF = {
            result["Vx_stddev"].as<double>(),
            result["Vy_stddev"].as<double>(),
            result["Vz_stddev"].as<double>()
        };
        parm_std_deviation.mass = result["mass_stddev"].as<double>();
        parm_std_deviation.length = result["length_stddev"].as<double>();
        parm_std_deviation.diameter = result["diameter_stddev"].as<double>();
        parm_std_deviation.impulse = result["impulse_stddev"].as<double>();
        parm_std_deviation.Cd_subsonic = result["cd_subsonic_stddev"].as<double>();
        parm_std_deviation.Cd_supersonic = result["cd_supersonic_stddev"].as<double>();


        init_run_parms.shape = result["shape"].as<std::string>();
        if (result["seed"].as<std::string>() == "null") { seed = NULL; }
        else { seed = std::stod(result["seed"].as<std::string>()); }
        dT = result["dT"].as<double>();
        simCount = result["simcount"].as<int>();
        fileprefix = result["filePrefix"].as<std::string>();
		full_output = result["fullOutput"].as<bool>();
        rand_pos = result["randPos"].as<bool>();
        rand_vel = result["randVel"].as<bool>();
        rand_mass = result["randMass"].as<bool>();
        rand_length = result["randLength"].as<bool>();
        rand_diameter = result["randDiameter"].as<bool>();
        rand_impulse = result["randImpulse"].as<bool>();
        rand_Cd_subsonic = result["randCd_subsonic"].as<bool>();
        rand_Cd_supersonic = result["randCd_supersonic"].as<bool>();
    }
    catch (cxxopts::OptionParseException) {
        std::cout << "Invalid arguments provided" << std::endl;
        return 1;
    }

    //Initialize random number generator, using seed if provided
    std::random_device rd;
    std::mt19937 rng;
    if (seed == NULL) rng.seed(rd());
    else rng.seed(seed);

    //Run sim for each case
    for (int i = 0; i < simCount; ++i) {
        RunParms runParms = init_run_parms;

        if (rand_pos) runParms.pos_LLA = Dispersion::Disperse3DVector(rng, init_run_parms.pos_LLA, parm_std_deviation.pos_LLA);
        if (rand_vel) runParms.vel_ECEF = Dispersion::Disperse3DVector(rng, init_run_parms.vel_ECEF, parm_std_deviation.vel_ECEF);
        if (rand_mass) runParms.mass = Dispersion::DisperseInput(rng, init_run_parms.mass, parm_std_deviation.mass, false);
        if (rand_length) runParms.length = Dispersion::DisperseInput(rng, init_run_parms.length, parm_std_deviation.length, false);
        if (rand_diameter) runParms.diameter = Dispersion::DisperseInput(rng, init_run_parms.diameter, parm_std_deviation.diameter, false);
        if (rand_impulse) runParms.impulse = Dispersion::DisperseInput(rng, init_run_parms.impulse, parm_std_deviation.impulse);
        if (rand_Cd_subsonic) runParms.Cd_subsonic = Dispersion::DisperseInput(rng, init_run_parms.Cd_subsonic, parm_std_deviation.Cd_subsonic, false);
        if (rand_Cd_supersonic) runParms.Cd_supersonic = Dispersion::DisperseInput(rng, init_run_parms.Cd_supersonic, parm_std_deviation.Cd_supersonic, false);

        Earth::Coords impulseUnitVector = Dispersion::Random3DUnitVector(rng);
        Earth::Coords newVel{
            runParms.vel_ECEF.x + runParms.impulse * impulseUnitVector.x,
            runParms.vel_ECEF.y + runParms.impulse * impulseUnitVector.y,
            runParms.vel_ECEF.z + runParms.impulse * impulseUnitVector.z,
        };
        Earth::Coords Pos_ECEF = Earth::LatLonAltToECEF(runParms.pos_LLA);
        Projectile * projectile;
        if (runParms.shape == "cylinder") {
            projectile = new Cylinder(Pos_ECEF, newVel, runParms.mass, runParms.diameter, runParms.length, runParms.Cd_subsonic, runParms.Cd_supersonic);
        }
        else if (runParms.shape == "sphere") {
            projectile = new Sphere(Pos_ECEF, newVel, runParms.mass, runParms.diameter, runParms.Cd_subsonic, runParms.Cd_supersonic);
        }
        else {
            std::cout << "Invalid shape provided" << std::endl;
            return 1;
        }
        try {
            Simulation sim = Simulation(projectile, dT, full_output, i, fileprefix);
            sim.run();
        }
        catch (int e) {
            if (e == 10) {
                std::cout << "atmosphere.csv not found" << std::endl;
                return -1;
            }
			else if (e == 20) {
				std::cout << "Unexpected error converging while converting ECEF to Lat/Lon/Alt";
				return -1;
			}
            throw e;
        }
            
    }

    return 0;
}
} // namespace trajectorysim

//Main with no namespace required for compilation
int main(int argc, char *argv[]) {
    return trajectorysim::main(argc, argv);
}