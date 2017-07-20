#include <EnginePCH.h>

#include <Graphics/Shader.h>

using namespace Graphics;

void Shader::OnFileChanged( Core::FileChangeEventArgs& e )
{
    FileChanged( e );
}
