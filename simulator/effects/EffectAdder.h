#pragma once
#include "EffectTemplated.h"
#include "Mover.h"
#include "Simulator.h"
#include <functional>
#include <string>
#include <tuple>

struct EffectAdder {
  // contains a list of add effect methods for different kinds of common
  // effects, which can add the effect to the simulator or return the effect
  // object also provides a method to provide mover parameters for the effect in
  // the proper format or to add directly to the mover

  // Drag
  static std::tuple<float> createMoverDragParams(float drag_coeff) {
    // provides interface for creating mover params for Drag effect
    return std::make_tuple(drag_coeff);
  }

  static void addDragToMover(float drag_coeff, Mover &mover) {
    mover.effectParams["Drag"] = createMoverDragParams(drag_coeff);
  }

  static void addDrag(Simulator& sim, float strength, float default_drag_coeff) {
    // optionally provide simulator, otherwise use this->sim. if neither, throw
    // error
    auto func = [](Mover &mover, std::tuple<float> globalParams,
                   std::tuple<float> moverParams) {
      float drag_coeff = std::get<0>(moverParams);
      float strength = std::get<0>(globalParams);
      mover.apply_force(-1 * drag_coeff * strength * mover.velocity);
    };
    std::string name = "Drag";
    std::tuple<float> globalParams = std::make_tuple(strength);
    std::tuple<float> moverParams = createMoverDragParams(default_drag_coeff);
    sim.add_effectWrapper<std::tuple<float>, std::tuple<float>>(
        func, name, globalParams, moverParams);
  }

  static EffectTemplated<std::tuple<float>, std::tuple<float>>
  createDragTemplated(float strength, float default_drag_coeff) {

    auto func = [](Mover &mover, std::tuple<float> globalParams,
                   std::tuple<float> moverParams) {
      float drag_coeff = std::get<0>(moverParams);
      float strength = std::get<0>(globalParams);
      mover.apply_force(-1 * drag_coeff * strength * mover.velocity);
    };
    std::string name = "DragTemplated";
    std::tuple<float> globalParams = std::make_tuple(strength);
    std::tuple<float> moverParams = createMoverDragParams(default_drag_coeff);
    return EffectTemplated<std::tuple<float>, std::tuple<float>>(globalParams,
                                                                 func, name);
  }

  // Lorentz
  static std::tuple<float> createMoverLorentzParams(float charge) {
    // provides interface for creating mover params for Lorentz effect
    return std::make_tuple(charge);
  }

  static void addLorentzToMover(float charge, Mover &mover) {
    mover.effectParams["Lorentz"] = createMoverLorentzParams(charge);
  }

  static void addLorentz(Simulator& sim, float strength, float default_charge) {
    // optionally provide simulator, otherwise use this->sim. if neither, throw
    // error
    auto func = [](Mover &mover, std::tuple<float> globalParams,
                   std::tuple<float> moverParams) {
      float charge = std::get<0>(moverParams);
      float strength = std::get<0>(globalParams);
      float PI = 3.14159265359;
      Vect2 force = strength * charge * mover.velocity.rotate(PI / 2);
      mover.apply_force(force);
    };
    std::string name = "Lorentz";
    std::tuple<float> globalParams = std::make_tuple(strength);
    std::tuple<float> moverParams = createMoverLorentzParams(default_charge);
    sim.add_effectWrapper<std::tuple<float>, std::tuple<float>>(
        func, name, globalParams, moverParams);
  }

  // Attractor
  static std::tuple<> createMoverAttractorParams() { // no mover args
    // provides interface for creating mover params for Attractor effect
    return std::make_tuple();
  }

  static void addAttractorToMover(float strength, Vect2 position, float min_distance,
                           Mover &mover) {
    mover.effectParams["Attractor"] = createMoverAttractorParams();
  }

