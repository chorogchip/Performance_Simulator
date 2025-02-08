#include "simulator_cache.h"

#include <memory>
#include <cstring>
#include <cassert>

namespace ps {

    static inline size_t GenMask(size_t bits) { return ((size_t)1 << bits) - (size_t)1; }
    static inline size_t MaskWithBit(size_t data, size_t bits) { return data & GenMask(bits); }
    static inline size_t MaskWithOffsetLR(size_t data, size_t offset_bits, size_t bits) { return MaskWithBit(data >> offset_bits, bits); }

    
    CacheSimBasic::CacheSimBasic(int total_mem_bits, int block_size_bits, int cache_line_bits, int associativity):
        cache_data_(nullptr),
        cache_use_data_(),
        total_mem_bits_(total_mem_bits),
        block_size_bits_(block_size_bits),
        cache_line_bits_(cache_line_bits),
        tag_bits_(total_mem_bits_ - block_size_bits_ - cache_line_bits_),
        associativity_(associativity),
        cache_entry_count_((size_t)1 << (size_t)(cache_line_bits_ + associativity_))
    {
        assert(0 <= total_mem_bits_ && total_mem_bits_ <= 64);
        assert(0 <= block_size_bits_ && block_size_bits_ <= 64);
        assert(0 <= cache_line_bits_ && cache_line_bits_ <= 64);
        assert(0 <= tag_bits_ && tag_bits_ <= 64);
        assert(1 <= associativity && associativity <= ((size_t)1 << (size_t)63));
        cache_data_ = (CacheBlockData*)malloc(sizeof(CacheBlockData) * cache_entry_count_);
        std::memset(cache_data_, 0, sizeof(CacheBlockData) * cache_entry_count_);
    }

    CacheSimBasic::~CacheSimBasic() {
        std::free(cache_data_);
    }

    void CacheSimBasic::Address(size_t mem_address) {

        cache_use_data_.address_count++;

        const size_t cache_line_index = MaskWithOffsetLR(mem_address, block_size_bits_, cache_line_bits_);
        const size_t cache_tag = MaskWithOffsetLR(mem_address, block_size_bits_ + cache_line_bits_, tag_bits_);
        const size_t start = cache_line_index * associativity_;

        size_t target = (size_t)-1;
        for (size_t i = start; i < start + associativity_; ++i) {
            if (cache_data_[i].valid && cache_data_[i].tag == cache_tag) {
                target = i;
                break;
            }
        }
        if (target != (size_t)-1) {
            // cache hit
            cache_use_data_.hit_count++;
            cache_use_data_.summed_cache_occupancy += cache_use_data_.prev_cache_occupancy;
            cache_data_[target].used_order = cache_use_data_.address_count;
        } else {
            // cache miss
            target = (size_t)-1;
            size_t used_order = (size_t)-1;
            for (size_t i = start; i < start + associativity_; ++i) {
                if (!cache_data_[i].valid) {
                    cache_data_[i].valid = true;
                    cache_data_[i].tag = cache_tag;
                    cache_data_[i].used_order = cache_use_data_.address_count;

                    cache_use_data_.prev_cache_occupancy += 1;
                    cache_use_data_.summed_cache_occupancy += cache_use_data_.prev_cache_occupancy;

                    goto END_LOAD;
                } else {
                    if (used_order > cache_data_[i].used_order) {
                        used_order = cache_data_[i].used_order;
                        target = i;
                    }
                }
            }
            assert(target != (size_t)-1);

            cache_data_[target].tag = cache_tag;
            cache_data_[target].used_order = cache_use_data_.address_count;

            cache_use_data_.summed_cache_occupancy += cache_use_data_.prev_cache_occupancy;

            END_LOAD:;
        }
    }

    void CacheSimBasic::Plot() {

    }

    std::string CacheSimBasic::Print() {

    }
}