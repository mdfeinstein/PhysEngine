#pragma once
#include "Mover.h"
#include "Effect.h"
#include "Vect2.h"
#include <cmath>
#include <string>
#include <typeindex>
#include <any>
#include <vector>
#include <functional>

class TimeVaryingForce : public Effect {
  // Applies a time-varying force to movers, scaled by a mover-specific parameter
  public:
    std::function<Vect2(float)> forceFunction;
    float& currentTime;
    TimeVaryingForce(std::function<Vect2(float)> forceFunction,
     float& currentTime) // reference to current time in simulator, 
     // or some other time variable that outlives the effect
      : forceFunction(forceFunction), currentTime(currentTime) {
        paramCount = 1; // One mover-specific parameter (scale factor)
    };
    
    void apply(Mover* mover) override {
      // Get the current time-dependent force vector
      Vect2 currentForce = forceFunction(currentTime);
      
      // Apply the force scaled by the mover's parameter
      float scaleFactor = paramsFromMover(mover);
      Vect2 scaledForce = currentForce * scaleFactor;
      mover->apply_force(scaledForce);
    }
    

    
    std::any interpretParams(std::vector<std::any> params) override {
      if (params.size() != paramCount) {
        throw std::invalid_argument("TimeVaryingForce::interpretParams: incorrect number of parameters. Expected " + 
                                   std::to_string(paramCount) + " parameter(s).");
      }
      float scaleFactor = std::any_cast<float>(params[0]);
      return scaleFactor;
    }
    
    float paramsFromMover(Mover* mover) {
      std::any params = interpretParams(mover->interactionParams[typeid(TimeVaryingForce)]);
      return std::any_cast<float>(params);
    }
}; 