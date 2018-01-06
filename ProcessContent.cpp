#include "CommonDefinitions.h"
#include "Video.h"
#include "Preroll.h"
#include "Content.h"
#include "Playlist.h"

#include<unordered_map>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <iostream>

void PrintVideoNames( std::list<Video*> v )
{
	for( Video* vid : v )
		std::cout << vid->GetName() << "\n";
	
}

void ReadChildVideoArray( boost::property_tree::ptree& parentNode, std::list<Video*>& videoSet )
{
	for ( boost::property_tree::ptree::value_type &video : parentNode.get_child( "videos" ) )
	{
		boost::property_tree::ptree vidRoot = video.second;
		std::string vidName = vidRoot.get( "name", "videoNameDefault" );
		
		boost::property_tree::ptree vidAttrRoot = vidRoot.get_child( "attributes" );
		std::string lang = vidAttrRoot.get( "language", "langDefault" );
		std::string ar = vidAttrRoot.get( "aspect", "aspectDefault" );

		std::cout << vidName << "\t" << lang << "\t" << ar << "\t\t";

		std::list<Country> cntries; 
		for ( boost::property_tree::ptree::value_type &cntry : vidAttrRoot.get_child( "countries" ) )
		{
			std::string cntryName = cntry.second.data();
			Country c = CountryFromString( cntryName );
			if ( c != CNTRY_UNKNOWN )
				cntries.push_back( c );
			
			std::cout << cntryName << "\t";
		}
		std::cout << "\n";

		Language l = LanguageFromString( lang );
		AspectRatio a = AspectRatioFromString( ar );

		if ( l != LANG_UNKNOWN && a != AR_UNKNOWN )
		{
			Video* v = new Video( vidName, l, a, cntries );
			videoSet.push_back( v );
		}
	}

}

int ReadFile( std::string dataFileName, std::unordered_map<std::string, Content*>& contentMap, std::unordered_map<std::string, Preroll*>& prerollMap )
{
	boost::property_tree::ptree data;
	boost::property_tree::read_json( dataFileName, data );
	for ( boost::property_tree::ptree::value_type &show : data.get_child( "content" ) )
	{
		boost::property_tree::ptree showRoot = show.second;
		std::string name = showRoot.get( "name", "videoNameDefault" );
		std::cout << name << "\n";

		std::list<std::string> prList;
		for ( boost::property_tree::ptree::value_type &preroll : showRoot.get_child( "preroll" ) )
		{
			boost::property_tree::ptree prRoot = preroll.second;
			std::string prerollName = prRoot.get( "name", "prerollNameDefault" );
			prList.push_back( prerollName );
			std::cout << prerollName << "\n";
		}

		std::list<Video*> videoSet;
		ReadChildVideoArray( showRoot, videoSet );

		Content* content = new Content( prList, videoSet );
		contentMap[name] = content; 
	}

	for ( boost::property_tree::ptree::value_type &pr : data.get_child( "preroll" ) )
	{
		boost::property_tree::ptree prRoot = pr.second;
		std::string name = prRoot.get( "name", "prerollNameDefault" );
		std::cout << name << "\n";

		std::list<Video*> prerollVideoSet;
		ReadChildVideoArray( prRoot, prerollVideoSet );

		Preroll* preroll = new Preroll( prerollVideoSet );
		prerollMap[name] = preroll;
	}

	return 1;
}

