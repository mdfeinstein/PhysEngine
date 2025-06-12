#pragma once
#include "Mover.h"
#include "Effect.h"

class LorentzEffect : public Effect {
  //Applies a drag force to movers
  public:
    float magneticStrength;
    LorentzEffect(float magneticStrength = 1) : magneticStrength(magneticStrength) {paramCount = 1;}; 
    void apply(Mover* mover);
    float paramsFromMover(Mover* mover);
    std:: any interpretParams(std::vector<std::any> params) override;
};

void LorentzEffect::apply(Mover* mover) {
  float PI = 3.14159265359;
  Vect2 force = magneticStrength * paramsFromMover(mover) * mover->velocity.rotate(PI/2);
  // Vect2 force = magneticStrength * 1 * mover->velocity.rotate(PI/2);
  mover->apply_force(force);
}

float LorentzEffect::paramsFromMover(Mover* mover) {
    std::any params = interpretParams(mover->interactionParams[typeid(LorentzEffect)]);
    return std::any_cast<float>(params);
}

std::any LorentzEffect::interpretParams(std::vector<std::any> params) {
    if (params.size() != paramCount) {
        throw std::invalid_argument("LorentzEffect::interpretParams: incorrect number of parameters. Expected " + 
                                    std::to_string(paramCount) + " parameter(s).");
    }
    float charge = std::any_cast<float>(params[0]);
    return charge;
}
