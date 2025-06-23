#include "Simulator.h"
/* 
Simulator holds the simulation objects with metadata. Facilitates interactions between objects.
Steps forward in time. Can hold recorders to record data.
Kinds of interacitons enabled are defined in this object
 */

// should changes to the simulator object all be behind the updateLock? 
// Currently I have the SimulatorCommander in place to process changes in a thread-safe manner
// but should the simulator itself have these protections?   
Simulator::Simulator(float dt) : global_dt(dt) {};

int Simulator::add_mover(std::type_index type, MoverArgs args, 
    std::unordered_map<std::type_index, std::vector<std::any>> interactionParams) {
    //id handled by factory now, should work without this
    // int id = current_id;
    // current_id++;
    auto mover = factory.createMover(type, args, interactionParams);
    // mover->id = id;
    int id = mover->id;
    movers.push_back(std::move(mover));
    return id;
}

bool Simulator::remove_mover(int id) {
    //assume ids are sorted, do binary search
    //if adding movers is done on multiple threads, will need to make this thread safe
    //first step would be to make current_id a std::atomic
    //but that is necessary anyway for thread safety, regardless of this binary search
    auto it = find_mover(id);
    if (it != movers.end()) {
        movers.erase(it); //mover is destroyed. RigidConnectedMover should remove itself from group
        return true;
    }
    return false;
}

bool Simulator::remove_movers(std::vector<int>& ids) {
    bool found_any=false;
    for (auto& id : ids) {
        auto it = find_mover(id);
        if (it != movers.end()) {
            movers.erase(it); //mover is destroyed. RigidConnectedMover should remove itself from group
            found_any=true;
        }
    }
    return found_any;
}

bool Simulator::replace_mover(int id, Mover* replacementMover) {
    // find mover with id, replace pointer with new mover
    // old mover is destroyed
    // bool signifies if replacement was successful
    auto it = find_mover(id);
    replacementMover->id = id;
    if (it != movers.end()) {
        (*it).reset(replacementMover);
        return true;
    }
    return false;
}

bool Simulator::replace_mover(int id, std::unique_ptr<Mover> replacementMover) {
    auto it = find_mover(id);
    replacementMover->id = id;
    if (it != movers.end()) {
        (*it).swap(replacementMover);
        return true;
    }
    return false;
}

void Simulator::add_wall(const Vect2& pointA, const Vect2& pointB) {
    //add wall to simulator
    walls.push_back(std::make_unique<Wall>(pointA, pointB));
}

void Simulator::create_group(std::vector<int>& mover_ids) {
    // for each mover, create RigidConnectedMover clone, replace in simulator->movers,
    // after all have been swapped, add to group

    std::vector<RigidConnectedMover*> connectedMovers;
    std::unordered_set<int> mover_ids_set(mover_ids.begin(), mover_ids.end());
    for (int id : mover_ids_set) {
        auto it = find_mover(id);
        if (it != movers.end()) {
            RigidConnectedMover* mover = new RigidConnectedMover(*(*it), nullptr);
            mover->id = id;
            connectedMovers.push_back(mover);
            (*it).reset(mover);
        }
    }
    groups.push_back(
        std::make_unique<RigidConnectedGroup>(connectedMovers)
    );
}

void Simulator::ungroup(RigidConnectedGroup* group) {
    // replace all movers in group with NewtMovers,
    // and remove group from simulator

    //get ids
    std::vector<int> mover_ids;
    std::vector<std::unique_ptr<Mover>> convertedMovers;
    for (auto& mover : group->movers) {
        mover_ids.push_back(mover->id);
        std::unique_ptr<Mover> clonedPtr = mover->cloneToNewtMover();
        convertedMovers.push_back(std::move(clonedPtr));  
    }
    //delete group using erase-remove idiom
    auto predicate = [group](std::unique_ptr<RigidConnectedGroup>& group_to_check) {
        return group_to_check.get() == group;
    };

    groups.erase(std::remove_if(groups.begin(), groups.end(), predicate), groups.end());
    //replace movers in simulator with NewtMovers
    for (int i = 0; i < mover_ids.size(); i++) {
        int id = mover_ids[i];
        auto& mover = convertedMovers[i];
        auto it = find_mover(id);
        if (it != movers.end()) {
            (*it).swap(mover);
        }
    }
};

