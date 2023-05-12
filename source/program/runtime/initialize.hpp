#pragma once

#include <cstdint>
#include <cstdlib>

void initialize_mod(uint8_t *downloaded_data, size_t downloaded_size,
                    uint64_t data_id);
bool mod_exists();
void free_mod();