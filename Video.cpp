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
