#ifndef CONTENT_H
#define CONTENT_H

#include "CommonDefinitions.h"
#include "Video.h"
#include <unordered_set>

class Content
{
private:
	std::list<std::string> prerollNames;
	std::list<Video*> videos;
	
public:
	Content( std::list<std::string>& prerolls, std::list<Video*>& v)
	{
		prerollNames = prerolls;
		videos = v;
	}

	std::list<std::string> GetPrerollNames();

	std::list<Video*> GetVideosForCountry( Country );
};

#endif
