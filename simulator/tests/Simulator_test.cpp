#include <gtest/gtest.h>
#include "Simulator.h"
#include "CoulombInteraction.h"
#include "Drag.h"
#include "Gravity.h"
#include "Attractor.h"
#include "SpringInteraction.h"
#include "ConstantAcceleration.h"
#include "EffectAdder.h"

class SimulatorFixture : public ::testing::Test {
  protected:
  Simulator sim = Simulator(0.01);
};

TEST_F(SimulatorFixture, AddMoverNoArgs) { //should follow default of factory
  EXPECT_NO_THROW(sim.add_mover(typeid(Mover)));
  EXPECT_EQ(sim.movers.size(), 1);
  EXPECT_EQ(sim.movers[0]->id, 0);
  EXPECT_NO_THROW(sim.add_mover(typeid(NewtMover)));
  EXPECT_EQ(sim.movers.size(), 2);
  EXPECT_EQ(sim.movers[1]->id, 1);
};

TEST_F(SimulatorFixture, AddUnknownMoverTypeThrows) {
  EXPECT_THROW(sim.add_mover(typeid(int)), std::invalid_argument);
};

TEST_F(SimulatorFixture, AddMoverWithArgs) {
  MoverArgs args = MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6), 20, 20);
  sim.add_mover(typeid(Mover), args);
  EXPECT_EQ(sim.movers[0]->position, Vect2(1,2));
  EXPECT_EQ(sim.movers[0]->velocity, Vect2(3,4));
  EXPECT_EQ(sim.movers[0]->accel, Vect2(5,6));
  EXPECT_EQ(sim.movers[0]->radius, 20);
  EXPECT_EQ(sim.movers[0]->mass, 20);
};

TEST_F(SimulatorFixture, AddThenRemoveMover) {
  int id = sim.add_mover(typeid(Mover));
  EXPECT_TRUE(sim.remove_mover(id));
  EXPECT_EQ(sim.movers.size(), 0);
};

TEST_F(SimulatorFixture, AddThreeMoversRemoveFirstThenUpdate) {
  int id_first = sim.add_mover(typeid(Mover));
  sim.add_mover(typeid(Mover));
  sim.add_mover(typeid(Mover));
  EXPECT_TRUE(sim.remove_mover(id_first));
  EXPECT_EQ(sim.movers.size(), 2);
  EXPECT_EQ(sim.movers[0]->id, 1);
  EXPECT_EQ(sim.movers[1]->id, 2);
  EXPECT_NO_THROW(sim.update(1));
};

TEST_F(SimulatorFixture, RemoveNonExistantMoverReturnsFalse) {
  sim.add_mover(typeid(Mover));
  sim.add_mover(typeid(Mover));
  bool result = sim.remove_mover(3);
  EXPECT_FALSE(result);
};

TEST_F(SimulatorFixture, RemoveMoversSomeExist) {
  sim.add_mover(typeid(Mover));
  sim.add_mover(typeid(Mover));
  sim.add_mover(typeid(Mover));
  std::vector<int> ids = {0,2,4};
  auto result = sim.remove_movers(ids);
  EXPECT_TRUE(result);
  EXPECT_EQ(sim.movers.size(), 1);
  EXPECT_EQ(sim.movers[0]->id, 1);
};

TEST_F(SimulatorFixture, RemoveMoversNoneExist) {
  sim.add_mover(typeid(Mover));
  sim.add_mover(typeid(Mover));
  std::vector<int> ids = {5,8,10};
  auto result = sim.remove_movers(ids);
  EXPECT_FALSE(result);
  EXPECT_EQ(sim.movers.size(), 2);
};

