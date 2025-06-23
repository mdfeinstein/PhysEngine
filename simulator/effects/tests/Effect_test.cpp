#include <gtest/gtest.h>
#include "Effect.h"
#include "Effect2.h"
#include "EffectTemplated.h"
#include "EffectTypes/Drag2.h"
#include "EffectTypes/Drag.h"
#include "EffectTypes/LorentzEffect.h"
#include "EffectTypes/Attractor.h"
#include "EffectTypes/ConstantAcceleration.h"
#include "EffectTypes/ConstantForce.h"
#include "EffectTypes/TimeVaryingForce.h"
#include "EffectTypes/ForceField.h"
#include "Mover.h"
#include <cmath>

// Base Effect Test Fixture
class EffectFixture : public ::testing::Test {
protected:
  MoverArgs defaultArgs = MoverArgs(Vect2(0, 0), Vect2(0, 0), Vect2(0, 0), 1, 1);
  
  // Helper to create a mover for tests
  std::unique_ptr<Mover> createTestMover(const MoverArgs& args = MoverArgs(Vect2(0, 0), Vect2(0, 0), Vect2(0, 0), 1, 1)) {
    return std::make_unique<NewtMover>(args);
  }
  
  // Helper to create a mover with specified position and velocity
  std::unique_ptr<Mover> createMover(Vect2 position, Vect2 velocity, float mass = 1.0f) {
    return std::make_unique<NewtMover>(MoverArgs(position, velocity, Vect2(0, 0), 1.0f, mass));
  }
  
  // Helper to set up the interactionParams map in a mover for a specific effect
  template<typename EffectType>
  void setupMoverParams(Mover* mover, std::vector<std::any> params) {
    mover->interactionParams[typeid(EffectType)] = params;
  }
};

// Base Effect Class Tests
TEST_F(EffectFixture, BaseEffectParameterCount) {
  Effect baseEffect;
  EXPECT_EQ(baseEffect.paramCount, 0);
}

TEST_F(EffectFixture, BaseEffectInterpretParamsWithCorrectParamCount) {
  Effect baseEffect;
  std::vector<std::any> emptyParams;
  EXPECT_NO_THROW(baseEffect.interpretParams(emptyParams));
}

TEST_F(EffectFixture, BaseEffectInterpretParamsWithIncorrectParamCount) {
  Effect baseEffect;
  std::vector<std::any> tooManyParams = {1.0f};
  EXPECT_THROW(baseEffect.interpretParams(tooManyParams), std::invalid_argument);
}

// Drag Effect Tests
class DragEffectFixture : public EffectFixture {};

TEST_F(DragEffectFixture, ConstructorSetsCorrectParameterCount) {
  Drag dragEffect(2.0f);
  EXPECT_EQ(dragEffect.paramCount, 1);
  EXPECT_EQ(dragEffect.strength, 2.0f);
}

TEST_F(DragEffectFixture, InterpretParamsValid) {
  Drag dragEffect;
  std::vector<std::any> validParams = {3.0f};
  std::any result = dragEffect.interpretParams(validParams);
  float dragCoeff = std::any_cast<float>(result);
  EXPECT_FLOAT_EQ(dragCoeff, 3.0f);
}

TEST_F(DragEffectFixture, InterpretParamsInvalidCount) {
  Drag dragEffect;
  std::vector<std::any> invalidParams = {1.0f, 2.0f};
  EXPECT_THROW(dragEffect.interpretParams(invalidParams), std::invalid_argument);
}

