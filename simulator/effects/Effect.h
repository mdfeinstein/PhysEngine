#pragma once
#include "Mover.h"
class Effect {
  //represents effects that apply to all movers, such as planetary gravity, damping, etc.
  public:
  int paramCount = 0;
  void virtual apply(Mover* mover) {
  }
  std::any virtual interpretParams(std::vector<std::any> params);
};

inline std::any Effect::interpretParams(std::vector<std::any> params) {
    // check param list for completeness and correctness
    // return a tuple wrapped in an any
    // effect method will need to know how to interpret this

    //by default, correct interpretParams is that it is empty
    //so check that here
    if (params.size() != paramCount) {
        throw std::invalid_argument("Effect::interpretParams: incorrect number of parameters. Expected " + 
                                    std::to_string(paramCount) + " parameter(s).");
    }
    return std::any();
}