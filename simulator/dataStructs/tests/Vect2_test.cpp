#include <gtest/gtest.h>
#include "Vect2.h"

/*define a text fixture class declaring a 0 vector, a 1 vector, and 3 other vectors of Vect2*/
class Vect2Fixture : public ::testing::Test {
  protected:
    Vect2 zero = Vect2(0, 0);
    Vect2 one = Vect2(1, 1);
    Vect2 v1 = Vect2(1, 2);
    Vect2 v2 = Vect2(3, -4);
    Vect2 v3 = Vect2(-5, 6);
    const float pi = 3.14159265358979323846;
};

//default constructor equals zero
TEST_F(Vect2Fixture, DefaultConstructor) {
  EXPECT_FLOAT_EQ(zero.x, 0);
  EXPECT_FLOAT_EQ(zero.y, 0);
}

//copy constructor
TEST_F(Vect2Fixture, CopyConstructor) {
  Vect2 v = Vect2(v1);
  EXPECT_FLOAT_EQ(v.x, 1);
  EXPECT_FLOAT_EQ(v.y, 2);
}

//non-const reference assignment affects both objects
TEST_F(Vect2Fixture, ReferenceAssignment) {
  Vect2& v = zero;
  v = v1;
  EXPECT_FLOAT_EQ(v.x, 1);
  EXPECT_FLOAT_EQ(v.y, 2);
  EXPECT_FLOAT_EQ(zero.x, 1);
  EXPECT_FLOAT_EQ(zero.y, 2);
}

/*test using the fixture class*/
TEST_F(Vect2Fixture, Magnitude) {
  // Expect magnitude to be correct
  EXPECT_FLOAT_EQ(zero.mag(), 0);
  EXPECT_FLOAT_EQ(one.mag(), sqrt(2));
  EXPECT_FLOAT_EQ(v1.mag(), sqrt(pow(1, 2) + pow(2, 2)));
  EXPECT_FLOAT_EQ(v2.mag(), sqrt(pow(3, 2) + pow(4, 2)));
  EXPECT_FLOAT_EQ(v3.mag(), sqrt(pow(5, 2) + pow(6, 2)));
}

//test angle
TEST_F(Vect2Fixture, Angle) {
  //define pi

  EXPECT_FLOAT_EQ(zero.angle(), 0);
  EXPECT_FLOAT_EQ(one.angle(), pi/4);
  EXPECT_FLOAT_EQ((-1*one).angle(), -pi+pi/4);
  EXPECT_FLOAT_EQ(Vect2(sqrt(3),1).angle(), 30*pi/180);
}

//test polar_set
TEST_F(Vect2Fixture, PolarSet) {
  Vect2 v = zero;
  v.polar_set(1, 0);
  EXPECT_NEAR(v.x, 1, 1e-6);
  EXPECT_NEAR(v.y, 0, 1e-6);
  v.polar_set(1, 90);
  EXPECT_NEAR(v.x, 0, 1e-6);
  EXPECT_NEAR(v.y, 1, 1e-6);
  v.polar_set(1, 180);
  EXPECT_NEAR(v.x, -1, 1e-6);
  EXPECT_NEAR(v.y, 0, 1e-6);
  v.polar_set(1, 270);
  EXPECT_NEAR(v.x, 0, 1e-6);
  EXPECT_NEAR(v.y, -1, 1e-6);
  v.polar_set(1, 360);
  EXPECT_NEAR(v.x, 1, 1e-6);
  EXPECT_NEAR(v.y, 0, 1e-6);
  v = zero;
  v.polar_set(5, 90);
  EXPECT_NEAR(v.x, 0, 1e-6);
  EXPECT_NEAR(v.y, 5, 1e-6);
}

TEST_F(Vect2Fixture, Addition) {
  // test addition of all fixture vectors, accounting for floats
  Vect2 v = zero + zero;
  EXPECT_FLOAT_EQ(v.x, 0);
  EXPECT_FLOAT_EQ(v.y, 0);
  v = one + one;
  EXPECT_FLOAT_EQ(v.x, 2);
  EXPECT_FLOAT_EQ(v.y, 2);
  v = v1 + v2;
  EXPECT_FLOAT_EQ(v.x, 4);
  EXPECT_FLOAT_EQ(v.y, -2);
  v = v2 + v3;
  EXPECT_FLOAT_EQ(v.x, -2);
  EXPECT_FLOAT_EQ(v.y, 2);
  v = v3 + v1;
  EXPECT_FLOAT_EQ(v.x, -4);
  EXPECT_FLOAT_EQ(v.y, 8);
}



TEST_F(Vect2Fixture, Subtraction) {
  // test subtraction of all fixture vectors, accounting for floats
  Vect2 v = zero - zero;
  EXPECT_FLOAT_EQ(v.x, 0);
  EXPECT_FLOAT_EQ(v.y, 0);
  v = one - one;
  EXPECT_FLOAT_EQ(v.x, 0);
  EXPECT_FLOAT_EQ(v.y, 0);
  v = v1 - v2;
  EXPECT_FLOAT_EQ(v.x, -2);
  EXPECT_FLOAT_EQ(v.y, 6);
  v = v2 - v3;
  EXPECT_FLOAT_EQ(v.x, 8);
  EXPECT_FLOAT_EQ(v.y, -10);
  v = v3 - v1;
  EXPECT_FLOAT_EQ(v.x, -6);
  EXPECT_FLOAT_EQ(v.y, 4);
}