  static void addAttractor(Simulator& sim, float strength, Vect2 position, float min_distance) {
    // optionally provide simulator, otherwise use this->sim. if neither, throw
    // error
    auto func = [](Mover &mover, std::tuple<float, Vect2, float> globalParams,
                   std::tuple<> moverParams) {
      auto [strength, position, min_distance] = globalParams;
      Vect2 r = position - mover.position;
      float magnitude = r.mag();
      if (magnitude < min_distance) { return; } // need some code to prevent infinities
      Vect2 force = strength * mover.mass * r / pow(magnitude, 3);
      mover.apply_force(force);
    };
    std::string name = "Attractor";
    std::tuple<float, Vect2, float> globalParams =
        std::make_tuple(strength, position, min_distance);
    std::tuple<> moverParams = createMoverAttractorParams();
    sim.add_effectWrapper<std::tuple<float, Vect2, float>, std::tuple<>>(
        func, name, globalParams, moverParams);
  }

  // ConstantForce
  static std::tuple<float> createMoverConstantForceParams(float scale_factor = 1.0f) {
    // provides interface for creating mover params for ConstantForce effect
    return std::make_tuple(scale_factor);
  }

  static void addConstantForceToMover(float scale_factor, Mover &mover) {
    mover.effectParams["ConstantForce"] =
        createMoverConstantForceParams(scale_factor);
  }
  static void addConstantForce(Simulator& sim, Vect2 force_vector, float default_scale_factor) {
    // optionally provide simulator, otherwise use this->sim. if neither, throw
    // error
    auto func = [](Mover &mover, std::tuple<Vect2> globalParams,
                   std::tuple<float> moverParams) {
      float scale_factor = std::get<0>(moverParams);
      Vect2 force_vector = std::get<0>(globalParams);
      mover.apply_force(force_vector * scale_factor);
    };
    std::string name = "ConstantForce";
    std::tuple<Vect2> globalParams = std::make_tuple(force_vector);
    std::tuple<float> moverParams =
        createMoverConstantForceParams(default_scale_factor);
    sim.add_effectWrapper<std::tuple<Vect2>, std::tuple<float>>(
        func, name, globalParams, moverParams);
  }
  static EffectTemplated<std::tuple<Vect2>, std::tuple<float>>
  createConstantForceTemplated(Vect2 force_vector,
                               float default_scale_factor = 1.0f) {
    auto func = [](Mover &mover, std::tuple<Vect2> globalParams,
                   std::tuple<float> moverParams) {
      float scale_factor = std::get<0>(moverParams);
      Vect2 force_vector = std::get<0>(globalParams);
      mover.apply_force(force_vector * scale_factor);
    };
    std::string name = "ConstantForceTemplated";
    std::tuple<Vect2> globalParams = std::make_tuple(force_vector);
    std::tuple<float> moverParams =
        createMoverConstantForceParams(default_scale_factor);
    return EffectTemplated<std::tuple<Vect2>, std::tuple<float>>(globalParams,
                                                                 func, name);
  }

  // ForceField
  static std::tuple<float> createMoverForceFieldParams(float scale_factor = 1.0f) {
    // provides interface for creating mover params for ForceField effect
    return std::make_tuple(scale_factor);
  }

