#include <gtest/gtest.h>
#include "MoverFactory.h"

class MoverFactoryFixture : public ::testing::Test {
  protected:
  MoverFactory factory = MoverFactory();
};

TEST_F(MoverFactoryFixture, RegisterMoverDefaultsWithNotFullArgsThrows) {
  MoverArgs notFull = MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6));
  EXPECT_THROW(
    factory.registerMoverDefaults(typeid(Mover), notFull),
    std::invalid_argument
  );
};


TEST_F(MoverFactoryFixture, CreateMoverWithDefaults) {
  MoverArgs defaultArgs(Vect2(), Vect2(), Vect2(), 10, 20);
  factory.registerMoverDefaults(typeid(Mover), defaultArgs);
  auto mover = factory.createMover(typeid(Mover));
  EXPECT_EQ(mover->position, Vect2());
  EXPECT_EQ(mover->velocity, Vect2());
  EXPECT_EQ(mover->accel, Vect2());
  EXPECT_EQ(mover->radius, 10);
  EXPECT_EQ(mover->mass, 20);
};

TEST_F(MoverFactoryFixture, CreateMoverWithSomeArgs) {
  MoverArgs defaultArgs(Vect2(), Vect2(), Vect2(), 10, 20);
  factory.registerMoverDefaults(typeid(Mover), defaultArgs);
  MoverArgs args(Vect2(1,2), Vect2(3,4), Vect2(5,6));
  auto mover = factory.createMover(typeid(Mover), args);
  EXPECT_EQ(mover->position, Vect2(1,2));
  EXPECT_EQ(mover->velocity, Vect2(3,4));
  EXPECT_EQ(mover->accel, Vect2(5,6));
  EXPECT_EQ(mover->radius, 10);
  EXPECT_EQ(mover->mass, 20);
};

TEST_F(MoverFactoryFixture, AddInteractionWithDefaults) {
  factory.registerInteraction(typeid(Interaction), std::vector<std::any>({1.0f, 2, 'a'}));
  auto mover = factory.createMover(typeid(Mover));
  float first = std::any_cast<float>(mover->interactionParams[typeid(Interaction)][0]);
  EXPECT_EQ(first, 1.0f);
  float second = std::any_cast<int>(mover->interactionParams[typeid(Interaction)][1]);
  EXPECT_EQ(second, 2);
  char third = std::any_cast<char>(mover->interactionParams[typeid(Interaction)][2]);
  EXPECT_EQ(third, 'a');
};

TEST_F(MoverFactoryFixture, MoverIdsAssignedInOrder) {
  auto mover1 = factory.createMover(typeid(Mover));
  auto mover2 = factory.createMover(typeid(Mover));
  auto mover3 = factory.createMover(typeid(Mover));
  EXPECT_EQ(mover1->id, 0);
  EXPECT_EQ(mover2->id, 1);
  EXPECT_EQ(mover3->id, 2);
};