TEST_F(DragEffectFixture, ApplyEffect) {
  Drag dragEffect(2.0f);
  auto mover = createMover(Vect2(0, 0), Vect2(10, 5), 2.0f);
  setupMoverParams<Drag>(mover.get(), {3.0f}); // Mover-specific drag coefficient
  
  // Initial zero force
  EXPECT_EQ(dynamic_cast<NewtMover*>(mover.get())->force_sum.load(), Vect2(0, 0));
  
  // Apply the drag effect
  dragEffect.apply(mover.get());
  
  // Expected force based on implementation: -dragCoeff * velocity 
  // (strength parameter is not used in the apply method)
  // = -3.0 * (10, 5) = (-30, -15)
  Vect2 expectedForce = Vect2(-60, -30);
  Vect2 actualForce = dynamic_cast<NewtMover*>(mover.get())->force_sum.load();
  
  float tolX = std::abs(expectedForce.x) * 0.01f;
  float tolY = std::abs(expectedForce.y) * 0.01f;
  EXPECT_NEAR(actualForce.x, expectedForce.x, tolX);
  EXPECT_NEAR(actualForce.y, expectedForce.y, tolY);
}

// Drag Effect2 Tests
class Drag2EffectFixture : public EffectFixture {};

TEST_F(Drag2EffectFixture, ConstructorSetsCorrectParameterCount) {
  Drag2 dragEffect(2.0f);
  EXPECT_EQ(dragEffect.globalParamCount, 1);
  EXPECT_EQ(dragEffect.strength, 2.0f);
}

TEST_F(Drag2EffectFixture, InterpretParamsValid) {
  Drag2 dragEffect;
  std::vector<std::any> validParams = {3.0f};
  dragEffect.parseMoverParams(validParams);
  EXPECT_FLOAT_EQ(dragEffect.drag_coeff, 3.0f);
}

TEST_F(Drag2EffectFixture, InterpretParamsInvalidCount) {
  Drag2 dragEffect;
  std::vector<std::any> invalidParams = {1.0f, 2.0f};
  EXPECT_THROW(dragEffect.parseMoverParams(invalidParams), std::invalid_argument);
}

TEST_F(Drag2EffectFixture, ApplyEffect) {
  Drag2 dragEffect(2.0f);
  auto mover = createMover(Vect2(0, 0), Vect2(10, 5), 2.0f);
  setupMoverParams<Drag2>(mover.get(), {3.0f}); // Mover-specific drag coefficient
  
  // Initial zero force
  EXPECT_EQ(dynamic_cast<NewtMover*>(mover.get())->force_sum.load(), Vect2(0, 0));
  
  // Apply the drag effect
  dragEffect.apply(mover.get());
  
  // Expected force based on implementation: -dragCoeff * velocity 
  // (strength parameter is not used in the apply method)
  // = -3.0 * (10, 5) = (-30, -15)
  Vect2 expectedForce = Vect2(-60, -30);
  Vect2 actualForce = dynamic_cast<NewtMover*>(mover.get())->force_sum.load();
  
  float tolX = std::abs(expectedForce.x) * 0.01f;
  float tolY = std::abs(expectedForce.y) * 0.01f;
  EXPECT_NEAR(actualForce.x, expectedForce.x, tolX);
  EXPECT_NEAR(actualForce.y, expectedForce.y, tolY);
}

// Drag EffectTemplated Tests
class DragTemplatedEffectFixture : public EffectFixture {
  protected:
  // define drag effect with templated parameters
  using DragTemplated = EffectTemplated<std::tuple<float>, std::tuple<float>>;
  //function for create effect
  std::function<void(Mover&, std::tuple<float>, std::tuple<float>)> dragFunc =
   [](Mover& mover, std::tuple<float> globalParams, std::tuple<float> moverParams) {
      float strength = std::get<0>(globalParams);
      float drag_coeff = std::get<0>(moverParams);
      mover.apply_force(-1*drag_coeff*strength*mover.velocity);
   };
  // set global and mover params
  std::tuple<float> globalParams = {2.0f};
  std::tuple<float> moverParams = {3.0f};
  // construct EffectTemplated object
  DragTemplated dragEffect = DragTemplated(globalParams, dragFunc, "DragTemplated");
  //helper to setup mover effectParams
  void setupMoverEffectParams(Mover* mover, std::any params, std::string effectName) {
    mover->effectParams[effectName] = params;
  }
};

