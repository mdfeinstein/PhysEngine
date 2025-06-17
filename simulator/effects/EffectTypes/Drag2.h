#pragma once
#include "Mover.h"
#include "Effect2.h"

class Drag2 : public Effect2 {
  //Applies a drag force to movers
  public:
    //global parameters
    float strength;
    //mover-specific parameters
    mutable float drag_coeff; //mutable because it is set in each apply call
    Drag2(float strength = 1) : strength(strength), Effect2(1,1) {};
    Drag2(std::vector<std::any> globalParams) : Effect2(1,1) {
        parseGlobalParams(globalParams);
    }
    void parseGlobalParams(std::vector<std::any> params) override;
    void parseMoverParams(std::vector<std::any> params) const override;
    void apply(Mover* mover) const override;
};

void Drag2::parseGlobalParams(std::vector<std::any> params) {
    if (params.size() != globalParamCount) {
        throw std::invalid_argument("Drag2::parseGlobalParams: incorrect number of parameters. Expected " + 
                                    std::to_string(globalParamCount) + " parameter(s).");
    }
    strength = std::any_cast<float>(params[0]);
}

void Drag2::parseMoverParams(std::vector<std::any> params) const {
    if (params.size() != moverParamCount) {
        throw std::invalid_argument("Drag2::parseMoverParams: incorrect number of parameters. Expected " + 
                                    std::to_string(moverParamCount) + " parameter(s).");
    }
    drag_coeff = std::any_cast<float>(params[0]);
}

void Drag2::apply(Mover* mover) const {
    std::vector<std::any> moverParams = mover->interactionParams[typeid(Drag2)];
    parseMoverParams(moverParams); //stores result in drag_coeff
    mover->apply_force(-1*drag_coeff*strength*mover->velocity);
}

