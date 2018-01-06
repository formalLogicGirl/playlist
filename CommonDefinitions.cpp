#include "CommonDefinitions.h"

Country CountryFromString( std::string s )
{
	switch( s )
	{
	case "CA" : return CNTRY_CA;
	case "UK" : return CNTRY_UK;
	case "US" : return CNTRY_US;
	default : return CNTRY_UNKNOWN;
	}
	
}

Language LanguageFromString( std::string s )
{
	switch( s )
	{
	case "English" : return LANG_ENGLISH;
	case "Spanish" : return LANG_SPANISH;
	default : return LANG_UNKNOWN;
	}
}

AspectRatio AspectRatioFromString( std::string s )
{
	switch( s )
	{
	case "4:3" : return AR_4TO3;
	case "16:9" : return AR_16TO9;
	default : return AR_UNKNOWN;
	}
}
