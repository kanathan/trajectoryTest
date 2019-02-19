#include "stdafx.h"
#include "Simulation.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace trajectorysim {

bool isFileExist(const char *filename) {
    std::ifstream infile(filename);
    return infile.good();
}

Simulation::Simulation(Projectile* projectile, double dT, bool fulloutput, int run_num, std::string fileprefix)
{
    Simulation::projectile = projectile;
    Simulation::dT = dT;
    Simulation::run_num = run_num;
    Simulation::fulloutput = fulloutput;
    Simulation::fileprefix = fileprefix;
    earth = Earth();

}

void Simulation::run() {
    std::ostringstream solution_filename;
    solution_filename << fileprefix << "solution.csv";
    bool fileexists = isFileExist(solution_filename.str().c_str());
    solutionfile.open(solution_filename.str(), std::ios_base::app);
    if (!fileexists) {
        solutionfile << "runNum, dT, tot time, pos_x, pos_y, pos_z, alt, mass, diameter, length, area, subsonic Cd, supersonic Cd, init vel_x, init vel_y, init vel_z" << std::endl;
    }

    if (fulloutput) {
        std::ostringstream filename;
        filename << fileprefix << "run_" << run_num << ".csv";
        runfile.open(filename.str());
        runfile << "time,pos_x,pos_y,pos_z,vel_x,vel_y,vel_z,alt" << std::endl;
    }
    initpos = projectile->GetPos();
    initvel = projectile->GetVel();

    time = 0;

    stepoutput();

    int count = 0;

    while ((projectile->getAltitude() > 0) && (count < 100000)) {
        time += dT;

        Earth::Properties airProp = earth.setProperties(projectile->getAltitude());

        bool subsonic = (airProp.speed_of_sound > projectile->GetVelMag());
        Earth::Coords a_drag = projectile->GetDragAccel(airProp.density, subsonic, projectile->getFrontalArea());
        Earth::Coords a_grav = Earth::Gravity_Accel(projectile->GetPos());
        //sum accels
        Earth::Coords a_tot = {
            a_drag.x + a_grav.x,
            a_drag.y + a_grav.y,
            a_drag.z + a_grav.z
        };

        projectile->updatePosition(a_tot, dT);

        stepoutput();
        ++count;
    }

    solutionoutput();
    std::cout << "Run " << run_num << ": Simulation ended at " << time << " secs" << std::endl;

    runfile.close();
    solutionfile.close();

}

void Simulation::stepoutput() {
    Earth::Coords pos_ECEF = projectile->GetPos();
    Earth::Coords vel_ECEF = projectile->GetVel();
    runfile << std::setprecision(9);
    runfile << time << ",";
    runfile << pos_ECEF.x << "," << pos_ECEF.y << "," << pos_ECEF.z << ",";
    runfile << vel_ECEF.x << "," << vel_ECEF.y << "," << vel_ECEF.z << ",";
    runfile << projectile->getAltitude() << ",";
    runfile << std::endl;
}

void Simulation::solutionoutput() {
    Earth::Coords Pos_ECEF = projectile->GetPos();

    solutionfile << std::setprecision(9);
    solutionfile << run_num << "," << dT << "," << time << ",";
    solutionfile << Pos_ECEF.x << "," << Pos_ECEF.y << "," << Pos_ECEF.z << ",";
    solutionfile << projectile->getAltitude() << ",";
    solutionfile << projectile->getProperties() << ",";
    solutionfile << projectile->getDragCoeff(true) << "," << projectile->getDragCoeff(false) << ",";
    solutionfile << initvel.x << "," << initvel.y << "," << initvel.z << ",";
    solutionfile << std::endl;
}
} // namespace trajectorysim