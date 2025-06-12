# CMake generated Testfile for 
# Source directory: C:/Users/Mati/Documents/PhysEngineRestructured
# Build directory: C:/Users/Mati/Documents/PhysEngineRestructured/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(Vect2_Test "C:/Users/Mati/Documents/PhysEngineRestructured/build/tests/Vect2_test.exe")
set_tests_properties(Vect2_Test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Mati/Documents/PhysEngineRestructured/CMakeLists.txt;74;add_test;C:/Users/Mati/Documents/PhysEngineRestructured/CMakeLists.txt;0;")
add_test(Mover_Test "C:/Users/Mati/Documents/PhysEngineRestructured/build/tests/Mover_test.exe")
set_tests_properties(Mover_Test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Mati/Documents/PhysEngineRestructured/CMakeLists.txt;75;add_test;C:/Users/Mati/Documents/PhysEngineRestructured/CMakeLists.txt;0;")
add_test(MoverFactory_Test "C:/Users/Mati/Documents/PhysEngineRestructured/build/tests/MoverFactory_test.exe")
set_tests_properties(MoverFactory_Test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Mati/Documents/PhysEngineRestructured/CMakeLists.txt;76;add_test;C:/Users/Mati/Documents/PhysEngineRestructured/CMakeLists.txt;0;")
add_test(RigidMover_Test "C:/Users/Mati/Documents/PhysEngineRestructured/build/tests/RigidMover_test.exe")
set_tests_properties(RigidMover_Test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Mati/Documents/PhysEngineRestructured/CMakeLists.txt;77;add_test;C:/Users/Mati/Documents/PhysEngineRestructured/CMakeLists.txt;0;")
add_test(Simulator_Test "C:/Users/Mati/Documents/PhysEngineRestructured/build/tests/Simulator_test.exe")
set_tests_properties(Simulator_Test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Mati/Documents/PhysEngineRestructured/CMakeLists.txt;78;add_test;C:/Users/Mati/Documents/PhysEngineRestructured/CMakeLists.txt;0;")
add_test(Effect_Test "C:/Users/Mati/Documents/PhysEngineRestructured/build/tests/Effect_test.exe")
set_tests_properties(Effect_Test PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Mati/Documents/PhysEngineRestructured/CMakeLists.txt;79;add_test;C:/Users/Mati/Documents/PhysEngineRestructured/CMakeLists.txt;0;")
subdirs("simulator")
subdirs("gui")
subdirs("pybind")
subdirs("apps/pybindApp")
subdirs("apps/standalone1")
