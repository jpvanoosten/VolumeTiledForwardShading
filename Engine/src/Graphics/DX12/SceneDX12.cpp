#include <EnginePCH.h>

#include <assimp/importerdesc.h>

#include <Application.h>

#include <Graphics/DX12/SceneDX12.h>
#include <Graphics/DX12/DeviceDX12.h>
#include <Graphics/DX12/TextureDX12.h>
#include <Graphics/DX12/VertexBufferDX12.h>
#include <Graphics/DX12/IndexBufferDX12.h>
#include <Graphics/ComputeCommandBuffer.h>
#include <Graphics/Mesh.h>
#include <Graphics/SceneNode.h>
#include <Graphics/Material.h>

#include <LogManager.h>
#include <SceneVisitor.h>

using namespace Core;
using namespace Graphics;

#define EXPORT_FORMAT "assbin"
#define EXPORT_EXTENSION "assbin"

// A private class that is registered with Assimp's importer
// Provides feedback on the loading progress of the scene files.
// 

class ProgressHandler : public Assimp::ProgressHandler
{
public:
    ProgressHandler( SceneDX12& scene, const std::wstring& fileName )
        : m_Scene( scene )
        , m_FileName( fileName )
    {}

    virtual bool Update( float percentage )
    {
        Core::ProgressEventArgs progressEventArgs( m_Scene, m_FileName, percentage );

        m_Scene.OnLoadingProgress( progressEventArgs );

        return !progressEventArgs.Cancel;
    }

private:
    SceneDX12& m_Scene;
    std::wstring m_FileName;
};


SceneDX12::SceneDX12( std::shared_ptr<DeviceDX12> device )
    : m_Device( device )
{}

SceneDX12::~SceneDX12()
{

}

bool SceneDX12::LoadFromFile( std::shared_ptr<ComputeCommandBuffer> computeCommandBuffer, const std::wstring& fileName )
{
    fs::path filePath = fileName;
    fs::path parentPath;

    m_SceneFile = fileName;

    if ( filePath.has_parent_path() )
    {
        parentPath = filePath.parent_path();
    }
    else
    {
        parentPath = fs::current_path();
    }

    const aiScene* scene;
    Assimp::Importer importer;

    importer.SetProgressHandler( new ProgressHandler( *this, fileName ) );

    Application::Get().SetLoadingMessage( fileName );

    fs::path exportPath = filePath;
    exportPath.replace_extension( EXPORT_EXTENSION );

    if ( fs::exists( exportPath ) && fs::is_regular_file( exportPath ) )
    {
        LOG_INFO( "Loading scene ", exportPath );
        // If a previously exported file exists, load that instead (scene has already been preprocessed).
        scene = importer.ReadFile( exportPath.string(), 0 );
    }
    else
    {
        LOG_INFO( "Loading scene ", filePath );

        // If no serialized version of the model file exists (or the original model is newer than the serialized version),
        // reimport the original scene and export it as binary.
        importer.SetPropertyFloat( AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f );
        importer.SetPropertyInteger( AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE );

        unsigned int preprocessFlags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_OptimizeGraph;
        scene = importer.ReadFile( filePath.string(), preprocessFlags );

        if ( scene )
        {
            // Now export the preprocessed scene so we can load it faster next time.
            Assimp::Exporter exporter;
            exporter.Export( scene, EXPORT_FORMAT, exportPath.string(), preprocessFlags );
        }
    }

    if ( !scene )
    {
        LogManager::LogError( importer.GetErrorString() );
        return false;
    }
    else
    {
        // If we have a previously loaded scene, delete it.
        glm::mat4 localTransform( 1 );
        if ( m_RootNode )
        {
            // Save the root nodes local transform
            // so it can be restored on reload.
            localTransform = m_RootNode->GetLocalTransform();
            m_RootNode.reset();
        }
        // Delete the previously loaded assets.
        m_MaterialMap.clear();
        m_Materials.clear();
        m_Meshes.clear();

        // Import scene materials.
        for ( unsigned int i = 0; i < scene->mNumMaterials; ++i )
        {
            ImportMaterial( computeCommandBuffer, *scene->mMaterials[i], parentPath );
        }
        // Import meshes
        for ( unsigned int i = 0; i < scene->mNumMeshes; ++i )
        {
            ImportMesh( computeCommandBuffer, *scene->mMeshes[i] );
        }

        m_RootNode = ImportSceneNode( computeCommandBuffer, m_RootNode, scene->mRootNode );
        m_RootNode->SetLocalTransform( localTransform );
    }

    return true;
}

