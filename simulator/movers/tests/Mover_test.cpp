#include <gtest/gtest.h>
#include "Mover.h"

class MoverArgsFixture : public ::testing::Test {
  protected:
  MoverArgs defaultArgs = MoverArgs(Vect2(), Vect2(), Vect2(), 1, 1);
  MoverArgs full = MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6), 20, 20);
  MoverArgs empty = MoverArgs();
  MoverArgs partial1 = MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6));
  MoverArgs partial2 = MoverArgs();

  void SetUp() {
    // Code here will be called immediately after the constructor (right before each test).
    partial2.radius = 20;
    partial2.mass = 20;
  }

};

TEST_F(MoverArgsFixture, Construction) {
  EXPECT_EQ(full.position, Vect2(1,2));
  EXPECT_EQ(full.velocity, Vect2(3,4));
  EXPECT_EQ(full.acceleration, Vect2(5,6));
  EXPECT_EQ(full.radius, 20);
  EXPECT_EQ(full.mass, 20);
}

TEST_F(MoverArgsFixture, AllFilledOnFull) {
  EXPECT_TRUE(full.allFilled());
}

TEST_F(MoverArgsFixture, AllFilledOnEmpty) {
  EXPECT_FALSE(empty.allFilled());
}

TEST_F(MoverArgsFixture, AllFilledOnPartial1) {
  EXPECT_FALSE(partial1.allFilled());
}

TEST_F(MoverArgsFixture, AllFilledOnPartial2) {
  EXPECT_FALSE(partial2.allFilled());
}

TEST_F(MoverArgsFixture, AllFilledOnDefault) {
  EXPECT_TRUE(defaultArgs.allFilled());
}

TEST_F(MoverArgsFixture, AllFilledAfterApplyDefauts) {
  empty.applyDefaults(defaultArgs);
  EXPECT_TRUE(empty.allFilled());
  partial1.applyDefaults(defaultArgs);
  EXPECT_TRUE(partial1.allFilled());
  partial2.applyDefaults(defaultArgs);
  EXPECT_TRUE(partial2.allFilled());
}

TEST_F(MoverArgsFixture, ApplyDefaultsOnFull) {
  MoverArgs fullCopy = full;
  fullCopy.applyDefaults(defaultArgs);
  EXPECT_EQ(fullCopy.position, full.position);
  EXPECT_EQ(fullCopy.velocity, full.velocity);
  EXPECT_EQ(fullCopy.acceleration, full.acceleration);
  EXPECT_EQ(fullCopy.radius, full.radius);
  EXPECT_EQ(fullCopy.mass, full.mass);
}

TEST_F(MoverArgsFixture, ApplyDefaultsOnEmpty) {
  empty.applyDefaults(defaultArgs);
  EXPECT_EQ(empty.position, defaultArgs.position);
  EXPECT_EQ(empty.velocity, defaultArgs.velocity);
  EXPECT_EQ(empty.acceleration, defaultArgs.acceleration);
  EXPECT_EQ(empty.radius, defaultArgs.radius);
  EXPECT_EQ(empty.mass, defaultArgs.mass);
}

TEST_F(MoverArgsFixture, ApplyDefaultsOnPartial1) {
  MoverArgs partial1Copy = partial1;
  partial1Copy.applyDefaults(defaultArgs);
  EXPECT_EQ(partial1Copy.position, partial1.position);
  EXPECT_EQ(partial1Copy.velocity, partial1.velocity);
  EXPECT_EQ(partial1Copy.acceleration, partial1.acceleration);
  EXPECT_EQ(partial1Copy.radius, defaultArgs.radius);
  EXPECT_EQ(partial1Copy.mass, defaultArgs.mass);
}

TEST_F(MoverArgsFixture, ApplyDefaultsOnPartial2) {
  MoverArgs partial2Copy = partial2;
  partial2Copy.applyDefaults(defaultArgs);
  EXPECT_EQ(partial2Copy.position, defaultArgs.position);
  EXPECT_EQ(partial2Copy.velocity, defaultArgs.velocity);
  EXPECT_EQ(partial2Copy.acceleration, defaultArgs.acceleration);
  EXPECT_EQ(partial2Copy.radius, partial2.radius);
  EXPECT_EQ(partial2Copy.mass, partial2.mass);
}


class MoverFixture : public ::testing::Test {
  public:
    MoverFixture() : mover(full) {};
  protected:
  MoverArgs defaultArgs = MoverArgs(Vect2(), Vect2(), Vect2(), 1, 1);
  MoverArgs full = MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6), 20, 20);
  MoverArgs empty = MoverArgs();
  MoverArgs partial1 = MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6));
  MoverArgs partial2 = MoverArgs();

  Mover mover;
  MoverArgs moverArgs;
};

TEST_F(MoverFixture, ConstructionWithFullArgs) {
  EXPECT_EQ(mover.position, Vect2(1,2));
  EXPECT_EQ(mover.velocity, Vect2(3,4));
  EXPECT_EQ(mover.accel, Vect2(5,6));
  EXPECT_EQ(mover.radius, 20);
  EXPECT_EQ(mover.mass, 20);
}

TEST_F(MoverFixture, ConstructionWithEmptyArgsThrows) {
  moverArgs = empty;
  EXPECT_THROW(mover = Mover(moverArgs), std::invalid_argument);
};

TEST_F(MoverFixture, ConstructionWithPartial1ArgsThrows) {
  moverArgs = partial1;
  EXPECT_THROW(mover = Mover(moverArgs), std::invalid_argument);
};

TEST_F(MoverFixture, ConstructionWithPartial2ArgsThrows) {
  moverArgs = partial2;
  EXPECT_THROW(mover = Mover(moverArgs), std::invalid_argument);  
};