void Simulator::delete_group(RigidConnectedGroup* group) {
    // deletes group and all movers in group from simulator
    //get ids
    std::vector<int> mover_ids;
    if (group == nullptr) return;
    for (auto& mover : group->movers) {
        mover_ids.push_back(mover->id);
    }
    ungroup(group);
    remove_movers(mover_ids);
};

void Simulator::add_interactingGroup(std::vector<int>& mover_ids, std::function<void(Mover&, Mover&)> interaction) {
    auto smartPtr = std::make_unique<InteractingGroup>(*this, mover_ids, interaction);
    interactingGroups.push_back(std::move(smartPtr));
}

void Simulator::update() {
    std::lock_guard<std::mutex> lock(updateLock);
    int item_count = movers.size();
    int thread_count = std::thread::hardware_concurrency(); 
    std::vector<std::future<void>> futures;
    int chunk_size = item_count / thread_count;
    for (int i_thread = 0; i_thread < thread_count; i_thread++) {
        int start = i_thread * chunk_size;
        int end = start + chunk_size;
        if (i_thread == thread_count - 1) {
            end = item_count;
        }
        futures.push_back(
            threadPool.enqueue([this, start, end](){
                for (int i = start; i < end; i++) {
                    Mover& mover1 = *movers[i];
                    for (int j = i+1; j < movers.size(); j++){
                        Mover& mover2 = *movers[j];
                        for (auto& interactionWrapper : interactionWrappers) {
                            interactionWrapper->apply(mover1, mover2);
                        }
                    }
                    for (auto& effectWrapper : effectWrappers) {
                        effectWrapper->apply(mover1);
                    }
                }
            })
        );
    }

    // sync threads back up, since dont want position/velocity updates before all forces are applied
    for (auto& future : futures) {
        future.get();
    }
    futures.clear();
    //update interacting groups
    for (auto& group : interactingGroups) {
        group->applyInteractions(); //should modify this to run multithreaded
    }

    //update movers using threadPool
    for (int i_thread = 0; i_thread < thread_count; i_thread++) {
        int start = i_thread * chunk_size;
        int end = start + chunk_size;
        if (i_thread == thread_count - 1) {
            end = item_count;
        }
        futures.push_back(
            threadPool.enqueue( [this, start, end](){
                for (int i = start; i < end; i++) {
                    Mover& mover = *movers[i];
                    bool reflected = false;
                    for (auto& wall : walls) {
                        reflected = wall->reflect(mover, global_dt);
                        if (reflected) break; //only reflect once
                    }
                    if (!reflected) //update didnt occured within wall->reflect
                        mover.update(global_dt);
                }
            })
        );
    }
    //rejoin threads befroe ending update
    for (auto& future : futures) {
        future.get(); 
    }
    current_time += global_dt;
}

void Simulator::update(int steps) {
    for (int i = 0; i < steps; i++) {
        update();
    }
}

void Simulator::update_unithread() {
    for (int i = 0; i < movers.size(); i++) { 
        Mover& mover1 = *movers[i];
        for (int j = i+1; j < movers.size(); j++){ 
            Mover& mover2 = *movers[j];
            for (auto& interactionWrapper : interactionWrappers) {
                interactionWrapper->apply(mover1, mover2);
            }
        }
        for (auto& effectWrapper : effectWrappers) {
            effectWrapper->apply(mover1);
        }
        mover1.update(global_dt);
    }
}    

std::vector<std::unique_ptr<Mover>>::iterator Simulator::find_mover(int id) {  
    //returns iterator to mover in movers with given id
    auto compare = [this](std::unique_ptr<Mover>& mover, int id) {
        return mover->id < id;
    };
    auto it = std::lower_bound(movers.begin(), movers.end(), id, compare);
    if (it != movers.end() && (*it)->id == id) {
        return it;
    }
    return movers.end();
}

void Simulator::reset() {
    //clear all objects and reset the timer
    movers.clear();
    walls.clear();
    effectWrappers.clear();
    interactionWrappers.clear();
    groups.clear();
    interactingGroups.clear();
    current_id = 0;
    current_time = 0;
    factory = MoverFactory();
}
