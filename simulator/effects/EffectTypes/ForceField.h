#pragma once
#include "Effect.h"
#include <functional>
#include "Vect2.h"

class ForceField : public Effect {
public:
    std::function<Vect2(Vect2)> fieldFunction;

    ForceField(std::function<Vect2(Vect2)> func)
        : fieldFunction(func) {
        paramCount = 1; // Expects one parameter (e.g., scale factor)
    }

    void apply(Mover* mover) override {
        Vect2 position = mover->position;
        Vect2 fieldForce = fieldFunction(position);

        // Retrieve mover-specific scale factor from interactionParams
        float scale = paramsFromMover(mover);

        // Apply the scaled force to the mover
        mover->apply_force(fieldForce * scale);
    }

    std::any interpretParams(std::vector<std::any> params) override { 
        if (params.size() != paramCount) {
            throw std::invalid_argument("ForceField expects exactly 1 parameter (scale factor).");
        }
        return std::any_cast<float>(params[0]);
    }

    float paramsFromMover(Mover* mover) {
        auto it = mover->interactionParams.find(typeid(ForceField));
        if (it == mover->interactionParams.end()) {
            throw std::invalid_argument("ForceField::paramsFromMover: missing interaction parameters for ForceField.");
        }
        std::any params = interpretParams(it->second);
        return std::any_cast<float>(params);
    }
};