TEST_F(DragTemplatedEffectFixture, ConstructorSetsCorrectParameters) {
  std::unique_ptr<Mover> mover = createMover(Vect2(0, 0), Vect2(10, 5), 2.0f);
  setupMoverEffectParams(mover.get(), std::make_tuple<float>(3.0f), "DragTemplated");
  dragEffect.getMoverParams(*mover);
  EXPECT_EQ(std::get<0>(dragEffect.globalParams), 2.0f);
  // EXPECT_EQ(std::get<0>(dragEffect.moverParams), 3.0f);
}

TEST_F(DragTemplatedEffectFixture, ApplyEffect) {
  auto mover = createMover(Vect2(0, 0), Vect2(10, 5), 2.0f);
  setupMoverEffectParams(mover.get(), std::make_tuple<float>(3.0f), "DragTemplated");
  // Initial zero force
  EXPECT_EQ(dynamic_cast<NewtMover*>(mover.get())->force_sum.load(), Vect2(0, 0));
  // Apply the drag effect thru function pointer
  auto applyFunction = dragEffect.getApplyFunction();
  applyFunction(*mover);

  Vect2 expectedForce = Vect2(-60, -30);
  Vect2 actualForce = dynamic_cast<NewtMover*>(mover.get())->force_sum.load();
  
  float tolX = std::abs(expectedForce.x) * 0.01f;
  float tolY = std::abs(expectedForce.y) * 0.01f;
  EXPECT_NEAR(actualForce.x, expectedForce.x, tolX);
  EXPECT_NEAR(actualForce.y, expectedForce.y, tolY);
}

// Lorentz Effect Tests
class LorentzEffectFixture : public EffectFixture {};

TEST_F(LorentzEffectFixture, ConstructorSetsCorrectParameterCount) {
  LorentzEffect lorentzEffect(2.5f);
  EXPECT_EQ(lorentzEffect.paramCount, 1);
  EXPECT_EQ(lorentzEffect.magneticStrength, 2.5f);
}

TEST_F(LorentzEffectFixture, InterpretParamsValid) {
  LorentzEffect lorentzEffect;
  std::vector<std::any> validParams = {3.0f};
  std::any result = lorentzEffect.interpretParams(validParams);
  float charge = std::any_cast<float>(result);
  EXPECT_FLOAT_EQ(charge, 3.0f);
}

TEST_F(LorentzEffectFixture, InterpretParamsInvalidCount) {
  LorentzEffect lorentzEffect;
  std::vector<std::any> invalidParams = {1.0f, 2.0f};
  EXPECT_THROW(lorentzEffect.interpretParams(invalidParams), std::invalid_argument);
}

TEST_F(LorentzEffectFixture, ApplyEffect) {
  LorentzEffect lorentzEffect(2.0f);
  auto mover = createMover(Vect2(0, 0), Vect2(10, 0), 1.0f);
  setupMoverParams<LorentzEffect>(mover.get(), {3.0f}); // Mover-specific charge
  
  // Initial zero force
  EXPECT_EQ(dynamic_cast<NewtMover*>(mover.get())->force_sum.load(), Vect2(0, 0));
  
  // Apply the Lorentz effect
  lorentzEffect.apply(mover.get());
  
  // Lorentz force: F = q * (v × B)
  // Based on the implementation, we rotate velocity PI/2 (90 degrees counterclockwise)
  // For velocity (10, 0), rotating PI/2 counterclockwise gives (0, 10)
  // Force = magneticStrength * charge * rotated velocity
  // = 2.0 * 3.0 * (0, 10) = (0, 60)
  Vect2 expectedForce = Vect2(0, 60); // Counterclockwise rotation
  Vect2 actualForce = dynamic_cast<NewtMover*>(mover.get())->force_sum.load();
  
  float tolX = std::max(std::abs(expectedForce.x) * 0.01f, 0.01f);
  float tolY = std::abs(expectedForce.y) * 0.01f;
  EXPECT_NEAR(actualForce.x, expectedForce.x, tolX);
  EXPECT_NEAR(actualForce.y, expectedForce.y, tolY);
}

