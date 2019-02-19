#pragma once
#include "Earth.h"

namespace trajectorysim {

class Projectile
{
public:
    static const double k_PI;

    Projectile();
    Projectile(Earth::Coords position, Earth::Coords velocity, double mass);
    Earth::Coords GetPos();
    Earth::Coords GetVel();

    // Returns magnitude of the ECEF position
    double GetPosMag();

    // Returns magnitude of the velocity in ECEF coords
    double GetVelMag();

    // Updates the current position of the projectile
    void updatePosition(Earth::Coords accel, double dT);

    // Returns the drag coefficient
    double getDragCoeff(bool subsonic);

    // Returns acceleration on the projectile due to drag
    Earth::Coords GetDragAccel(double density, bool subsonic, double area);
    double getAltitude();
    double getMass();

    // Returns frontal area used in drag calculations
    virtual double getFrontalArea();

    // Returns a string of projectile properties for use in file output
    virtual std::string getProperties();

protected:
    void setAltitude(double altitude);
    void setMass(double mass);
    void setCd_Subsonic(double cd);
    void setCd_Supersonic(double cd);

private:
    Earth::Coords Pos_ECEF;
    Earth::Coords vel_ECEF;
    double altitude;
    double mass;
    double Cd_subsonic;
    double Cd_supersonic;
};

class Cylinder : public Projectile
{
public:
    Cylinder();
    Cylinder(Earth::Coords position, Earth::Coords velocity, double mass, double diameter, double length, double Cd_subsonic, double Cd_supersonic);
    virtual double getFrontalArea();
    double getDiameter();
    double getLength();
    virtual std::string getProperties();

private:
    double diameter;
    double length;
    double width;
    double area;
};

class Sphere : public Projectile
{
public:
    Sphere();
    Sphere(Earth::Coords position, Earth::Coords velocity, double mass, double diameter, double Cd_subsonic, double Cd_supersonic);
    virtual double getFrontalArea();
    double getDiameter();
    virtual std::string getProperties();

private:
    double diameter;
    double area;
};
} // namespace trajectorysim