bool SceneDX12::LoadFromString( std::shared_ptr<ComputeCommandBuffer> computeCommandBuffer, const std::string& strScene, const std::string& format )
{
    Assimp::Importer importer;
    const aiScene* scene = nullptr;

    importer.SetProgressHandler( new ProgressHandler( *this, L"String" ) );

    importer.SetPropertyFloat( AI_CONFIG_PP_GSN_MAX_SMOOTHING_ANGLE, 80.0f );
    importer.SetPropertyInteger( AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE );

    unsigned int preprocessFlags = aiProcessPreset_TargetRealtime_MaxQuality;

    scene = importer.ReadFileFromMemory( strScene.data(), strScene.size(), preprocessFlags, format.c_str() );

    if ( !scene )
    {
        LOG_ERROR( importer.GetErrorString() );
        return false;
    }
    else
    {
        // If we have a previously loaded scene, delete it.
        if ( m_RootNode )
        {
            m_RootNode.reset();
        }

        // Import scene materials.
        for ( unsigned int i = 0; i < scene->mNumMaterials; ++i )
        {
            ImportMaterial( computeCommandBuffer, *scene->mMaterials[i], fs::current_path() );
        }
        // Import meshes
        for ( unsigned int i = 0; i < scene->mNumMeshes; ++i )
        {
            ImportMesh( computeCommandBuffer, *scene->mMeshes[i] );
        }

        m_RootNode = ImportSceneNode( computeCommandBuffer, m_RootNode, scene->mRootNode );
    }

    return true;
}

void SceneDX12::Render( Core::RenderEventArgs& renderEventArgs )
{
    if ( m_RootNode )
    {
        m_RootNode->Render( renderEventArgs );
    }
}

std::shared_ptr<SceneNode> SceneDX12::GetRootNode() const
{
    return m_RootNode;
}

void SceneDX12::Accept( Core::SceneVisitor& visitor )
{
    visitor.Visit( *this );
    if ( m_RootNode )
    {
        m_RootNode->Accept( visitor );
    }
}

