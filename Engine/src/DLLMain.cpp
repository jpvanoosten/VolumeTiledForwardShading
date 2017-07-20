#include <EnginePCH.h>

HINSTANCE g_DLLHandle = nullptr;

BOOLEAN WINAPI DllMain( IN HINSTANCE hDllHandle,
                        IN DWORD     nReason,
                        IN LPVOID    Reserved )
{
    BOOLEAN bSuccess = TRUE;

    //  Perform global initialization.
    switch ( nReason )
    {
    case DLL_PROCESS_ATTACH:
        g_DLLHandle = hDllHandle;
        // For optimization.
        // See: https://msdn.microsoft.com/en-us/library/ms682579(v=vs.85).aspx
        DisableThreadLibraryCalls( g_DLLHandle );
        break;
    case DLL_PROCESS_DETACH:
        g_DLLHandle = nullptr;
        break;
    }

    return bSuccess;
}
