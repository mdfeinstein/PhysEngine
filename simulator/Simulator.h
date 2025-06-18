#pragma once
#include "Mover.h"
#include "Interaction.h"
#include "MoverFactory.h"
#include "Vect2.h"
#include "Effect.h"
#include "EffectTemplated.h"
#include "RigidMovers.h"
#include "InteractingGroup.h"
#include "Wall.h"
#include <vector>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <thread>
#include "ThreadGuard.h"
#include "ThreadPool.h"
#include <functional>

/* 
Simulator holds the simulation objects with metadata. Facilitates interactions between objects.
Steps forward in time. Can hold recorders to record data.
Kinds of interacitons enabled are defined in this object
 */
//forward declarations
class InteractingGroup;

class Simulator {
public:
    float global_dt;
    int current_id = 0;
    float current_time = 0;
    MoverFactory factory = MoverFactory();
    std::vector< std::unique_ptr<Mover>> movers;
    std::vector< std::unique_ptr<Wall>> walls;
    std::vector< std::unique_ptr<Interaction>> interactions;
    std::vector< std::unique_ptr<Effect>> effects;
    std::vector< std::function<void(Mover&)>> effectApplyFunctions;
    std::vector< std::unique_ptr<RigidConnectedGroup> > groups;
    std::vector<std::unique_ptr<InteractingGroup>> interactingGroups;
    float interaction_min_distance = 1;
    ThreadPool threadPool = ThreadPool(std::thread::hardware_concurrency());

    Simulator(float dt);

    int add_mover(std::type_index type, MoverArgs args = MoverArgs(), 
        std::unordered_map<std::type_index, std::vector<std::any>> interactionParams
         = std::unordered_map<std::type_index, std::vector<std::any>>()); //returns id of mover
    bool remove_mover(int id);
    bool remove_movers(std::vector<int>& ids);
    bool replace_mover(int id, Mover* mover);
    bool replace_mover(int id, std::unique_ptr<Mover> mover);
    void create_group(std::vector<int>& mover_ids);
    void ungroup(RigidConnectedGroup* group);
    void delete_group(RigidConnectedGroup* group);
    void add_interactingGroup(std::vector<int>& mover_ids, std::function<void(Mover&, Mover&)> interaction);
    void remove_interactingGroup(int id);
    void remove_interactingGroupByMoverId(int moverId); //find group with moverId and remove it
    void add_interaction(Interaction* interaction, std::vector<std::any> default_params = std::vector<std::any>());
    void add_effect(Effect* effect, std::vector<std::any> default_params = std::vector<std::any>());
    
    template<typename GlobalParamTuple, typename MoverParamTuple>
    void add_effectFunction(
        std::function<void(Mover&, GlobalParamTuple, MoverParamTuple)> effectFunction,
        std::string name,
        GlobalParamTuple globalParams,
        MoverParamTuple moverParamsDefault
    );
    
    void add_wall(const Vect2& pointA, const Vect2& pointB);
    void update();
    void update(int steps);
    void update_unithread();
    std::vector<std::unique_ptr<Mover>>::iterator find_mover(int id); // returns iterator to mover with id
    void reset();
    private:
    std::mutex updateLock; //ensure only one thread can trigger an update at a time
};

template<typename GlobalParamTuple, typename MoverParamTuple>
void Simulator::add_effectFunction(
    std::function<void(Mover&, GlobalParamTuple, MoverParamTuple)> effectFunction,
    std::string name,
    GlobalParamTuple globalParams,
    MoverParamTuple moverParamsDefault
){
    // construct EffectTemplated object
    EffectTemplated<GlobalParamTuple, MoverParamTuple> effectTemplated(
        globalParams, effectFunction, name);
    // get function pointer to apply function
    auto applyFunction = effectTemplated.getApplyFunction();
    // add function pointer to simulator 
    effectApplyFunctions.push_back(applyFunction);
    // register effect and defaults with factory
    factory.registerEffect(name, moverParamsDefault); //implicit cast from tuple to any works?
}