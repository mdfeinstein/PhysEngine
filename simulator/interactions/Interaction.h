# pragma once
#include "Vect2.h"
#include "Mover.h"
#include <any>
#include<vector>

/* 
Mediator factory class that defines interactions amongst all combinations of simulation objects, typically
returning a force vector.
By default, the interaction does nothing, and specific interactions can override that behavior by inheriting from Interaction
This seems to be a case of the Command pattern.
*/

class Interaction {
    public:
    void virtual interact(Mover*, Mover*);
    float min_distance = 1;
    int paramCount = 0;
    private:
    std::any virtual interpretParams(std::vector<std::any> params);
    
};
