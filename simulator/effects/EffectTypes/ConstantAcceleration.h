#pragma once
#include "Mover.h"
#include "Effect.h"
#include "Vect2.h"

class ConstantAcceleration : public Effect {
  // Applies a constant acceleration to movers
  public:
    Vect2 acceleration;
    
    ConstantAcceleration(Vect2 acceleration) 
      : acceleration(acceleration) {
        paramCount = 0; // No mover-specific parameters
    };
    
    void apply(Mover* mover) override {
      // F = ma
      Vect2 force = mover->mass * acceleration;
      mover->apply_force(force);
    }
}; 