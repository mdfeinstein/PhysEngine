#pragma once
#include "Mover.h"
#include "Interaction.h"
#include "Vect2.h"
#include <cmath>
#include <string>

/*
Includes implentation for a gravity interaction for all kinds of simulation objects,
and defines a gravity Interaction object
*/


class Gravity : public Interaction {
    public:
        float G;
        Gravity(float G = 0.0001) : G(G) {paramCount = 0;};
        void interact(Mover* mover1, Mover* mover2);
};

void Gravity::interact(Mover* mover1, Mover* mover2){
    //move interaction logic here, isntead of inside mover
    Vect2 r = mover1->position - mover2->position;
    float magnitude = r.mag();
    if (magnitude < min_distance) { //need some code to prevent infinities
        magnitude = min_distance;
    };
    Vect2 force = -G*mover1->mass*mover2->mass*r*1/pow(magnitude, 3);
    mover1->apply_force(force);
    mover2->apply_force(-1*force);
};
