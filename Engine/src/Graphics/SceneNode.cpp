#include <EnginePCH.h>

#include <Events.h>
#include <SceneVisitor.h>
#include <Graphics/Mesh.h>
#include <Graphics/SceneNode.h>

using namespace Graphics;

SceneNode::SceneNode( const glm::mat4& localTransform )
    : m_LocalTransform( localTransform )
    , m_Name( "SceneNode" )
{
    m_InverseTransform = glm::inverse( m_LocalTransform );
}

SceneNode::~SceneNode()
{
    // Delete children.
    m_Children.clear();
}

const std::string& SceneNode::GetName() const
{
    return m_Name;
}

void  SceneNode::SetName( const std::string& name )
{
    m_Name = name;
}

glm::mat4 SceneNode::GetLocalTransform() const
{
    return m_LocalTransform;
}

void SceneNode::SetLocalTransform( const glm::mat4& localTransform )
{
    m_LocalTransform = localTransform;
    m_InverseTransform = glm::inverse( localTransform );
}

glm::mat4 SceneNode::GetInverseLocalTransform() const
{
    return m_InverseTransform;
}

glm::mat4 SceneNode::GetWorldTransform() const
{
    return GetParentWorldTransform() * m_LocalTransform;
}

void SceneNode::SetWorldTransform( const glm::mat4& worldTransform )
{
    glm::mat4 inverseParentTransform = glm::inverse( GetParentWorldTransform() );
    SetLocalTransform( inverseParentTransform * worldTransform );
}

glm::mat4 SceneNode::GetInverseWorldTransform() const
{
    return glm::inverse( GetWorldTransform() );
}

glm::mat4 SceneNode::GetParentWorldTransform() const
{
    glm::mat4 parentTransform( 1.0f );
    if ( std::shared_ptr<SceneNode> parent = m_pParentNode.lock() )
    {
        parentTransform = parent->GetWorldTransform();
    }

    return parentTransform;
}

void SceneNode::AddChild( std::shared_ptr<SceneNode> pNode )
{
    if ( pNode )
    {
        NodeList::iterator iter = std::find( m_Children.begin(), m_Children.end(), pNode );
        if ( iter == m_Children.end() )
        {
            glm::mat4 worldTransform = pNode->GetWorldTransform();
            pNode->m_pParentNode = shared_from_this();
            glm::mat4 localTransform = GetInverseWorldTransform() * worldTransform;
            pNode->SetLocalTransform( localTransform );
            m_Children.push_back( pNode );
            if ( !pNode->GetName().empty() )
            {
                m_ChildrenByName.insert( NodeNameMap::value_type( pNode->GetName(), pNode ) );
            }
        }
    }
}

void SceneNode::RemoveChild( std::shared_ptr<SceneNode> pNode )
{
    if ( pNode )
    {
        NodeList::iterator iter = std::find( m_Children.begin(), m_Children.end(), pNode );
        if ( iter != m_Children.end() )
        {
            pNode->SetParent( std::weak_ptr<SceneNode>() );

            m_Children.erase( iter );

            // Also remove it from the name map.
            NodeNameMap::iterator iter2 = m_ChildrenByName.find( pNode->GetName() );
            if ( iter2 != m_ChildrenByName.end() )
            {
                m_ChildrenByName.erase( iter2 );
            }
        }
        else
        {
            // Maybe this node appears lower in the hierarchy...
            for ( auto child : m_Children )
            {
                child->RemoveChild( pNode );
            }
        }
    }
}

void SceneNode::SetParent( std::weak_ptr<SceneNode> wpNode )
{
    // Parents own their children.. If this node is not owned
    // by anyone else, it will cease to exist if we remove it from it's parent.
    // As a precaution, store myself as a shared pointer so I don't get deleted
    // half-way through this function!
    // Technically self deletion shouldn't occur because the thing invoking this function
    // should have a shared_ptr to it.
    std::shared_ptr<SceneNode> me = shared_from_this();

    if ( std::shared_ptr<SceneNode> parent = wpNode.lock() )
    {
        parent->AddChild( shared_from_this() );
    }
    else if ( parent = m_pParentNode.lock() )
    {
        // Setting parent to NULL.. remove from current parent and reset parent node.
        glm::mat4 worldTransform = GetWorldTransform();
        parent->RemoveChild( shared_from_this() );
        m_pParentNode.reset();
        SetLocalTransform( worldTransform );
    }
}

void SceneNode::AddMesh( std::shared_ptr<Mesh> mesh )
{
    assert( mesh );
    MeshList::iterator iter = std::find( m_Meshes.begin(), m_Meshes.end(), mesh );
    if ( iter == m_Meshes.end() )
    {
        m_Meshes.push_back( mesh );
    }
}

void SceneNode::RemoveMesh( std::shared_ptr<Mesh> mesh )
{
    assert( mesh );
    MeshList::iterator iter = std::find( m_Meshes.begin(), m_Meshes.end(), mesh );
    if ( iter != m_Meshes.end() )
    {
        m_Meshes.erase( iter );
    }
}

void SceneNode::Render( Core::RenderEventArgs& args )
{
    // First render all my meshes.
    for ( auto mesh : m_Meshes )
    {
        mesh->Render( args );
    }
    
    // Now recurse into children
    for ( auto child : m_Children )
    {
        child->Render( args );
    }
}

void SceneNode::Accept( Core::SceneVisitor& visitor )
{
    visitor.Visit( *this );

    // Visit meshes.
    for ( auto mesh : m_Meshes )
    {
        mesh->Accept( visitor );
    }

    // Now visit children
    for ( auto child : m_Children )
    {
        child->Accept( visitor );
    }
}