// Attractor Tests
class AttractorFixture : public EffectFixture {};

TEST_F(AttractorFixture, Constructor) {
  Attractor attractor(2.0f, Vect2(10, 20), 5.0f);
  EXPECT_EQ(attractor.paramCount, 0);
  EXPECT_FLOAT_EQ(attractor.strength, 2.0f);
  EXPECT_EQ(attractor.position, Vect2(10, 20));
  EXPECT_FLOAT_EQ(attractor.min_distance, 5.0f);
}

TEST_F(AttractorFixture, ApplyEffect) {
  Attractor attractor = Attractor(2.0f, Vect2(10, 0), 5.0f);
  auto mover = createMover(Vect2(0, 0), Vect2(0, 0), 3.0f);
  
  // Initial zero force
  EXPECT_EQ(dynamic_cast<NewtMover*>(mover.get())->force_sum.load(), Vect2(0, 0));
  
  // Apply the attractor effect
  attractor.apply(mover.get());
  
  // Calculate force using the exact same approach as in Attractor::apply
  Vect2 r = Vect2(10, 0) - Vect2(0, 0); // position - mover->position
  float magnitude = r.mag(); // = 10
  // Vect2 force = strength * mover->mass * r / pow(magnitude, 3)
  Vect2 force = 2.0f * 3.0f * r / std::pow(magnitude, 3);
  
  // Expected force should match the calculation from the implementation 
  Vect2 expectedForce = force;
  Vect2 actualForce = dynamic_cast<NewtMover*>(mover.get())->force_sum.load();
  float tolX = std::max(std::abs(expectedForce.x) * 0.01f, 0.0001f);
  float tolY = std::max(std::abs(expectedForce.y) * 0.01f, 0.0001f);
  EXPECT_NEAR(actualForce.x, expectedForce.x, tolX);
  EXPECT_NEAR(actualForce.y, expectedForce.y, tolY);
}

TEST_F(AttractorFixture, ApplyEffectWithinMinDistance) {
  Attractor attractor(2.0f, Vect2(3, 0), 5.0f);
  auto mover = createMover(Vect2(0, 0), Vect2(0, 0), 3.0f);
  
  // Initial zero force
  EXPECT_EQ(dynamic_cast<NewtMover*>(mover.get())->force_sum.load(), Vect2(0, 0));
  
  // Apply the attractor effect - should do nothing as distance < min_distance
  attractor.apply(mover.get());
  
  // Force should still be zero
  Vect2 actualForce = dynamic_cast<NewtMover*>(mover.get())->force_sum.load();
  EXPECT_EQ(actualForce, Vect2(0, 0));
}

// ConstantAcceleration Tests
class ConstantAccelerationFixture : public EffectFixture {};

TEST_F(ConstantAccelerationFixture, Constructor) {
  ConstantAcceleration accel(Vect2(0, -9.8));
  EXPECT_EQ(accel.paramCount, 0);
  EXPECT_EQ(accel.acceleration, Vect2(0, -9.8));
}

TEST_F(ConstantAccelerationFixture, ApplyEffect) {
  ConstantAcceleration accel(Vect2(0, -9.8f));
  auto mover = createMover(Vect2(0, 0), Vect2(0, 0), 2.0f);
  
  // Initial zero force
  EXPECT_EQ(dynamic_cast<NewtMover*>(mover.get())->force_sum.load(), Vect2(0, 0));
  
  // Apply the constant acceleration effect
  accel.apply(mover.get());
  
  // Force = mass * acceleration
  // = 2.0 * (0, -9.8) = (0, -19.6)
  Vect2 expectedForce = Vect2(0, -19.6f);
  Vect2 actualForce = dynamic_cast<NewtMover*>(mover.get())->force_sum.load();
  
  float tolX = std::max(std::abs(expectedForce.x) * 0.01f, 0.01f);
  float tolY = std::abs(expectedForce.y) * 0.01f;
  EXPECT_NEAR(actualForce.x, expectedForce.x, tolX);
  EXPECT_NEAR(actualForce.y, expectedForce.y, tolY);
}

