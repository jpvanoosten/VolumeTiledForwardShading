#include <EnginePCH.h>
#include <Application.h>
#include <DependencyTracker.h>

using namespace Core;

DependencyTracker::DependencyTracker()
{
    Application& app = Application::Get();
    m_EventConnections.push_back( app.FileChanged += boost::bind( &DependencyTracker::OnFileChanged, this, _1 ) );
}

DependencyTracker::DependencyTracker( const std::wstring& baseFile )
    : DependencyTracker()
{
    fs::path basePath( baseFile );
    if ( basePath.has_filename() )
    {
        // Only store the filename without the basepath.
        // The base path will be prepended in the IsStale method.
        m_Dependencies.push_back( basePath.filename().wstring() );

        m_LastLoadtime = ( fs::exists( baseFile ) && fs::is_regular_file( baseFile ) ) ? fs::last_write_time( baseFile ) : fs::file_time_type::clock::now();

        SetBaseFile( baseFile );
    }
}

DependencyTracker::~DependencyTracker()
{
}

const DependencyTracker& DependencyTracker::operator=( const DependencyTracker& other )
{
    MutexLock lock( m_Mutex );

    // Check for copy to self.
    if ( this != &other )
    {
        m_BaseFile = other.m_BaseFile;
        m_DependencyPath = other.m_DependencyPath;
        m_ParentPath = other.m_ParentPath;
        m_Dependencies = other.m_Dependencies;
        m_LastLoadtime = other.m_LastLoadtime;
    }

    return *this;
}

void DependencyTracker::SetBaseFile( const std::wstring& baseFile )
{
    MutexLock lock( m_Mutex );

    m_BaseFile = baseFile;
    m_DependencyPath = fs::path( m_BaseFile + L".dep");
    if ( m_DependencyPath.has_parent_path() )
    {
        m_ParentPath = m_DependencyPath.parent_path();
    }
    else
    {
        m_ParentPath = fs::current_path();
    }
}

void DependencyTracker::AddDependency( const std::wstring& dependencyFile )
{
    MutexLock lock( m_Mutex );

    DependencyList::iterator iter = std::find( m_Dependencies.begin(), m_Dependencies.end(), dependencyFile );
    if ( iter == m_Dependencies.end() )
    {
        m_Dependencies.push_back( dependencyFile );
    }
}

void DependencyTracker::OnFileChanged( FileChangeEventArgs& e )
{
    MutexLock lock( m_Mutex );
    bool bFileChanged = false;

    for ( std::wstring dependency : m_Dependencies )
    {
        if ( e.Action == FileAction::Modified )
        {
            fs::path dependencyPath = m_ParentPath / dependency;
            fs::path changedFilePath = e.Path;
            try
            {
                // Make sure the canonical paths are the same.
                if ( fs::canonical( dependencyPath ) == fs::canonical( changedFilePath ) )
                {
                    if ( m_LastLoadtime < fs::last_write_time( dependencyPath ) )
                    {
                        bFileChanged = true;
                        break;
                    }
                }
            } catch ( fs::filesystem_error& )
            {
                // This can happen when a change is detected of a temporary file (Photoshop creates these when saving PSD files)
                // fs::canonical will throw an exception if the file doesn't exist.
            }
        }
    }

    if ( bFileChanged )
    {
        // Fire the event once if any file dependency changed
        FileChanged( e );
    }
}

bool DependencyTracker::IsStale()
{
    MutexLock lock( m_Mutex );

    try
    {
        for ( std::wstring dependency : m_Dependencies )
        {
            // Check to see if the asset needs to be reloaded.
            if ( fs::exists( m_ParentPath / dependency ) && m_LastLoadtime < fs::last_write_time( m_ParentPath / dependency ) )
            {
                return true;
            }
        }
    } 
    catch ( fs::filesystem_error& fserror )
    {
        OutputDebugStringA( fserror.what() );
    }

    return false;
}

void DependencyTracker::SetLastLoadTime( fs::file_time_type lastLoadTime )
{
    m_LastLoadtime = lastLoadTime;
}

fs::file_time_type DependencyTracker::GetLastLoadTime() const
{
    return m_LastLoadtime;
}

bool DependencyTracker::Save()
{
    MutexLock lock( m_Mutex );

    std::wofstream dependenciesOutputStream( m_DependencyPath, std::ios::out );
    if ( dependenciesOutputStream.is_open() )
    {
        boost::archive::xml_woarchive woa( dependenciesOutputStream );
        woa << boost::serialization::make_nvp( "DependencyTracker", *this );
        return true;
    }

    return false;
}

bool DependencyTracker::Load()
{
    MutexLock lock( m_Mutex );

    std::wifstream dependenciesInputStream( m_DependencyPath, std::ios::in );
    if ( dependenciesInputStream.is_open() )
    {
        boost::archive::xml_wiarchive wia( dependenciesInputStream );
        wia >> boost::serialization::make_nvp( "DependencyTracker", *this );

        return true;
    }
    return false;
}
