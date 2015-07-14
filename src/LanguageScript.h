#ifndef LANGUAGESCRIPT_H_INCLUDED
#define LANGUAGESCRIPT_H_INCLUDED

//#pragma once
#include "ScriptFile.h"
//#include "LanguageScript.h"
#include <forward_list>
#include <cstdint>

namespace TRLEScript
{
    //struct LanguageString;

	struct GameStrings
	{
		std::forward_list<LanguageString *> *Strings = nullptr;
		std::forward_list<LanguageString *> *PSXStrings = nullptr;
		std::forward_list<LanguageString *> *PCStrings = nullptr;

		int numStrings = 0;
		int numPSXStrings = 0;
		int numPCStrings = 0;
	};


	class LanguageScript :
		public ScriptFile
	{
	public:
		LanguageScript(const char *filename);
		virtual ~LanguageScript();			//new 12-7-15
		//~LanguageScript();

		LanguageScript(const LanguageScript &a);
		LanguageScript & operator= (const LanguageScript &a);

		LanguageScript(LanguageScript&&);
		LanguageScript& operator= (LanguageScript&&);

		
		void CompileToDAT(const char *data) override;
		//void CompileToDAT(const char *data, int length);
		void Parse() override;
		const char *GetLanguageName() const;

		int GetStringIndex(const char *str);
		//uint16_t GetStringIndex(const char *str);// const;
		GameflowScriptHeader *GetGameflowScript() const;
		GameStrings *GetGameStrings() const;

	protected:
		GameStrings *gameStrings;
		const char *langName;
		//char *langName;

	private:
		uint16_t GetTotalLength(std::forward_list<LanguageString *> *strings) const;

		const uint8_t magicByte = 0xa5;
		char *auxString;
		bool created;

		void Copy(const LanguageScript &a);
		void Clear();
	};
}



#endif // LANGUAGESCRIPT_H_INCLUDED
