#pragma once
#include "Mover.h"
class Effect2 {
  //represents effects that apply to all movers, such as planetary gravity, damping, etc.

  //meant to be subclassed. Subclass must have fields for global parameters AND fields for
  // mover-specific parameters.

  // It must also implement parseGlobalParams and parseMoverParams, which will store the
  // parsed results in the appropriate member variables after any-casting to the correct type
  // paramCounts must also be set by subclass
  public:
  int globalParamCount;
  int moverParamCount;
  Effect2(int globalParamCount, int moverParamCount) : globalParamCount(globalParamCount), moverParamCount(moverParamCount) {}
  void virtual apply(Mover* mover) const { //doesnt change mover, except mutable mover specific parameter fields
  }
  void virtual parseGlobalParams(std::vector<std::any> params) = 0;
  void virtual parseMoverParams(std::vector<std::any> params) const = 0;
};
