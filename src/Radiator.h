#ifndef __RADIATOR__H__
#define __RADIATOR__H__

#include "R3Shapes/R3Shapes.h"

class Radiator {
    public:
        Radiator(double s, R3Point &p) : strength(s), position(p) {};
        ~Radiator(void); 

    private:
        double strength;
        R3Point position;   
};

#endif
