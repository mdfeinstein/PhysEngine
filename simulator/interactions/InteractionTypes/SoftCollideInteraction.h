#pragma once
#include "Mover.h"
#include "Interaction.h"
#include "Vect2.h"
#include <cmath>
#include <string>
#include <typeindex>
#include <any>
#include <vector>
#include <tuple>

class SoftCollide : public Interaction {
  //models collission by applying a repulsive force between movers
  //ONLY when they are overlapping
  // I think it will be a combo of a spring force and a 1/r^2 kind of repulsion,
  //with two global params controlling each and two mover params
    public:
      float globalSpringStrength = 1;
      float globalRepulsionStrength = 1;
      SoftCollide(float globalSpringStrength = 1, float globalRepulsionStrength = 1) 
        : globalSpringStrength(globalSpringStrength), globalRepulsionStrength(globalRepulsionStrength) {
          paramCount = 2;
        };
      void interact(Mover* mover1, Mover* mover2);
      std::any interpretParams(std::vector<std::any> params);
      std::tuple<float, float> paramsFromMover(Mover* mover); //expect two floats describing individual spring and repulsion strengths
};

void SoftCollide::interact(Mover* mover1, Mover* mover2){
  //move interaction logic here, isntead of inside mover
  Vect2 r = mover1->position - mover2->position;
  float magnitude = r.mag();
  magnitude = std::max(magnitude, min_distance);
  float activation_distance = mover1->radius + mover2->radius;
  if (magnitude < activation_distance) {
    auto [springStrength1, repulsionStrength1] = paramsFromMover(mover1);
    auto [springStrength2, repulsionStrength2] = paramsFromMover(mover2);

    float springMag = globalSpringStrength*springStrength1*springStrength2*abs(magnitude - activation_distance);
    float repulsionMag = globalRepulsionStrength*repulsionStrength1*repulsionStrength2*(1/pow(magnitude, 2));
    Vect2 force = (springMag + repulsionMag)*r/magnitude;
    mover1->apply_force(force);
    mover2->apply_force(-1*force);
  }
}

std::any SoftCollide::interpretParams(std::vector<std::any> params) {

  if (params.size() != paramCount) {
    throw std::invalid_argument("SoftCollide::interpretParams: incorrect number of parameters. Expected " + std::to_string(paramCount) + " parameter(s).");
  }
  std::tuple<float, float> strengths = std::make_tuple(std::any_cast<float>(params[0]), std::any_cast<float>(params[1]));
  return strengths;
}

std::tuple<float, float> SoftCollide::paramsFromMover(Mover* mover) {
  std::any params = interpretParams(mover->interactionParams[typeid(SoftCollide)]);
  return std::any_cast<std::tuple<float, float>>(params);
}