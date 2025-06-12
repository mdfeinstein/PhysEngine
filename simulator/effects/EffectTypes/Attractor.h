#pragma once
#include "Mover.h"
#include "Effect.h"



class Attractor : public Effect {
    //Applies a gravity-like force to movers
    //for now, fixed position
    public:
    float min_distance; //within this distance, no effect.
    float strength; //positive is attractive, negative is repulsive
    Vect2 position;
    Attractor(float strength = 1, Vect2 position = Vect2(), float min_distance = 10) : strength(strength), position(position), min_distance(min_distance) {paramCount = 0;};
    void apply(Mover* mover);
};

void Attractor::apply(Mover* mover){
    Vect2 r = position - mover->position;
    float magnitude = r.mag();
    if (magnitude < min_distance) { //need some code to prevent infinities
        return;
    }
    Vect2 force = strength*mover->mass*r/pow(magnitude, 3);
    mover->apply_force(force);
};

