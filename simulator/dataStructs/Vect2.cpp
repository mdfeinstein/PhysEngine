#include "Vect2.h"

// Constructors
Vect2::Vect2() : x(0), y(0) {}; //default constructor creates zero vector Vect2() : 
Vect2::Vect2(float x, float y) : x(x), y(y) {}; //constructor with x and y
// Operators

//// self operators
float Vect2::mag() const {return sqrt( pow(x, 2) + pow(y, 2) );}; //vector magnitude
float Vect2::angle() const {return atan2( y, x );}; //vector angle, radians
void Vect2::polar_set(float mag, float angleDeg) { //set x and y based on polar values
    const float pi = 3.14159265358979323846;
    float angleRad = angleDeg * (pi / 180);
    x = mag * cos(angleRad);
    y = mag * sin(angleRad);
};

//// vector binary operations
Vect2 Vect2::operator+(Vect2 b) const {
    float x3, y3;
    x3 = this->x + b.x;
    y3 = this->y + b.y;
    Vect2 v3 = Vect2(x3, y3);
    return v3; 
};

Vect2& Vect2::operator+=(Vect2 b) { 
    float x3, y3;
    x3 = this->x + b.x;
    y3 = this->y + b.y;
    this->x = x3;
    this->y = y3;
    return *this; 
};

Vect2 Vect2::operator-(Vect2 b) const { 
    float x3, y3;
    x3 = this->x - b.x;
    y3 = this->y - b.y;
    Vect2 v3 = Vect2(x3, y3);
    return v3; 
};
Vect2 Vect2::operator*(float scalar) const { //scalar mulitplication
    float x3, y3;
    x3 = this->x * scalar;
    y3 = this->y * scalar;
    Vect2 v3 = Vect2(x3, y3);
    return v3; 
};

Vect2 Vect2::operator/(float scalar) const { //scalar division
    float x3, y3;
    x3 = this->x / scalar;
    y3 = this->y / scalar;
    Vect2 v3 = Vect2(x3, y3);
    return v3; 
};

bool Vect2::operator==(Vect2 v) const {
    return (x == v.x && y == v.y);
};
//vector multiplications
Vect2 Vect2::operator*(Vect2 v2) const {
    float x3, y3;
    x3 = this->x * v2.x;
    y3 = this->y * v2.y;
    Vect2 v3 = Vect2(x3, y3);
    return v3; 
};

float Vect2::dot(Vect2 b) const{
    return x * b.x + y * b.y;
};
float Vect2::cross(Vect2 b) const { //cross product magnitude
    return x*b.y - y*b.x;
};
Vect2 Vect2::projection(Vect2 b) const { //returns projected vector of this onto b
if (b.mag() == 0) return Vect2(0,0); // projecting onto zero vector is zero
float r = this->mag();
float theta = this->angle() - b.angle();
return b * r*cos(theta) / b.mag();
}
Vect2 Vect2::rotate(float angle) const { //return rotated vector. Angle in radians
float x_new, y_new;
// float theta = this->angle();
x_new = x*cos(angle) - y*sin(angle);
y_new = x*sin(angle) + y*cos(angle);
return Vect2(x_new, y_new);
};
//overloading output operator
std::ostream& operator<< (std::ostream& os, const Vect2& v) {
    os << '(' << v.x << ','<<v.y<<')';
    return os;
}

Vect2 operator*(float scalar, Vect2 vector) {
    float newx = scalar * vector.x;
    float newy = scalar * vector.y;
    Vect2 result = Vect2(newx, newy);
    return result;
};
