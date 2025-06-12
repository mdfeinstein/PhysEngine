//#include "pch.h"
//#include "framework.h"
#include "Mover.h"

void MoverArgs::applyDefaults(const MoverArgs& defaultArgs) {
    // apply defaultArgs to every field that has nullopt
    if (!position) position = defaultArgs.position;
    if (!velocity) velocity = defaultArgs.velocity;
    if (!acceleration) acceleration = defaultArgs.acceleration;
    if (!radius) radius = defaultArgs.radius;
    if (!mass) mass = defaultArgs.mass;
};

bool MoverArgs::allFilled() const {
    //checks that all fields are filled out
    if (!position) return false;
    if (!velocity) return false;
    if (!acceleration) return false;
    if (!radius) return false;
    if (!mass) return false;
    //if we are here, all fields are filled
    return true;
};



Mover::Mover(MoverArgs args) {
        if (!args.allFilled()) {
            //throw error
            throw std::invalid_argument("Mover arguments not completely filled out");
        }
        position = args.position.value();
        velocity = args.velocity.value();
        accel = args.acceleration.value();
        radius = args.radius.value();
        mass = args.mass.value();
    };


std::array<Vect2, 3> Mover::next_vecs(float dt) {
    // Vect2 acc = accel;
    // Vect2 vel = velocity + accel*dt;
    // Vect2 pos = position + vel*dt;
    // lets try the kinematic equation
    Vect2 pos = position + velocity*dt + 0.5*accel*dt*dt;
    Vect2 vel = velocity + accel*dt;
    Vect2 acc = accel;
    std::array<Vect2, 3> result = {pos, vel, acc}; 
    return result;
};

void Mover::update(float dt) {
    std::array<Vect2, 3> vect_arr = this->next_vecs(dt);
    this->position = vect_arr[0];
    this->velocity = vect_arr[1];
    this->accel = vect_arr[2];

};

void Mover::apply_force(Vect2 force) { 
};


void Mover::applyCollision(float dt, Mover* mover) { 
    //this requires some more thought...
    // want to check after all forces have been applied,
    //but also need to check all pair wise itneractions.
    //may not fit neatly in the Interaction loop...
    // maybe a separate loop for collisions in the Simulation update?
    auto[thisNextPosition, thisNextVelocity, thisNextAcceleration] = this->next_vecs(dt);
    auto[thatNextPosition, thatNextVelocity, thatNextAcceleration] = mover->next_vecs(dt);
    float min_dist = this->radius + mover->radius;
    if ( (thisNextPosition - thatNextPosition).mag() < min_dist) {
        // reverse next velocities and set as current velocities
        // this collision will be applied before the acceleration kicks in
        // leave the acceleration as it is, so that the forces in a frame are not affected
        this->velocity = -1*thisNextVelocity;
        mover->velocity = -1*thatNextVelocity;
        // Vect2 offset = (thisNextPosition - thatNextPosition)*1/2;
        // this->position = thisNextPosition + offset;
        // mover->position = thatNextPosition - offset;
    }
};


NewtMover::NewtMover(MoverArgs args) : Mover(args) {if (mass == 0) throw std::invalid_argument("NewtMover mass must be non-zero");};
NewtMover::NewtMover(Mover& mover) : Mover(mover), force_sum(Vect2()) {if (mover.mass == 0) throw std::invalid_argument("NewtMover mass must be non-zero");};
NewtMover::NewtMover(NewtMover& mover) : Mover(mover) {
    if (mover.mass == 0) throw std::invalid_argument("NewtMover mass must be non-zero");
    this->force_sum = mover.force_sum.load();
};

std::array<Vect2, 3> NewtMover::next_vecs(float dt) {
    if (mass == 0) throw std::invalid_argument("NewtMover mass must be non-zero");
    Vect2 accel = force_sum.load() / mass; // as of now, next_vecs is expected to be called after all force_sum modifications are complete
    Vect2 pos = position + velocity*dt + 0.5*accel*dt*dt;
    Vect2 vel = velocity + accel*dt;
    std::array<Vect2, 3> result = {pos, vel, accel}; 
    return result;
};

void NewtMover::apply_force(Vect2 force) {
    // atomic addition of Vect2 forces
    Vect2 currentForce = force_sum.load();
    Vect2 new_force = currentForce + force;
    while (!force_sum.compare_exchange_weak(currentForce, new_force)){
        // if the current force is not equal to the new force, try again after loading the new current force
        force_sum.store(currentForce);
    }
};

void NewtMover::update(float dt) {
    std::array<Vect2, 3> vect_arr = next_vecs(dt);
    //checks on positional or velocity bounds can be performed now before the update
    // ...
    this->position = vect_arr[0];
    this->velocity = vect_arr[1];
    this->accel = vect_arr[2];
    // reset force_sum
    this->force_sum = Vect2();
} 

