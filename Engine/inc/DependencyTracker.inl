
template<class Archive>
void DependencyTracker::serialize( Archive& ar, const unsigned int version )
{
    ar & boost::serialization::make_nvp( "BasePath", m_BaseFile );
    ar & boost::serialization::make_nvp( "Dependencies", m_Dependencies );

    if ( Archive::is_loading::value )
    {
        SetBaseFile( m_BaseFile );
    }
}
