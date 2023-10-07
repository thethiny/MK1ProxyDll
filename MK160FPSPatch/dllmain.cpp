// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "exports.h"

HANDLE hConsole = NULL;

void LoadECPFiles()
{
    HMODULE hModule = GetModuleHandle(nullptr);
    WCHAR path[MAX_PATH];
    GetModuleFileName(hModule, path, MAX_PATH);

    std::filesystem::path filePath(path);
    std::filesystem::path directoryPath = filePath.parent_path();

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
    {
        if (wcscmp(L".ehp", entry.path().extension().c_str()) == 0)
        {
            wprintf(L"Loading Hook Plugin %s!\n", entry.path().c_str());
            auto libName = entry.path().c_str();
            HMODULE module = LoadLibrary(libName);
            if (module)
            {
                printf("Plugin Loaded Successfully!\n");
            }
            else
            {
                DWORD error = GetLastError();
                LPVOID errorMessage;
                FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL,
                    error,
                    0, // Default language
                    (LPWSTR)&errorMessage,
                    0,
                    NULL);

                // Print error message
                wprintf(L"Failed to load plugin (Error %d): %s\n", error, errorMessage);

                // Create the message text with "Failed to load plugin" and the error message
                WCHAR message[512];
                swprintf_s(message, 512, L"Failed to load plugin: %s", errorMessage);

                // Create the title text with "Error: %d" where %d is the error code
                WCHAR title[128];
                swprintf_s(title, 128, L"Error: %d", error);

                MessageBox(NULL, message, title, MB_ICONERROR);

                LocalFree(errorMessage); // Free the error message buffer
            }
        }
    }
}

void CreateConsole()
{
	AllocConsole();

	FILE* fNull;
	freopen_s(&fNull, "CONOUT$", "w", stdout);
	freopen_s(&fNull, "CONOUT$", "w", stderr);

	std::string consoleName = "EHP Console Window";
	SetConsoleTitleA(consoleName.c_str());
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hConsole, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hConsole, dwMode);
	printf("EHP Dll Loader\n");
}

void Init()
{
#ifdef _DEBUG
	CreateConsole();
#endif
	LoadECPFiles();
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        LoadOriginalDLL();
        Init();
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

