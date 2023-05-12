#include "state.hpp"

char State::tokenBuffer[100] = {0};
char State::domainBuffer[100] = {0};
char State::domainPortBuffer[6] = {0};
char State::extraPortBuffer[6] = {0};

in_addr State::serverIpAddr = {0};

char State::tokenHashBuffer[33] = {0};

uint64_t State::last_data_id = 0;

wasm_state_struct State::wasm_state = wasm_state_struct{};

std::vector<patch_struct> State::applied_patches;
std::unordered_set<uintptr_t> State::applied_patches_addr;