  static void addForceFieldToMover(float scale_factor, Mover &mover) {
    mover.effectParams["ForceField"] =
        createMoverForceFieldParams(scale_factor);
  }
  static void addForceField(Simulator& sim, std::function<Vect2(Vect2)> field_function, float default_scale_factor) {
    // optionally provide simulator, otherwise use this->sim. if neither, throw
    // error
    auto func = [](Mover &mover,
                   std::tuple<std::function<Vect2(Vect2)>> globalParams,
                   std::tuple<float> moverParams) {
      float scale_factor = std::get<0>(moverParams);
      auto field_function = std::get<0>(globalParams);
      Vect2 field_force = field_function(mover.position);
      mover.apply_force(field_force * scale_factor);
    };
    std::string name = "ForceField";
    std::tuple<std::function<Vect2(Vect2)>> globalParams =
        std::make_tuple(field_function);
    std::tuple<float> moverParams =
        createMoverForceFieldParams(default_scale_factor);
    sim.add_effectWrapper<std::tuple<std::function<Vect2(Vect2)>>,
                           std::tuple<float>>(func, name, globalParams,
                                              moverParams);
  }
  static EffectTemplated<std::tuple<std::function<Vect2(Vect2)>>, std::tuple<float>>
  createForceFieldTemplated(std::function<Vect2(Vect2)> field_function,
                            float default_scale_factor = 1.0f) {
    auto func = [](Mover &mover,
                   std::tuple<std::function<Vect2(Vect2)>> globalParams,
                   std::tuple<float> moverParams) {
      float scale_factor = std::get<0>(moverParams);
      auto field_function = std::get<0>(globalParams);
      Vect2 field_force = field_function(mover.position);
      mover.apply_force(field_force * scale_factor);
    };
    std::string name = "ForceFieldTemplated";
    std::tuple<std::function<Vect2(Vect2)>> globalParams =
        std::make_tuple(field_function);
    std::tuple<float> moverParams =
        createMoverForceFieldParams(default_scale_factor);
    return EffectTemplated<std::tuple<std::function<Vect2(Vect2)>>,
                           std::tuple<float>>(globalParams, func, name);
  }

  // TimeVaryingForce
  static std::tuple<float>
  createMoverTimeVaryingForceParams(float scale_factor = 1.0f) {
    // provides interface for creating mover params for TimeVaryingForce effect
    return std::make_tuple(scale_factor);
  }

  static void addTimeVaryingForceToMover(float scale_factor, Mover &mover) {
    mover.effectParams["TimeVaryingForce"] =
        createMoverTimeVaryingForceParams(scale_factor);
  }

  static void addTimeVaryingForce(Simulator& sim, std::function<Vect2(float)> force_function, float &current_time, float default_scale_factor) {
    // optionally provide simulator, otherwise use this->sim. if neither, throw
    // error
    auto func =
        [](Mover &mover,
           std::tuple<std::function<Vect2(float)>, float &> globalParams,
           std::tuple<float> moverParams) {
          float scale_factor = std::get<0>(moverParams);
          auto force_function = std::get<0>(globalParams);
          float &current_time = std::get<1>(globalParams);
          Vect2 current_force = force_function(current_time);
          mover.apply_force(current_force * scale_factor);
        };
    std::string name = "TimeVaryingForce";
    std::tuple<std::function<Vect2(float)>, float &> globalParams =
        std::forward_as_tuple(force_function, current_time);
    std::tuple<float> moverParams =
        createMoverTimeVaryingForceParams(default_scale_factor);
    sim.add_effectWrapper<std::tuple<std::function<Vect2(float)>, float &>,
                           std::tuple<float>>(func, name, globalParams,
                                              moverParams);
  }
  static EffectTemplated<std::tuple<std::function<Vect2(float)>, float &>,
                  std::tuple<float>>
  createTimeVaryingForceTemplated(std::function<Vect2(float)> force_function,
                                  float &current_time,
                                  float default_scale_factor = 1.0f) {
    auto func =
        [](Mover &mover,
           std::tuple<std::function<Vect2(float)>, float &> globalParams,
           std::tuple<float> moverParams) {
          float scale_factor = std::get<0>(moverParams);
          auto force_function = std::get<0>(globalParams);
          float &current_time = std::get<1>(globalParams);
          Vect2 current_force = force_function(current_time);
          mover.apply_force(current_force * scale_factor);
        };
    std::string name = "TimeVaryingForceTemplated";
    std::tuple<std::function<Vect2(float)>, float &> globalParams =
        std::forward_as_tuple(force_function, current_time);
    std::tuple<float> moverParams =
        createMoverTimeVaryingForceParams(default_scale_factor);
    return EffectTemplated<std::tuple<std::function<Vect2(float)>, float &>,
                           std::tuple<float>>(globalParams, func, name);
  }
};
