#include <string>
//#include <list>
#include "CommonDefinitions.h"
#include "Video.h"

std::string Video::GetName() const
{
	return name;
}

bool Video::HasCountry( const Country c )
{
	for( auto country = countries.begin(); country != countries.end(); ++country )
	{
		if ( c == *country ) return true;
	}

	return false;
}

AspectRatio Video::GetAspectRatio() const
{
	return aspectRatio;
}

Language Video::GetLanguage() const
{
	return language;
}

std::list<Video*> FilterVideosByCountry( std::list<Video*> videos, Country c )
{
	std::list<Video*> resultSet;
	
	for( Video* v : videos )
	{
		if( v->HasCountry( c ) )
			resultSet.push_back( v );
	}

	return resultSet;
}

std::list<Video*> FilterVideosByLanguage( std::list<Video*> videos, Language l )
{
	std::list<Video*> resultSet;
	
	for( Video* v : videos )
	{
		if( v->GetLanguage() == l )
			resultSet.push_back( v );
	}

	return resultSet;
}

std::list<Video*> FilterVideosByAspectRatio( std::list<Video*> videos, AspectRatio a )
{
	std::list<Video*> resultSet;
	
	for( Video* v : videos )
	{
		if( v->GetAspectRatio() == a )
			resultSet.push_back( v );
	}

	return resultSet;
}

