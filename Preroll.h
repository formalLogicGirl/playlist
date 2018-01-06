#ifndef PREROLL_H
#define PREROLL_H

#include "CommonDefinitions.h"
#include "Video.h"
#include <unordered_set>
#include <list>

class Preroll
{
private:
	std::list<Video*> videos;
	
public:
	Preroll( std::list<Video*>& v)
	{
		videos = v;
	}

	std::list<std::string> GetVideoNamesForCountryLanguageAspect( const Country c, const Language l, const AspectRatio ar );

	std::list<std::string> GetVideoNamesForCountryLanguage( const Country c, const Language l );

};

#endif