TEST_F(SimulatorFixture, ReplaceMoverRawPtrThenUpdate) {
  sim.add_mover(typeid(Mover));
  sim.add_mover(typeid(Mover));
  MoverArgs args = MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6), 20, 20);
  Mover* mover = new NewtMover(args);
  sim.replace_mover(0, mover);
  EXPECT_EQ(sim.movers.size(), 2);
  EXPECT_EQ(sim.movers[0]->id, 0);
  EXPECT_EQ(sim.movers[0]->position, Vect2(1,2));
  EXPECT_EQ(sim.movers[0]->velocity, Vect2(3,4));
  EXPECT_EQ(sim.movers[0]->accel, Vect2(5,6));
  EXPECT_EQ(sim.movers[0]->radius, 20);
  EXPECT_EQ(sim.movers[0]->mass, 20);
  EXPECT_NO_THROW(sim.update(1));
};

TEST_F(SimulatorFixture, ReplaceMoverUniquePtrThenUpdate) {
  sim.add_mover(typeid(Mover));
  sim.add_mover(typeid(Mover));
  MoverArgs args = MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6), 20, 20);
  std::unique_ptr<Mover> mover = std::make_unique<NewtMover>(args);
  sim.replace_mover(0, std::move(mover));
  EXPECT_EQ(sim.movers.size(), 2);
  EXPECT_EQ(sim.movers[0]->id, 0);
  EXPECT_NO_THROW(sim.update(1));
};

TEST_F(SimulatorFixture, CreateInteractionRightDefaultArgsCreateMoverCheckDefaultArgsAndUpdate) {
  Interaction* interaction = new Coulomb(1.0);
  EXPECT_NO_THROW(sim.add_interaction(interaction, {1.0f}));
  EXPECT_EQ(sim.interactions.size(), 1);
  sim.add_mover(typeid(NewtMover));
  //check default interactionParams
  float moverInteractionParam = std::any_cast<float>(sim.movers[0]->interactionParams[typeid(Coulomb)][0]);
  EXPECT_EQ(moverInteractionParam, 1.0f);
  sim.add_mover(typeid(NewtMover), MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6), 20, 20), {{typeid(Coulomb), {10.0f}}});
  moverInteractionParam = std::any_cast<float>(sim.movers[1]->interactionParams[typeid(Coulomb)][0]);
  EXPECT_EQ(moverInteractionParam, 10.0f);
  EXPECT_NO_THROW(sim.update(1));
};

TEST_F(SimulatorFixture, CreateInteractionWrongDefaultArgsThrows) {
  Interaction* interaction = new Coulomb(1.0);
  EXPECT_THROW(sim.add_interaction(interaction, {1.0f, 2.0f}), std::invalid_argument);
};

TEST_F(SimulatorFixture, CreateEffectRightDefaultArgsCreateMoverCheckDefaultArgsAndUpdate) {
  Effect* effect = new Drag(1.0);
  EXPECT_NO_THROW(sim.add_effect(effect, {1.0f}));
  EXPECT_EQ(sim.effects.size(), 1);
  sim.add_mover(typeid(NewtMover));
  //check default interactionParams
  float moverEffectParam = std::any_cast<float>(sim.movers[0]->interactionParams[typeid(Drag)][0]);
  EXPECT_EQ(moverEffectParam, 1.0f);
  sim.add_mover(typeid(NewtMover), MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6), 20, 20), {{typeid(Drag), {5.0f}}});
  moverEffectParam = std::any_cast<float>(sim.movers[1]->interactionParams[typeid(Drag)][0]);
  EXPECT_EQ(moverEffectParam, 5.0f);
  EXPECT_NO_THROW(sim.update(1));
};

TEST_F(SimulatorFixture, CreateEffectWrongDefaultArgsThrows) {
  Effect* effect = new Drag(1.0);
  EXPECT_THROW(sim.add_effect(effect, {1.0f, 2.0f}), std::invalid_argument);
};

// Group tests
TEST_F(SimulatorFixture, CreateGroupWithValidMoversAndUpdate) {
  int id1 = sim.add_mover(typeid(NewtMover));
  int id2 = sim.add_mover(typeid(NewtMover));
  int id3 = sim.add_mover(typeid(NewtMover));
  
  std::vector<int> mover_ids = {id1, id2, id3};
  EXPECT_NO_THROW(sim.create_group(mover_ids));
  EXPECT_EQ(sim.groups.size(), 1);
  EXPECT_EQ(sim.groups[0]->movers.size(), 3);
  EXPECT_NO_THROW(sim.update(1));
}

