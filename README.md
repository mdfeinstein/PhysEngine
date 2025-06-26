# PhysEngine
 
A 2D Newtonian solver that allows objects to move according to F = ma, under the influence of effects and interactions that are easily specified.
The simulation is multi-threaded to icnrease the speed.
A simple GUI to watch and interact with the simulation is implemented using wxWidgets.
PyBind11 is used to create some python bindings, allowing the simulation to be controlled and its data accessed through a scripting interface.

## Demoed Features

<div align="center">
 A Spring-like interaction along with a "soft collide" interaction, with 500 movers
 <img src="showcase/500_movers_and_spring.gif" width="480"/>
 <br>
 (Low quality image from .gif conversion)
</div>  

<div align="center">
 Live interactions can be easily added and customized
 <img src="showcase/kicking.gif" width="480"/>
 <br>
 A "kick" is applied on command to the moused over object, and the force of the kick increases further from the center 
</div>  




