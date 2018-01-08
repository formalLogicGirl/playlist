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
#include <thread>

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

		// std::cout << vidName << "\t" << lang << "\t" << ar << "\t\t";

		std::list<Country> cntries; 
		for ( boost::property_tree::ptree::value_type &cntry : vidAttrRoot.get_child( "countries" ) )
		{
			std::string cntryName = cntry.second.data();
			Country c = CountryFromString( cntryName );
			if ( c != CNTRY_UNKNOWN )
				cntries.push_back( c );
			
			// std::cout << cntryName << "\t";
		}
		// std::cout << "\n";

		Language l = LanguageFromString( lang );
		AspectRatio a = AspectRatioFromString( ar );

		if ( l != LANG_UNKNOWN && a != AR_UNKNOWN )
		{
			Video* v = new Video( vidName, l, a, cntries );
			videoSet.push_back( v );
		}
	}

}

void ReadFile( std::string dataFileName, std::unordered_map<std::string, Content*>& contentMap, std::unordered_map<std::string, Preroll*>& prerollMap )
{
	boost::property_tree::ptree data;
	boost::property_tree::read_json( dataFileName, data );
	for ( boost::property_tree::ptree::value_type &show : data.get_child( "content" ) )
	{
		boost::property_tree::ptree showRoot = show.second;
		std::string name = showRoot.get( "name", "videoNameDefault" );
		// std::cout << name << "\n";

		std::list<std::string> prList;
		for ( boost::property_tree::ptree::value_type &preroll : showRoot.get_child( "preroll" ) )
		{
			boost::property_tree::ptree prRoot = preroll.second;
			std::string prerollName = prRoot.get( "name", "prerollNameDefault" );
			prList.push_back( prerollName );
			// std::cout << prerollName << "\n";
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
		// std::cout << name << "\n";

		std::list<Video*> prerollVideoSet;
		ReadChildVideoArray( prRoot, prerollVideoSet );

		Preroll* preroll = new Preroll( prerollVideoSet );
		prerollMap[name] = preroll;
	}
}

void ConstructPlaylists( std::string contentName, Country country, std::unordered_map<std::string, Content*>& contentMap, std::unordered_map<std::string, Preroll*>& prerollMap, std::list<Playlist>& pl, ErrorCode* errorCode )
{
	//std::list<Playlist> pl;

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
			*errorCode = ECODE_NO_CONTENT_VIDEO_FOUND;
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
				// Can pr be played in same country, with the same aspect ratio and language as cVideo?
				// std::list<std::string> pVids = pr->GetVideoNamesForCountryLanguageAspect( country, language, aspect );
				/*
				std::cout << "Videos for country" << sCountry << "preroll name" << prName << "\n";
				for( std::string s : pVids )
					std::cout << s << "\n";
				*/
				std::list<Video*> vAll = pr->GetVideos();
				std::list<Video*> vByCountry = FilterVideosByCountry( vAll, country );
				if( vByCountry.size() == 0 )
				{
					*errorCode = ECODE_NO_PREROLL_FOR_COUNTRY;
					prerollNotFound = true;
					break;					
				}
				std::list<Video*> vByLanguage = FilterVideosByLanguage( vByCountry, language );
				if( vByLanguage.size() == 0 )
				{
					*errorCode = ECODE_NO_PREROLL_FOR_LANGUAGE;
					prerollNotFound = true;
					break;					
				}
				std::list<Video*> pVids = FilterVideosByAspectRatio( vByLanguage, aspect );

				if( pVids.size() == 0 )
				{
					*errorCode = ECODE_NO_PREROLL_FOR_ASPECT;
					prerollNotFound = true;
					break;
				}				

				// Add to playlists
				std::list<Playlist> playlistsForOneContentVideoNextVersion;
				if ( playlistsForOneContentVideo.size() == 0 )
				{
					for( Video* pVid : pVids )
					{
						Playlist newP;
						newP.push_back( pVid->GetName() );
						playlistsForOneContentVideoNextVersion.push_back( newP );
					}
				}
				else
				{
					for( Video* pVid : pVids )
					{						
						for( Playlist p : playlistsForOneContentVideo )
						{
							// p + pVid
							Playlist newP;
							newP.assign( p.begin(), p.end() );
							newP.push_back( pVid->GetName() );
							playlistsForOneContentVideoNextVersion.push_back( newP );
						}
					}
				}
				// copy over : playlistsForOneContentVideo <- playlistsForOneContentVideoNextVersion
				playlistsForOneContentVideo.clear();
				playlistsForOneContentVideo.assign ( playlistsForOneContentVideoNextVersion.begin(), playlistsForOneContentVideoNextVersion.end() );
			} // loop for prName

			if ( !prerollNotFound )  // All prerolls have been successfully attached
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
	
	//return pl;
}

