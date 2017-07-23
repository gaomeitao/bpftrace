#include <iostream>
#include <unistd.h>

#include "common.h"
#include "libbpf.h"

#include "map.h"

namespace bpftrace {

Map::Map(const std::string &name, const SizedType &type, const MapKey &key)
  : name_(name), type_(type), key_(key)
{
  int key_size = key.size();
  if (type.type == Type::quantize)
    key_size += 8;
  if (key_size == 0)
    key_size = 8;

  int value_size = type.size;
  int max_entries = 128;
  int flags = 0;
  mapfd_ = bpf_create_map(BPF_MAP_TYPE_HASH, key_size, value_size, max_entries, flags);
  if (mapfd_ < 0)
  {
    std::cerr << "Error creating map: '" << name_ << "'" << std::endl;
  }
}

Map::Map(enum bpf_map_type map_type)
{
  int key_size, value_size, max_entries, flags;

  if (map_type == BPF_MAP_TYPE_STACK_TRACE)
  {
    key_size = 4;
    value_size = sizeof(uintptr_t) * MAX_STACK_SIZE;
    max_entries = 128;
    flags = 0;
  }
  else if (map_type == BPF_MAP_TYPE_PERF_EVENT_ARRAY)
  {
    std::vector<int> cpus = ebpf::get_online_cpus();
    key_size = 4;
    value_size = 4;
    max_entries = cpus.size();
    flags = 0;
  }
  else
  {
    abort();
  }
  mapfd_ = bpf_create_map(map_type, key_size, value_size, max_entries, flags);
  if (mapfd_ < 0)
  {
    std::cerr << "Error creating map: '" << name_ << "' (" << mapfd_ << ")" << std::endl;
  }
}

Map::~Map()
{
  close(mapfd_);
}

} // namespace bpftrace
