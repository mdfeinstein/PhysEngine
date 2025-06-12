#pragma once
#include "Simulator.h"
#include "SimulatorCommander.h"
#include <string>
#include <any>
#include <deque>
#include <memory>

//forward declare SimulatorCommander
class SimulatorCommander;

//Commands define possible actions that can be taken by the simulator
//CommandFactory is a factory class that creates commands based on a string name (or typeid)
//SimulatorCommander will hold a CommandFactory so that it can create commands
struct SimulatorCommand {
  // Interface for a command to be executed by the simulator
  // Implementers will need to define their own invoke function, which needs to be able to cast the args
  // into their types and call the appropriate function
  //Implementers should also define a name for identification purposes
  SimulatorCommand(const std::vector<std::any>& args)
    : args(args) {};
  
  void virtual invoke(Simulator& simulator) = 0;
  void virtual argParse() = 0; // check that args are valid and store the parsed values 
  std::string name;
  // function handle to call
  // arguments to pass to function
  std::vector<std::any> args;

  //utility functions
  std::vector<int> static getMoverIdsByPosition(Simulator& simulator, Vect2 position);
};

struct AddMoverCommand : SimulatorCommand {
  std::type_index type = typeid(Mover); //placeholder. validate will grab the right type
  MoverArgs moverArgs;
  std::unordered_map<std::type_index, std::vector<std::any>> interactionParams;

  AddMoverCommand(const std::vector<std::any>& args) 
    : SimulatorCommand(args) { 
    name = "AddMover";
    argParse();
  }
  void invoke(Simulator& simulator) override {
    simulator.add_mover(type, moverArgs, interactionParams);
  }
  void argParse() override {
    if (args.size() < 1 || args.size() > 3) {
      throw std::invalid_argument("AddMoverCommand: incorrect number of arguments. should be 1,2,or 3. Got "
        + std::to_string(args.size()));
    }
    //process first arg and declare defaults for others
    type = std::any_cast<std::type_index>(args[0]);
    moverArgs = MoverArgs();
    interactionParams = {};

    if (args.size() > 1) //process second arg if it exists
      moverArgs = std::any_cast<MoverArgs>(args[1]);
    

    if (args.size() == 3) //process third arg if it exists
      interactionParams = std::any_cast<std::unordered_map<std::type_index, std::vector<std::any>>>(args[2]);
  };
};

struct StepCommand : SimulatorCommand { //update simulator n times (1 by default)
  int steps = 1;

  StepCommand(const std::vector<std::any>& args) : SimulatorCommand(args) {   
    name = "Step";
    argParse();
  } //args are empty or contain a single int

  void invoke(Simulator& simulator) override {
    //throw if args < 1 or > 1, telling how many args expected and how many given
    simulator.update(steps);
  }
  void argParse() override {
    if (args.size() > 1) {
      throw std::invalid_argument("stepCommand: incorrect number of arguments. should be 1 or 2. Got "
        + std::to_string(args.size()));
    }
    if (args.size() == 1) steps = std::any_cast<int>(args[0]);
  }
};

struct deleteMoverCommand : SimulatorCommand {
  int id;
  deleteMoverCommand(const std::vector<std::any>& args) : SimulatorCommand(args) {
    name = "DeleteMover";
    argParse();
  }
  void invoke(Simulator& simulator) override {
    simulator.remove_mover(id);
  }
  void argParse() override {
    if (args.size() != 1) {
      throw std::invalid_argument("deleteMoverCommand: incorrect number of arguments. should be 1. Got "
        + std::to_string(args.size()));
    }
    id = std::any_cast<int>(args[0]);
  }
};

struct CreateGroup : SimulatorCommand {
  std::vector<int> moverIds;
  CreateGroup(const std::vector<std::any>& args) : SimulatorCommand(args) {
    name = "CreateGroup";
    argParse();
  }
  void invoke(Simulator& simulator) override {
    simulator.create_group(moverIds);
  }
  void argParse() override {
    if (args.size() != 1) {
      throw std::invalid_argument("CreateGroup: incorrect number of arguments. should be 1. Got "
        + std::to_string(args.size()));
    }
    moverIds = std::any_cast<std::vector<int>>(args[0]);
  }
};

struct Ungroup : SimulatorCommand {
  int id;
  Ungroup(const std::vector<std::any>& args) : SimulatorCommand(args) {
    name = "Ungroup";
    argParse();
  }
  void invoke(Simulator& simulator) override {
    auto it = simulator.find_mover(id);
    auto& moverPtr = (*it);
    if (typeid(*moverPtr) == typeid(RigidConnectedMover)) {
      RigidConnectedMover* mover = dynamic_cast<RigidConnectedMover*>(moverPtr.get());
      RigidConnectedGroup* group = mover->group;
      simulator.ungroup(group);
    }
  }

  void argParse() override {
    if (args.size() != 1) {
      throw std::invalid_argument("Ungroup: incorrect number of arguments. should be 1. Got "
        + std::to_string(args.size()));
    }
    id = std::any_cast<int>(args[0]);
  }
};

