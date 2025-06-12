#pragma once
#include "Simulator.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <optional>

template <typename T>
struct RotatingArray { 
  // a circular array that is intended to hold "size" most recent elements and provide access
  // new values will override the oldest elements AND return the oldest element
  // useful for keeping most recent results in memory, while providing older results to client code,
  // perhaps for storage in a different buffer that can then be written to an output stream
  std::vector<T> data;
  int size;
  int current_idx = 0;
  int num_elements = 0; //current # of elements
  
  RotatingArray(int size) : size(size) {
    data.resize(size);
  }

  std::optional<T> add(T newElement) {
    T return_value;
    if (num_elements == size) {
      return_value = data[current_idx];
      data[current_idx] = newElement;
      current_idx = (current_idx + 1) % size;
      return return_value;
    }
    else {
      data[current_idx] = newElement;
      current_idx = (current_idx + 1) % size;
      num_elements++;
      return std::nullopt;
    }
  }

  std::optional<T> pop() {
    if (num_elements == 0) {
      return std::nullopt;
    }
    T popped = data[current_idx];
    current_idx = (current_idx - 1) % size;
    num_elements--;
    return popped;
  }

  std::optional<T&> get_previous(int stepsBack) { //hm, should this provide a pointer or reference?
    if (stepsBack > num_elements) {
      return std::nullopt;
    }
    int idx = (current_idx - stepsBack) % size;
    return data[idx];
  }

};

//enum for recordable data 
enum RECORDABLE_DATA {
    POSITION_X,
    POSITION_Y,
    VELOCITY_X,
    VELOCITY_Y,
    ACCELERATION_X,
    ACCELERATION_Y,
    MASS
}; // woudl like an extensible enum so that new recordable data can be added for more specific monitors

class Monitor {
  // general monitor interface for movers that can be configured to record specified data at specified intervals
  // this data can be written to a file or streamed to a GUI, or kept in memory for analysis
  public:
    Mover* mover;
    std::vector<float> times;
    std::unordered_map<RECORDABLE_DATA, std::vector<float> > data;
    int monitorEveryTStep = 1; // how many time steps between recordings float
    std::ostream* outputStream = nullptr;
    //hold this many steps, then flush all data to outputStream
    int stepsBeforeFlush = 0;
    int data_idx = 0; //resets upon flush
    Monitor(Mover* mover, std::vector<RECORDABLE_DATA> recordableData, std::ostream* outputStream = nullptr, int stepsBeforeFlush = 0);
    Monitor(Mover* mover);
    void record(int tStep, float time);
    void flush();
    void addRecordableData(RECORDABLE_DATA dataToRecord);
    void openTextFileToStreamTo(std::string fileName);
    void getRecordableData(RECORDABLE_DATA dataToGet);







};