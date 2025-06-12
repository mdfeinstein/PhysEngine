#include "RigidMovers.h"

RigidConnectedMover::RigidConnectedMover(MoverArgs args, RigidConnectedGroup* group) : NewtMover(args), group(group) {
  if (group != nullptr) {
    group->movers.push_back(this);
    group->computeProperties();
  }
}


RigidConnectedMover::RigidConnectedMover(Mover& mover, RigidConnectedGroup* group) : NewtMover(mover), group(group) {
  if (group != nullptr) {
    group->movers.push_back(this);
    group->computeProperties();
  }
}

RigidConnectedMover::~RigidConnectedMover() {
  if (group != nullptr) {
    //erase_if equals to this
    group->movers.erase(std::remove_if(group->movers.begin(), group->movers.end(), 
    [this](RigidConnectedMover* mover) { return mover == this; }), 
    group->movers.end());
    // group->movers.erase(group->movers.begin() + group_idx);
    group->computeProperties();
  }
};

std::array<Vect2, 3> RigidConnectedMover::next_vecs(float dt) {
  if (group == nullptr) NewtMover::next_vecs(dt);
  return group->next_vecs(this, dt);
};

void RigidConnectedMover::update(float dt) {
  std::array<Vect2, 3> result;
  if (group==nullptr) result = NewtMover::next_vecs(dt);
  else if (group->movers.size()== 1) { //if one mover in group, don't treat as group.
    result = NewtMover::next_vecs(dt);
  } 
  else {
    group->update(dt);
    result = next_vecs(dt);
  }
  auto [pos, vel, acc] = result;
  position = pos;
  velocity = vel;
  accel = acc;
  force_sum = Vect2();
  if (group!=nullptr) group->moverUpdated(); //mover checks in and group resets if needed
};

std::unique_ptr<NewtMover> RigidConnectedMover::cloneToNewtMover() {
  return std::make_unique<NewtMover>(*this);
  // return std::unique_ptr<NewtMover>(new NewtMover(*this));
}



RigidConnectedGroup::RigidConnectedGroup(std::vector<RigidConnectedMover*> movers) : movers(movers) {
  for (auto& mover : movers) {
    mover->group = this;
  }
    computeProperties();
};

RigidConnectedGroup::~RigidConnectedGroup() {
  for (auto mover : movers) {
    mover->group = nullptr;
  }
}

void RigidConnectedGroup::computeProperties() {
  //calculates center of mass, moverMoments, moment of inertia, and total mass
  totalMass = 0;
  momentOfInertia = 0;
  centerOfMass = Vect2();
  currentGroupIdx = 0;
  for (auto& mover : movers) {
    totalMass += mover->mass;
    centerOfMass = centerOfMass + mover->mass * mover->position;
    mover->group_idx = currentGroupIdx; //this index setting is happening in too many places. prune later.
    currentGroupIdx++;
  }
  centerOfMass = centerOfMass / totalMass;
  // now that we have COM, calculate moments and moment of inertia
  moverMoments.clear();
  for (auto& mover : movers) {
    Vect2 r = mover->position - centerOfMass;
    moverMoments.push_back(r);
    momentOfInertia += mover->mass * r.mag()*r.mag(); //sum on M*r^2
  }
  linearPosition = centerOfMass;
  //compute linear and angular velocties
  Vect2 linearMomentum = Vect2();
  float angularMomentum = 0;
  for (int i = 0; i < movers.size(); i++) {
    linearMomentum = linearMomentum + movers[i]->velocity*movers[i]->mass;
    angularMomentum = angularMomentum + movers[i]->mass*moverMoments[i].cross(movers[i]->velocity);
  }
  linearVelocity = linearMomentum / totalMass;
  angularVelocity = angularMomentum / momentOfInertia;
  angularPosition = 0; //reset angular position or objects may be rotated
  NumMoversLeftToUpdate = movers.size();
};

void RigidConnectedGroup::update(float dt) {
  // if two members of group are updating on different threads, whichever gets there first
  // will do the update while the other thread is locked out. 
  // after first succeeds in updating, isUpdated will change and lock will be released
  // subsequent threads will then do the isUpdated check and move on
  // one problem is that all subsequent threads will be locked into waiting to check isUpdated
  // but assuming small group sizes, this should be minimal downtime.
  std::lock_guard<std::mutex> lock(update_mutex);
  if (isUpdated) return;
  //reset force sum and torque sum
  forceSum = Vect2();
  torqueSum = 0;
  //add forces and torques from each mover
  for (int i = 0; i < currentGroupIdx; i++) {
    auto& mover = movers[i];
    forceSum = forceSum + mover->force_sum;
    torqueSum += moverMoments[i].cross(mover->force_sum);
  }
  //calculate linear acceleration, veloctity, and position
  linearAcceleration = forceSum / totalMass;
  linearPosition = linearPosition + linearVelocity*dt + 0.5*linearAcceleration*dt*dt;
  linearVelocity = linearVelocity + linearAcceleration*dt;
  //calculate angular acceleration, velocity, and position
  angularAcceleration = torqueSum / momentOfInertia; //if group is down to one element, MOI would be zero, but no actual rotation occurs.
  angularPosition = angularPosition + angularVelocity*dt + 0.5*angularAcceleration*dt*dt;
  angularVelocity = angularVelocity + angularAcceleration*dt;
  //set switch to true
  isUpdated = true;
};

std::array<Vect2, 3> RigidConnectedGroup::next_vecs(RigidConnectedMover* mover, float dt) {
  //return next_vecs by using linear and angular information, along with the mover's moment
  //next position and velocity
  Vect2 moment = moverMoments[mover->group_idx];
  moment = moment.rotate(angularPosition);

  Vect2 nextPosition = moment + linearPosition;
  Vect2 nextVelocity = linearVelocity + angularVelocity * moment.rotate(PI/2);
  Vect2 nextAcceleration = linearAcceleration + angularAcceleration * moment;
  return {nextPosition, nextVelocity, nextAcceleration};
};

void RigidConnectedGroup::reset() {
    // resets data for next update
    forceSum = Vect2();
    torqueSum = 0;
    isUpdated = false;
    NumMoversLeftToUpdate = movers.size();
};

void RigidConnectedGroup::moverUpdated() {
  //atomically decrement NumMoversLeftToUpdate
  NumMoversLeftToUpdate--;
  if (NumMoversLeftToUpdate.load() == 0) {
    this->reset();
  }
}