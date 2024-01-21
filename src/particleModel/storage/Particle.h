/*
 * Particle.h
 *
 *  Created on: 23.02.2010
 *      Author: eckhardw
 */

#pragma once

#include <iostream>
#include <array>
#include <string>
#include <iostream>
#include <fstream>
#include "utils/ArrayUtils.h"

class Particle {
private:
    /**
    * Position of the particle
    */
    std::array<double, 3> x;

    /**
    * Velocity of the particle
    */
    std::array<double, 3> v;

    /**
    * Mass of this particle
    */
    double m;

    /**
    * Type of the particle. Use it for whatever you want (e.g. to separate
    * molecules belonging to different bodies, matters, and so on)
    */
    int type;

    /**
    * Force (which was) effective on this particle
    */
    std::array<double, 3> f_1;

    /**
    * Force (which was) effective on this particle
    */
    std::array<double, 3> f_2;

    /**
    * Used for switching between f1 and f2, determines which was effective on
    * this particle meaning which force vector is referring to the current old
    * force (f2 in the beginning)
    */
    bool secondIsOld = true;

    std::array<int,3> boundaries_crossed;

public:
    explicit Particle(int type = 0);

    Particle(const Particle &other);

    Particle(
        // for visualization, we need always 3 coordinates
        // -> in case of 2d, we use only the first and the second
        std::array<double, 3> x_arg, std::array<double, 3> v_arg, double m_arg,
        int type = 0);

    virtual ~Particle();

    std::array<int,3>& getBoundariesCrossed(){return boundaries_crossed;}
    int& getBoundariesCrossed(int i){return boundaries_crossed[i];};
    void incBoundariesCrossedI(int i){
        //std::cout << "Called periodic inc\n"; 
        boundaries_crossed[i]+=1;
        // std::cout << "cross: " << boundaries_crossed << "\n";
        }
    void decBoundariesCrossedI(int i){
        //std::cout << "Called periodic inc\n";
        boundaries_crossed[i]-=1; 
        //std::cout << "cross: " << boundaries_crossed << "\n";
        };
    void setPeriodicZero(){boundaries_crossed = {0,0,0};};

    const std::array<double, 3> &getX() const;

    const std::array<double, 3> &getV() const;

    const std::array<double, 3> &getF() const;

    const std::array<double, 3> &getOldF() const;

    double getM() const;

    int getType() const;



    void setX(int index, double value);

    void setX(std::array<double,3> new_x);


    void setV(int index, double value);

    void setV(std::array<double,3> new_v);
    
    void addX(int index, double value);

    void addX(std::array<double,3> &x_add);

    void addF(int index, double value);

    void addF(std::array<double,3> add_f);

    void shiftF();

    bool operator==(Particle &other);

    bool operator==(const Particle &other) const;

    std::string toString() const;


    friend std::ostream& operator<<(std::ostream& os, const Particle& particle) {
        os << particle.x[0] << ' ' << particle.x[1] << ' ' << particle.x[2] << ' '
           << particle.v[0] << ' ' << particle.v[1] << ' ' << particle.v[2] << ' '
           << particle.m << ' ' << particle.type << ' '
           << particle.f_1[0] << ' ' << particle.f_1[1] << ' ' << particle.f_1[2] << ' '
           << particle.f_2[0] << ' ' << particle.f_2[1] << ' ' << particle.f_2[2] << ' '
           << particle.secondIsOld << ' ';
        return os;
    }
    // Deserialization function to load Particle object from a file
    friend std::istream& operator>>(std::istream& is, Particle& particle) {
        is >> particle.x[0] >> particle.x[1] >> particle.x[2]
           >> particle.v[0] >> particle.v[1] >> particle.v[2]
           >> particle.m >> particle.type
           >> particle.f_1[0] >> particle.f_1[1] >> particle.f_1[2]
           >> particle.f_2[0] >> particle.f_2[1] >> particle.f_2[2]
           >> particle.secondIsOld;
        return is;
    }
};

