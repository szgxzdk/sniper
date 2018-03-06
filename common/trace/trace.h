#ifndef _TRACE_H_
#define _TRACE_H_

#include <stdio.h>
#include <pthread.h>

#include <vector>
using namespace std;

namespace trace {

  class flow_record {
  public:
    int sender, receiver, size;
    long time;
    bool is_roi;

    flow_record(int sender, int receiver, int size, long time, bool is_roi) :
      sender(sender), receiver(receiver), size(size), time(time), is_roi(is_roi) {}
    flow_record() {sender = receiver = size = time = -1; is_roi = false; }
  };

  class inst_record {
  public:
    int core_id;
    long time;
    bool is_roi;

    inst_record(long time, int core_id, bool is_roi) : core_id(core_id), time(time), is_roi(is_roi) {}
    inst_record() {core_id = time = -1; is_roi = false; }
  };

  class trace_manager {
  private:
    static trace_manager * singleton;

    int roi_count;
    vector<bool> thread_roi;

    static const long VECTOR_SIZE = 50000000L;
    pthread_mutex_t lock;

#ifdef _FLOW_TRACE_
    vector<flow_record> flows;
    size_t flow_count = 0;
#endif
#ifdef _INST_TRACE_
    vector<inst_record> insts;
    size_t inst_count = 0;
#endif

    void save_flow();
    void save_inst();

  public:
    void add_record(const flow_record & r);
    void add_record(const inst_record & r);

    trace_manager(int ncores) : roi_count(0)
    {
      pthread_mutex_init(&lock, NULL);
      thread_roi.resize(ncores, false);
#ifdef _FLOW_TRACE_
      flows.resize(VECTOR_SIZE);
#endif
#ifdef _INST_TRACE_
      insts.resize(VECTOR_SIZE);
#endif
    }

    inline void roi_inc() { ++roi_count; }
    inline void roi_dec() { --roi_count; }

    inline void set_thread_roi(int tid, bool enabled) { thread_roi[tid] = enabled; }
    inline bool is_thread_roi(int tid) { return thread_roi[tid]; }
    inline bool is_packet_roi(int sender, int receiver) {
      return is_thread_roi(sender) || is_thread_roi(receiver);
    }
    inline bool is_roi() { return roi_count > 0; }

    static trace_manager * getSingleton() { return singleton; }

    static void init(int ncores) {
      if (!singleton)
        delete singleton;
      singleton = new trace_manager(ncores);
    }
    static void release() { delete singleton; singleton = NULL; }

    void save();
  };
}

#endif
