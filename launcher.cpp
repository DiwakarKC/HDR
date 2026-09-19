#include <windows.h>
#include <iostream>
#include <string>
#include <cstring>

int main()
{
    // --------------------------------------------------
    // Get the folder containing this launcher
    // --------------------------------------------------

    char appPath[MAX_PATH] = {};

    DWORD length = GetModuleFileNameA(
        NULL,
        appPath,
        MAX_PATH
    );

    if (length == 0 || length >= MAX_PATH)
    {
        std::cerr << "Could not determine application directory.\n";
        return 1;
    }

    char* lastSlash = strrchr(appPath, '\\');

    if (lastSlash == nullptr)
    {
        std::cerr << "Could not determine application directory.\n";
        return 1;
    }

    *lastSlash = '\0';

    // --------------------------------------------------
    // Create path to bin folder
    // --------------------------------------------------

    std::string dllPath =
        std::string(appPath) + "\\bin";

    // --------------------------------------------------
    // Get current PATH
    // --------------------------------------------------

    DWORD pathLength = GetEnvironmentVariableA(
        "PATH",
        NULL,
        0
    );

    if (pathLength == 0)
    {
        std::cerr << "Could not read PATH environment variable.\n";
        return 1;
    }

    std::string oldPath(pathLength, '\0');

    GetEnvironmentVariableA(
        "PATH",
        &oldPath[0],
        pathLength
    );

    // Remove the extra null character
    oldPath.resize(strlen(oldPath.c_str()));

    // --------------------------------------------------
    // Add our bin folder to PATH
    // --------------------------------------------------

    std::string newPath =
        dllPath + ";" + oldPath;

    if (!SetEnvironmentVariableA(
            "PATH",
            newPath.c_str()))
    {
        std::cerr << "Failed to modify PATH.\n";
        return 1;
    }

    // --------------------------------------------------
    // Actual application
    // --------------------------------------------------

    std::string exePath =
        std::string(appPath) +
        "\\Image_Processing.exe";

    // --------------------------------------------------
    // Start Image_Processing.exe
    // --------------------------------------------------

    STARTUPINFOA si = {};
    PROCESS_INFORMATION pi = {};

    si.cb = sizeof(si);

    if (!CreateProcessA(
            exePath.c_str(),
            NULL,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            appPath,
            &si,
            &pi))
    {
        std::cerr
            << "Failed to start Image_Processing.exe\n";

        std::cerr
            << "Windows error code: "
            << GetLastError()
            << "\n";

        return 1;
    }

    // --------------------------------------------------
    // Wait for application to finish
    // --------------------------------------------------

    WaitForSingleObject(
        pi.hProcess,
        INFINITE
    );

    // --------------------------------------------------
    // Cleanup
    // --------------------------------------------------

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
