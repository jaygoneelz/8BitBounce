#pragma once
#include "application.h"
#include <cstdlib>
#include <crtdbg.h>

#ifdef ENGINE_PLATFORM_WIN

extern engine::application* engine::create_application();

/// \brief Adjusts the console to take 1/3rd of the monitor height.
void move_console();


int main(int argc, char** argv)
{
    if (std::getenv("BOUNCE_SMOKE")) {
        _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
        _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
        _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    }

    if (!std::getenv("BOUNCE_SMOKE")) move_console();

    engine::logger::init();
    LOG_CORE_WARN("Initialized logger");
    LOG_INFO("Welcome to the new engine!");

    auto app = engine::create_application();
    app->run();
    delete app;
}

void move_console()
{
#ifdef ENGINE_DEBUG
    auto hwnd = GetConsoleWindow();
    SetWindowPos(hwnd, HWND_TOP, 0, static_cast<int>(1080 * 0.72), 1920, static_cast<int>(1080 * 0.25), SWP_NOZORDER);
    ShowWindow(hwnd, SW_SHOW);
#else
    ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
}

#else
    #error engine only supports Windows!
#endif
