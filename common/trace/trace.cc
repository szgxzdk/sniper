#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "trace.h"
using namespace trace;

trace_manager * trace_manager::singleton;

void trace_manager::add_record(const flow_record & r) {
#ifdef _FLOW_TRACE_
  pthread_mutex_lock(&lock);
  if (flow_count == flows.size())
    save_flow();
  flows[flow_count++] = r;
  pthread_mutex_unlock(&lock);
#endif
}

void trace_manager::add_record(const inst_record & r) {
#ifdef _INST_TRACE_
  pthread_mutex_lock(&lock);
  if (inst_count == insts.size())
    save_inst();
  insts[inst_count++] = r;
  pthread_mutex_unlock(&lock);
#endif
}

void trace_manager::save() {
  save_flow();
  save_inst();
}

void trace_manager::save_flow() {
#ifdef _FLOW_TRACE_
  FILE * fp;

  if ((fp = fopen("flow_trace_marked.log", "w")) == NULL) {
    printf("Fail to open trace file\n");
    exit(1);
  }

  for (std::vector<flow_record>::iterator it = flows.begin();
       it != flows.begin() + flow_count; it++)
    fprintf(fp, "%c %i %i %ld %d\n", it->is_roi ? 'R' : 'N', it->sender, it->receiver, it->time, it->size);

  flow_count = 0;
  fclose(fp);
#endif
}

void trace_manager::save_inst() {
#ifdef _INST_TRACE_
  FILE * fp;

  if ((fp = fopen("inst_trace_marked.log", "w")) == NULL) {
    printf("Fail to open trace file\n");
    exit(1);
  }

  for (std::vector<inst_record>::iterator it = insts.begin(); it != insts.begin() + inst_count; it++)
    fprintf(fp, "%c %ld %d\n", it->is_roi ? 'R' : 'N', it->time, it->core_id);

  inst_count = 0;
  fclose(fp);
#endif
}
