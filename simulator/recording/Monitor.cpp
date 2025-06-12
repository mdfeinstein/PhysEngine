#include "Monitor.h"

Monitor::Monitor(Mover* mover, std::vector<RECORDABLE_DATA> recordableData, 
  std::ostream* outputStream, int stepsBeforeFlush) : mover(mover),
  outputStream(outputStream), stepsBeforeFlush(stepsBeforeFlush) {
    for (auto& data : recordableData) {
      this->addRecordableData(data);
    }
  };

Monitor::Monitor(Mover* mover) : mover(mover) {};

void Monitor::record(int tStep, float time) {
  if (tStep % monitorEveryTStep == 0) {
    times.push_back(time);
    for (auto& data : data) {
      getRecordableData(data.first);
    };
    data_idx++;
    if (data_idx == stepsBeforeFlush) {
      flush();
      data_idx = 0;
    }
  };
};

void Monitor::getRecordableData(RECORDABLE_DATA dataToGet) {
  // get data from mover
  switch (dataToGet) {
    case POSITION_X:
      data[dataToGet].push_back(mover->position.x);
      break;
    case POSITION_Y:
      data[dataToGet].push_back(mover->position.y);
      break;
    case VELOCITY_X:
      data[dataToGet].push_back(mover->velocity.x);
      break;
    case VELOCITY_Y:
      data[dataToGet].push_back(mover->velocity.y);
      break;
    case ACCELERATION_X:
      data[dataToGet].push_back(mover->accel.x);
      break;
    case ACCELERATION_Y:
      data[dataToGet].push_back(mover->accel.y);
      break;
    case MASS:
      data[dataToGet].push_back(mover->mass);
      break;
    default:
      break;
    };
};
