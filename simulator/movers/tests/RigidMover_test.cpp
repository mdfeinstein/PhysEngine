#include <gtest/gtest.h>
#include "RigidMovers.h"

class RigidMoverFixture : public ::testing::Test {
  protected:
  MoverArgs args1 = MoverArgs(Vect2(), Vect2(), Vect2(), 1, 1);
  MoverArgs args2 = MoverArgs(Vect2(50,0), Vect2(), Vect2(), 1, 1);
};

TEST_F(RigidMoverFixture, ConstructionFromMoverArgs) {
  RigidConnectedMover mover1(args1);
  EXPECT_EQ(mover1.position, Vect2());
  EXPECT_EQ(mover1.velocity, Vect2());
  EXPECT_EQ(mover1.accel, Vect2());
  EXPECT_EQ(mover1.radius, 1);
  EXPECT_EQ(mover1.mass, 1);
  RigidConnectedMover mover2(args2);
  EXPECT_EQ(mover2.position, Vect2(50,0));
  EXPECT_EQ(mover2.velocity, Vect2());
  EXPECT_EQ(mover2.accel, Vect2());
  EXPECT_EQ(mover2.radius, 1);
  EXPECT_EQ(mover2.mass, 1);
};

TEST_F(RigidMoverFixture, ConstructionFromMover) {
  Mover regularMover(args1);
  RigidConnectedMover mover1(regularMover);
  EXPECT_EQ(mover1.position, Vect2());
  EXPECT_EQ(mover1.velocity, Vect2());
  EXPECT_EQ(mover1.accel, Vect2());
  EXPECT_EQ(mover1.radius, 1);
  EXPECT_EQ(mover1.mass, 1);
};

TEST_F(RigidMoverFixture, ConstructionFromNewtMover) {
  NewtMover newtMover(args1);
  RigidConnectedMover mover1(newtMover);
  EXPECT_EQ(mover1.position, Vect2());
  EXPECT_EQ(mover1.velocity, Vect2());
  EXPECT_EQ(mover1.accel, Vect2());
  EXPECT_EQ(mover1.radius, 1);
  EXPECT_EQ(mover1.mass, 1);
};

TEST_F(RigidMoverFixture, ConstructGroup) {
  RigidConnectedMover mover1(args1);
  RigidConnectedMover mover2(args2);
  RigidConnectedGroup group({&mover1, &mover2});
  EXPECT_EQ(group.movers.size(), 2);
  EXPECT_EQ(group.linearPosition, Vect2(25,0));
  EXPECT_EQ(group.linearVelocity, Vect2());
  EXPECT_EQ(group.angularVelocity, 0);
  EXPECT_EQ(group.centerOfMass, Vect2(25,0));
  EXPECT_EQ(group.totalMass, 2);
  EXPECT_EQ(group.moverMoments.size(), 2);
  EXPECT_EQ(group.momentOfInertia, 2*25*25);
};

TEST_F(RigidMoverFixture, DestroyRigidMoverInGroup) {
  //make group with three movers
  RigidConnectedMover mover1(args1);
  RigidConnectedMover mover2(args2);
  MoverArgs args3 = args2;
  args3.position = Vect2(0,50);
  RigidConnectedMover* mover3 = new RigidConnectedMover(args3);
  RigidConnectedGroup group({&mover1, &mover2, mover3});
  delete mover3;
  EXPECT_EQ(group.movers.size(), 2);
  EXPECT_EQ(group.linearPosition, Vect2(25,0));
  EXPECT_EQ(group.linearVelocity, Vect2());
  EXPECT_EQ(group.angularVelocity, 0);
  EXPECT_EQ(group.centerOfMass, Vect2(25,0));
  EXPECT_EQ(group.totalMass, 2);
  EXPECT_EQ(group.moverMoments.size(), 2);
  EXPECT_EQ(group.momentOfInertia, 2*25*25);
};

TEST_F(RigidMoverFixture, ApplyTorqueThenUpdateOnce) {
  //make group with two movers
  RigidConnectedMover mover1(args1);
  RigidConnectedMover mover2(args2);
  RigidConnectedGroup group({&mover1, &mover2});
  mover1.apply_force(Vect2(0,-2.5));
  mover2.apply_force(Vect2(0,+2.5));
  mover1.update(1);
  mover2.update(1);
  float angularAcceleration = 25.0f*5.0f/(2.0f*25.0f*25.0f);
  float angle = 0.5*angularAcceleration;
  Vect2 expectedPosition = Vect2(25,0) + Vect2(25,0).rotate(angle);
  EXPECT_NEAR(mover2.position.x, expectedPosition.x, 1e-6);
  EXPECT_NEAR(mover2.position.y, expectedPosition.y, 1e-6);
};

TEST_F(RigidMoverFixture, ApplyUnbalancedForceThenUpdateOnce) {
  //make group with two movers
  RigidConnectedMover mover1(args1);
  RigidConnectedMover mover2(args2);
  RigidConnectedGroup group({&mover1, &mover2});
  Vect2 force(0,5);
  mover2.apply_force(force);
  mover1.update(1);
  mover2.update(1);
  //calculate expected position
  float angularAcceleration = 25.0f*5.0f/(2.0f*25.0f*25.0f);
  float angle = 0.5*angularAcceleration;
  Vect2 linearShift = 0.5*force/2 + Vect2(25,0); //shift of mover2 due to linear acceleration
  Vect2 expectedPosition = linearShift + Vect2(25,0).rotate(angle); //add rotational shift
  EXPECT_NEAR(mover2.position.x, expectedPosition.x, 1e-6);
  EXPECT_NEAR(mover2.position.y, expectedPosition.y, 1e-6);
};

TEST_F(RigidMoverFixture, DestroyGroupFreesMovers) {
  //make group with two movers
  RigidConnectedMover mover1(args1);
  RigidConnectedMover mover2(args2);
  RigidConnectedGroup* group = new RigidConnectedGroup({&mover1, &mover2});
  delete group;
  EXPECT_EQ(mover1.group, nullptr);
  EXPECT_EQ(mover2.group, nullptr);
};

TEST_F(RigidMoverFixture, UngroupedMoverUpdatesLikeNewtMover) {
  RigidConnectedMover mover1(args1);
  mover1.apply_force(Vect2(0,5));
  EXPECT_NO_THROW(mover1.update(1));
  Vect2 expectedPosition = 0.5*Vect2(0,5);
  EXPECT_NEAR(mover1.position.x, expectedPosition.x, 1e-6);
  EXPECT_NEAR(mover1.position.y, expectedPosition.y, 1e-6);
};

TEST_F(RigidMoverFixture, CloneToNewtMoverAndUpdate) {
  RigidConnectedMover mover1(args1);
  mover1.apply_force(Vect2(0,5));
  std::unique_ptr<NewtMover> mover2(mover1.cloneToNewtMover());  
  mover2->update(1);
  Vect2 expectedPosition = 0.5*Vect2(0,5);
  EXPECT_NEAR(mover2->position.x, expectedPosition.x, 1e-6);
  EXPECT_NEAR(mover2->position.y, expectedPosition.y, 1e-6);
};

