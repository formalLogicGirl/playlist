#include "CommonDefinitions.h"
#include "Video.h"
#include "Preroll.h"
#include <unordered_set>
#include <list>


std::list<std::string> Preroll::GetVideoNamesForCountryLanguageAspect( const Country c, const Language l, const AspectRatio ar )
{
	std::list<std::string> vSet;
	for( auto v = videos.begin(); v != videos.end(); ++v )
	{
		if ( (*v)->HasCountry( c ) && ( (*v)->GetLanguage() == l ) && ( (*v)->GetAspectRatio() == ar ) )
			vSet.push_back( (*v)->GetName() );
	}
	
	return vSet;
}

std::list<Video*> Preroll::GetVideos()
{
	return videos;
}