TEST_F(SimulatorFixture, CreateGroupWithSomeInvalidIDs) {
  int id1 = sim.add_mover(typeid(NewtMover));
  int id2 = sim.add_mover(typeid(NewtMover));
  
  std::vector<int> mover_ids = {id1, id2, 999}; // 999 doesn't exist
  EXPECT_NO_THROW(sim.create_group(mover_ids));
  EXPECT_EQ(sim.groups.size(), 1);
  // Only the valid movers should be in the group
  EXPECT_EQ(sim.groups[0]->movers.size(), 2);
  EXPECT_NO_THROW(sim.update(1));
}

TEST_F(SimulatorFixture, CreateGroupWithRepeatedIDs) {
  int id1 = sim.add_mover(typeid(NewtMover));
  int id2 = sim.add_mover(typeid(NewtMover));
  
  std::vector<int> mover_ids = {id1, id2, id1}; // id1 repeated
  EXPECT_NO_THROW(sim.create_group(mover_ids));
  EXPECT_EQ(sim.groups.size(), 1);
  // Repeated movers should only be added once
  EXPECT_EQ(sim.groups[0]->movers.size(), 2);
  EXPECT_NO_THROW(sim.update(1));
}

TEST_F(SimulatorFixture, UngroupAndUpdate) {
  int id1 = sim.add_mover(typeid(NewtMover));
  int id2 = sim.add_mover(typeid(NewtMover));
  
  std::vector<int> mover_ids = {id1, id2};
  sim.create_group(mover_ids);
  EXPECT_EQ(sim.groups.size(), 1);
  
  RigidConnectedGroup* group = sim.groups[0].get();
  EXPECT_NO_THROW(sim.ungroup(group));
  EXPECT_EQ(sim.groups.size(), 0);
  EXPECT_EQ(sim.movers.size(), 2);
  EXPECT_NO_THROW(sim.update(1));
}

TEST_F(SimulatorFixture, DeleteGroupAndUpdate) {
  int id1 = sim.add_mover(typeid(NewtMover));
  int id2 = sim.add_mover(typeid(NewtMover));
  
  std::vector<int> mover_ids = {id1, id2};
  sim.create_group(mover_ids);
  EXPECT_EQ(sim.groups.size(), 1);
  
  RigidConnectedGroup* group = sim.groups[0].get();
  EXPECT_NO_THROW(sim.delete_group(group));
  EXPECT_EQ(sim.groups.size(), 0);
  EXPECT_EQ(sim.movers.size(), 0); // All movers should be deleted
  EXPECT_NO_THROW(sim.update(1));
}

// Memory management test
class DeletionSpy : public NewtMover {
public:
  static int instanceCount;
  
  DeletionSpy(MoverArgs args) : NewtMover(args) {
    instanceCount++;
  }
  
  ~DeletionSpy() override {
    instanceCount--;
  }
};

int DeletionSpy::instanceCount = 0;

TEST_F(SimulatorFixture, OriginalMoverIsFreedOnGroupCreation) {
  DeletionSpy::instanceCount = 0;
  
  // Add a regular mover to get ID 0
  sim.add_mover(typeid(NewtMover));
  
  // Replace it with our spy mover
  Mover* spyMover = new DeletionSpy(MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6), 20, 20));
  sim.replace_mover(0, spyMover);
  EXPECT_EQ(DeletionSpy::instanceCount, 1);
  
  // Create another mover
  int id2 = sim.add_mover(typeid(NewtMover));
  
  // Create group with both movers
  std::vector<int> mover_ids = {0, id2};
  sim.create_group(mover_ids);
  
  // The original DeletionSpy should be destroyed during group creation
  EXPECT_EQ(DeletionSpy::instanceCount, 0);
  EXPECT_EQ(sim.groups.size(), 1);
  EXPECT_NO_THROW(sim.update(1));
}

