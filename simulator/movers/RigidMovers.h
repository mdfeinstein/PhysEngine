#pragma once
#include "Mover.h"
#include <cmath>
#include <atomic>
#include <mutex>
#include <algorithm>
#include <memory>

const double PI = 3.14159265358979323846;
//forward declarations
class RigidConnectedGroup;
class RigidConnectedMover;

class RigidConnectedMover : public NewtMover {
  //pattern: create movers, link them in group.
  // that means they need to be intialized with a nullptr to group
  friend RigidConnectedGroup;
  public:
    RigidConnectedGroup* group;
    int group_idx;
    RigidConnectedMover(MoverArgs args, RigidConnectedGroup* group = nullptr);
    // RigidConnectedMover(NewtMover& mover, RigidConnectedGroup* group = nullptr);
    RigidConnectedMover(Mover& mover, RigidConnectedGroup* group = nullptr);
    ~RigidConnectedMover();
    std::array<Vect2, 3> virtual next_vecs(float dt) override;
    void virtual update(float dt) override;
    std::unique_ptr<NewtMover> cloneToNewtMover();

};

class RigidConnectedGroup {
  friend RigidConnectedMover;
  public:
    //WARNING: if referenced mover is deleted, this group is invalid.
    //Add destructor in RigidConnectedMover that deletes group
    std::vector<RigidConnectedMover*> movers; 
    Vect2 linearPosition, linearVelocity, linearAcceleration;
    float angularPosition = 0;
    float angularVelocity = 0;
    float angularAcceleration = 0;
    Vect2 centerOfMass;
    float totalMass;
    std::vector<Vect2> moverMoments;
    float momentOfInertia;
    Vect2 forceSum = Vect2();
    float torqueSum = 0;
    RigidConnectedGroup(std::vector<RigidConnectedMover*> movers);
    ~RigidConnectedGroup();
    void update(float dt);
    std::array<Vect2, 3> next_vecs(RigidConnectedMover*, float dt);
  private:
    void computeProperties();
    void reset();
    void moverUpdated();
    bool isUpdated = false; //when to reset this? after the last mover calls update?
    std::atomic<int> NumMoversLeftToUpdate;
    std::mutex update_mutex;
    int currentGroupIdx = 0;
};


