#ifndef VIDEO_H
#define VIDEO_H

#include <string>
#include <list>
#include "CommonDefinitions.h"

class Video
{
private:
	std::string name;
	std::list<Country> countries;
	Language language;
	AspectRatio aspectRatio;
	
public:
	Video( std::string & vidName, Language lang, AspectRatio ar, std::list<Country> & cntries )
	{
		name = vidName;
		language = lang;
		aspectRatio = ar;
		countries = std::list<Country>();
		countries = cntries;
	}

    std::string GetName() const;
	
	bool HasCountry( const Country c );

	AspectRatio GetAspectRatio() const;

	Language GetLanguage() const;

	bool operator ==(const Video & obj) const
	{
		return ( obj.GetName() == name );
	}

};

std::list<Video*> FilterVideosByCountry( std::list<Video*>, Country );
std::list<Video*> FilterVideosByLanguage( std::list<Video*>, Language );
std::list<Video*> FilterVideosByAspectRatio( std::list<Video*>, AspectRatio );
/*
namespace std
{
	template<>
		struct hash<Video>
    {
		size_t
			operator()( const Video & obj ) const
		{
			return hash<string>()( obj.GetName() );
		}
    };
}
*/
#endif
