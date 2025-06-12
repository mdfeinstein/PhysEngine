#pragma once
#include "Mover.h"
#include "RigidMovers.h"
#include "Interaction.h"
#include <vector>
#include <any>
#include <typeindex>
#include <unordered_map>
#include <functional>
#include <memory>

class MoverFactory
{
  // handles the construction of different mover types, including the
  // application of primary parameters as well as interaction
  // parameters. Holds default values for movers as well as interaction parameters
  // will hvae maps of mover types to vectors of default values
  // and maps of interaction types to vectors of default values
  // and map of types to contructors.
  // has public function to create movers as well as to add interactions and set their defaults,
  // as well as to set mover defaults, and to register mover constructors
public:


  MoverFactory();

  // define function to register mover types and default values
  void registerMoverDefaults(std::type_index type, MoverArgs defaultArgs);

  // define function to register interaction types and default values
  void registerInteraction(std::type_index interaction, 
    std::vector<std::any> defaultInteractionArgs);

  // define function to create mover constructors and then register them
  void registerMoverConstructor(std::type_index type);
  std::unique_ptr<Mover> createMover(std::type_index type, MoverArgs args = MoverArgs(),
    std::unordered_map<std::type_index, std::vector<std::any>> interactionParams={});

  private:

  void reregisterKnownMoverConstructors();
  std::unordered_map<std::type_index, MoverArgs> moverDefaults;                   // default args for each kind of mover
  std::unordered_map<std::type_index, std::vector<std::any>> interactionDefaults; // default args for each kind of interaction
  // constructors shoudl take btoh MvoerArgs and interaction params as args. interaction params will override the defaults as far as they are defined
  std::unordered_map<std::type_index, std::function<std::unique_ptr<Mover>(MoverArgs,
                                                                           std::unordered_map<std::type_index, std::vector<std::any>> interactionParams)>>
      moverConstructors;
  int current_mover_id = 0;
};
