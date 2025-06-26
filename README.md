# PhysEngine
 
A 2D Newtonian solver written from scratch in C++ that allows objects to move according to F = ma, under the influence of effects and interactions that are easily specified.
The simulation is multi-threaded to better utilize the computing hardware.
A simple GUI to watch and interact with the simulation is implemented using wxWidgets.
PyBind11 is used to create some python bindings, allowing the simulation to be controlled and its data accessed through a scripting interface.

## Demoed Features

<div align="center">
 <h3> A Spring-like interaction along with a "soft collide" interaction, with 500 movers<br> </h3>
 <img src="showcase/500_movers_and_spring.gif" width="480"/>
 <br>
 (Low quality image from .gif conversion)
</div>  
<br>
<div align="center">
 <h3> Live interactions can be easily added and customized<br> </h3>
 <img src="showcase/kicking.gif" width="480"/>
 <br>
 A "kick" is applied on command to the moused over object, and the force of the kick increases further from the center 
</div>  
<br>
<div align="center">
 <h3> Movers can be added and grouped in real time <br> </h3>
 <img src="showcase/adding_and_grouping.gif" width="480"/>
 <br>
 Movers are created, grouped into a rigid body, and "kicked" into motion
</div>  

</div>  
<br>
<div align="center">
 <h3> Python Scripting <br> </h3>
 <img src="showcase/python_scripting.gif" width="640"/>
 <br>
 Python scripting is powered by PyBind11. Simulations can be flexibly setup and run through a python interface, no recompiling necessary. Perfect for exploring simulation parameters.
</div><br>

Python can also be used to run and record/plot simulation data without the GUI:  

<div align="center">
 <img src="showcase/python_sim_plot.png" width="640"/>
 <br>
 A mover is attached to another mover by a spring, and its position is recorded over tens of thousands of steps and plotted. Directly from Python.
</div><br>