void SceneDX12::ImportMaterial( std::shared_ptr<ComputeCommandBuffer> computeCommandBuffer, const aiMaterial& material, fs::path parentPath )
{
    aiString materialName;
    aiString aiTexturePath;
    aiTextureOp aiBlendOperation;
    float blendFactor;
    aiColor4D diffuseColor;
    aiColor4D specularColor;
    aiColor4D ambientColor;
    aiColor4D emissiveColor;
    float opacity;
    float indexOfRefraction;
    float reflectivity;
    float shininess;
    float bumpIntensity;

    std::shared_ptr<DeviceDX12> deviceDX12 = m_Device.lock();

    std::shared_ptr<Material> pMaterial = deviceDX12->CreateMaterial();

    if ( material.Get( AI_MATKEY_COLOR_AMBIENT, ambientColor ) == aiReturn_SUCCESS )
    {
        pMaterial->SetAmbientColor( glm::vec4( ambientColor.r, ambientColor.g, ambientColor.b, ambientColor.a ) );
    }
    if ( material.Get( AI_MATKEY_COLOR_EMISSIVE, emissiveColor ) == aiReturn_SUCCESS )
    {
        pMaterial->SetEmissiveColor( glm::vec4( emissiveColor.r, emissiveColor.g, emissiveColor.b, emissiveColor.a ) );
    }
    if ( material.Get( AI_MATKEY_COLOR_DIFFUSE, diffuseColor ) == aiReturn_SUCCESS )
    {
        pMaterial->SetDiffuseColor( glm::vec4( diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a ) );
    }
    if ( material.Get( AI_MATKEY_COLOR_SPECULAR, specularColor ) == aiReturn_SUCCESS )
    {
        pMaterial->SetSpecularColor( glm::vec4( specularColor.r, specularColor.g, specularColor.b, specularColor.a ) );
    }
    if ( material.Get( AI_MATKEY_SHININESS, shininess ) == aiReturn_SUCCESS )
    {
        pMaterial->SetSpecularPower( shininess );
    }
    if ( material.Get( AI_MATKEY_OPACITY, opacity ) == aiReturn_SUCCESS )
    {
        pMaterial->SetOpacity( opacity );
    }
    if ( material.Get( AI_MATKEY_REFRACTI, indexOfRefraction ) )
    {
        pMaterial->SetIndexOfRefraction( indexOfRefraction );
    }
    if ( material.Get( AI_MATKEY_REFLECTIVITY, reflectivity ) == aiReturn_SUCCESS )
    {
        pMaterial->SetReflectance( glm::vec4( reflectivity ) );
    }
    if ( material.Get( AI_MATKEY_BUMPSCALING, bumpIntensity ) == aiReturn_SUCCESS )
    {
        pMaterial->SetBumpIntensity( bumpIntensity );
    }

    // Load ambient textures.
    if ( material.GetTextureCount( aiTextureType_AMBIENT ) > 0 &&
         material.GetTexture( aiTextureType_AMBIENT, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation ) == aiReturn_SUCCESS )
    {
        fs::path texturePath( aiTexturePath.C_Str() );
        std::shared_ptr<Texture> pTexture = deviceDX12->CreateTexture( computeCommandBuffer, ( parentPath / texturePath ).wstring() );
        pMaterial->SetTexture( Material::TextureType::Ambient, pTexture );
    }

    // Load emissive textures.
    if ( material.GetTextureCount( aiTextureType_EMISSIVE ) > 0 &&
         material.GetTexture( aiTextureType_EMISSIVE, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation ) == aiReturn_SUCCESS )
    {
        fs::path texturePath( aiTexturePath.C_Str() );
        std::shared_ptr<Texture> pTexture = deviceDX12->CreateTexture( computeCommandBuffer, ( parentPath / texturePath ).wstring() );
        pMaterial->SetTexture( Material::TextureType::Emissive, pTexture );
    }

    // Load diffuse textures.
    if ( material.GetTextureCount( aiTextureType_DIFFUSE ) > 0 &&
         material.GetTexture( aiTextureType_DIFFUSE, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation ) == aiReturn_SUCCESS )
    {
        fs::path texturePath( aiTexturePath.C_Str() );
        std::shared_ptr<Texture> pTexture = deviceDX12->CreateTexture( computeCommandBuffer, ( parentPath / texturePath ).wstring() );
        pMaterial->SetTexture( Material::TextureType::Diffuse, pTexture );
    }

    // Load specular texture.
    if ( material.GetTextureCount( aiTextureType_SPECULAR ) > 0 &&
         material.GetTexture( aiTextureType_SPECULAR, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation ) == aiReturn_SUCCESS )
    {
        fs::path texturePath( aiTexturePath.C_Str() );
        std::shared_ptr<Texture> pTexture = deviceDX12->CreateTexture( computeCommandBuffer, ( parentPath / texturePath ).wstring() );
        pMaterial->SetTexture( Material::TextureType::Specular, pTexture );
    }


    // Load specular power texture.
    if ( material.GetTextureCount( aiTextureType_SHININESS ) > 0 &&
         material.GetTexture( aiTextureType_SHININESS, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation ) == aiReturn_SUCCESS )
    {
        fs::path texturePath( aiTexturePath.C_Str() );
        std::shared_ptr<Texture> pTexture = deviceDX12->CreateTexture( computeCommandBuffer, ( parentPath / texturePath ).wstring() );
        pMaterial->SetTexture( Material::TextureType::SpecularPower, pTexture );
    }

    if ( material.GetTextureCount( aiTextureType_OPACITY ) > 0 &&
         material.GetTexture( aiTextureType_OPACITY, 0, &aiTexturePath, nullptr, nullptr, &blendFactor, &aiBlendOperation ) == aiReturn_SUCCESS )
    {
        fs::path texturePath( aiTexturePath.C_Str() );
        std::shared_ptr<Texture> pTexture = deviceDX12->CreateTexture( computeCommandBuffer, ( parentPath / texturePath ).wstring() );
        pMaterial->SetTexture( Material::TextureType::Opacity, pTexture );
    }

    // Load normal map texture.
    if ( material.GetTextureCount( aiTextureType_NORMALS ) > 0 &&
         material.GetTexture( aiTextureType_NORMALS, 0, &aiTexturePath ) == aiReturn_SUCCESS )
    {
        fs::path texturePath( aiTexturePath.C_Str() );
        std::shared_ptr<Texture> pTexture = deviceDX12->CreateTexture( computeCommandBuffer, ( parentPath / texturePath ).wstring() );
        pMaterial->SetTexture( Material::TextureType::Normal, pTexture );
    }
    // Load bump map (only if there is no normal map).
    else if ( material.GetTextureCount( aiTextureType_HEIGHT ) > 0 &&
              material.GetTexture( aiTextureType_HEIGHT, 0, &aiTexturePath, nullptr, nullptr, &blendFactor ) == aiReturn_SUCCESS )
    {
        fs::path texturePath( aiTexturePath.C_Str() );
        std::shared_ptr<Texture> pTexture = deviceDX12->CreateTexture( computeCommandBuffer, ( parentPath / texturePath ).wstring() );

        // Some materials actually store normal maps in the bump map slot. Assimp can't tell the difference between 
        // these two texture types, so we try to make an assumption about whether the texture is a normal map or a bump
        // map based on its pixel depth. Bump maps are usually 8 BPP (grayscale) and normal maps are usually 24 BPP or higher.
        Material::TextureType textureType = ( pTexture->GetBPP() >= 24 ) ? Material::TextureType::Normal : Material::TextureType::Bump;

        pMaterial->SetTexture( textureType, pTexture );
    }

    //m_MaterialMap.insert( MaterialMap::value_type( materialName.C_Str(), pMaterial ) );
    m_Materials.push_back( pMaterial );
}

