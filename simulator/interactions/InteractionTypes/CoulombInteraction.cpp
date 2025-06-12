#include "CoulombInteraction.h"

void Coulomb::interact(Mover* mover1, Mover* mover2){
    //move interaction logic here, isntead of inside mover
    Vect2 r = mover1->position - mover2->position;
    float magnitude = r.mag();
    magnitude = std::max(magnitude, min_distance);
    float q1 = paramsFromMover(mover1);
    float q2 = paramsFromMover(mover2);
    Vect2 force = K*q1*q2*r*1/pow(magnitude, 3);
    mover1->apply_force(force);
    mover2->apply_force(-1*force);
};

std::any Coulomb::interpretParams(std::vector<std::any> params) {
    //expected params is just charge, so we expect a single float
    //perhaps a tuple should be returned for consistency with larger param sets, but I think its fine.
    // just look here at interpretParams to see what the any contains.
    if (params.size() != paramCount) {
        throw std::invalid_argument("Coulomb::interpretParams: incorrect number of parameters. Expected " + std::to_string(paramCount) + " parameter(s).");
    }
    float charge = std::any_cast<float>(params[0]);
    return charge;
}

float Coulomb::paramsFromMover(Mover* mover) {
    std::any params = interpretParams(mover->interactionParams[typeid(Coulomb)]);
    return std::any_cast<float>(params);
}