TEST_F(MoverFixture, NextVecs) {
  auto [pos,vel,acc] = mover.next_vecs(1);
  EXPECT_NEAR(pos.x, Vect2(6.5,9).x, 1e-6);
  EXPECT_NEAR(pos.y, Vect2(6.5,9).y, 1e-6);
  EXPECT_NEAR(vel.x, Vect2(8,10).x, 1e-6);
  EXPECT_NEAR(vel.y, Vect2(8,10).y, 1e-6);
  EXPECT_NEAR(acc.x, Vect2(5,6).x, 1e-6);
  EXPECT_NEAR(acc.y, Vect2(5,6).y, 1e-6);
};

TEST_F(MoverFixture, Update) {
  //compare against 1/2*a*t^2 + v0*t + x0 kinematic equation
  for (int i = 0; i < 100; i++) {
    mover.update(0.01);
  }
  Vect2 pos = mover.position;
  Vect2 vel = mover.velocity;
  Vect2 acc = mover.accel;
  // the simulation is coarse so will not follow result well.
  EXPECT_NEAR(pos.x, Vect2(6.5,9).x, 1e-3);
  EXPECT_NEAR(pos.y, Vect2(6.5,9).y, 1e-3);
  EXPECT_NEAR(vel.x, Vect2(8,10).x, 1e-3);
  EXPECT_NEAR(vel.y, Vect2(8,10).y, 1e-3);
  EXPECT_NEAR(acc.x, Vect2(5,6).x, 1e-3);
  EXPECT_NEAR(acc.y, Vect2(5,6).y, 1e-3);
};

class NewtMoverFixture : public ::testing::Test {
  public:
    NewtMoverFixture() : mover(full) {};
  protected:
  MoverArgs full = MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6), 20, 20);
  MoverArgs empty = MoverArgs();
  MoverArgs partial1 = MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6));
  MoverArgs partial2 = MoverArgs();

  NewtMover mover;
};

TEST_F(NewtMoverFixture, ConstructionFromMoverArgs) {
  EXPECT_NO_THROW(NewtMover fullMover(full));
  EXPECT_THROW(NewtMover emptyMover(empty), std::invalid_argument);
  EXPECT_THROW(NewtMover partial1Mover(partial1), std::invalid_argument);
  EXPECT_THROW(NewtMover partial2Mover(partial2), std::invalid_argument);
  full.mass = 0;
  EXPECT_THROW(NewtMover fullMover(full), std::invalid_argument);
};

TEST_F(NewtMoverFixture, ConstructionFromMover) {
  Mover regularMover(full);
  EXPECT_NO_THROW(NewtMover fullMover(regularMover));
  EXPECT_NO_THROW(NewtMover fullMover(mover)); //copy from NewtMover
};

TEST_F(NewtMoverFixture, ConstructionWithNullMassThrows) {
  full.mass = 0;
  EXPECT_THROW(NewtMover mover(full), std::invalid_argument);
  Mover regularMover(full); //allowed for regular mover
  EXPECT_THROW(NewtMover mover(regularMover), std::invalid_argument); //but problem for copy
};

TEST_F(NewtMoverFixture, ApplyForce) {
  mover.apply_force(Vect2(1,1));
  mover.apply_force(Vect2(10,5));
  EXPECT_NEAR(mover.force_sum.load().x, Vect2(11,6).x, 1e-6);
  EXPECT_NEAR(mover.force_sum.load().y, Vect2(11,6).y, 1e-6);
};

TEST_F(NewtMoverFixture, NextVecs) { 
  Vect2 force(100, 120);
  mover.apply_force(force);
  auto [pos,vel,acc] = mover.next_vecs(1);
  EXPECT_NEAR(pos.x, Vect2(6.5,9).x, 1e-6);
  EXPECT_NEAR(pos.y, Vect2(6.5,9).y, 1e-6);
  EXPECT_NEAR(vel.x, Vect2(8,10).x, 1e-6);
  EXPECT_NEAR(vel.y, Vect2(8,10).y, 1e-6);
  EXPECT_NEAR(acc.x, Vect2(5,6).x, 1e-6);
  EXPECT_NEAR(acc.y, Vect2(5,6).y, 1e-6);
};

TEST_F(NewtMoverFixture, UpdateConstantForce) {
  Vect2 force(100, 120);
  for (int i = 0; i < 100; i++) {
    mover.apply_force(force);
    mover.update(0.01);
  }
  Vect2 pos = mover.position;
  Vect2 vel = mover.velocity;
  Vect2 acc = mover.accel;
  // the simulation is coarse so will not follow result well.
  EXPECT_NEAR(pos.x, Vect2(6.5,9).x, 1e-3);
  EXPECT_NEAR(pos.y, Vect2(6.5,9).y, 1e-3);
};

TEST_F(MoverFixture, UpdateSinForce) {
  const float pi = 3.14159265358979323846;
  auto sinForce = [pi](float t, float freq, float amp) 
  -> float { return amp*sin(2*pi*freq*t); };
  float freq = 1/5; //five cycles
  float amp = 1;
  float t = 0;
  float dt = 0.01;
  NewtMover sinMover( MoverArgs(Vect2(0,0), Vect2(0,0), Vect2(0,0), 1, 1));
  for (int i = 0; i < 100; i++) {
    sinMover.apply_force(Vect2(sinForce(t, freq, amp), 0));
    sinMover.update(dt);
    t += dt;
    //check position at different moments
    if (i % 15 == 0) {
      Vect2 pos = sinMover.position;
      //compare to double derivative of sine function
      float expectedPosX = -1*pow((2*pi*freq),2)*sinForce(t, freq, amp);
      EXPECT_NEAR(pos.x, expectedPosX, 1e-3);
    }
  }
}


  