struct DeleteGroup : public SimulatorCommand {
  int id;
  DeleteGroup(const std::vector<std::any>& args) : SimulatorCommand(args) {
    name = "DeleteGroup";
    argParse();
  }
  void invoke(Simulator& simulator) override {
    auto it = simulator.find_mover(id);
    if (it == simulator.movers.end()) return; //exit early if mover not found
    auto& moverPtr = (*it);
    if (typeid(*moverPtr) == typeid(RigidConnectedMover)) {
      RigidConnectedMover* mover = dynamic_cast<RigidConnectedMover*>(moverPtr.get());
      if (mover == nullptr) return; //exit early if not a rigid connected mover
      RigidConnectedGroup* group = mover->group;
      simulator.delete_group(group);
    }
  }

  void argParse() override {
    if (args.size() != 1) {
      throw std::invalid_argument("DeleteGroup: incorrect number of arguments. should be 1. Got "
        + std::to_string(args.size()));
    }
    id = std::any_cast<int>(args[0]);
  }
};

template <std::size_t Index, typename... Args>
using NthType = typename std::tuple_element<Index, std::tuple<Args...>>::type;

template <typename... Args, std::size_t... I>
auto castArgs(std::vector<std::any>& args, std::index_sequence<I...>) {
    return std::make_tuple(std::any_cast<NthType<I, Args...>>(args[I])...);
}

template <typename... Args>
auto castArgs(std::vector<std::any>& args) {
    return castArgs<Args...>(args, std::index_sequence_for<Args...>{});
}

template <typename T, typename... Args>
T* make_instance_raw(std::tuple<Args...> args) {
    return std::apply([](auto&&... unpackedArgs) {
        return new T(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
    }, args);
}

template <class InteractionType, typename... InteractionArgs>
struct addInteraction : SimulatorCommand {
  std::vector<std::any> interaction_args;
  std::vector<std::any> default_params;

  addInteraction(const std::vector<std::any>& args) 
    : SimulatorCommand(args) { 
    name = "AddInteraction";
    argParse();
  }
  void invoke(Simulator& simulator) override {
    auto interaction_args_tuple = castArgs<InteractionArgs...>(interaction_args);
    InteractionType* interaction = make_instance_raw<InteractionType>(interaction_args_tuple);
    simulator.add_interaction(interaction, default_params);
  }
  void argParse() override {
    if (args.size()==1) {
      interaction_args = std::any_cast<std::vector<std::any>>(args[0]);
      default_params = {};
    }
    else if (args.size()==2) {
      interaction_args = std::any_cast<std::vector<std::any>>(args[0]);
      default_params = std::any_cast<std::vector<std::any>>(args[1]);
    }
    else {
      throw std::invalid_argument("AddInteraction: incorrect number of arguments. should be 1 or 2. Got "
        + std::to_string(args.size()) + ". args should be vector<any> or vector<any>, vector<any>");
    }
  };
};

template <class EffectType, typename... EffectArgs>
struct addEffect : SimulatorCommand {
  std::vector<std::any> effect_args;
  std::vector<std::any> default_params;

  addEffect(const std::vector<std::any>& args) 
    : SimulatorCommand(args) { 
    name = "AddEffect";
    argParse();
  }
  void invoke(Simulator& simulator) override {
    auto effect_args_tuple = castArgs<EffectArgs...>(effect_args); 
    EffectType* effect = make_instance_raw<EffectType>(effect_args_tuple);
    simulator.add_effect(effect, default_params); 
  }
  void argParse() override {
    if (args.size()==1) {
      effect_args = std::any_cast<std::vector<std::any>>(args[0]);
      default_params = {};
    }
    else if (args.size()==2) {
      effect_args = std::any_cast<std::vector<std::any>>(args[0]);
      default_params = std::any_cast<std::vector<std::any>>(args[1]);
    }
    else {
      throw std::invalid_argument("AddEffect: incorrect number of arguments. should be 1 or 2. Got "
        + std::to_string(args.size()) + ". args should be vector<any> or vector<any>, vector<any>");
    }
  };
};

struct AffectMover : public SimulatorCommand { 
  /*command that can directly affect a mover,
  such as applying a force, setting velocity,
  etc. Takes a function handle and a mover id as args
  The function will have to fully contain all the details, no args other than mover will be passed to it
  A lambda capturing the parameters should be the usual solution
  */
  std::function<void(Mover&)> funcToApply;
  int mover_id;

  AffectMover(const std::vector<std::any>& args) : SimulatorCommand(args) {
    name = "AffectMover";
    argParse();
  }

  void invoke(Simulator& simulator) override{
    auto it = simulator.find_mover(mover_id);
    if (it == simulator.movers.end()) return; //exit early if mover not found
    auto& mover = *(*it);
    funcToApply(mover);
  }
  void argParse() override {
    if (args.size() != 2) {
      throw std::invalid_argument("AffectMover: incorrect number of arguments. should be 2. Got "
        + std::to_string(args.size()));
    }
    funcToApply = std::any_cast<std::function<void(Mover&)>>(args[0]);
    mover_id = std::any_cast<int>(args[1]);
  };
};

//utility func implementations
std::vector<int> SimulatorCommand::getMoverIdsByPosition(Simulator& simulator, Vect2 position) {
  std::vector<int> moverIds;
  for (auto& mover : simulator.movers) {
    float distance = (mover->position - position).mag();
    if (distance < mover->radius) {
      moverIds.push_back(mover->id);
    }
  }
  return moverIds;
}