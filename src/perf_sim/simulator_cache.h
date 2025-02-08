#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace ps {

    struct CacheBlockData {
        bool valid;
        size_t tag;
        size_t used_order;
    };

    struct CacheUseData {
        int64_t address_count;
        int64_t hit_count;
        int64_t summed_cache_occupancy;
        int64_t prev_cache_occupancy;
    };

    class CacheSimBasic {
    private:
        CacheBlockData* cache_data_;
        CacheUseData cache_use_data_;
        
        int total_mem_bits_;
        int block_size_bits_;
        int cache_line_bits_;
        int tag_bits_;
        int associativity_;
        size_t cache_entry_count_;

    public:
        CacheSimBasic(int total_mem_bits, int block_size_bits, int cache_line_bits, int associativity);
        ~CacheSimBasic();

        void Address(size_t mem_address);
        void Plot();
        std::string Print();
    };

}