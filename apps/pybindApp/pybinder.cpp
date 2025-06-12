#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include "Vect2.h"
#include "Mover.h"
#include "RigidMovers.h"
#include "Simulator.h"
#include "Interaction.h"
#include "SpringInteraction.h"
#include "CoulombInteraction.h"
#include "SoftCollideInteraction.h"
#include "LorentzEffect.h"
#include "Drag.h"
#include "InteractingGroup.h"
#include "launchWidget.h"
#include "SimulatorCommand.h"
#include "SimulatorCommander.h"


namespace py = pybind11;




void report_mover_positions(Simulator& sim) {
    for (auto& mover : sim.movers) {
        std::cout << mover->id << ": " << mover->position.x << ", " << mover->position.y << std::endl;
    }
};

Vect2 get_mover_position(Simulator& sim, int id) {
    return sim.movers[id]->position;
};

Vect2 get_mover_velocity(Simulator& sim, int id) {
    return sim.movers[id]->velocity;
};


void add_spring_interaction(Simulator& sim, float k, float x0) {
    sim.add_interaction(new Spring(k, x0));
};

void add_coulomb_interaction(Simulator& sim, float k) {
    sim.add_interaction(new Coulomb(k), {0.0f}); //default charge is zero
};

void add_lorentz_effect(Simulator& sim, float magneticStrength) {
    sim.add_effect(new LorentzEffect(magneticStrength), {0.0f}); //default charge is zero
};

void add_drag(Simulator& sim, float dragStrength) {
    sim.add_effect(new Drag(dragStrength), {1.0f});
};

void add_softCollide_interaction(Simulator& sim, float springStrength, float repulsionStrength) {
    sim.add_interaction(new SoftCollide(springStrength, repulsionStrength), 
    {1.0f,1.0f} //default collssion params for each mover 
    );
};

void add_springGroup(Simulator& sim, float k, float x0, std::vector<int> moverIds) {
    std::function<void(Mover&, Mover&)> interaction = [k, x0](Mover& mover1, Mover& mover2) {
        Vect2 r = mover1.position - mover2.position;
        float magnitude = r.mag();
        float springForceMag = -k*(magnitude - x0);
        Vect2 springForce = springForceMag * r/magnitude;
        mover1.apply_force(springForce);
        mover2.apply_force(-1*springForce);
    };
    sim.add_interactingGroup(moverIds, interaction);
};

std::unordered_map<std::string, std::type_index> type_map = {
    {"Mover", typeid(Mover)},
    {"NewtMover", typeid(NewtMover)},
    {"RigidConnectedMover", typeid(RigidConnectedMover)},
    {"Interaction", typeid(Interaction)},
    {"Spring", typeid(Spring)},
    {"Coulomb", typeid(Coulomb)},
    {"SoftCollide", typeid(SoftCollide)},
    {"LorentzEffect", typeid(LorentzEffect)},
    {"DragEffect", typeid(Drag)} 
};

std::type_index resolve_type_index(const std::string& type_name) {
    auto it = type_map.find(type_name);
    if (it == type_map.end()) {
        throw std::runtime_error("Unknown type: " + type_name);
    }
    return it->second;
};

std::any py_to_any(py::object py_val) {
    if (py::isinstance<py::int_>(py_val)) {
        return py_val.cast<int>();
    } else if (py::isinstance<py::float_>(py_val)) {
        return py_val.cast<float>();
    } else if (py::isinstance<py::str>(py_val)) {
        return py_val.cast<std::string>();
    } else {
        // Store unknown types as py::object in std::any
        return py_val;
    }
};

int add_mover(Simulator& sim, std::string type, MoverArgs args, 
    std::unordered_map<std::string, std::vector<py::object>> interactionParams = {}) {
    //convert strings in interacitonParams to typeid and then create interactionParams map
    std::unordered_map<std::type_index, std::vector<std::any>> interactionParams2;

    for (auto& [key, value] : interactionParams) {
        std::type_index interactionType = resolve_type_index(key);
        std::vector<std::any> convertedValues;
        //convert py::object to std::any after casting it to relevant type
        for (auto& py_val : value) {
            convertedValues.push_back(py_to_any(py_val));
        }

        interactionParams2[interactionType] = std::move(convertedValues);
    }

    if (type == "Mover") {
        return sim.add_mover(std::type_index(typeid(Mover)), args, interactionParams2);
    }
    else if (type == "NewtMover") {
        return sim.add_mover(std::type_index(typeid(NewtMover)), args, interactionParams2);
    }
    else if (type == "RigidConnectedMover") {
        return sim.add_mover(std::type_index(typeid(RigidConnectedMover)), args, interactionParams2);
    }
    else {
        throw std::invalid_argument("Unknown Mover type");
    }
};