// Tests for find_mover function
TEST_F(SimulatorFixture, FindMoverExistingId) {
  int id = sim.add_mover(typeid(NewtMover));
  auto it = sim.find_mover(id);
  EXPECT_NE(it, sim.movers.end());
  EXPECT_EQ((*it)->id, id);
}

TEST_F(SimulatorFixture, FindMoverNonExistingId) {
  sim.add_mover(typeid(NewtMover));
  auto it = sim.find_mover(999); // ID that doesn't exist
  EXPECT_EQ(it, sim.movers.end());
}

TEST_F(SimulatorFixture, FindMoverVariousPositions) {
  // Test finding movers at different positions in the vector
  int id1 = sim.add_mover(typeid(NewtMover));
  int id2 = sim.add_mover(typeid(NewtMover));
  int id3 = sim.add_mover(typeid(NewtMover));
  
  // First mover
  auto it1 = sim.find_mover(id1);
  EXPECT_NE(it1, sim.movers.end());
  EXPECT_EQ((*it1)->id, id1);
  
  // Middle mover
  auto it2 = sim.find_mover(id2);
  EXPECT_NE(it2, sim.movers.end());
  EXPECT_EQ((*it2)->id, id2);
  
  // Last mover
  auto it3 = sim.find_mover(id3);
  EXPECT_NE(it3, sim.movers.end());
  EXPECT_EQ((*it3)->id, id3);
}

TEST_F(SimulatorFixture, FindMoverAfterRemovals) {
  // Test finding movers after some have been removed
  int id1 = sim.add_mover(typeid(NewtMover));
  int id2 = sim.add_mover(typeid(NewtMover));
  int id3 = sim.add_mover(typeid(NewtMover));
  
  // Remove middle mover
  sim.remove_mover(id2);
  
  // First mover should still be found
  auto it1 = sim.find_mover(id1);
  EXPECT_NE(it1, sim.movers.end());
  EXPECT_EQ((*it1)->id, id1);
  
  // Removed mover should not be found
  auto it2 = sim.find_mover(id2);
  EXPECT_EQ(it2, sim.movers.end());
  
  // Last mover should still be found
  auto it3 = sim.find_mover(id3);
  EXPECT_NE(it3, sim.movers.end());
  EXPECT_EQ((*it3)->id, id3);
}

TEST_F(SimulatorFixture, FindMoverIdJumps) {
  // The find_mover function relies on the movers being sorted by ID
  // Let's test a case with non-sequential IDs to ensure sorting works
  
  // Create properly filled MoverArgs
  MoverArgs args = MoverArgs(Vect2(1,2), Vect2(3,4), Vect2(5,6), 20, 20);
  
  // First create movers with normal sequential IDs
  int id1 = sim.add_mover(typeid(NewtMover), args);
  int id2 = sim.add_mover(typeid(NewtMover), args);
  
  // Then add a mover with a specific ID via replace_mover
  Mover* customMover = new NewtMover(args);
  int customId = 100; // A much higher ID than the sequential ones
  customMover->id = customId;
  
  // Add a placeholder mover that we'll replace
  int tempId = sim.add_mover(typeid(NewtMover), args);
  
  //set the id of the placeholder mover to the custom id
  auto it = sim.find_mover(tempId);
  (*it)->id = customId;
  
  // All movers should be findable regardless of ID order
  auto it1 = sim.find_mover(id1);
  EXPECT_NE(it1, sim.movers.end());
  EXPECT_EQ((*it1)->id, id1);
  
  auto it2 = sim.find_mover(id2);
  EXPECT_NE(it2, sim.movers.end());
  EXPECT_EQ((*it2)->id, id2);
  
  auto it3 = sim.find_mover(customId);
  EXPECT_NE(it3, sim.movers.end());
  EXPECT_EQ((*it3)->id, customId);
}

// Tests for the update method
class UpdateTestFixture : public ::testing::Test {
protected:
  Simulator sim = Simulator(0.01);
  
