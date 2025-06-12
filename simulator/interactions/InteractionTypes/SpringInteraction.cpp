#include "SpringInteraction.h"

/*
implements a spring interaction for all kinds of simulation objects,
whereby a spring force is applied to objects. If they are closer than the equilibrium distance,
the force is repulsive, and if they are further, the force is attractive.
*/

void Spring::interact(Mover* mover1, Mover* mover2){
    Vect2 r = mover1->position - mover2->position;
    float magnitude = r.mag();
    //apply min_distance
    magnitude = std::max(magnitude, min_distance);
    float springForceMag = -k*(magnitude - x0);
    Vect2 springForce = springForceMag * r/magnitude;
    mover1->apply_force(springForce);
    mover2->apply_force(-1*springForce);
};