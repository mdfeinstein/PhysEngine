#pragma once
#include "Simulator.h"
#include "SimulatorCommand.h"
#include "SpringInteraction.h"
#include "CoulombInteraction.h"
#include "Gravity.h"
#include "SoftCollideInteraction.h"
#include "LorentzEffect.h"
#include "Drag.h"

#include <string>
#include <any>
#include <deque>
#include <memory>
#include <mutex>

//forward declare SimulatorCommand
class SimulatorCommand;

class SimulatorCommander
// Handles commands to control the simulator using a command queue
{
public:
	SimulatorCommander(Simulator& simulator);
  template <typename CommandType>
  void addCommand(const std::vector<std::any>& args); //could also take typeid instead of name
  void addCommandAddMover(std::type_index type, MoverArgs args = MoverArgs(), 
    std::unordered_map<std::type_index, std::vector<std::any>> interactionParams = {});
  void addCommandDeleteMover(int id);
  void addCommandStep(int steps = 1);
  void addCommandCreateGroup(std::vector<int> moverIds);
  void addCommandUngroup(int id);
  void addCommandDeleteGroup(int id);
  template <class InteractionType, typename... InteractionArgs>
  void addCommandAddInteraction(std::vector<std::any> interaction_args, std::vector<std::any> default_params = {});
  void addCommandAddSpring(float k = 1, float x0 = 100);
  void addCommandAddCoulomb(float k = 1, float charge = 0);
  void addCommandAddSoftCollide(float globalSpringStrength = 1, float globalRepulsionStrength = 1,
    float defaultMoverSpringStrength = 1, float defaultMoverRepulsionStrength = 1); //"default" params are for mover properties 
  void addCommandAddGravity(float G = 1);
  template <class EffectType, typename... EffectArgs>
  void addCommandAddEffect(std::vector<std::any> effect_args, std::vector<std::any> default_params = {});
  void addCommandAddLorentzEffect(float magneticStrength=1, float defaultCharge = 0);
  void addCommandAddDragEffect(float strength=1, float default_coeff = 1);
  void addCommandAffectMover(std::function<void(Mover&)> funcToApply, int mover_id);
  void invokeCommand();
  void update();
  void runSimulator();
  void pauseSimulator();
  void togglePause();
  
  Simulator& simulator; //perhaps this should own the simulator with shared ptr

private:
  std::deque<std::unique_ptr<SimulatorCommand>> commandQueue;
  //lock for the queue
  std::mutex commandQueueMutex;
  std::mutex updateMutex;
	bool running = false; //possibly should be atomic for thread saftey?
};

SimulatorCommander::SimulatorCommander(Simulator& simulator) : simulator(simulator) {};


template <typename CommandType>
void SimulatorCommander::addCommand(const std::vector<std::any>& args) {
  //acquire lock on queue so only one command is added at a time, and it will wait until update call is finished
  std::lock_guard<std::mutex> lock(commandQueueMutex);
  auto commandPtr = std::make_unique<CommandType>(args);
  commandQueue.push_back(std::move(commandPtr));
};

void SimulatorCommander::addCommandAddMover(std::type_index type, MoverArgs args, 
  std::unordered_map<std::type_index, std::vector<std::any>> interactionParams) {
  addCommand<AddMoverCommand>({type, args, interactionParams});
};

void SimulatorCommander::addCommandDeleteMover(int id) {
  addCommand<deleteMoverCommand>({id});
};

void SimulatorCommander::addCommandStep(int steps) {
  addCommand<StepCommand>({steps});
};

void SimulatorCommander::addCommandCreateGroup(std::vector<int> moverIds) {
  addCommand<CreateGroup>({moverIds});
};

void SimulatorCommander::addCommandUngroup(int id) {
  addCommand<Ungroup>({id});
};

void SimulatorCommander::addCommandDeleteGroup(int id) {
  addCommand<DeleteGroup>({id});
};

template <class InteractionType, typename... InteractionArgs>
void SimulatorCommander::addCommandAddInteraction(std::vector<std::any> interaction_args, std::vector<std::any> default_params) {
  addCommand<addInteraction<InteractionType, InteractionArgs...>>({interaction_args, default_params});
}

void SimulatorCommander::addCommandAddSpring(float k, float x0) {
  addCommandAddInteraction<Spring, float, float>({k, x0}, {});
}

void SimulatorCommander::addCommandAddCoulomb(float k, float charge) {
  addCommandAddInteraction<Coulomb, float>({k}, {charge});
}

void SimulatorCommander::addCommandAddSoftCollide(float globalSpringStrength, float globalRepulsionStrength,
  float defaultMoverSpringStrength, float defaultMoverRepulsionStrength) {
  addCommandAddInteraction<SoftCollide, float, float>({globalSpringStrength, globalRepulsionStrength},
     {defaultMoverSpringStrength, defaultMoverRepulsionStrength});
}

void SimulatorCommander::addCommandAddGravity(float G) {
  addCommandAddInteraction<Gravity, float>({G}, {});
}

template<typename EffectType, typename... EffectArgs>
void SimulatorCommander::addCommandAddEffect(std::vector<std::any> effect_args, std::vector<std::any> default_params) {
  addCommand<addEffect<EffectType, EffectArgs...>>({effect_args, default_params});
}

void SimulatorCommander::addCommandAddDragEffect(float strength, float default_coeff) {
  addCommandAddEffect<Drag, float>({strength}, {default_coeff});
}

void SimulatorCommander::addCommandAddLorentzEffect(float magneticStrength, float defaultCharge) {
  addCommandAddEffect<LorentzEffect, float>({magneticStrength}, {defaultCharge});
}

void SimulatorCommander::addCommandAffectMover(std::function<void(Mover&)> funcToApply, int mover_id) {
  addCommand<AffectMover>({funcToApply, mover_id});
}


void SimulatorCommander::invokeCommand() {
  if (commandQueue.empty()) {
    return;
  }
  auto& commandPtr = commandQueue.front();
  commandPtr->invoke(simulator);
  commandQueue.pop_front();
};

void SimulatorCommander::update() { 
  std::lock_guard<std::mutex> lock(updateMutex);
  {//acquire lock on queue during invokes
    std::lock_guard<std::mutex> lock(commandQueueMutex);
    while (!commandQueue.empty()) {
      //may need to limit this to a certain number of times per update, or it could drop the frames
      invokeCommand();
    }
  }//release lock during simulator update so commands can be queued

  if (running) simulator.update();
};

void SimulatorCommander::runSimulator() {
  running = true;
};

void SimulatorCommander::pauseSimulator() {
  running = false;
};

void SimulatorCommander::togglePause() {
  if (running) pauseSimulator();
  else runSimulator();
};

