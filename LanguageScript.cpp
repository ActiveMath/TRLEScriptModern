#include "LanguageScript.h"
#include "StringHelper.h"
#include <forward_list>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

using namespace TRLEScript;

	LanguageScript::LanguageScript(const LanguageScript &a) : ScriptFile(a)
	{
		Copy(a);
	}

	LanguageScript& LanguageScript::operator= (const LanguageScript &a)
	{
		if (this != &a)
		{
			Clear();
			Copy(a);
		}

		return *this;
	}

	LanguageScript::LanguageScript(LanguageScript &&a) : ScriptFile(std::move(a))
	{
		gameStrings = a.gameStrings;
		a.gameStrings = nullptr;
	}

	LanguageScript& LanguageScript::operator= (LanguageScript &&a)
	{
		Clear();

		gameStrings = a.gameStrings;
		a.gameStrings = nullptr;

		return *this;
	}

	void LanguageScript::Copy(const LanguageScript &a)
	{
		gameStrings = new GameStrings;

		gameStrings->numPSXStrings = a.gameStrings->numPSXStrings;
		gameStrings->numPCStrings = a.gameStrings->numPCStrings;
		gameStrings->numStrings = a.gameStrings->numStrings;

		gameStrings->PSXStrings = new std::forward_list < LanguageString * >;
		gameStrings->PCStrings = new std::forward_list < LanguageString * >;
		gameStrings->Strings = new std::forward_list < LanguageString * >;

		for (auto str : *a.gameStrings->PSXStrings)
		{
			LanguageString *newString = new LanguageString;
			newString->index = str->index;
			newString->stringLength = str->stringLength;
			newString->string = new char[newString->stringLength + 1];
			strcpy(newString->string, str->string);

			gameStrings->PSXStrings->push_front(newString);
		}

		for (auto str : *a.gameStrings->PCStrings)
		{
			LanguageString *newString = new LanguageString;
			newString->index = str->index;
			newString->stringLength = str->stringLength;
			newString->string = new char[newString->stringLength + 1];
			strcpy(newString->string, str->string);

			gameStrings->PCStrings->push_front(newString);
		}

		for (auto str : *a.gameStrings->Strings)
		{
			LanguageString *newString = new LanguageString;
			newString->index = str->index;
			newString->stringLength = str->stringLength;
			newString->string = new char[newString->stringLength + 1];
			strcpy(newString->string, str->string);

			gameStrings->Strings->push_front(newString);
		}

		gameStrings->PSXStrings->reverse();
		gameStrings->PCStrings->reverse();
		gameStrings->Strings->reverse();
	}

	LanguageScript::LanguageScript(const char *languageName) : ScriptFile(languageName)
	{
		isGameflowScript = false;
		isLanguageScript = true;

		langName = languageName;

		compilingToDAT = false;
		created = false;
	}

	const char *LanguageScript::GetLanguageName() const
	{
		return langName;
	}

	GameflowScriptHeader *LanguageScript::GetGameflowScript() const
	{
		if (!created)
			return nullptr;
		return gameflowScript;
	}

	GameStrings *LanguageScript::GetGameStrings() const
	{
		if (!created)
			return nullptr;
		return gameStrings;
	}

	uint16_t LanguageScript::GetTotalLength(std::forward_list<LanguageString *> *strings) const
	{
		uint16_t totalLength = 0;

		for (auto str : *strings)
		{
			totalLength += str->stringLength + 1;
		}

		return totalLength;
	}

	int LanguageScript::GetStringIndex(const char *str)
	//uint16_t LanguageScript::GetStringIndex(const char *str) const
	{
		if (!created)
		{
			AddNewMessage(MessageTypes::Error, "Attempted to locate a string in " + std::string(filename) + " before its compilation!");
			return -1;
		}

		for (auto it : *gameStrings->Strings)
		{
			if (strcmp(it->string, str) == 0)
				return it->index;
		}

		for (auto it : *gameStrings->PSXStrings)
		{
			if (strcmp(it->string, str) == 0)
				return it->index;
		}

		for (auto it : *gameStrings->PCStrings)
		{
			if (strcmp(it->string, str) == 0)
				return it->index;
		}

		AddNewMessage(MessageTypes::Error, "\"" + std::string(str) + "\" not found in strings");
		return -1;
	}

	void LanguageScript::Parse()
	{
	    return;	//not implemented
	}

	void LanguageScript::CompileToDAT(const char *sourceData)
	//void LanguageScript::CompileToDAT(const char *sourceData, int length)
	{
		compilingToDAT = true;

		GameflowScriptHeader *gameflow = SourceParse(sourceData);
		//GameflowScriptHeader *gameflow = SourceParse(sourceData, length);
		gameflowScript = gameflow;
		if (gameflow == nullptr)
			return;

		//created = true;

		GameStrings *allStrings = new GameStrings;
		allStrings->numStrings = gameflow->numStrings;
		allStrings->numPSXStrings = gameflow->numPSXStrings;
		allStrings->numPCStrings = gameflow->numPCStrings;

		allStrings->PSXStrings = new std::forward_list < LanguageString * > ;
		allStrings->PCStrings = new std::forward_list < LanguageString * >;
		allStrings->Strings = new std::forward_list < LanguageString * >;

		for (auto str : *gameflow->PSXStrings)
		{
			LanguageString* newString = new LanguageString;
			newString->index = str->index;
			newString->stringLength = str->stringLength;
			newString->string = new char[newString->stringLength + 1];
			strcpy(newString->string, str->string);

			allStrings->PSXStrings->push_front(newString);
		}

		for (auto str : *gameflow->PCStrings)
		{
			LanguageString* newString = new LanguageString;
			newString->index = str->index;
			newString->stringLength = str->stringLength;
			newString->string = new char[newString->stringLength + 1];
			strcpy(newString->string, str->string);

			allStrings->PCStrings->push_front(newString);
		}

		for (auto str : *gameflow->Strings)
		{
			LanguageString* newString = new LanguageString;
			newString->index = str->index;
			newString->stringLength = str->stringLength;
			newString->string = new char[newString->stringLength + 1];
			strcpy(newString->string, str->string);

			allStrings->Strings->push_front(newString);
		}

		allStrings->PSXStrings->reverse();
		allStrings->PCStrings->reverse();
		allStrings->Strings->reverse();
		
		/*allStrings->Strings = gameflow->Strings;
		allStrings->PSXStrings = gameflow->PSXStrings;
		allStrings->PCStrings = gameflow->PCStrings;*/

		gameStrings = allStrings;

		allStrings->Strings->reverse();
		allStrings->PSXStrings->reverse();
		allStrings->PCStrings->reverse();

		char *DATName = StringHelper::ReplaceThreeLetterExtension(filename, "dat");
		std::string completeFilename(std::string(StringHelper::ToUpper(DATName)));
		delete[] DATName;

		FILE *output = fopen(completeFilename.c_str(), "wb");
		if (output == nullptr)
		{
			AddNewMessage(MessageTypes::Error, "Cannot open " + std::string(filename) + '!');
			return;
		}

		fwrite(&allStrings->numStrings, sizeof(uint16_t), 1, output);
		fwrite(&allStrings->numPSXStrings, sizeof(uint16_t), 1, output);
		fwrite(&allStrings->numPCStrings, sizeof(uint16_t), 1, output);

		uint16_t totalGenericLength = GetTotalLength(allStrings->Strings);
		uint16_t totalPSXLength = GetTotalLength(allStrings->PSXStrings);
		uint16_t totalPCLength = GetTotalLength(allStrings->PCStrings);

		fwrite(&totalGenericLength, sizeof(uint16_t), 1, output);
		fwrite(&totalPSXLength, sizeof(uint16_t), 1, output);
		fwrite(&totalPCLength, sizeof(uint16_t), 1, output);

		//output offset table
		uint16_t currentOffset = 0;
		for (auto str : *allStrings->Strings)
		{
			fwrite(&currentOffset, sizeof(uint16_t), 1, output);
			currentOffset += str->stringLength + 1;
		}

		for (auto str : *allStrings->PSXStrings)
		{
			fwrite(&currentOffset, sizeof(uint16_t), 1, output);
			currentOffset += str->stringLength + 1;
		}

		for (auto str : *allStrings->PCStrings)
		{
			fwrite(&currentOffset, sizeof(uint16_t), 1, output);
			currentOffset += str->stringLength + 1;
		}

		//output string array
		for (auto str : *allStrings->Strings)
		{
			for (int i = 0; i < str->stringLength; ++i)
			{
				uint8_t currentChar = (static_cast<uint8_t>(str->string[i]) ^ magicByte);
				fwrite(&currentChar, sizeof(uint8_t), 1, output);
			}

			fputc('\0', output);
		}

		for (auto str : *allStrings->PSXStrings)
		{
			for (int i = 0; i < str->stringLength; ++i)
			{
				uint8_t currentChar = (static_cast<uint8_t>(str->string[i]) ^ magicByte);
				fwrite(&currentChar, sizeof(uint8_t), 1, output);
			}

			fputc('\0', output);
		}

		for (auto str : *allStrings->PCStrings)
		{
			for (int i = 0; i < str->stringLength; ++i)
			{
				uint8_t currentChar = (static_cast<uint8_t>(str->string[i]) ^ magicByte);
				fwrite(&currentChar, sizeof(uint8_t), 1, output);
			}

			fputc('\0', output);
		}

		fclose(output);
		created = true;
	}

	void LanguageScript::Clear()
	{
		if (gameStrings != nullptr)
		{
			for (auto str : *gameStrings->PSXStrings)
			{
				delete[] str->string;
				delete str;
			}

			for (auto str : *gameStrings->PCStrings)
			{
				delete[] str->string;
				delete str;
			}

			for (auto str : *gameStrings->Strings)
			{
				delete[] str->string;
				delete str;
			}

			delete gameStrings;	//new 13-7-15
		}
	}

	LanguageScript::~LanguageScript()
	{
		Clear();
	}
