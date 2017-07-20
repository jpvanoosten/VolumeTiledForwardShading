#include <GamePCH.h>

#include <PrintProfileDataVisitor.h>
#include <Graphics/Profiler.h>

PrintProfileDataVisitor::PrintProfileDataVisitor( const std::wstring& outputFileName )
    : m_Frame(0)
{
    m_OutputFile.open( outputFileName, std::ios::out );
}

PrintProfileDataVisitor::~PrintProfileDataVisitor()
{
    if ( m_OutputFile.is_open() )
    {
        m_OutputFile.close();
    }
}

void PrintProfileDataVisitor::PrintStats( const Core::Statistic<double>& stat )
{
    const double* pData;
    uint32_t numValues;
    uint32_t offset;

    // The first entry is the current average.
    m_OutputFile << ", " << stat.GetAverage() * 1000.0;

    std::tie( pData, numValues, offset ) = stat.GetSamples();

    for ( uint32_t i = 0; i < numValues; ++i )
    {
        // profiling data is in seconds. n * 1000 to convert to milliseconds.
        m_OutputFile << ", " << ( pData[( i + offset ) % numValues] * 1000.0 );
    }
}

void PrintProfileDataVisitor::Visit( Graphics::Profiler& profiler )
{
    // Check the current frame so we can restrict the output of profiler markers
    // to those with "recent" profiling data.
    m_Frame = profiler.GetCurrentFrame();
}

void PrintProfileDataVisitor::Visit( Graphics::ProfileNode& profileMarker )
{
    // Only write profile data that has "recent" data.
    // When switching techniques, we only want to output the current techniques
    // profiling data.
    //if (m_Frame - profileMarker.CpuFrame < 100)
    //{
    //    m_OutputFile << profileMarker.Name << " (CPU)";

    //    PrintStats(profileMarker.CpuStats);

    //    m_OutputFile << std::endl;
    //}

    // Only write profile data that has "recent" data.
    // When switching techniques, we only want to output the current techniques
    // profiling data.
    if (m_Frame - profileMarker.GpuFrame < 100 && profileMarker.GpuStats.GetNumSamples() > 0)
    {
        m_OutputFile << profileMarker.Name << " (GPU)";
        PrintStats(profileMarker.GpuStats);

        m_OutputFile << std::endl;
    }
}
