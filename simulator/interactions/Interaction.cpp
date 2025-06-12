#include "Interaction.h"

/* 
Mediator factory class that defines interactions amongst all combinations of simulation objects, typically
returning a force vector.
By default, the interaction does nothing, and specific interactions can override that behavior by inheriting from Interaction
This seems to be a case of the Command pattern.
*/


void Interaction::interact(Mover*, Mover*) {
}

std::any Interaction::interpretParams(std::vector<std::any> params) {
    // check param list for completeness and correctness
    // return a tuple wrapped in an any
    // itneraction method will need to know how to interpret this

    //by default, correct interpretParams is that it is empty
    //so check that here
    if (params.size() != paramCount) {
        throw std::invalid_argument("Interaction::interpretParams: incorrect number of parameters. Expected " + std::to_string(paramCount) + " parameter(s).");
    }
    return std::any();
}
