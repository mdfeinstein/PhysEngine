#include "InteractingGroup.h"


InteractingGroup::InteractingGroup(Simulator& simulator, 
  std::vector<int>& moverIds, 
  std::function<void(Mover&, Mover&)> interaction) : simulator(simulator),
  moverIds(moverIds), interaction(interaction) {
  //grab simMoversIdx from moverIds and store it in simMoversIdx
  simMoversIdx.resize(moverIds.size());
  for (int groupIdx = 0; groupIdx < moverIds.size(); groupIdx++) { 
    updateMoverIdx(groupIdx);
  }
};

// might be worth multithreading this, depending on expected group sizes
// but test this way first for simplicity
void InteractingGroup::applyInteractions() {
  for (int i = 0; i < moverIds.size(); i++) {
    Mover& mover1 = getMover(i);
    for (int j = i+1; j < moverIds.size(); j++) {
      Mover& mover2 = getMover(j);
      interaction(mover1, mover2);
    }
  }
};

bool InteractingGroup::addMover(int moverId) {
  moverIds.push_back(moverId);
  simMoversIdx.push_back(-1); //placeholder value since updateMoverIdx will write to this position
  try {
    updateMoverIdx(moverIds.size() - 1);
  }
  catch (std::invalid_argument& e) { //if moverId not found in simulator.movers
    moverIds.pop_back();
    simMoversIdx.pop_back();
    return false;
  }
  return true;
};

bool InteractingGroup::removeMover(int moverId) {
  //find moverId in moverIds and remove it
  auto it = std::find(moverIds.begin(), moverIds.end(), moverId);
  if (it == moverIds.end()) return false;
  auto groupIdx = std::distance(moverIds.begin(), it);
  moverIds.erase(it);
  simMoversIdx.erase(simMoversIdx.begin() + groupIdx);
  return true;
};

Mover& InteractingGroup::getMover(int groupIdx) {
  // try to use simMoversIdx to get mover from simulator.movers
  int simMoverIdx = simMoversIdx[groupIdx];
  bool inRange = simMoverIdx >= 0 && simMoverIdx < simulator.movers.size();
  bool upToDate = false;
  if (inRange)
    upToDate = simulator.movers[simMoverIdx]->id == moverIds[groupIdx];
  if (inRange && upToDate)
    return *simulator.movers[simMoverIdx];
  else {
    simMoverIdx = updateMoverIdx(groupIdx);
    return *simulator.movers[simMoverIdx];
  }
};

int InteractingGroup::updateMoverIdx(int groupIdx) {
  int moverId = moverIds[groupIdx];
  auto it = simulator.find_mover(moverId);
  if (it == simulator.movers.end()) {
    throw std::invalid_argument("InteractingGroup: moverId "
      + std::to_string(moverIds[groupIdx]) + " not found in simulator.movers");
  }
  int simMoverIdx = it - simulator.movers.begin();
  simMoversIdx[groupIdx] = simMoverIdx;
  return simMoverIdx;
};