#include <iostream>
#include <cstdio>
#include "library.h"
#include "libxl.h"
#include <cstring>
#include "StringUtil.hpp"

using namespace std;
using namespace libxl;
#pragma  comment(lib, "lua54.lib")

//  修改测试前一定先把  E:\tools\windows\lua\5.4.4\lib\lua\5.4 下面的删了， 不然加载的是这个里
//  修改测试前一定先把  E:\tools\windows\lua\5.4.4\lib\lua\5.4 下面的删了， 不然加载的是这个里
//  修改测试前一定先把  E:\tools\windows\lua\5.4.4\lib\lua\5.4 下面的删了， 不然加载的是这个里

const int FILE_PATH_LEN = 1024;

typedef struct XlExecl {
    Book *book = nullptr;
    char filePath[FILE_PATH_LEN]{};
    Sheet *currentSheet = nullptr;
} ST_EXECL;

bool FileExists(LPCTSTR szFilePath) {
    FILE *f;
    if ((f = fopen(szFilePath, "rb")) != nullptr) { //stdio.h
        fclose(f);
        return true;
    } else {
        return false;
    }
}

int newExecl(lua_State *L) {
//    lua_newuserdata(L, FILE_PATH_LEN + sizeof(Book) + sizeof(Sheet));
    lua_newuserdata(L, sizeof(ST_EXECL));

    return 1;
}

int openFile(lua_State *L) {
    auto *xl = (ST_EXECL *) lua_touserdata(L, 1);
    const char *filePath = luaL_checkstring(L, 2);
    strcpy_s(xl->filePath, FILE_PATH_LEN, filePath);

    bool isFileExists = FileExists(filePath);

//    Book *book = nullptr;
    if (hzStringUtil::endWith(filePath, ".xls")) {
        xl->book = xlCreateBook();    //  xls
    } else if (hzStringUtil::endWith(filePath, ".xlsx")) {
        xl->book = xlCreateXMLBook();    //  xlsx
    }

    bool bLoad = true;
    if (xl->book) {
        xl->book->setKey("hz", "windows-202f240d0bcee30069b36468a7p1r8ge");
//        book->setLocale("utf-8");
        if (isFileExists) {  // 文件存在才加载
            bLoad = xl->book->load(filePath);
        }
    }

//    lua_pushlightuserdata(L, xl);
    lua_pushboolean(L, xl->book != nullptr && bLoad);

    return 1;
}

int addSheet(lua_State *L) {
    auto *xl = (ST_EXECL *) lua_touserdata(L, 1);
    const char *sheetName = luaL_checkstring(L, 2);

    Sheet *sheet = xl->book->addSheet(sheetName);
    lua_pushboolean(L, sheet != nullptr);

    return 1;
}

int setCurrentSheet(lua_State *L) {
    auto *xl = (ST_EXECL *) lua_touserdata(L, 1);
    int sheetIndex = lua_tointeger(L, 2);

    xl->currentSheet = xl->book->getSheet(sheetIndex);
    lua_pushboolean(L, xl->currentSheet != nullptr);

    return 1;
}

int getSheetCount(lua_State *L) {
    auto *xl = (ST_EXECL *) lua_touserdata(L, 1);
    lua_pushinteger(L, xl->book->sheetCount());

    return 1;
}

int getSheetName(lua_State *L) {
    auto *xl = (ST_EXECL *) lua_touserdata(L, 1);
    int sheetIndex = lua_tointeger(L, 2);

    const char *name = xl->book->getSheetName(sheetIndex);
    lua_pushstring(L, name);

    return 1;
}

int readCurrentSheet(lua_State *L) {
    int colNum = lua_tointeger(L, -1);
    int rowNum = lua_tointeger(L, -2);
    auto *xl = (ST_EXECL *) lua_touserdata(L, -3);

    if (xl->currentSheet) {
        CellType cellType = xl->currentSheet->cellType(rowNum, colNum);
        switch (cellType) {
            case libxl::CELLTYPE_NUMBER: {
                double val = xl->currentSheet->readNum(rowNum, colNum);
                lua_pushnumber(L, val);
                break;
            }
                // 默认字符串处理
            default: {
                const char *val = xl->currentSheet->readStr(rowNum, colNum);
//                cout << val << endl;
                lua_pushstring(L, val);
            }
        }
    } else {
        lua_pushnil(L);
    }

    return 1;
}

int writeCurrentSheet(lua_State *L) {
    const char *val = lua_tostring(L, -1);
    int colNum = lua_tointeger(L, -2);
    int rowNum = lua_tointeger(L, -3);
    auto *xl = (ST_EXECL *) lua_touserdata(L, -4);

    if (xl->currentSheet) {
        xl->currentSheet->writeStr(rowNum, colNum, val);
    }

    return 0;
}

int saveFile(lua_State *L) {
    auto *xl = (ST_EXECL *) lua_touserdata(L, 1);
    if (xl->book) {
        xl->book->save(xl->filePath);
    }

    return 0;
}

int closeFile(lua_State *L) {
    auto *xl = (ST_EXECL *) lua_touserdata(L, 1);
    if (xl->book) {
        xl->book->save(xl->filePath);
        xl->book->release();
        xl->book = nullptr;
    }

    return 0;
}

const luaL_Reg myLib[] =
        {
                {"openFile",          openFile},
                {"newExecl",          newExecl},
                {"addSheet",      addSheet},
                {"setCurrentSheet",   setCurrentSheet},
                {"getSheetCount",     getSheetCount},
                {"getSheetName",      getSheetName},
                {"readCurrentSheet",  readCurrentSheet},
                {"writeCurrentSheet", writeCurrentSheet},
                {"saveFile",          saveFile},
                {"closeFile",         closeFile},
                {nullptr,             nullptr}       //数组中最后一对必须是{NULL, NULL}，用来表示结束
        };

int luaopen_lualibxlCore(lua_State *L) {
    luaL_newlib(L, myLib);

    return 1;            // 把myLib表压入了栈中，所以就需要返回1
}