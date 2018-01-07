#ifndef COMMON_H
#define COMMON_H

#include <string>

enum ErrorCode
{
	ECODE_NO_PREROLL_FOR_COUNTRY,
	ECODE_NO_PREROLL_FOR_LANGUAGE,
	ECODE_NO_PREROLL_FOR_ASPECT,
	ECODE_NO_CONTENT_VIDEO_FOUND
};

enum Country
{
	CNTRY_CA,
	CNTRY_UK,
	CNTRY_US,
	CNTRY_UNKNOWN
};

enum Language
{
	LANG_ENGLISH,
	LANG_SPANISH,
	LANG_UNKNOWN
};

enum AspectRatio
{
	AR_4TO3,
	AR_16TO9,
	AR_UNKNOWN
};

inline Country CountryFromString( std::string s )
{
	if( s == "CA" )
		return CNTRY_CA;
	if( s == "UK")
		return CNTRY_UK;
	if( s == "US")
		return CNTRY_US;
	return CNTRY_UNKNOWN;
}

inline std::string CountryToString( Country c )
{
	if( c == CNTRY_CA )
		return "CA";
	if( c == CNTRY_UK )
		return "UK";
	if( c == CNTRY_US )
		return "US";
	return "";
}

inline Language LanguageFromString( std::string s )
{
	if( s == "English")
		return LANG_ENGLISH;
	if( s == "Spanish")
		return LANG_SPANISH;
	return LANG_UNKNOWN;
}

inline AspectRatio AspectRatioFromString( std::string s )
{
	if( s == "4:3")
		return AR_4TO3;
	if( s == "16:9")
		return AR_16TO9;
	return AR_UNKNOWN;
}

#endif