  // Add a gravity interaction for physics tests
  void SetUp() override {
    Interaction* gravity = new Gravity(1.0); // Using G=1 for simple calculations
    sim.add_interaction(gravity, {});
  }
};

// Physics correctness tests
TEST_F(UpdateTestFixture, TwoObjectsGravityAttraction) {
  // Create two objects with equal mass at equal distances from origin
  MoverArgs args1 = MoverArgs(Vect2(-5, 0), Vect2(0, 0), Vect2(0, 0), 1.0, 10.0);
  MoverArgs args2 = MoverArgs(Vect2(5, 0), Vect2(0, 0), Vect2(0, 0), 1.0, 10.0);
  
  int id1 = sim.add_mover(typeid(NewtMover), args1);
  int id2 = sim.add_mover(typeid(NewtMover), args2);
  
  // Run simulation for a short time
  sim.update(10);
  
  // Objects should have moved toward each other due to gravity
  EXPECT_GT(sim.movers[0]->position.x, -5.0);
  EXPECT_LT(sim.movers[1]->position.x, 5.0);
  
  // Both objects should have non-zero velocity toward each other
  EXPECT_GT(sim.movers[0]->velocity.x, 0);
  EXPECT_LT(sim.movers[1]->velocity.x, 0);
}

TEST_F(UpdateTestFixture, SingleUpdateTimeAdvance) {
  // Time should advance by dt after each update
  float initial_time = sim.current_time;
  sim.update();
  EXPECT_FLOAT_EQ(initial_time + sim.global_dt, sim.current_time);
  
  sim.update(5);
  EXPECT_FLOAT_EQ(initial_time + 6 * sim.global_dt, sim.current_time);
}

// Additional physics tests with analytical solutions
TEST_F(UpdateTestFixture, ProjectileMotionAnalyticalComparison) {
  // Test a simple projectile motion under constant downward force
  // Create a constant acceleration effect for gravity (9.8 m/s^2 downward)
  Effect* constantGravity = new ConstantAcceleration(Vect2(0, -9.8));
  sim.add_effect(constantGravity, {});
  
  // Initial conditions
  Vect2 position = Vect2(0, 0);
  Vect2 velocity = Vect2(10, 20);
  Vect2 accel = Vect2(0, 0);
  float radius = 1.0;
  float mass = 1.0;
  
  // Create projectile
  MoverArgs args = MoverArgs(position, velocity, accel, radius, mass);
  sim.add_mover(typeid(NewtMover), args);
  
  // Number of time steps to simulate
  int steps = 50;
  float dt = sim.global_dt;
  float totalTime = steps * dt;
  
  // Run simulation
  sim.update(steps);
  
  // Calculate expected position based on analytical solution
  // x(t) = x0 + v0x*t
  // y(t) = y0 + v0y*t - 0.5*g*t^2
  float expectedX = position.x + velocity.x * totalTime;
  float expectedY = position.y + velocity.y * totalTime - 0.5f * 9.8f * totalTime * totalTime;
  
  // Compare simulation result with analytical solution using relative tolerance of 1e-3 * expected value
  float toleranceX = std::abs(expectedX) * 1e-3f;
  float toleranceY = std::abs(expectedY) * 1e-3f;
  // Ensure minimum tolerance to avoid division by zero issues
  toleranceX = std::max(toleranceX, 1e-6f);
  toleranceY = std::max(toleranceY, 1e-6f);
  
  EXPECT_NEAR(expectedX, sim.movers[0]->position.x, toleranceX);
  EXPECT_NEAR(expectedY, sim.movers[0]->position.y, toleranceY);
}

