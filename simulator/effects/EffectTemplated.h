#pragma once
#include "Mover.h"
#include <any>
#include <vector>
#include <tuple>
#include <functional>
#include <utility>
#include <string>

template <typename... Args, size_t... I>
std::tuple<Args...> unpack_any_vector_impl(const std::vector<std::any>& args, std::index_sequence<I...>) {
    return std::make_tuple(std::any_cast<Args>(args[I])...);
}

template<typename GlobalParamTuple, typename MoverParamTuple>
struct EffectTemplated;

template <typename... GlobalParams, typename... MoverParams>
struct EffectTemplated<std::tuple<GlobalParams...>, std::tuple<MoverParams...>> {
    std::tuple<GlobalParams...> globalParams;
    std::tuple<MoverParams...> moverParams;
    std::function<void(Mover&, std::tuple<GlobalParams...>, 
        std::tuple<MoverParams...>)> applyFunction;
    std::string name;
    
    EffectTemplated(std::tuple<GlobalParams...> globalParams, 
        std::function<void(Mover&, 
            std::tuple<GlobalParams...>, 
            std::tuple<MoverParams...>)> applyFunction, 
            std::string name) :
        globalParams(globalParams), applyFunction(applyFunction), name(name) {};
        
    void getMoverParams(Mover& mover) {
        // moverParams = unpack_any_vector_impl<MoverParams...>(
        //   mover.effectParams[this->name], // get vector of (any) params from mover  
        //     std::index_sequence_for<MoverParams...>{});
        moverParams = std::any_cast<std::tuple<MoverParams...>>(mover.effectParams[this->name]);
    }
    
    void apply(Mover& mover){
        getMoverParams(mover);
        applyFunction(mover, globalParams, moverParams);
    }

    std::function<void(Mover&)> getApplyFunction() {
      // capture a copy of this object in a lambda
      // which will call apply on mover. return the lambda
      return [effect=*this](Mover& mover) mutable {
        effect.apply(mover);
      };
    }
};
