// AlphIO.h
//
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2002 Iain Murray
//
/////////////////////////////////////////////////////////////////////////////


#ifndef __AlphIO_h__
#define __AlphIO_h__


#include "DasherTypes.h"

#include "MSVC_Unannoy.h"
#include <string>
#include <map>
#include <vector>
#include <utility> // for std::pair
#include <stdio.h> // for C style file IO

namespace expat {
	#include <expat.h>
}

namespace Dasher {class CAlphIO;}
class Dasher::CAlphIO
{
public:
	// This structure completely describes the characters used in alphabet
	struct AlphInfo
	{
		// Basic information
		std::string AlphID;
		bool Mutable; // If from user we may play. If from system defaults this is immutable. User should take a copy.
		
		// Complete description of the alphabet:
		std::string TrainingFile;
		Opts::AlphabetTypes Encoding;
		Opts::AlphabetTypes Type;
		Opts::ScreenOrientations Orientation;
		struct character
		{
			std::string Display;
			std::string Text;
		        std::string Colour;
		        std::string Foreground;
		};
		struct group
		{
			std::string Description;
			std::vector< character > Characters;
		};
		std::vector< group > Groups;
		character SpaceCharacter; // display and edit text of Space character. Typically (" ", "_"). Use ("", "") if no space character.
	};
	
	CAlphIO(std::string SystemLocation, std::string UserLocation);
	void GetAlphabets(std::vector< std::string >* AlphabetList) const;
	const AlphInfo& GetInfo(const std::string& AlphID);
	void SetInfo(const AlphInfo& NewInfo);
	void Delete(const std::string& AlphID);
private:
	AlphInfo BlankInfo;
	std::string SystemLocation;
	std::string UserLocation;
	std::map<std::string, AlphInfo> Alphabets; // map short names (file names) to descriptions
	
	void Save(const std::string& AlphID);
	void CreateDefault(); // Give the user an English alphabet rather than nothing if anything goes horribly wrong.
	
	// XML handling:
	/////////////////////////
	
	bool LoadMutable;
	void ParseFile(std::string Filename);
	
	// Alphabet types:
	std::map<std::string, Opts::AlphabetTypes> StoT;
	std::map<Opts::AlphabetTypes, std::string> TtoS;
	
	// & to &amp;  < to &lt; and > to &gt;  and if (Attribute) ' to &apos; and " to &quot;
	void XML_Escape(std::string* Text, bool Attribute); 
	
	// Data gathered
	std::string CData; // Text gathered from when an elemnt starts to when it ends
	AlphInfo InputInfo;
	
	// Callback functions. These involve the normal dodgy casting to a pointer
	// to an instance to get a C++ class to work with a plain C library.
	static void XML_StartElement(void *userData, const expat::XML_Char *name, const expat::XML_Char **atts);
	static void XML_EndElement(void *userData, const expat::XML_Char *name);
	static void XML_CharacterData(void *userData, const expat::XML_Char *s, int len);
};


#endif /* #ifndef __AlphIO_h__ */
