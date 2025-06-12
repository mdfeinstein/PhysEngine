#pragma once
#include "Mover.h"
#include "Interaction.h"
#include "Vect2.h"
#include <cmath>
#include <string>

/*
implements a spring interaction for all kinds of simulation objects,
whereby a spring force is applied to objects. If they are closer than the equilibrium distance,
the force is repulsive, and if they are further, the force is attractive.
*/


class Spring : public Interaction {
    public:
        float k; //spring constant
        float x0; //equilibrium distance
        Spring(float k = 1, float x0 = 5) : k(k), x0(x0) {paramCount = 0;};
        void interact(Mover* mover1, Mover* mover2);
        //dont need to override OnAdd, as there are no additional properties to set
};