// ConstantForce Tests
class ConstantForceFixture : public EffectFixture {};

TEST_F(ConstantForceFixture, Constructor) {
  ConstantForce force(Vect2(5, 10));
  EXPECT_EQ(force.paramCount, 1);
  EXPECT_EQ(force.forceVector, Vect2(5, 10));
}

TEST_F(ConstantForceFixture, InterpretParamsValid) {
  ConstantForce force(Vect2(0,0));
  std::vector<std::any> validParams = {2.5f};
  std::any result = force.interpretParams(validParams);
  float scale = std::any_cast<float>(result);
  EXPECT_FLOAT_EQ(scale, 2.5f);
}

TEST_F(ConstantForceFixture, InterpretParamsInvalidCount) {
  ConstantForce force(Vect2(0,0));
  std::vector<std::any> invalidParams = {1.0f, 2.0f};
  EXPECT_THROW(force.interpretParams(invalidParams), std::invalid_argument);
}

TEST_F(ConstantForceFixture, ApplyEffect) {
  ConstantForce force(Vect2(5.0f, 10.0f));
  auto mover = createMover(Vect2(0, 0), Vect2(0, 0), 1.0f);
  setupMoverParams<ConstantForce>(mover.get(), {2.0f}); // Scale factor of 2.0
  
  // Initial zero force
  EXPECT_EQ(dynamic_cast<NewtMover*>(mover.get())->force_sum.load(), Vect2(0, 0));
  
  // Apply the constant force effect
  force.apply(mover.get());
  
  // Force = forceVector * scale 
  // = (5.0, 10.0) * 2.0 = (10.0, 20.0)
  Vect2 expectedForce = Vect2(10.0f, 20.0f);
  Vect2 actualForce = dynamic_cast<NewtMover*>(mover.get())->force_sum.load();
  
  float tolX = std::abs(expectedForce.x) * 0.01f;
  float tolY = std::abs(expectedForce.y) * 0.01f;
  EXPECT_NEAR(actualForce.x, expectedForce.x, tolX);
  EXPECT_NEAR(actualForce.y, expectedForce.y, tolY);
}

// TimeVaryingForce Tests
class TimeVaryingForceFixture : public EffectFixture {};

TEST_F(TimeVaryingForceFixture, Constructor) {
  float timeSource = 0.0f;
  TimeVaryingForce force([](float t) { return Vect2(std::sin(t), std::cos(t)); }, timeSource);
  EXPECT_EQ(force.paramCount, 1);
}

TEST_F(TimeVaryingForceFixture, InterpretParamsValid) {
  float timeSource = 0.0f;
  TimeVaryingForce force([](float t) { return Vect2(t, t); }, timeSource);
  std::vector<std::any> validParams = {2.5f};
  std::any result = force.interpretParams(validParams);
  float scale = std::any_cast<float>(result);
  EXPECT_FLOAT_EQ(scale, 2.5f);
}

TEST_F(TimeVaryingForceFixture, InterpretParamsInvalidCount) {
  float timeSource = 0.0f;
  TimeVaryingForce force([](float t) { return Vect2(t, t); }, timeSource);
  std::vector<std::any> invalidParams = {1.0f, 2.0f};
  EXPECT_THROW(force.interpretParams(invalidParams), std::invalid_argument);
}