void PrintPlaylist( Playlist p )
{
	std::cout << "[\t";
	for( std::string s : p )
		std::cout << s << "\t";
	std::cout << "]\n";
}

void SplitContentMap( std::unordered_map<std::string, Content*>& contentMap,
					  std::unordered_map<std::string, Content*>& contentMap1,
					  std::unordered_map<std::string, Content*>& contentMap2 )
{
	auto first = contentMap.begin();
	auto last = contentMap.end();
	
	auto mid = contentMap.begin();
	std::advance( mid, std::distance( first, last )/2 );
	
	contentMap1.insert( first, mid );
	contentMap2.insert( mid, last );
}

void SplitPrerollMap( std::unordered_map<std::string, Preroll*>& prerollMap,
					  std::unordered_map<std::string, Preroll*>& prerollMap1,
					  std::unordered_map<std::string, Preroll*>& prerollMap2 )
{
	auto first = prerollMap.begin();
	auto last = prerollMap.end();
	
	auto mid = prerollMap.begin();
	std::advance( mid, std::distance( first, last )/2 );
	
	prerollMap1.insert( first, mid );
	prerollMap2.insert( mid, last );
}

void CombinePlaylists( std::list<Playlist>& l1,
					   std::list<Playlist>& l2,
					   std::list<Playlist>& l3,
					   std::list<Playlist>& l4,
					   std::list<Playlist>& result )
{
	result.splice( result.end(), l1 );;
	result.splice( result.end(), l2 );;
	result.splice( result.end(), l3 );;
	result.splice( result.end(), l4 );;
}

/*
  An illustration of how to parallelize computation.
  In this case, there is a fixed amount of parallelism by splitting each unordered_map into two.
  For a practical implementation the split should be based on some maximum size being exceeded.
  For large amount of data, the split should happen before the data is loaded in memory as an unordered_map.
 */
/*

Commented out -- Xcode wont compile the std::thread constructor calls

void ConstructPlaylistsParallel( std::string contentName, Country country, std::unordered_map<std::string, Content*>& contentMap, std::unordered_map<std::string, Preroll*>& prerollMap, std::list<Playlist>& result, ErrorCode* ec )
{
	// Split contentMap -> contentMap1, contentMap2
	std::unordered_map<std::string, Content*> contentMap1, contentMap2;
	SplitContentMap( contentMap, contentMap1, contentMap2 );
	
	// Split prerollMap -> prerollMap1, prerollMap2
	std::unordered_map<std::string, Preroll*> prerollMap1, prerollMap2;
	SplitPrerollMap( prerollMap, prerollMap1, prerollMap2 );

	// Create threads to compute in parallel
	std::list<Playlist> l11;
	ErrorCode ec11;
	std::thread t1( ConstructPlaylists, contentName, country, contentMap1, prerollMap1, l11, &ec11 );
	
	std::list<Playlist> l12;
	ErrorCode ec12;
	std::thread t2( ConstructPlaylists, contentName, country, contentMap1, prerollMap2, l12, &ec12 );
	
	std::list<Playlist> l21;
	ErrorCode ec21;
	std::thread t3( ConstructPlaylists, contentName, country, contentMap2, prerollMap1, l21, &ec21 );
	
	std::list<Playlist> l22;
	ErrorCode ec22;
	std::thread t4( ConstructPlaylists, contentName, country, contentMap2, prerollMap2, l22, &ec22 );

	// Wait for all threads to finish
	t1.join();
	t2.join();
	t3.join();
	t4.join();


	// Combine results - Merge all lists and all error codes
	CombinePlaylists( l11, l12, l21, l22, result );

	// TODO The error codes need to be merged with some logic as well
}
*/

std::list<Playlist> GetPlaylistsFromJSON( std::string filename, std::string contentId, Country c, ErrorCode* ec )	
{
	std::unordered_map<std::string, Content*> contentMap;
	std::unordered_map<std::string, Preroll*> prerollMap;

	ReadFile( filename, contentMap, prerollMap );

	/*
	for( std::string pn : contentMap["MI3"]->GetPrerollNames() )
		std::cout << pn << "\n";
	for( std::string v : prerollMap["WB1"]->GetVideoNamesForCountryLanguage( CNTRY_UK, LANG_ENGLISH ) )
		std::cout << v << "\n";
	*/

	std::list<Playlist> playlists;
	ConstructPlaylists( contentId, c, contentMap, prerollMap, playlists, ec );

	return playlists;
}

int main()
{
	ErrorCode ec;
	std::list<Playlist> playlists = GetPlaylistsFromJSON( "sample.json", "MI3", CNTRY_UK, &ec );
	std::cout << playlists.size() << "\n";
	for( Playlist p : playlists )
	{
		PrintPlaylist( p );
	}

	return 1;
}
