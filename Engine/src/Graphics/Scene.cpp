#include <EnginePCH.h>

#include <Graphics/Scene.h>

using namespace Graphics;

void Scene::OnLoadingProgress( Core::ProgressEventArgs& e )
{
    LoadingProgress( e );
}
