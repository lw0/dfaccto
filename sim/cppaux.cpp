#include <cstdint>
#include <vector>

#include "streammodel.hpp"


std::vector<StreamModel> g_models;

extern "C" {

int x_stmSetup(int totalSize, int burstSize, double bandwidth, double latency) {
  int id = g_models.size();
  g_models.emplace_back(totalSize, burstSize, bandwidth, latency);
  return id;
}

void x_stmTickSrc(int id, char v_rst, char * v_last, char * v_valid, char v_ready, char * v_done, int * v_state, int * v_delay, int * v_count, int * v_burst) {
  if (0 <= id && id < g_models.size()) {
    if (v_rst) {
      g_models[id].reset();
    } else {
      printf("%02d ", id);
      g_models[id].tick(v_ready, false);
    }
    *v_valid = g_models[id].active();
    *v_last  = g_models[id].last();

    *v_done  = g_models[id].done();
    *v_state = g_models[id].state();
    *v_delay = g_models[id].delay();
    *v_count = g_models[id].count();
    *v_burst = g_models[id].burst();

  } else {
    *v_last  = 0;
    *v_valid = 0;

    *v_done  = 0;
    *v_state = 0;
    *v_delay = 0;
    *v_count = 0;
    *v_burst = 0;
  }
}

void x_stmTickSnk(int id, char v_rst, char v_last, char v_valid, char * v_ready, char * v_done, int * v_state, int * v_delay, int * v_count, int * v_burst) {
  if (0 <= id && id < g_models.size()) {
    if (v_rst) {
      g_models[id].reset();
    } else {
      printf("%02d ", id);
      g_models[id].tick(v_valid, v_last);
    }
    *v_ready = g_models[id].active();

    *v_done  = g_models[id].done();
    *v_state = g_models[id].state();
    *v_delay = g_models[id].delay();
    *v_count = g_models[id].count();
    *v_burst = g_models[id].burst();
  } else {
    *v_ready = 0;

    *v_done  = 0;
    *v_state = 0;
    *v_delay = 0;
    *v_count = 0;
    *v_burst = 0;
  }
}

}
