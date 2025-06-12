#pragma once
#include "Vect2.h"
#include "Mover.h"
#include <cmath>
#include <optional>

class Wall {
public:
    Vect2 pointA;
    Vect2 pointB;

    Wall(Vect2 pointA, Vect2 pointB);

    bool checkCollision(const Mover& mover) const;
    bool reflect(Mover& mover, float dt) const;
private:
  std::optional<Vect2> InterceptPoint(Vect2 originalPosition,
    Vect2 nextPosition) const;
  
};

// Implementation
inline Wall::Wall(Vect2 pointA, Vect2 pointB)
    : pointA(pointA), pointB(pointB) {}

inline bool Wall::checkCollision(const Mover& mover) const {
    //determines if there will be a collision with the wall
    return InterceptPoint(mover.position, mover.position + mover.velocity).has_value();
}

inline bool Wall::reflect(Mover& mover, float dt) const {
    Vect2 currentPosition = mover.position;
    Vect2 currentVelocity = mover.velocity;
    auto [nextPosition, nextVelocity, _] = mover.next_vecs(dt);
    auto intercept = InterceptPoint(currentPosition, nextPosition);
    if (!intercept.has_value()) {
        return false; // No collision, no reflection
    }

    // Calculate the wall's normal vector
    Vect2 wallVector = pointB - pointA;
    Vect2 wallNormal= wallVector.rotate(pi/2); // Perpendicular to the wall
    wallNormal = wallNormal / wallNormal.mag(); // Normalize the normal vector

    // Reflect the velocity using the formula: v' = v - 2 * (v · n) * n
    float dotProduct = currentVelocity.dot(wallNormal);
    Vect2 reflectedVelocity = currentVelocity - wallNormal * (2 * dotProduct);

    // Calculate the time spent traveling to the intercept point
    float distanceToIntercept = (intercept.value() - currentPosition).mag();
    float timeToIntercept = distanceToIntercept / currentVelocity.mag();

    // Ensure the remaining time is non-negative
    float remainingTime = std::max(0.0f, dt - timeToIntercept);

    // Calculate the new position after reflection
    Vect2 newPosition = intercept.value() + reflectedVelocity * remainingTime;

    // Update the mover's position and velocity
    mover.position = newPosition;
    mover.velocity = reflectedVelocity;
    return true; // Reflection occurred
}

inline std::optional<Vect2> Wall::InterceptPoint(Vect2 originalPosition,
 Vect2 nextPosition) const {
    Vect2 r = nextPosition - originalPosition;
    Vect2 s = pointB - pointA;

    float denominator = r.cross(s);
    if (denominator == 0) {
        // Lines are parallel or collinear
        return std::nullopt;
    }

    Vect2 qp = pointA - originalPosition;
    float t = qp.cross(s) / denominator;
    float u = qp.cross(r) / denominator;

    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        // Intersection point is within the bounds of both line segments
        return originalPosition + r * t;
    }

    return std::nullopt; // No intersection within the bounds
}
