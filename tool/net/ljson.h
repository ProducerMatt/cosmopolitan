#ifndef COSMOPOLITAN_TOOL_NET_LJSON_H_
#define COSMOPOLITAN_TOOL_NET_LJSON_H_
#include "third_party/lua/lauxlib.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int DecodeJson(struct lua_State *, const char *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_NET_LJSON_H_ */