TEST_F(TimeVaryingForceFixture, ApplyEffect) {
  float pi = 3.14159265358979323846f;
  float timeSource = pi / 4.0f; // 45 degrees - sin = cos = 0.7071...
  
  TimeVaryingForce force([](float t) { return Vect2(std::sin(t), std::cos(t)); }, timeSource);
  auto mover = createMover(Vect2(0, 0), Vect2(0, 0), 1.0f);
  setupMoverParams<TimeVaryingForce>(mover.get(), {2.0f}); // Scale factor of 2.0
  
  // Initial zero force
  EXPECT_EQ(dynamic_cast<NewtMover*>(mover.get())->force_sum.load(), Vect2(0, 0));
  
  // Apply the time-varying force effect
  force.apply(mover.get());
  
  // Expected force at t=π/4: 
  // Force = forceFunc(t) * scaleFactor
  // sin(π/4) = cos(π/4) = 0.7071...
  const float sqrtHalf = 0.7071067811865475f; // More precise value
  Vect2 expectedForce = Vect2(2.0f * sqrtHalf, 2.0f * sqrtHalf);
  Vect2 actualForce = dynamic_cast<NewtMover*>(mover.get())->force_sum.load();
  
  float tolX = std::abs(expectedForce.x) * 0.01f;
  float tolY = std::abs(expectedForce.y) * 0.01f;
  EXPECT_NEAR(actualForce.x, expectedForce.x, tolX);
  EXPECT_NEAR(actualForce.y, expectedForce.y, tolY);
}

TEST_F(TimeVaryingForceFixture, TimeSourceReference) {
  float timeSource = 0.0f;
  
  // Use a simpler linear function for more predictable results
  TimeVaryingForce force([](float t) { return Vect2(t, t*2); }, timeSource);
  auto mover = createMover(Vect2(0, 0), Vect2(0, 0), 1.0f);
  setupMoverParams<TimeVaryingForce>(mover.get(), {1.0f});
  
  // Time = 0.0
  force.apply(mover.get());
  Vect2 zeroTimeForce = dynamic_cast<NewtMover*>(mover.get())->force_sum.load();
  
  // Force should be (0, 0) * 1.0 = (0, 0)
  EXPECT_NEAR(zeroTimeForce.x, 0.0f, 0.01f);
  EXPECT_NEAR(zeroTimeForce.y, 0.0f, 0.01f);
  
  // Change time and force should update accordingly
  timeSource = 2.0f;
  dynamic_cast<NewtMover*>(mover.get())->force_sum = Vect2(0, 0); // Reset force
  force.apply(mover.get());
  Vect2 laterTimeForce = dynamic_cast<NewtMover*>(mover.get())->force_sum.load();
  
  // Force should now be (2, 4) * 1.0 = (2, 4)
  EXPECT_NEAR(laterTimeForce.x, 2.0f, 0.02f); // Use 0.01 * expected = 0.02
  EXPECT_NEAR(laterTimeForce.y, 4.0f, 0.04f); // Use 0.01 * expected = 0.04
}

// ForceField Tests
class ForceFieldFixture : public EffectFixture {};

TEST_F(ForceFieldFixture, Constructor) {
    ForceField field([](Vect2 pos) { return pos*pos; });
    EXPECT_EQ(field.paramCount, 1);
}

TEST_F(ForceFieldFixture, ApplyEffect) {
    ForceField field([](Vect2 pos) { return pos*pos; });
    auto mover = createMover(Vect2(2, 2), Vect2(0, 0), 1.0f);
    setupMoverParams<ForceField>(mover.get(), {4.0f}); // Scale factor of 1.5

    // Initial zero force
    EXPECT_EQ(dynamic_cast<NewtMover*>(mover.get())->force_sum.load(), Vect2(0, 0));

    // Apply the force field effect
    field.apply(mover.get());

    // Force = fieldFunction(position) * scale
    // = (2^2, 2^2) * 4.0 = (16, 16)
    Vect2 expectedForce = Vect2(16.0f, 16.0f);
    Vect2 actualForce = dynamic_cast<NewtMover*>(mover.get())->force_sum.load();

    float tolX = std::abs(expectedForce.x) * 0.01f;
    float tolY = std::abs(expectedForce.y) * 0.01f;
    EXPECT_NEAR(actualForce.x, expectedForce.x, tolX);
    EXPECT_NEAR(actualForce.y, expectedForce.y, tolY);
}

