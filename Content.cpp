//#include "CommonDefinitions.h"
//#include "Video.h"
#include "Content.h"

std::list<std::string> Content::GetPrerollNames()
{
	return prerollNames;
}

std::list<Video*> Content::GetVideosForCountry( Country c )
{
	std::list<Video*> vSet;
	
	for( Video* v : videos )
	{
		if( v->HasCountry( c ) )
			vSet.push_back( v );
	}

	return vSet;
}