void SceneDX12::ImportMesh( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, const aiMesh& mesh )
{
    std::shared_ptr<Device> device = m_Device.lock();

    std::shared_ptr<Mesh> pMesh = device->CreateMesh();

    assert( mesh.mMaterialIndex < m_Materials.size() );
    pMesh->SetMaterial( m_Materials[mesh.mMaterialIndex] );

    std::vector<Mesh::Vertex> vertexData( mesh.mNumVertices );
    unsigned int i;

    if ( mesh.HasPositions() )
    {
        for ( i = 0; i < mesh.mNumVertices; ++i )
        {
            vertexData[i].Position = glm::vec3( mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z );
        }
    }

    if ( mesh.HasNormals() )
    {
        for ( i = 0; i < mesh.mNumVertices; ++i )
        {
            vertexData[i].Normal = glm::vec3( mesh.mNormals[i].x, mesh.mNormals[i].y, mesh.mNormals[i].z );
        }
    }

    if ( mesh.HasTangentsAndBitangents() )
    {
        for ( i = 0; i < mesh.mNumVertices; ++i )
        {
            vertexData[i].Tangent = glm::vec3( mesh.mTangents[i].x, mesh.mTangents[i].y, mesh.mTangents[i].z );
            vertexData[i].BiTangent = glm::vec3( mesh.mBitangents[i].x, mesh.mBitangents[i].y, mesh.mBitangents[i].z );
        }
    }

    if ( mesh.HasTextureCoords( 0 ) )
    {
        switch ( mesh.mNumUVComponents[0] )
        {
        case 1: // 1-component texture coordinates (U)
        {
            std::vector<float> texcoods1D( mesh.mNumVertices );
            for ( i = 0; i < mesh.mNumVertices; ++i )
            {
                vertexData[i].TexCoord = glm::vec3( mesh.mTextureCoords[0][i].x, 0, 0 );
            }
        }
        break;
        case 2: // 2-component texture coordinates (U,V)
        {
            for ( i = 0; i < mesh.mNumVertices; ++i )
            {
                vertexData[i].TexCoord = glm::vec3( mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y, 0 );
            }
        }
        break;
        case 3: // 3-component texture coordinates (U,V,W)
        {
            for ( i = 0; i < mesh.mNumVertices; ++i )
            {
                vertexData[i].TexCoord = glm::vec3( mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y, mesh.mTextureCoords[0][i].z );
            }
        }
        break;
        }
    }

    std::shared_ptr<VertexBuffer> vertexBuffer = device->CreateVertexBuffer( copyCommandBuffer, vertexData );
    pMesh->SetVertexBuffer( 0, vertexBuffer );

    // Extract the index buffer.
    if ( mesh.HasFaces() )
    {
        std::vector<unsigned int> indices;
        for ( unsigned int i = 0; i < mesh.mNumFaces; ++i )
        {
            const aiFace& face = mesh.mFaces[i];
            // Only extract triangular faces
            if ( face.mNumIndices == 3 )
            {
                indices.push_back( face.mIndices[0] );
                indices.push_back( face.mIndices[1] );
                indices.push_back( face.mIndices[2] );
            }
        }
        if ( indices.size() > 0 )
        {
            std::shared_ptr<IndexBuffer> indexBuffer = device->CreateIndexBuffer( copyCommandBuffer, indices );
            pMesh->SetIndexBuffer( indexBuffer );
        }
    }

    m_Meshes.push_back( pMesh );
}

