#include <iostream>
#include <cstdio>
#include "library.h"
#include "libxl.h"
#include "string.h"
#include "StringUtil.hpp"

using namespace std;
using namespace libxl;

#pragma  comment(lib, "lua54.lib")

Book *book = nullptr;
const char *filePath = nullptr;
Sheet *currentSheet = nullptr;

//typedef struct MyBook{
//    Book *books;
//};

// -----------------------------------
typedef struct Student{
    lua_Integer age;
    char name[20];
}Stu;

int newData(lua_State *L) {
    lua_newuserdata(L, sizeof(Stu));

    return 1;
}

int setData(lua_State *L)
{
    Stu *pData = (Stu *)lua_touserdata(L, 1);
    const char* name = luaL_checkstring(L, 2);
    lua_Integer age = luaL_checkinteger(L, 3);
    pData->age = age;
    strcpy_s(pData->name, 20, name);
    return 0;
}

int getData(lua_State *L)
{
    Stu *pData = (Stu *)lua_touserdata(L, 1);
    lua_pushstring(L, pData->name);
    lua_pushinteger(L, pData->age);

    return 2;
}
//------------------------------

int sayHelloFunc(lua_State *L) {
    printf("hello world!");
    return 0;
}

int createTest(lua_State *L) {
    Book *book = xlCreateBook();    //  xls
//    Book* book = xlCreateXMLBook();   // xlsx
    if (book) {
        book->setKey("hz", "windows-202f240d0bcee30069b36468a7p1r8ge");
//        book->setLocale("utf-8");
        Sheet *sheet = book->addSheet("Sheet1");
        if (sheet) {
            sheet->writeStr(2, 1, "Hello, World 你好 !");
            sheet->writeNum(4, 1, 1000);
            sheet->writeNum(5, 1, 2000);

            Font *font = book->addFont();
            font->setColor(COLOR_RED);
            font->setBold(true);
            Format *boldFormat = book->addFormat();
            boldFormat->setFont(font);
            sheet->writeFormula(6, 1, "SUM(B5:B6)", boldFormat);

            Format *dateFormat = book->addFormat();
            dateFormat->setNumFormat(NUMFORMAT_DATE);
            sheet->writeNum(8, 1, book->datePack(2008, 4, 29), dateFormat);

            sheet->setCol(1, 1, 12);
        }

        if (!book->save("D:\\example.xls")) {
            std::cout << book->errorMessage() << std::endl;
        }

        book->release();
    }
    return 0;
}

int readTest(lua_State *L) {
    Book *book = xlCreateBook();    //  xls
//    Book* book = xlCreateXMLBook();   // xlsx
    if (book) {
        book->setKey("hz", "windows-202f240d0bcee30069b36468a7p1r8ge");
//        book->setLocale("utf-8");

        if (book->load("D:\\1.xls")) {
            Sheet *sheetReader = book->getSheet(0);
            if (sheetReader) {
                {
                    CellType cellType = sheetReader->cellType(2, 1);
                    if (cellType == libxl::CELLTYPE_STRING) {
                        const char *val = sheetReader->readStr(2, 1);
//                        cout << val << endl;
                    }
                }

                {
                    CellType cellType = sheetReader->cellType(4, 1);
                    if (cellType == libxl::CELLTYPE_NUMBER) {
                        double d = sheetReader->readNum(4, 1);
                        cout << d << endl;
                    }
                }

            }
        }
    }
    return 0;
}

bool FileExists(LPCTSTR szFilePath) {
    FILE *f = nullptr;
    if ((f = fopen(szFilePath, "rb")) != nullptr) { //stdio.h
        fclose(f);
        return true;
    } else {
        return false;
    }
}

int openFile(lua_State *L) {
    filePath = lua_tostring(L, -1);
    bool isFileExists = FileExists(filePath);

    if (hzStringUtil::endWith(filePath, ".xls")) {
        book = xlCreateBook();    //  xls
    } else if (hzStringUtil::endWith(filePath, ".xlsx")) {
        book = xlCreateXMLBook();    //  xlsx
    }

    bool bLoad = true;
    if (book) {
        book->setKey("hz", "windows-202f240d0bcee30069b36468a7p1r8ge");
//        book->setLocale("utf-8");
        if (isFileExists) {  // 文件存在才加载
            bLoad = book->load(filePath);
        }
    }

    lua_pushboolean(L, book != nullptr && bLoad);

    return 1;
}

int addReadSheet(lua_State *L) {
    const char *sheetName = lua_tostring(L, -1);
    Sheet *sheet = book->addSheet(sheetName);
    lua_pushboolean(L, sheet != nullptr);

    return 1;
}

int getSheetCount(lua_State *L) {
    lua_pushinteger(L, book->sheetCount());

    return 1;
}

int getSheetName(lua_State *L) {
    int sheetIndex = lua_tointeger(L, -1);
    const char *name = book->getSheetName(sheetIndex);
    lua_pushstring(L, name);

    return 1;
}

int setCurrentSheet(lua_State *L) {
    int sheetIndex = lua_tointeger(L, -1);
    currentSheet = book->getSheet(sheetIndex);
    lua_pushboolean(L, currentSheet != nullptr);

    return 1;
}

int readCurrentSheet(lua_State *L) {
    int colNum = lua_tointeger(L, -1);
    int rowNum = lua_tointeger(L, -2);

    if (currentSheet) {
        CellType cellType = currentSheet->cellType(rowNum, colNum);
        switch (cellType) {
            case libxl::CELLTYPE_NUMBER: {
                double val = currentSheet->readNum(rowNum, colNum);
                lua_pushnumber(L, val);
                break;
            }
                // 默认字符串处理
            default: {
                const char *val = currentSheet->readStr(rowNum, colNum);
                cout << val << endl;
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

    if (currentSheet) {
        currentSheet->writeStr(rowNum, colNum, val);
    }

    return 0;

}

int saveFile(lua_State *L) {
    if (book) {
        book->save(filePath);
    }

    return 0;
}

int closeFile(lua_State *L) {
    if (book) {
        book->save(filePath);
        book->release();
        book = nullptr;
    }

    return 0;
}


const luaL_Reg myLib[] =
        {
                {"sayHello",          sayHelloFunc},
                {"createTest",        createTest},
                {"readTest",          readTest},
                {"openFile",          openFile},
                {"setCurrentSheet",   setCurrentSheet},
                {"addReadSheet",      addReadSheet},
                {"getSheetCount",     getSheetCount},
                {"getSheetName",      getSheetName},
                {"readCurrentSheet",  readCurrentSheet},
                {"writeCurrentSheet", writeCurrentSheet},
                {"saveFile",          saveFile},
                {"closeFile",         closeFile},

                {"newData",         newData},
                {"setData",         setData},
                {"getData",         getData},

                {NULL, NULL}       //数组中最后一对必须是{NULL, NULL}，用来表示结束
        };

int luaopen_lualibxl(lua_State *L) {
    luaL_newlib(L, myLib);

    return 1;            // 把myLib表压入了栈中，所以就需要返回1
}