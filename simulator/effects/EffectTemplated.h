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

struct EffectWrapper {
    // type erased wrapper for EffectTemplated. Should implement apply.

    template <typename... GlobalParams, typename... MoverParams>
    EffectWrapper(EffectTemplated<std::tuple<GlobalParams...>, std::tuple<MoverParams...>> effectTemplated) : 
        conceptPtr(std::make_unique<Model<std::tuple<GlobalParams...>, std::tuple<MoverParams...>>>(effectTemplated)) {};

    template <typename GlobalParamTuple, typename MoverParamTuple>
    EffectWrapper(GlobalParamTuple globalParams, 
        std::function<void(Mover&, GlobalParamTuple, MoverParamTuple)> applyFunction, 
        std::string name) : 
        conceptPtr(
            std::make_unique<Model<GlobalParamTuple, MoverParamTuple>>(
                EffectTemplated<GlobalParamTuple, MoverParamTuple>(globalParams, applyFunction, name)
            )) {};

    void apply(Mover& mover) {
        conceptPtr->apply(mover);
    }

    std::string getName() {
        return conceptPtr->getName();
    }


private:
    struct Concept {
        virtual ~Concept() = default;
        virtual void apply(Mover& mover) = 0;
        virtual std::string getName() = 0;
    };

    template <typename GlobalParamTuple, typename MoverParamTuple>
    struct Model : Concept {
        EffectTemplated<GlobalParamTuple, MoverParamTuple> effect;
        Model(EffectTemplated<GlobalParamTuple, MoverParamTuple> effect) : effect(effect) {};
        void apply(Mover& mover) override {
            effect.apply(mover);
        }
        std::string getName() override {
            return effect.name;
        }
    };

    std::unique_ptr<Concept> conceptPtr;

};