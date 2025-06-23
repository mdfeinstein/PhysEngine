#pragma once
#include "Mover.h"
#include <any>
#include <vector>
#include <tuple>
#include <functional>
#include <utility>
#include <string>
#include <memory>


template<typename GlobalParamTuple, typename MoverParamTuple>
struct InteractionTemplated;

template <typename... GlobalParams, typename... MoverParams>
struct InteractionTemplated<std::tuple<GlobalParams...>, std::tuple<MoverParams...>> {
    std::tuple<GlobalParams...> globalParams;
    std::tuple<MoverParams...> moverParams1;
    std::tuple<MoverParams...> moverParams2;
    std::function<void(Mover&, Mover&, std::tuple<GlobalParams...>, std::tuple<MoverParams...>, std::tuple<MoverParams...>)> applyFunction;
    std::string name;

    InteractionTemplated(
        std::tuple<GlobalParams...> globalParams,
        std::function<void(Mover&, Mover&, std::tuple<GlobalParams...>, std::tuple<MoverParams...>, std::tuple<MoverParams...>)> applyFunction,
        std::string name)
        : globalParams(globalParams), applyFunction(applyFunction), name(name) {}

    void getMoverParams(Mover& mover1, Mover& mover2) {
        moverParams1 = std::any_cast<std::tuple<MoverParams...>>(mover1.effectParams[this->name]);
        moverParams2 = std::any_cast<std::tuple<MoverParams...>>(mover2.effectParams[this->name]);
    }

    void apply(Mover& mover1, Mover& mover2) {
        getMoverParams(mover1, mover2);
        applyFunction(mover1, mover2, globalParams, moverParams1, moverParams2);
    }
};

struct InteractionWrapper {
    // Type-erased wrapper for InteractionTemplated. Should implement apply.
    template <typename GlobalParamTuple, typename MoverParamTuple>
    InteractionWrapper(GlobalParamTuple globalParams,
        std::function<void(Mover&, Mover&, GlobalParamTuple, MoverParamTuple, MoverParamTuple)> applyFunction,
        std::string name)
        : conceptPtr(
            std::make_unique<Model<GlobalParamTuple, MoverParamTuple>>(
                InteractionTemplated<GlobalParamTuple, MoverParamTuple>(globalParams, applyFunction, name)
            )) {}

    void apply(Mover& mover1, Mover& mover2) {
        conceptPtr->apply(mover1, mover2);
    }

    std::string getName() {
        return conceptPtr->getName();
    }

private:
    struct Concept {
        virtual ~Concept() = default;
        virtual void apply(Mover& mover1, Mover& mover2) = 0;
        virtual std::string getName() = 0;
    };

    template <typename GlobalParamTuple, typename MoverParamTuple>
    struct Model : Concept {
        InteractionTemplated<GlobalParamTuple, MoverParamTuple> interaction;
        Model(InteractionTemplated<GlobalParamTuple, MoverParamTuple> interaction) : interaction(interaction) {}
        void apply(Mover& mover1, Mover& mover2) override {
            interaction.apply(mover1, mover2);
        }
        std::string getName() override {
            return interaction.name;
        }
    };

    std::unique_ptr<Concept> conceptPtr;
};
