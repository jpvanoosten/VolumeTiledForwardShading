#include <EnginePCH.h>

#include <Application.h>
#include <LogManager.h>
#include <Events.h>
#include <Graphics/Profiler.h>

using namespace Core;

// Globals
static Application* gs_ApplicationInstance = nullptr;


Application::Application()
    : m_FixedTimestep( 1.0f / 60.0 )
    , m_bTerminateDirectoryChangeThread( false )
    , m_LoadingProgress( 0.0f )
    , m_LoadingProgressTotal( 0.0f )
{
    gs_ApplicationInstance = this;

    // Create a thread for notifying the application of file modifications.
    m_DirectoryChangeListenerThread = std::thread( &Application::CheckFileChanges, this );

    OnInitialize( EventArgs( *this ) );
}

Application::~Application()
{
    OnTerminate( EventArgs( *this ) );
    gs_ApplicationInstance = nullptr;
}

Application& Application::Get()
{
    assert( gs_ApplicationInstance != nullptr );
    return *gs_ApplicationInstance;
}

void Application::OnInitialize( EventArgs& e )
{
    Initialize( e );
}

void Application::RegisterDirectoryChangeListener( const std::wstring& dir, bool recursive )
{
    scoped_lock lock( m_DirectoryChangeMutex );
    m_DirectoryChanges.AddDirectory( dir, recursive, FILE_NOTIFY_CHANGE_LAST_WRITE );
}

// This is the directory change listener thread entry point.
void Application::CheckFileChanges()
{
    while ( !m_bTerminateDirectoryChangeThread )
    {
        scoped_lock lock( m_DirectoryChangeMutex );

        DWORD waitSignal = ::WaitForSingleObject( m_DirectoryChanges.GetWaitHandle(), 0 );
        switch ( waitSignal )
        {
        case WAIT_OBJECT_0:
            // A file has been modified
            if ( m_DirectoryChanges.CheckOverflow() )
            {
                // This could happen if a lot of modifications occur at once.
                LOG_WARNING( "Directory change overflow occurred." );
            }
            else
            {
                DWORD action;
                std::wstring fileName;
                m_DirectoryChanges.Pop( action, fileName );

                FileAction fileAction = FileAction::Unknown;
                switch ( action )
                {
                case FILE_ACTION_ADDED:
                    fileAction = FileAction::Added;
                    break;
                case FILE_ACTION_REMOVED:
                    fileAction = FileAction::Removed;
                    break;
                case FILE_ACTION_MODIFIED:
                    fileAction = FileAction::Modified;
                    break;
                case FILE_ACTION_RENAMED_OLD_NAME:
                    fileAction = FileAction::RenameOld;
                    break;
                case FILE_ACTION_RENAMED_NEW_NAME:
                    fileAction = FileAction::RenameNew;
                    break;
                default:
                    break;
                }

                FileChangeEventArgs fileChangedEventArgs( *this, fileAction, fileName );
                OnFileChange( fileChangedEventArgs );
            }
        default:
            break;
        }

        std::this_thread::yield();
    }
}

void Application::OnUpdate( UpdateEventArgs& e )
{
    CPU_MARKER( __FUNCTION__ );
    Update( e );
}

void Application::OnRender( RenderEventArgs& e )
{
    // TODO: Call the Window's render method?
}

void Application::OnJoystickButtonPressed( JoystickButtonEventArgs& e )
{
    JoystickButtonPressed( e );
}

void Application::OnJoystickButtonReleased( JoystickButtonEventArgs& e )
{
    JoystickButtonReleased( e );
}

void Application::OnJoystickPOV( JoystickPOVEventArgs& e )
{
    JoystickPOV( e );
}

void Application::OnJoystickAxis( JoystickAxisEventArgs& e )
{
    JoystickAxis( e );
}


void Application::OnTerminate( EventArgs& e )
{
    Terminate( e );
}

void Application::OnExit( EventArgs& e )
{
    Exit( e );

    m_bTerminateDirectoryChangeThread = true;
    if ( m_DirectoryChangeListenerThread.joinable() )
    {
        m_DirectoryChangeListenerThread.join();
    }
}

void Application::OnFileChange( FileChangeEventArgs& e )
{
    FileChanged( e );
}


