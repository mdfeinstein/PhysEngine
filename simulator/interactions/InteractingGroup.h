#pragma once
#include "Simulator.h"

class Simulator; //forward declare

//template to accept additional args?
class InteractingGroup {
  public:
    InteractingGroup(Simulator& simulator, std::vector<int>& moverIds, std::function<void(Mover&, Mover&)> interaction);
    void applyInteractions();
    bool addMover(int moverId);
    bool removeMover(int moverId);
    int updateMoverIdx(int groupIdx); //search for moverId in simulator.movers and update simMoversIdx

    std::vector<int> moverIds;
  private:
    Simulator& simulator;
    std::vector<int> simMoversIdx; //indices of movers in simulator.movers. check first before searching
    std::function<void(Mover&, Mover&)> interaction;
    Mover& getMover(int groupIdx); //get mover from simulator.movers by index of moverIds/simMoversIdx
};

