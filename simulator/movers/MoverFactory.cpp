#include "MoverFactory.h"


MoverFactory::MoverFactory()
{
  // by default, we will register the known mover types and
  // assign their defaults. this can be overridden by registering
  // position, velocity, acceleration, radius, mass
  // Mover:
  MoverArgs moverDefaultArgs(Vect2(), Vect2(), Vect2(), 1, 0);
  registerMoverDefaults(typeid(Mover), moverDefaultArgs);
  registerMoverConstructor(typeid(Mover));
  // NewtMover:
  MoverArgs newtmoverDefaultArgs(Vect2(), Vect2(), Vect2(), 1, 1);
  registerMoverDefaults(typeid(NewtMover), newtmoverDefaultArgs);
  registerMoverConstructor(typeid(NewtMover));
  // RigidConnectedMover:
  MoverArgs rigidmoverDefaultArgs(Vect2(), Vect2(), Vect2(), 1, 1);
  registerMoverDefaults(typeid(RigidConnectedMover), rigidmoverDefaultArgs);
  registerMoverConstructor(typeid(RigidConnectedMover));
}

// define function to register mover types and default values
void MoverFactory::registerMoverDefaults(std::type_index type, MoverArgs defaultArgs)
{
  if (!defaultArgs.allFilled())
    throw std::invalid_argument("Default mover arguments not completely filled out");

  moverDefaults[type] = defaultArgs;
}

// define function to register interaction types and default values
void MoverFactory::registerInteraction(std::type_index interaction, std::vector<std::any> defaultInteractionArgs)
{
  interactionDefaults[interaction] = defaultInteractionArgs;
  reregisterKnownMoverConstructors();
}
// define function to create mover constructors and then register them
void MoverFactory::registerMoverConstructor(std::type_index type)
{
  // function should create a mover of the specified type, according to the given args and interaction params
  // and fill in the rest with the default values, and return a unique_ptr to the mover

  // first, check that the type is allowed. need to expand this as more mover types are added
  if ((type != typeid(Mover)) && (type != typeid(NewtMover)) && (type != typeid(RigidConnectedMover)))
  {
    throw std::invalid_argument("type doesn't match known Movers");
  }
  // client is expected to provide MoverArgs (which can be all blank)
  //  and an interActionParams map of type_index -> vectors<any> of interactionParams
  //  each vector represents an empty, partial, or complete list of arguments that
  //  that interaction will expect.
  // the map could be completely empty, partially or completely filled. default interaction params will
  // fill in where necessary
  // the main catch is that the lists for each interaction type must be specified in order until
  // the client has specified everything they want. this is much the same as
  // default args for a function.
  moverConstructors[type] = [this, type](MoverArgs args,
                                          std::unordered_map<std::type_index, std::vector<std::any>> interactionParams)
  {
    // apply default values for mover fields
    args.applyDefaults(this->moverDefaults[type]);

    std::unique_ptr<Mover> mover;
    // assign to mover ptr the mover of specific type.
    // need to hand specify all possibilities here
    if (type == typeid(Mover))
    {
      mover = std::make_unique<Mover>(args);
    }
    else if (type == typeid(NewtMover))
      mover = std::make_unique<NewtMover>(args);
    else if (type == typeid(RigidConnectedMover))
      mover = std::make_unique<RigidConnectedMover>(args, nullptr);
    else {
      throw(std::runtime_error("type of mover not known to factory. But passed register check somehow? check code for consistency"));
    }


    // add interaction params by looping thru map
    if (!this->interactionDefaults.empty()) {
    for (const auto &keyValue : this->interactionDefaults) { 
        // type : param list
        // if interactionParams doesn't have the key, fill it in with the default and move on
        if (interactionParams.find(keyValue.first) == interactionParams.end())
        {
          interactionParams[keyValue.first] = keyValue.second;
        }
        else
        { // apply "overhanging" default interaction param values
          auto &thiskeyParams = interactionParams[keyValue.first];
          int idx = thiskeyParams.size();
          while (idx < keyValue.second.size())
          {
            thiskeyParams.push_back(keyValue.second[idx]);
            ++idx;
          }
        }
      }
    }
    mover->interactionParams = std::move(interactionParams);
    // return unique_ptr to base mover using implicit upcasting
    return mover;
  };
}

void MoverFactory::reregisterKnownMoverConstructors() { // to be called when interactions are added
//loop through mover types in moverConstructors and call appropriate registerMoverConstructor
  for (auto const& [key, value] : moverConstructors) {
    registerMoverConstructor(key);
  }
};

std::unique_ptr<Mover> MoverFactory::createMover(std::type_index type, MoverArgs args,
    std::unordered_map<std::type_index, std::vector<std::any>> interactionParams) {
  if (moverConstructors.find(type) == moverConstructors.end())
    throw std::invalid_argument("MoverFactory::createMover: type not found");
    
  auto mover = moverConstructors[type](args, interactionParams);
  mover->id = current_mover_id;
  current_mover_id++;
  return mover;
};