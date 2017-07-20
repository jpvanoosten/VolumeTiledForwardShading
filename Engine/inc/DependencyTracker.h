#pragma once

#include <filesystem>
namespace fs = std::experimental::filesystem;

#include "Object.h"
#include "Events.h"
#include "ReadDirectoryChanges.h"

class DependencyTracker
{
public:
    DependencyTracker();

    /**
     * Parameterized constructor.
     * @param fileName The main asset file that is to be tracked. Other assets 
     * that the base file has dependencies on should be expressed relative to the
     * base file.
     */
    explicit DependencyTracker( const std::wstring& baseFile );

    virtual ~DependencyTracker();

    const DependencyTracker& operator=( const DependencyTracker& other );

    /**
     * Set the base file that this dependency tracker tracks.
     */
    void SetBaseFile( const std::wstring& baseFile );

    /**
     * Add a dependency that should be tracked by this dependency tracker.
     */
    void AddDependency( const std::wstring& dependencyFile );

    /**
     * Check to see if the base file or any of the dependencies have changed
     * on disk.
     * Using this function is expensive and should only be checked during load
     * if the optimized asset needs to be reexported because the original 
     * asset or one of its dependencies have changed.
     * The asset should register for the FileChanged event and only reload the 
     * asset if that event is fired.
     * @returns true if the base file or any of the dependencies have been 
     * modified since the last load time.
     */
    bool IsStale();

    /**
     * Update the last load time to this value.
     * @param lastLoadTime Update the last load time by this time. By default, it
     * will use the default time of the system clock.
     */
    void SetLastLoadTime( fs::file_time_type lastLoadTime = fs::file_time_type::clock::now() );
    fs::file_time_type GetLastLoadTime() const;

    /**
     * This function will not save the assets that the dependency tracks.
     * 
     * Save the dependency file to disk.
     * The dependency file will be saved in the same folder as the base file
     * with the extension of the base file replaced with ".dep".
     * 
     * @returns true if the dependency file was successfully saved to disk.
     */
    bool Save();

    /**
     * This function will not load the assets that the dependency tracks.
     * 
     * Load the dependencies from the dependency file.
     * The dependency file will be loaded from the same folder as the base file
     * with the extension of the base file replaced with ".dep".
     * 
     * @returns true if the dependencies were successfully loaded.
     */
    bool Load();

    /**
     * This event will be fired if the base file or any of its
     * dependencies have changed on disk. This event only fires
     * if the file has changed at runtime. To check if a file
     * that has been previously exported needs to be re-exported, use the 
     * Dependency::IsStale method during loading of the optimized asset.
     */
    Core::FileChangeEvent FileChanged;

protected:
    
    // Callback function that gets invoked when a file modification has been detected.
    virtual void OnFileChanged( Core::FileChangeEventArgs& e );

private:
    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive& ar, const unsigned int version );
    
    // The base file.
    std::wstring m_BaseFile;

    // The dependency file is used to serialize an asset's dependencies
    // For some assets it is possible to automatically determine their dependencies,
    // but in most cases, we just want to manually describe an asset's 
    // dependencies (for example, for various model formats).
    fs::path m_DependencyPath;

    // The parent path stores the parent folder of the dependency file.
    // This is needed so that dependencies can be expressed relative
    // to the dependency file.
    fs::path m_ParentPath;

    typedef std::vector< std::wstring > DependencyList;
    DependencyList m_Dependencies;

    // The last time the base file and it's dependencies were loaded.
    // If the base file or it's dependencies are modified since they were 
    // last loaded, the dependency tracker will be marked "stale" and the 
    // assets should be reloaded.
    fs::file_time_type m_LastLoadtime;

    // Mutex to protect modifications to dependency tracker 
    typedef std::recursive_mutex Mutex;
    typedef std::unique_lock<Mutex> MutexLock;
    Mutex m_Mutex;


    Core::Event::ScopedConnections m_EventConnections;
};

#include "DependencyTracker.inl"

BOOST_CLASS_VERSION( DependencyTracker, 0 );
