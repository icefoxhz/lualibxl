#ifndef LUALIBXL_LIBRARY_H
#define LUALIBXL_LIBRARY_H

#include <lua.hpp>
// 注意入口函数为：luaopen_xxx    xxx 为 dll 名称
extern "C" __declspec(dllimport) int luaopen_lualibxlCore(lua_State *L);//定义导出函数

#endif //LUALIBXL_LIBRARY_H
