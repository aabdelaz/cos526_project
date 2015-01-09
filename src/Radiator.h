#ifndef __RADIATOR__H__
#define __RADIATOR__H__

#include "R3Shapes/R3Shapes.h"

class Radiator {
    public:
        R3Point GetPosition(void) { return position; }  
        Radiator(R3Point &p, double s) : strength(s), position(p) {};
        ~Radiator(void);
        void SetPosition(R3Point &p)
        {
        	position = p;
        }

        R3Point Position()
        {
        	return position;
        }

        void Move(R3Vector displacement)
        {
            position += displacement;
        }

        void SetStrength(double s)
        {
        	strength = s;
        }

        double Strength()
        {
        	return strength;
        }

    private:
        double strength;
        R3Point position;   
};

#endif
