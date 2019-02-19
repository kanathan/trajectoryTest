#include "stdafx.h"
#include "Projectile.h"
#include <cmath>
#include <sstream>

namespace trajectorysim {

Projectile::Projectile() {

}

Projectile::Projectile(Earth::Coords position, Earth::Coords velocity, double mass) {
    Pos_ECEF = position;
    vel_ECEF = velocity;
    Projectile::mass = mass;
    altitude = Earth::ECEFToAlt(Pos_ECEF);
}

const double Projectile::k_PI = 3.14159265359;

Earth::Coords Projectile::GetPos() {
    return Projectile::Pos_ECEF;
}

Earth::Coords Projectile::GetVel() {
    return Projectile::vel_ECEF;
}

double Projectile::GetPosMag() {
    return sqrt(pow(Projectile::Pos_ECEF.x, 2) + pow(Projectile::Pos_ECEF.y, 2) + pow(Projectile::Pos_ECEF.z, 2));
}

double Projectile::GetVelMag() {
    return sqrt(pow(Projectile::vel_ECEF.x, 2) + pow(Projectile::vel_ECEF.y, 2) + pow(Projectile::vel_ECEF.z, 2));
}

void Projectile::updatePosition(Earth::Coords accel, double dT) {
    Earth::Coords newPos, newVel;

    //Update position
    newPos.x = Pos_ECEF.x + vel_ECEF.x * dT + 0.5 * accel.x * pow(dT, 2);
    newPos.y = Pos_ECEF.y + vel_ECEF.y * dT + 0.5 * accel.y * pow(dT, 2);
    newPos.z = Pos_ECEF.z + vel_ECEF.z * dT + 0.5 * accel.z * pow(dT, 2);

    //Update velocity
    newVel.x = vel_ECEF.x + accel.x * dT;
    newVel.y = vel_ECEF.y + accel.y * dT;
    newVel.z = vel_ECEF.z + accel.z * dT;

    Pos_ECEF = newPos;
    vel_ECEF = newVel;
    altitude = Earth::ECEFToAlt(Pos_ECEF);
}

double Projectile::getDragCoeff(bool subsonic) {
    //May incorporate reynolds number in future. For now, just use a constant value
    if (subsonic) return Cd_subsonic;
    else return Cd_supersonic;
}

Earth::Coords Projectile::GetDragAccel(double density, bool subsonic, double area) {
    Earth::Coords a_drag;

    double a_drag_mag = -(getDragCoeff(subsonic) * area * density * pow(GetVelMag(), 2) / 2) / mass;

    double abs_vel = GetVelMag();
    double alpha = acos(vel_ECEF.x / abs_vel);
    double beta = acos(vel_ECEF.y / abs_vel);
    double gamma = acos(vel_ECEF.z / abs_vel);

    a_drag.x = cos(alpha) * a_drag_mag;
    a_drag.y = cos(beta) * a_drag_mag;
    a_drag.z = cos(gamma) * a_drag_mag;

    return a_drag;
}

double Projectile::getAltitude() { return altitude; }
double Projectile::getMass() { return mass; }
double Projectile::getFrontalArea() { return 0; }
void Projectile::setAltitude(double altitude) { Projectile::altitude = altitude; }
void Projectile::setMass(double mass) { Projectile::mass = mass; }
void Projectile::setCd_Subsonic(double cd) { Projectile::Cd_subsonic = cd; }
void Projectile::setCd_Supersonic(double cd) { Projectile::Cd_supersonic = cd; }

std::string Projectile::getProperties() {
    std::ostringstream output;
    output << mass << ",,," << getFrontalArea();
    return output.str();
}

Cylinder::Cylinder() {}
Cylinder::Cylinder(Earth::Coords position, Earth::Coords velocity, double mass, double diameter, double length, double Cd_subsonic, double Cd_supersonic) : Projectile(position, velocity, mass) {
    Projectile(position, velocity, mass);
    Cylinder::diameter = diameter;
    Cylinder::length = length;
    setCd_Subsonic(Cd_subsonic);
    setCd_Supersonic(Cd_supersonic);
}

double Cylinder::getDiameter() { return diameter; }
double Cylinder::getLength() { return length; }
double Cylinder::getFrontalArea() {
    return length * diameter;
}
std::string Cylinder::getProperties() {
    std::ostringstream output;
    output << getMass() << "," << diameter << "," << length << "," << getFrontalArea();
    return output.str();
}

Sphere::Sphere() {}
Sphere::Sphere(Earth::Coords position, Earth::Coords velocity, double mass, double diameter, double Cd_subsonic, double Cd_supersonic) : Projectile(position, velocity, mass) {
    Sphere::diameter = diameter;
    setCd_Subsonic(Cd_subsonic);
    setCd_Supersonic(Cd_supersonic);
}

double Sphere::getDiameter() { return diameter; }
double Sphere::getFrontalArea() {
    return k_PI * pow(diameter / 2.0, 2);
}
std::string Sphere::getProperties() {
    std::ostringstream output;
    output << getMass() << "," << diameter << ",," << getFrontalArea();
    return output.str();
}
} // namespace trajectorysim