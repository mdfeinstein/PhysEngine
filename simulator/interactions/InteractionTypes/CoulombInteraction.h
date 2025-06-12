#pragma once
#include "Mover.h"
#include "Interaction.h"
#include "Vect2.h"
#include <cmath>
#include <string>
#include <typeindex>
#include <any>
#include <vector>

/*
Includes implentation for a gravity interaction for all kinds of simulation objects,
and defines a gravity Interaction object
*/


class Coulomb : public Interaction {
    //Mover specified charge should explicitly provided as float.
    public:
        float K;
        Coulomb(float K = 1) : K(K) {paramCount = 1;};
        void interact(Mover* mover1, Mover* mover2);
        std::any interpretParams(std::vector<std::any> params);
        float paramsFromMover(Mover* mover);
};