std::shared_ptr<SceneNode> SceneDX12::ImportSceneNode( std::shared_ptr<CopyCommandBuffer> copyCommandBuffer, std::shared_ptr<SceneNode> parent, aiNode* aiNode )
{
    if ( !aiNode )
    {
        return nullptr;
    }

    // Assimp stores its matrices in row-major but GLM uses column-major.
    // We have to transpose the matrix before using it to construct a glm matrix.
    aiMatrix4x4 mat = aiNode->mTransformation;
    glm::mat4 localTransform( mat.a1, mat.b1, mat.c1, mat.d1,
                              mat.a2, mat.b2, mat.c2, mat.d2,
                              mat.a3, mat.b3, mat.c3, mat.d3,
                              mat.a4, mat.b4, mat.c4, mat.d4 );

    std::shared_ptr<SceneNode> pNode = std::make_shared<SceneNode>( localTransform );
    pNode->SetParent( parent );

    std::string nodeName( aiNode->mName.C_Str() );
    if ( !nodeName.empty() )
    {
        pNode->SetName( nodeName );
    }

    // Add meshes to scene node
    for ( unsigned int i = 0; i < aiNode->mNumMeshes; ++i )
    {
        assert( aiNode->mMeshes[i] < m_Meshes.size() );

        std::shared_ptr<Mesh> pMesh = m_Meshes[aiNode->mMeshes[i]];
        pNode->AddMesh( pMesh );
    }

    // Recursively Import children
    for ( unsigned int i = 0; i < aiNode->mNumChildren; ++i )
    {
        std::shared_ptr<SceneNode> pChild = ImportSceneNode( copyCommandBuffer, pNode, aiNode->mChildren[i] );
        pNode->AddChild( pChild );
    }

    return pNode;
}
