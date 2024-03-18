#pragma once

#define Py_BUILD_CORE_BUILTIN
#include <Python.h>

#include <codecvt>
#include <locale>
#include <string_view>
#include <vector>

inline void run_usdvieww(int argc, char** argv)
{
    Py_SetProgramName(L"usdview");
    // Initialize the Python interpreter
    Py_Initialize();

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::vector<std::wstring> w_argv_strs(argc);
    for (int i = 0; i < argc; ++i)
    {
        w_argv_strs[i] = converter.from_bytes(argv[i]);
    }
    w_argv_strs[0] = L"usdview";

    std::vector<wchar_t*> w_argv(argc);
    for (int i = 0; i < argc; ++i)
    {
        w_argv[i] = w_argv_strs[i].data();
    }

    // Set the command line arguments for the script
    PySys_SetArgv(argc, w_argv.data());

    PyObject* obj = Py_BuildValue("s", "usdview");

    // Open the script file
    FILE* file = _Py_fopen_obj(obj, "r+");

    // Run the script file
    PyRun_SimpleFile(file, "usdview");

    fclose(file);

    // Finalize the Python interpreter
    Py_Finalize();
}
