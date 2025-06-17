#pragma once
#include "Mover.h"
#include <any>
#include <vector>
#include <tuple>
#include <functional>
#include <utility>

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
    
    EffectTemplated(std::tuple<GlobalParams...> globalParams, 
        std::function<void(Mover&, 
            std::tuple<GlobalParams...>, 
            std::tuple<MoverParams...>)> applyFunction) :
        globalParams(globalParams), applyFunction(applyFunction) {};
        
    void getMoverParams(Mover& mover) {
        moverParams = unpack_any_vector_impl<MoverParams...>(mover.params, 
            std::index_sequence_for<MoverParams...>{});
    }
    
    void apply(Mover& mover){
        getMoverParams(mover);
        applyFunction(mover, globalParams, moverParams);
    }
};
