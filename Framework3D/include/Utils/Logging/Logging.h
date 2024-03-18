#pragma once

#include <iostream>

#include "USTC_CG.h"


USTC_CG_NAMESPACE_OPEN_SCOPE
enum log_level
{
    Info,
    Warning,
    Error
};

inline void logging(const std::string& log_content, log_level level = Warning)
{
    switch (level)
    {
        case Info:
#ifndef NDEBUG
            std::cout << "[[USTC_CG Info]]: " << log_content << std::endl;
#endif
            break;

        case Warning: std::cout << "\x1B[33m[[USTC_CG Warning]]: " <<
                      log_content << "\x1B[0m" << std::endl;
            break;
        case Error: std::cout << "\x1B[31m[[USTC_CG Error]]: " << log_content <<
                    "\x1B[0m" <<
                    std::endl;
            break;
        default: ;
    }
}

USTC_CG_NAMESPACE_CLOSE_SCOPE