TEST_F(UpdateTestFixture, SimpleHarmonicOscillatorAnalyticalComparison) {
  // Test a simple harmonic oscillator - a mass on a spring
  sim.interactions.clear();
  sim.effects.clear();
  sim.global_dt = 0.0001f;
  
  // Create a spring interaction with equilibrium at distance 5
  float k = 10.0f;                // Spring constant
  float equilibriumDist = 5.0f;   // Equilibrium distance
  Interaction* spring = new Spring(k, equilibriumDist);
  sim.add_interaction(spring, {});
  
  // Initial positions: first mass at origin, second mass displaced 5 units to the right of equilibrium
  float displacement = 5.0f;      // Initial displacement from equilibrium
  Vect2 pos1 = Vect2(0, 0);       // Fixed mass position
  Vect2 pos2 = Vect2(pos1.x + equilibriumDist + displacement, 0); // Displaced mass position
  
  // Create two masses: one fixed at origin, one displaced from equilibrium
  MoverArgs fixedMass = MoverArgs(pos1, Vect2(0, 0), Vect2(0, 0), 1.0, 10000.0); // Very heavy mass (effectively fixed)
  MoverArgs oscillatingMass = MoverArgs(pos2, Vect2(0, 0), Vect2(0, 0), 1.0, 1.0); // Displaced from equilibrium
  
  sim.add_mover(typeid(NewtMover), fixedMass);
  sim.add_mover(typeid(NewtMover), oscillatingMass);
  
  // Calculate expected oscillation parameters
  float m = 1.0f;                 // Mass of oscillating body
  float omega = sqrt(k/m);        // Angular frequency
  float amplitude = displacement; // Amplitude (initial displacement from equilibrium)
  
  // Run for 1/4 period - mass should move from maximum displacement to equilibrium
  float quarterPeriod = M_PI / (2 * omega);
  
  // Check time step is small enough for accuracy
  ASSERT_LT(sim.global_dt, quarterPeriod / 10) << "Time step too large for accurate simulation";
  
  sim.update(int(quarterPeriod / sim.global_dt));
  
  // After 1/4 period, mass should be at approximately equilibrium position (x=5)
  // with maximum negative velocity
  float expectedX = pos1.x + equilibriumDist;  // Equilibrium position
  float expectedV = -amplitude * omega;        // Maximum velocity at equilibrium
  
  // Compare using relative tolerance of 1e-3 * expected value
  float toleranceX = std::abs(expectedX) * 1e-3f;
  float toleranceV = std::abs(expectedV) * 1e-3f;
  // Ensure minimum tolerance to avoid division by zero issues
  toleranceX = std::max(toleranceX, 1e-6f);
  toleranceV = std::max(toleranceV, 1e-6f);
  
  EXPECT_NEAR(expectedX, sim.movers[1]->position.x, toleranceX);
  EXPECT_NEAR(expectedV, sim.movers[1]->velocity.x, toleranceV);
}

// Multi-threading tests
class ThreadingTestFixture : public ::testing::Test {
protected:
  // Use a very small dt to minimize floating point differences
  static constexpr float TEST_DT = 0.001f;
  
  Simulator multiThread = Simulator(TEST_DT);
  Simulator singleThread = Simulator(TEST_DT);
  
  // Helper to compare positions after update
  void CompareResults(int updates) {
    // Update both simulators
    multiThread.update(updates);
    for (int i = 0; i < updates; i++) {
      singleThread.update_unithread();
    }
    
    // Positions should match within a small tolerance
    ASSERT_EQ(multiThread.movers.size(), singleThread.movers.size());
    for (size_t i = 0; i < multiThread.movers.size(); i++) {
      EXPECT_NEAR(multiThread.movers[i]->position.x, singleThread.movers[i]->position.x, 1e-5);
      EXPECT_NEAR(multiThread.movers[i]->position.y, singleThread.movers[i]->position.y, 1e-5);
    }
  }
};

TEST_F(ThreadingTestFixture, EmptySimulatorThreading) {
  // Should not crash with zero movers
  EXPECT_NO_THROW(multiThread.update(10));
}