TEST_F(Vect2Fixture, DotProduct) {
  // test dot product of all fixture vectors, accounting for floats
  EXPECT_FLOAT_EQ(zero.dot(zero), 0);
  EXPECT_FLOAT_EQ(one.dot(one), 2);
  EXPECT_FLOAT_EQ(v1.dot(v2), -5);
  EXPECT_FLOAT_EQ(v2.dot(v3), -39);
  EXPECT_FLOAT_EQ(v3.dot(v1), 7);
}

//test cross product
TEST_F(Vect2Fixture, CrossProduct) {
  //one by zero is zero
  EXPECT_FLOAT_EQ(zero.cross(one), 0);
  //one by one is zero
  EXPECT_FLOAT_EQ(one.cross(one), 0);
  //vector rotated by 90 and crossed is itself squared
  EXPECT_NEAR(v1.cross(v1.rotate(pi/2)), v1.mag()*v1.mag(), 1e-6);
}

TEST_F(Vect2Fixture, VectorMultiplication) {
  // test elementwise Vector multiplication of all fixture vectors, accounting for floats
  Vect2 v = zero * zero;
  EXPECT_FLOAT_EQ(v.x, 0);
  EXPECT_FLOAT_EQ(v.y, 0);
  v = one * one;
  EXPECT_FLOAT_EQ(v.x, 1);
  EXPECT_FLOAT_EQ(v.y, 1);
  v = v1 * v2;
  EXPECT_FLOAT_EQ(v.x, v1.x*v2.x);
  EXPECT_FLOAT_EQ(v.y, v1.y*v2.y);
  v = v2 * v3;
  EXPECT_FLOAT_EQ(v.x, v2.x*v3.x);
  EXPECT_FLOAT_EQ(v.y, v2.y*v3.y);
  v = v3 * v1;
  EXPECT_FLOAT_EQ(v.x, v3.x*v1.x);
  EXPECT_FLOAT_EQ(v.y, v3.y*v1.y);
}
TEST_F(Vect2Fixture, ScalarMultiplication) {
  // test scalar multiplication of all fixture vectors, accounting for floats
  // and using each constant defined below.
  float c1 = 1;
  float c0 = 0;
  float c2 = 2;
  float c3 = -3;
  Vect2 v = zero * c1;
  EXPECT_FLOAT_EQ(v.x, 0);
  EXPECT_FLOAT_EQ(v.y, 0);
  v = one * c1;
  EXPECT_FLOAT_EQ(v.x, 1);
  EXPECT_FLOAT_EQ(v.y, 1);
  v = v1 * c2;
  EXPECT_FLOAT_EQ(v.x, c2*v1.x);
  EXPECT_FLOAT_EQ(v.y, c2*v1.y);
  v = v2 * c2;
  EXPECT_FLOAT_EQ(v.x, c2*v2.x);
  EXPECT_FLOAT_EQ(v.y, c2*v2.y);
  v = v3 * c2;
  EXPECT_FLOAT_EQ(v.x, c2*v3.x);
  EXPECT_FLOAT_EQ(v.y, c2*v3.y);
  v = v1 * c3;
  EXPECT_FLOAT_EQ(v.x, c3*v1.x);
  EXPECT_FLOAT_EQ(v.y, c3*v1.y);
  v = v2 * c0;
  EXPECT_FLOAT_EQ(v.x, 0);
  EXPECT_FLOAT_EQ(v.y, 0);
  //test that order of scalar and vector doesnt matter
  v = c1*v2;
  EXPECT_FLOAT_EQ(v.x, c1*v2.x);
  EXPECT_FLOAT_EQ(v.y, c1*v2.y);
}

TEST_F(Vect2Fixture, PlusEquals) {
  //test += with test fixture vectors
  v1 += v2;
  EXPECT_FLOAT_EQ(v1.x, 4);
  EXPECT_FLOAT_EQ(v1.y, -2);
  //check that it doesnt change the second vector
  EXPECT_FLOAT_EQ(v2.x, 3);
  EXPECT_FLOAT_EQ(v2.y, -4);
  v2 += v3;
  EXPECT_FLOAT_EQ(v2.x, -2);
  EXPECT_FLOAT_EQ(v2.y, 2);
  //check that it doesnt change the third vector
  EXPECT_FLOAT_EQ(v3.x, -5);
  EXPECT_FLOAT_EQ(v3.y, 6);
}

//test projection
TEST_F(Vect2Fixture, Projection) {
  Vect2 xVect = Vect2(1, 0);
  Vect2 yVect = Vect2(0, 1);
  // expect projection to be zero
  EXPECT_NEAR(xVect.projection(yVect).x, 0, 1e-6);
  EXPECT_NEAR(xVect.projection(yVect).y, 0, 1e-6);
  // project v2 onto x and y and expect x and y components
  EXPECT_NEAR(v2.projection(xVect).x, v2.x, 1e-6);
  EXPECT_NEAR(v2.projection(yVect).y, v2.y, 1e-6);
  //projection onto zero vector should be zero
  EXPECT_NEAR(v2.projection(zero).x, 0, 1e-6);
  EXPECT_NEAR(v2.projection(zero).y, 0, 1e-6);
}

TEST_F(Vect2Fixture, Rotate) {
  //expect rotation of zero vector to be zero
  EXPECT_NEAR(zero.rotate(pi).x, 0, 1e-6);
  EXPECT_NEAR(zero.rotate(pi).y, 0, 1e-6);
  //expect 180 degree rotation of one vector to be -1
  EXPECT_NEAR(one.rotate(pi).x, -1, 1e-6);
  EXPECT_NEAR(one.rotate(pi).y, -1, 1e-6);
  //expect 90 degree rotation of v1 (1,2) to be (-2,1)
  EXPECT_NEAR(v1.rotate(pi/2).x, -2, 1e-6);
  EXPECT_NEAR(v1.rotate(pi/2).y, 1, 1e-6);
}
