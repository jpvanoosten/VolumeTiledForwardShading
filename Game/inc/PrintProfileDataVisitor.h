#pragma once

#include <ProfilerVisitor.h>

namespace Core
{
    template<typename T>
    class Statistic;
}

/**
 * Visits all profile nodes and prints the profile data to a CSV file for analysis.
 */
class PrintProfileDataVisitor : public Core::ProfilerVisitor
{
public:
    PrintProfileDataVisitor( const std::wstring& outputFileName );

    virtual ~PrintProfileDataVisitor();

    virtual void Visit( Graphics::Profiler& profiler ) override;
    virtual void Visit( Graphics::ProfileNode& profileMarker ) override;

private:

    void PrintStats( const Core::Statistic<double>& stat );

    std::ofstream m_OutputFile;

    // The profiling frame when stats are captured.
    uint64_t m_Frame;
};