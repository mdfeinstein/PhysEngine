# pragma once
# include <cmath>
# include <iostream>
const float pi = 3.14159265358979323846f; // pi constant
class Vect2 {
    public:
        float x, y;
        // Constructors
        Vect2(); //default constructor creates zero vector Vect2() : 
        Vect2(float x, float y); //constructor with x and y
        // Operators
        //// self operators
        float mag() const;
        float angle() const;
        void polar_set(float mag, float angleDeg);
        //// vector binary operations
        Vect2 operator+(Vect2 b) const;
        Vect2& operator+=(Vect2 b);

        Vect2 operator-(Vect2 b) const;
        Vect2 operator*(float scalar) const;
        Vect2 operator/(float scalar) const;

        bool operator==(Vect2 v) const;
        //vector multiplications
        Vect2 operator*(Vect2 v2) const;

        float dot(Vect2 b) const;
        float cross(Vect2 b) const;
        Vect2 projection(Vect2 b) const;
        Vect2 rotate(float angle) const;
};
//overloading output operator
std::ostream& operator<< (std::ostream& os, const Vect2& v);
Vect2 operator*(float scalar, Vect2 vector);