// Define the Python module
PYBIND11_MODULE(phys_engine, m) {
    m.def("launch_gui", &run_wx_app, "launch wxWidgets GUI");
    m.def("launch_gui_newthread", &run_wx_app_newthread, "launch wxWidgets GUI in new thread");
    m.def("add_mover", &add_mover);
    m.def("report_mover_positions", &report_mover_positions);
    m.def("typeid", &resolve_type_index, "provide type index by name", py::arg("type_name"));

    py::class_<Vect2>(m, "Vect2")
        .def(py::init<float, float>(), py::arg("x")=0, py::arg("y")=0)
        .def_readwrite("x", &Vect2::x)
        .def_readwrite("y", &Vect2::y)
        //def add operator overload
        .def(py::self + py::self)
        .def(py::self += py::self)
        .def(py::self - py::self)
        //multiply operator overload
        .def(py::self * float())
        .def(py::self * py::self)
        //other funcs
        .def("polar_set", &Vect2::polar_set)
        .def("mag", &Vect2::mag)
        .def("angle", &Vect2::angle)
        .def("dot", &Vect2::dot)
        .def("cross", &Vect2::cross)
        .def("projection", &Vect2::projection)
        .def("rotate", &Vect2::rotate);
    
    py::class_<MoverArgs>(m, "MoverArgs")
        .def(py::init<Vect2, Vect2, Vect2, int, int>(), 
        py::arg("position") = Vect2(0,0), py::arg("velocity")= Vect2(0,0),
         py::arg("accel")= Vect2(0,0), py::arg("mass")=1.0, py::arg("radius")=1.0);

    py::class_<Mover>(m, "Mover")
        .def(py::init<MoverArgs>())
        .def("update", &Mover::update)
        .def_readwrite("position", &Mover::position)
        .def_readwrite("velocity", &Mover::velocity)
        .def_readwrite("accel", &Mover::accel)
        .def_readwrite("mass", &Mover::mass)
        .def_readwrite("radius", &Mover::radius);
    //Newtmover
    py::class_<NewtMover, Mover>(m, "NewtMover")
        .def(py::init<MoverArgs>())
        //define overriding update, which overrides bases virtual
        .def("update", &NewtMover::update)
        .def("apply_force", &NewtMover::apply_force);
    
    py::class_<Interaction>(m, "Interaction")
        .def("interact", &Interaction::interact, py::return_value_policy::reference);
    
    py::class_<Spring, Interaction>(m, "SpringInteraction")
        .def(py::init<float, float>())
        .def("interact", &Spring::interact, py::return_value_policy::reference);
    
    py::class_<Simulator>(m, "Simulator")
        .def(py::init<float>())
        .def("add_mover", &add_mover, "add mover to simulator", py::arg("type"), py::arg("args"),
        py::arg("interactionParams") = std::unordered_map<std::string, std::vector<py::object>>()
    )
        .def("create_group", &Simulator::create_group, "create group of movers", py::arg("mover_ids"))
        .def("remove_mover", &Simulator::remove_mover, 
            "remove mover from simulator by id. Returns true/false if found and removed", py::arg("id"))
        .def("replace_mover", 
            py::overload_cast<int, Mover*>(&Simulator::replace_mover),
        "replace mover in simulator with new mover. Returns true/false if found and replaced", py::arg("id"), py::arg("replacement_mover"))
        //two update methods, use py::overload_cast
        .def("update", py::overload_cast<int>(&Simulator::update),
    "update simulator by number of steps", py::arg("steps"))
        .def("update", py::overload_cast<>(&Simulator::update),
    "update simulator by one step")
        .def("add_interaction", &Simulator::add_interaction)
        .def("add_spring_interaction", &add_spring_interaction, 
        "Add a spring interaction to the simulator", py::arg("k"), py::arg("x0"))
        .def("add_coulomb_interaction", &add_coulomb_interaction, 
        "Add a coulomb interaction to the simulator", py::arg("k"))
        .def("add_softCollide_interaction", &add_softCollide_interaction, 
        "Add a soft collide interaction to the simulator", py::arg("springStrength"), py::arg("repulsionStrength"))
        .def("add_lorentz", &add_lorentz_effect,
        "Add magnetic field and apply lorentz effect", py::arg("magneticStrength"))
        .def("add_drag", &add_drag,
        "Add drag force and apply drag effect", py::arg("dragStrength"))
        .def("add_springGroup", &add_springGroup, 
        "Add a spring interactionGroup to the simulator", py::arg("k"), py::arg("x0"), py::arg("moverIds"))
        .def("get_mover_position", &get_mover_position, "get mover position by id", py::arg("id"))
        .def("get_mover_velocity", &get_mover_velocity, "get mover velocity by id", py::arg("id"))
        .def("report_mover_positions", &report_mover_positions);
}