TEST_F(ThreadingTestFixture, SingleMoverThreading) {
  // Add one mover to both simulators
  MoverArgs args = MoverArgs(Vect2(1, 2), Vect2(3, 4), Vect2(0, 0), 1.0, 1.0);
  multiThread.add_mover(typeid(NewtMover), args);
  singleThread.add_mover(typeid(NewtMover), args);
  
  // Add same interaction to both
  Interaction* gravity = new Gravity(1.0);
  multiThread.add_interaction(gravity, {});
  gravity = new Gravity(1.0);  // Need a new instance for the second simulator
  singleThread.add_interaction(gravity, {});
  
  // Results should match between multi and single threaded versions
  CompareResults(5);
}

TEST_F(ThreadingTestFixture, ManyMoversThreading) {
  // Create a number of movers that exceeds typical thread count
  const int NUM_MOVERS = 50;
  
  // Add same interactions to both simulators
  Interaction* gravity = new Gravity(1.0);
  multiThread.add_interaction(gravity, {});
  gravity = new Gravity(1.0);
  singleThread.add_interaction(gravity, {});
  
  // Add identical set of movers to both simulators
  for (int i = 0; i < NUM_MOVERS; i++) {
    float x = static_cast<float>(i) - NUM_MOVERS/2.0f;
    MoverArgs args = MoverArgs(Vect2(x, 0), Vect2(0, 0), Vect2(0, 0), 1.0, 1.0);
    multiThread.add_mover(typeid(NewtMover), args);
    singleThread.add_mover(typeid(NewtMover), args);
  }
  
  // Results should match between multi and single threaded versions
  CompareResults(3);  // Use fewer updates as this test is more computationally intensive
}

// Templated Effect tests
class TemplatedEffectTestFixture : public ::testing::Test {
protected:
  Simulator sim = Simulator(0.01);
  EffectAdder effectAdder = EffectAdder(&sim);
  MoverArgs args1 = MoverArgs(Vect2(1, 2), Vect2(3, 4), Vect2(0, 0), 1.0, 1.0);
  MoverArgs args2 = MoverArgs(Vect2(5, 6), Vect2(7, 8), Vect2(0, 0), 1.0, 1.0);
};

TEST_F(TemplatedEffectTestFixture, DragEffect) {
  // add drag effect to simulator
  effectAdder.addDrag(1.0, 1.0);
  // add movers to simulator
  sim.add_mover(typeid(NewtMover), args1);
  sim.add_mover(typeid(NewtMover), args2);
  effectAdder.addDragToMover(2.0, *sim.movers[0]);
  // run some steps 
  sim.update(100);
  //check that velocity is lower than initial velocity
  EXPECT_LT(sim.movers[0]->velocity.mag(), args1.velocity.value().mag());
  EXPECT_LT(sim.movers[1]->velocity.mag(), args2.velocity.value().mag());
  //check that direction is still the same
  Vect2 direction0_actual = sim.movers[0]->velocity / sim.movers[0]->velocity.mag();
  Vect2 direction1_actual = sim.movers[1]->velocity / sim.movers[1]->velocity.mag();
  Vect2 direction0_expected = args1.velocity.value() / args1.velocity.value().mag();
  Vect2 direction1_expected = args2.velocity.value() / args2.velocity.value().mag();
  EXPECT_NEAR(direction0_actual.x, direction0_expected.x, 1e-5);
  EXPECT_NEAR(direction0_actual.y, direction0_expected.y, 1e-5);
  EXPECT_NEAR(direction1_actual.x, direction1_expected.x, 1e-5);
  EXPECT_NEAR(direction1_actual.y, direction1_expected.y, 1e-5);
};

//test multiple added effects works
TEST_F(TemplatedEffectTestFixture, AttractorandDragEffect) {
  // add attractor effect to simulator
  effectAdder.addAttractor(1.0, Vect2(0, 0), 1.0);
  // add drag effect to simulator
  effectAdder.addDrag(1.0, 1.0);
  // add movers to simulator
  sim.add_mover(typeid(NewtMover), args1);
  sim.add_mover(typeid(NewtMover), args2);
  effectAdder.addDragToMover(2.0, *sim.movers[0]);
  // run some steps and chekc that it doesnt crash
  EXPECT_NO_THROW(sim.update(10));
};