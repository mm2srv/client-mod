
#define VTBL_WRAP(out, in)                                                                  \
union {                                                                                     \
    decltype(in) m_Underlying;                                                              \
    uintptr_t m_Vtbl;                                                                       \
}* out = reinterpret_cast<decltype(out)>(in);
