#ifndef _APPXNOC_H_
#define _APPXNOC_H_

#include "network.h"
#include "core.h"

namespace appxnoc {

  class global_cntlr {

  private:

    static global_cntlr * singleton;

    Core * core;

    int cache_line_size;
    int data_flit_num;

  public:
    global_cntlr(Core * core) : core(core) { }

    static global_cntlr * get_singleton() { return singleton; }

    static void init(Core * core, int cache_line_size) {
      if (!singleton)
        delete singleton;
      singleton = new global_cntlr(core);

      singleton->cache_line_size = cache_line_size;
      singleton->data_flit_num = cache_line_size / 64;
    }

    static void release() { delete singleton; singleton = NULL; }

    int get_data_flit_num() { return data_flit_num; }
  };

  class local_cntlr {

  private:
    Core * core;
    int appx_level;

  public:
    local_cntlr(Core * core) : core(core) { appx_level = 0; }

    NetPacket compress(NetPacket packet) {
      packet.length = get_compressed_size();
      return packet;
    }

    void set_appx_level(int level) { appx_level = level; }

  private:
    int get_compressed_size() {
      return (global_cntlr::get_singleton()->get_data_flit_num() - appx_level + 1) * 64 - 8;
    }
  };
}

#endif
