#pragma once
#include "Mover.h"
#include "Effect.h"
#include "Vect2.h"
#include <cmath>
#include <string>
#include <typeindex>
#include <any>
#include <vector>

class ConstantForce : public Effect {
  // Applies a constant force to movers, scaled by a mover-specific parameter
  public:
    Vect2 forceVector;
    
    ConstantForce(Vect2 forceVector) 
      : forceVector(forceVector) {
        paramCount = 1; // One mover-specific parameter (scale factor)
    };
    
    void apply(Mover* mover) override {
      // Apply the force scaled by the mover's parameter
      float scaleFactor = paramsFromMover(mover);
      Vect2 scaledForce = forceVector * scaleFactor;
      mover->apply_force(scaledForce);
    }
    
    std::any interpretParams(std::vector<std::any> params) override {
      if (params.size() != paramCount) {
        throw std::invalid_argument("ConstantForce::interpretParams: incorrect number of parameters. Expected " + 
                                   std::to_string(paramCount) + " parameter(s).");
      }
      float scaleFactor = std::any_cast<float>(params[0]);
      return scaleFactor;
    }
    
    float paramsFromMover(Mover* mover) {
      std::any params = interpretParams(mover->interactionParams[typeid(ConstantForce)]);
      return std::any_cast<float>(params);
    }
}; 