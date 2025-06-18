#pragma once
#include "Simulator.h"
#include "Mover.h"
#include "EffectTemplated.h"
#include <tuple>
#include <functional>
#include <string>

struct EffectAdder {
  // contains a list of add effect methods for different kinds of common effects, 
  // which can add the effect to the simulator or return the effect object
  // also provides a method to provide mover parameters for the effect in the proper format
  // or to add directly to the mover

  Simulator* sim; //simulator not required to be able to use this class, 
  // but add methods without explicit sim wont work without it 

  EffectAdder(Simulator* sim = nullptr) : sim(sim) {};

    std::tuple<float> createMoverDragParams(float drag_coeff){
    //provides interface for creating mover params for Drag effect
    return std::make_tuple(drag_coeff);
  }

  void addDragToMover(float drag_coeff, Mover& mover) {
    mover.effectParams["Drag"] = createMoverDragParams(drag_coeff);
  }

  //Drag
  void addDrag(float strength, float default_drag_coeff, Simulator* sim = nullptr) {
    // optionally provide simulator, otherwise use this->sim. if neither, throw error
    if (sim == nullptr)
      if (this->sim == nullptr)
        throw std::invalid_argument("EffectAdder::addDrag: simulator not provided");
      else sim = this->sim;

    auto func = [](Mover& mover, std::tuple<float> globalParams, std::tuple<float> moverParams) {
          float drag_coeff = std::get<0>(moverParams);
          float strength = std::get<0>(globalParams);
          mover.apply_force(-1*drag_coeff*strength*mover.velocity);
        };
    std::string name = "Drag";
    std::tuple<float> globalParams = std::make_tuple(strength);
    std::tuple<float> moverParams = createMoverDragParams(default_drag_coeff);
    sim->add_effectFunction<std::tuple<float>, std::tuple<float>>(
      func, name, globalParams, moverParams
    );
  }

  EffectTemplated<std::tuple<float>, std::tuple<float>> createDragTemplated(float strength, float default_drag_coeff) {
    // optionally provide simulator, otherwise use this->sim. if neither, throw error
    auto func = [](Mover& mover, std::tuple<float> globalParams, std::tuple<float> moverParams) {
          float drag_coeff = std::get<0>(moverParams);
          float strength = std::get<0>(globalParams);
          mover.apply_force(-1*drag_coeff*strength*mover.velocity);
        };
    std::string name = "DragTemplated";
    std::tuple<float> globalParams = std::make_tuple(strength);
    std::tuple<float> moverParams = createMoverDragParams(default_drag_coeff);
    return EffectTemplated<std::tuple<float>, std::tuple<float>>(globalParams, func, name);
  }


  //Lorentz
  std::tuple<float> createMoverLorentzParams(float charge) {
    //provides interface for creating mover params for Lorentz effect
    return std::make_tuple(charge);
  }

  void addLorentzToMover(float charge, Mover& mover) {
    mover.effectParams["Lorentz"] = createMoverLorentzParams(charge);
  }

  void addLorentz(float strength, float default_charge, Simulator* sim = nullptr) {
    // optionally provide simulator, otherwise use this->sim. if neither, throw error
    if (sim == nullptr)
      if (this->sim == nullptr)
        throw std::invalid_argument("EffectAdder::addLorentz: simulator not provided");
      else sim = this->sim;

    auto func = [](Mover& mover, std::tuple<float> globalParams, std::tuple<float> moverParams) {
          float charge = std::get<0>(moverParams);
          float strength = std::get<0>(globalParams);
          float PI = 3.14159265359;
          Vect2 force = strength * charge* mover.velocity.rotate(PI/2);
          mover.apply_force(force);
        };
    std::string name = "Lorentz";
    std::tuple<float> globalParams = std::make_tuple(strength);
    std::tuple<float> moverParams = createMoverLorentzParams(default_charge);
    sim->add_effectFunction<std::tuple<float>, std::tuple<float>>(
      func, name, globalParams, moverParams
    );
  }

  //Attractor
  std::tuple<> createMoverAttractorParams() { //no mover args
    //provides interface for creating mover params for Attractor effect
    return std::make_tuple();
  }

  void addAttractorToMover(float strength, Vect2 position, float min_distance, Mover& mover) {
    mover.effectParams["Attractor"] = createMoverAttractorParams();
  }

  void addAttractor(float strength, Vect2 position, float min_distance, Simulator* sim = nullptr) {
    // optionally provide simulator, otherwise use this->sim. if neither, throw error
    if (sim == nullptr)
      if (this->sim == nullptr)
        throw std::invalid_argument("EffectAdder::addAttractor: simulator not provided");
      else sim = this->sim;

    auto func = [](Mover& mover, std::tuple<float, Vect2, float> globalParams, std::tuple<> moverParams) {
          auto [strength, position, min_distance] = globalParams;
          Vect2 r = position - mover.position;
          float magnitude = r.mag();
          if (magnitude < min_distance) { //need some code to prevent infinities
              return;
          }
          Vect2 force = strength*mover.mass*r/pow(magnitude, 3);
          mover.apply_force(force);
        };
    std::string name = "Attractor";
    std::tuple<float, Vect2, float> globalParams = std::make_tuple(strength, position, min_distance);
    std::tuple<> moverParams = createMoverAttractorParams();
    sim->add_effectFunction<std::tuple<float, Vect2, float>, std::tuple<>>(
      func, name, globalParams, moverParams
    );
  }
      
};

