#pragma once
#include "Mover.h"
#include "Effect.h"

class Drag : public Effect {
  //Applies a drag force to movers
  public:
    float strength;
    Drag(float strength = 1) : strength(strength) {paramCount = 1;};
    void apply(Mover* mover);
    std::any interpretParams(std::vector<std::any> params) override;
    float paramsFromMover(Mover* mover);
};

void Drag::apply(Mover* mover){
    float drag_coeff = paramsFromMover(mover);
    mover->apply_force(-1*drag_coeff*mover->velocity);
}

std::any Drag::interpretParams(std::vector<std::any> params) {
    if (params.size() != paramCount) {
        throw std::invalid_argument("Drag::interpretParams: incorrect number of parameters. Expected " + 
                                    std::to_string(paramCount) + " parameter(s).");
    }
    float drag_coeff = std::any_cast<float>(params[0]);
    return drag_coeff;
}

float Drag::paramsFromMover(Mover* mover) {
    std::any params = interpretParams(mover->interactionParams[typeid(Drag)]);
    return std::any_cast<float>(params);
}