std::list<Playlist> ConstructPlaylists( std::string contentName, Country country, std::unordered_map<std::string, Content*> contentMap, std::unordered_map<std::string, Preroll*> prerollMap, ErrorCode* errorCode )
{
	std::list<Playlist> pl;

	std::string sCountry = CountryToString( country );
	Content* content = contentMap[contentName];
	if( content != NULL )
	{
		std::list<Video*> cVids = content->GetVideosForCountry( country );
		/*
		std::cout << "Videos for country" << sCountry << "\n";
		PrintVideoNames( cVids );
		*/
		if( cVids.size() == 0 )
		{
			*errorCode = ECODE_NO_CONTENT_FOUND;
		}
		for( Video* cVideo : cVids )
		{
			std::list<Playlist> playlistsForOneContentVideo;
			Language language = cVideo->GetLanguage();
			AspectRatio aspect = cVideo->GetAspectRatio();

			bool prerollNotFound = false;
			
			// Try to construct a playlist including all the prerolls
			for( std::string prName : content->GetPrerollNames() )
			{
				Preroll* pr = prerollMap[prName];
				// Can pr be played in country?
				// Can pr be played with the same aspect ratio and language as cVideo?
				std::list<std::string> pVids = pr->GetVideoNamesForCountryLanguageAspect( country, language, aspect );
				/*
				std::cout << "Videos for country" << sCountry << "preroll name" << prName << "\n";
				for( std::string s : pVids )
					std::cout << s << "\n";
				*/

				if( pVids.size() > 0 )
				{
					// Add to playlists
					std::list<Playlist> playlistsForOneContentVideoNextVersion;
					if ( playlistsForOneContentVideo.size() == 0 )
					{
						for( std::string pVid : pVids )
						{
							Playlist newP;
							newP.push_back( pVid );
							playlistsForOneContentVideoNextVersion.push_back( newP );
						}
					}
					else
					{
						for( std::string pVid : pVids )
						{

							for( Playlist p : playlistsForOneContentVideo )
							{
								// p + pVid
								Playlist newP;
								newP.assign( p.begin(), p.end() );
								newP.push_back( pVid );
								playlistsForOneContentVideoNextVersion.push_back( newP );
							}
						}
					}
					// copy over : playlistsForOneContentVideo <- playlistsForOneContentVideoNextVersion
					playlistsForOneContentVideo.clear();
					playlistsForOneContentVideo.assign ( playlistsForOneContentVideoNextVersion.begin(), playlistsForOneContentVideoNextVersion.end() );
				}
				else
				{
					prerollNotFound = true;
					*errorCode = ECODE_NO_PREROLL_FOUND;
					break;
				}				
			} // loop for prName

			if ( !prerollNotFound )  // All prerolls have been attached
			{
				// Add cVideo to playlist. Then add the resulting playlist to the master list
				for( Playlist p : playlistsForOneContentVideo )
				{
					p.push_back( cVideo->GetName() );
					pl.push_back( p );
				}
			}

		} // loop for cVideo
	}
	
	return pl;
}

void PrintPlaylist( Playlist p )
{
	std::cout << "[\t";
	for( std::string s : p )
		std::cout << s << "\t";
	std::cout << "]\n";
}

/*
std::list<Playlist> ConstructPlaylistsParallel( std::string contentName, Country country, std::unordered_map<std::string, Content*> contentMap, std::unordered_map<std::string, Preroll*> prerollMap, ErrorCode* ec )
{
	// Split contentMap -> contentMap1, contentMap2
	
	// Split prerollMap -> prerollMap1, prerollMap2

	std::list<Playlist> l11 = ConstructPlaylists( contentName, country, contentMap1, prerollMap1 );
	std::list<Playlist> l12 = ConstructPlaylists( contentName, country, contentMap1, prerollMap2 );
	std::list<Playlist> l21 = ConstructPlaylists( contentName, country, contentMap2, prerollMap1 );
	std::list<Playlist> l22 = ConstructPlaylists( contentName, country, contentMap2, prerollMap2 );

	// Merge all lists and return
	
}
*/
int main()
{
	std::cout << "Hello\n";

	std::unordered_map<std::string, Content*> contentMap;
	std::unordered_map<std::string, Preroll*> prerollMap;

	ReadFile( "sample.json", contentMap, prerollMap );
	/*
	for( std::string pn : contentMap["MI3"]->GetPrerollNames() )
		std::cout << pn << "\n";
	for( std::string v : prerollMap["WB1"]->GetVideoNamesForCountryLanguage( CNTRY_UK, LANG_ENGLISH ) )
		std::cout << v << "\n";
	*/
	ErrorCode ec;
	std::list<Playlist> playlists = ConstructPlaylists( "MI3", CNTRY_UK, contentMap, prerollMap, &ec );
	std::cout << playlists.size() << "\n";
	for( Playlist p : playlists )
	{
		PrintPlaylist( p );
	}

	return 1;
}
