#pragma once
#include "Vect2.h"
#include <cmath>
#include <array>
#include <unordered_map>
#include <vector>
#include <any>
#include <typeindex>
#include <optional>
#include <atomic>
#include <string>

struct MoverArgs {
    // id is not defined here, since it needs to be assigned by simulator
    std::optional<Vect2> position;
    std::optional<Vect2> velocity;
    std::optional<Vect2> acceleration;
    std::optional<float> radius;
    std::optional<float> mass;
    MoverArgs(
            std::optional<Vect2> pos = std::nullopt, 
            std::optional<Vect2> vel = std::nullopt, 
            std::optional<Vect2> acc = std::nullopt,
            std::optional<float> radius = std::nullopt, 
            std::optional<float> mass = std::nullopt)
            : position(pos), velocity(vel), acceleration(acc), radius(radius), mass(mass) {};

    void applyDefaults(const MoverArgs& defaultArgs);

    bool allFilled() const;
};


class Mover {
    public:
    int id;
    Vect2 position, velocity, accel;
    float radius = 1; // for now, all movers will be circular
    float mass = 0;
    std::unordered_map<std::type_index, std::vector<std::any>> interactionParams;
    std::unordered_map<std::string, std::any> effectParams; //any is tuple of arbitrary type

    virtual ~Mover()=default; 
    Mover(MoverArgs args);
    std::array<Vect2, 3> virtual next_vecs(float dt);
    void virtual update(float dt);
    void virtual apply_force(Vect2 force);
    void virtual applyCollision(float dt, Mover* mover);
};


class NewtMover : public Mover {
    public:
    std::atomic<Vect2> force_sum = Vect2();
    // Vect2 force_sum = Vect2(); //tally of forces on NewtMover. Starts at 0, and is reset at "update"
    NewtMover(MoverArgs args);
    NewtMover(Mover& mover);
    NewtMover(NewtMover& mover);
    std::array<Vect2, 3> next_vecs(float dt);
    void apply_force(Vect2 force);
    void update(float dt);
};