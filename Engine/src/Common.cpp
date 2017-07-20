#include <EnginePCH.h>

#include <Common.h>

extern HINSTANCE g_DLLHandle;

namespace Core
{
    std::string GetStringResource( int ID, const std::string& type )
    {
        HMODULE hModule = GetModuleHandle( nullptr );
        HRSRC hResource = FindResourceA( hModule, MAKEINTRESOURCE( ID ), type.c_str() );
        if ( hResource )
        {
            HGLOBAL hResourceData = LoadResource( hModule, hResource );
            DWORD resourceSize = SizeofResource( hModule, hResource );
            if ( hResourceData && resourceSize > 0 )
            {
                const char* resourceData = static_cast<const char*>( LockResource( hResourceData ) );
                std::string strData( resourceData, resourceSize );
                return strData;
            }
        }
        // Just return an empty string.
        return std::string();
    }
}