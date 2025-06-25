# PhysEngineRestructured
 
A 2D Newtonian solver that allows objects to move according to F = ma, under the influence of effects and interactions that are easily specified.
A simple GUI to watch and interact with the simulation is implemented using wxWidgets.
PyBind11 is used to create some python bindings, allowing the simulation to be controlled and its data accessed through a scripting interface.

# Demoed Features

A Spring-like interaction along with a soft collide interaction, with 500 movers
![Basic Demo](showcase/500_movers_and_spring.gif)
