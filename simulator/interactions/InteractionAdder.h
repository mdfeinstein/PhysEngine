#pragma once
#include "InteractionTemplated.h"
#include <tuple>
#include <functional>
#include <string>
#include "InteractionTypes/Gravity.h"
#include "InteractionTypes/SpringInteraction.h"
#include "InteractionTypes/SoftCollideInteraction.h"
#include "InteractionTypes/CoulombInteraction.h"
#include "../Simulator.h"

struct InteractionAdder {
  // Helper class to add predefined interactions to simulator.
  // To add a new interaction, need to implement the following:
  // 
  // createMover[Interaction]Params(args...)
  // add[Interaction]ToMover(Mover& mover)
  // add[Interaction](args..., Simulator* sim = nullptr)
  // 
  // 

    Simulator* sim;
    InteractionAdder(Simulator* sim = nullptr) : sim(sim) {}

    // Gravity
    std::tuple<> createMoverGravityParams() {
        return std::make_tuple();
    }
    void addGravityToMover(Mover& mover) {
        mover.effectParams["Gravity"] = createMoverGravityParams();
    }
    void addGravity(float G = 0.0001f, float min_distance = 1e-3f, Simulator* sim = nullptr) {
        if (sim == nullptr)
            if (this->sim == nullptr)
                throw std::invalid_argument("InteractionAdder::addGravity: simulator not provided");
            else sim = this->sim;
        auto func = std::function<void(Mover&, Mover&, std::tuple<float, float>, std::tuple<>, std::tuple<>)>(
            [](Mover& m1, Mover& m2, std::tuple<float, float> global, std::tuple<>, std::tuple<>) {
                auto [G, min_dist] = global;
                Vect2 r = m1.position - m2.position;
                float magnitude = r.mag();
                if (magnitude < min_dist) magnitude = min_dist;
                Vect2 force = -G * m1.mass * m2.mass * r * 1.0f / pow(magnitude, 3);
                m1.apply_force(force);
                m2.apply_force(-1 * force);
            });
        std::string name = "Gravity";
        std::tuple<float, float> globalParams = std::make_tuple(G, min_distance);
        std::tuple<> moverParams = createMoverGravityParams();
        sim->add_interactionWrapper<std::tuple<float, float>, std::tuple<>>(
            func, name, globalParams, moverParams);
    }

    // Spring
    std::tuple<> createMoverSpringParams() {
        return std::make_tuple();
    }
    void addSpringToMover(Mover& mover) {
        mover.effectParams["Spring"] = createMoverSpringParams();
    }
    void addSpring(float k = 1.0f, float x0 = 5.0f, Simulator* sim = nullptr) {
        if (sim == nullptr)
            if (this->sim == nullptr)
                throw std::invalid_argument("InteractionAdder::addSpring: simulator not provided");
            else sim = this->sim;
        auto func = std::function<void(Mover&, Mover&, std::tuple<float, float>, std::tuple<>, std::tuple<>)>(
            [](Mover& m1, Mover& m2, std::tuple<float, float> global, std::tuple<>, std::tuple<>) {
                auto [k, x0] = global;
                Vect2 r = m1.position - m2.position;
                float dist = r.mag();
                float forceMag = -k * (dist - x0);
                Vect2 force = forceMag * r / dist;
                m1.apply_force(force);
                m2.apply_force(-1 * force);
            });
        std::string name = "Spring";
        std::tuple<float, float> globalParams = std::make_tuple(k, x0);
        std::tuple<> moverParams = createMoverSpringParams();
        sim->add_interactionWrapper<std::tuple<float, float>, std::tuple<>>(
            func, name, globalParams, moverParams);
    }

    // SoftCollide
    std::tuple<float, float> createMoverSoftCollideParams(float springStrength = 1.0f, float repulsionStrength = 1.0f) {
        return std::make_tuple(springStrength, repulsionStrength);
    }
    void addSoftCollideToMover(float springStrength, float repulsionStrength, Mover& mover) {
        mover.effectParams["SoftCollide"] = createMoverSoftCollideParams(springStrength, repulsionStrength);
    }
    void addSoftCollide(float globalSpringStrength = 1.0f, float globalRepulsionStrength = 1.0f, float min_distance = 1e-3f, float defaultSpringStrength = 1.0f, float defaultRepulsionStrength = 1.0f, Simulator* sim = nullptr) {
        if (sim == nullptr)
            if (this->sim == nullptr)
                throw std::invalid_argument("InteractionAdder::addSoftCollide: simulator not provided");
            else sim = this->sim;
        auto func = std::function<void(Mover&, Mover&, std::tuple<float, float, float>, std::tuple<float, float>, std::tuple<float, float>)>(
            [](Mover& m1, Mover& m2, std::tuple<float, float, float> global, std::tuple<float, float> mover1, std::tuple<float, float> mover2) {
                auto [globalSpring, globalRepulsion, min_dist] = global;
                auto [spring1, repulse1] = mover1;
                auto [spring2, repulse2] = mover2;
                Vect2 r = m1.position - m2.position;
                float magnitude = r.mag();
                magnitude = std::max(magnitude, min_dist);
                float activation_distance = m1.radius + m2.radius;
                if (magnitude < activation_distance) {
                    float springMag = globalSpring * spring1 * spring2 * fabs(magnitude - activation_distance);
                    float repulsionMag = globalRepulsion * repulse1 * repulse2 * (1.0f / pow(magnitude, 2));
                    Vect2 force = (springMag + repulsionMag) * r / magnitude;
                    m1.apply_force(force);
                    m2.apply_force(-1 * force);
                }
            });
        std::string name = "SoftCollide";
        std::tuple<float, float, float> globalParams = std::make_tuple(globalSpringStrength, globalRepulsionStrength, min_distance);
        std::tuple<float, float> moverParams = createMoverSoftCollideParams(defaultSpringStrength, defaultRepulsionStrength);
        sim->add_interactionWrapper<std::tuple<float, float, float>, std::tuple<float, float>>(
            func, name, globalParams, moverParams);
    }

    // Coulomb
    std::tuple<float> createMoverCoulombParams(float charge = 1.0f) {
        return std::make_tuple(charge);
    }
    void addCoulombToMover(float charge, Mover& mover) {
        mover.effectParams["Coulomb"] = createMoverCoulombParams(charge);
    }
    void addCoulomb(float K = 1.0f, float min_distance = 1e-3f, float defaultCharge = 1.0f, Simulator* sim = nullptr) {
        if (sim == nullptr)
            if (this->sim == nullptr)
                throw std::invalid_argument("InteractionAdder::addCoulomb: simulator not provided");
            else sim = this->sim;
        auto func = std::function<void(Mover&, Mover&, std::tuple<float, float>, std::tuple<float>, std::tuple<float>)>(
            [](Mover& m1, Mover& m2, std::tuple<float, float> global, std::tuple<float> mover1, std::tuple<float> mover2) {
                auto [K, min_dist] = global;
                auto [q1] = mover1;
                auto [q2] = mover2;
                Vect2 r = m1.position - m2.position;
                float magnitude = r.mag();
                if (magnitude < min_dist) magnitude = min_dist;
                Vect2 force = K * q1 * q2 * r / pow(magnitude, 3);
                m1.apply_force(force);
                m2.apply_force(-1 * force);
            });
        std::string name = "Coulomb";
        std::tuple<float, float> globalParams = std::make_tuple(K, min_distance);
        std::tuple<float> moverParams = createMoverCoulombParams(defaultCharge);
        sim->add_interactionWrapper<std::tuple<float, float>, std::tuple<float>>(
            func, name, globalParams, moverParams);
    }
};
