#include "LanguageScript.h"
#include "StringHelper.h"
#include "Gameflow.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <stack>

int main(int argc, char *argv[])
{
	const char *programVersion = "v1.1.0";
	const char *programName = "Tomb Raider Level Editor Script Compiler - Modern Edition";

	if (argc == 1)
	{
		std::cout << programName << ' ' << programVersion << std::endl << std::endl
			<< "Usage: " << argv[0] << " <infile1> <infile2> <infile3>... <infileN>..." << std::endl;

		return 0;
	}

	else if (argc > 1)
	{
		for (int i = 1; i < argc; ++i)
		{
			char *inFile = argv[i];

			TRLEScript::Gameflow scriptFile(StringHelper::ReplaceThreeLetterExtension(inFile, "dat"));
			std::fstream gameflowStream(inFile, std::fstream::in);
			if (!gameflowStream)
			{
				std::cout << "ERROR: Could not open " << inFile << '!' << std::endl;
				return 0;
			}

			gameflowStream.seekg(0, gameflowStream.end);
			int size = (int)gameflowStream.tellg();
			gameflowStream.seekg(0, gameflowStream.beg);
			char *buffer = new char[size];
			gameflowStream.read(buffer, size);
			int readChars = (int)gameflowStream.gcount();
			buffer[readChars] = '\0';
			gameflowStream.close();

			std::cout << "Compiling " << inFile << "..." << std::endl;

			scriptFile.CompileToDAT(buffer);
			delete[] buffer;

			std::stack<TRLEScript::Message> primaryLanguageErrors = scriptFile.GetPrimaryLanguageScript()->GetErrorStack();	//message stack for the primary language strings file
			while (!primaryLanguageErrors.empty())
			{
				std::cout << "ERROR: " << primaryLanguageErrors.top().text << std::endl;
				primaryLanguageErrors.pop();
			}

			std::stack<TRLEScript::Message> gameflowErrors = scriptFile.GetErrorStack();
			while (!gameflowErrors.empty())
			{
				std::cout << "ERROR: " << gameflowErrors.top().text << std::endl;
				gameflowErrors.pop();
			}

			std::stack<TRLEScript::Message> primaryLanguageWarnings = scriptFile.GetPrimaryLanguageScript()->GetWarningStack();
			while (!primaryLanguageWarnings.empty())
			{
				std::cout << "WARNING: " << primaryLanguageWarnings.top().text << std::endl;
				primaryLanguageWarnings.pop();
			}

			std::stack<TRLEScript::Message> gameflowWarnings = scriptFile.GetWarningStack();
			while (!gameflowWarnings.empty())
			{
				std::cout << "WARNING: " << gameflowWarnings.top().text << std::endl;
				gameflowWarnings.pop();
			}


			std::stack<std::string> otherLanguages = scriptFile.GetLanguages();
			while (!otherLanguages.empty())
			{
				std::string languageFilename = otherLanguages.top();
				otherLanguages.pop();

				TRLEScript::LanguageScript languageFile(languageFilename.c_str());
				std::fstream stringsStream(languageFilename.c_str(), std::fstream::in);
				if (!stringsStream)
				{
					std::cout << "WARNING: Could not open " << languageFilename << '!' << std::endl;
					continue;
				}

				stringsStream.seekg(0, stringsStream.end);
				int size = (int)stringsStream.tellg();
				stringsStream.seekg(0, stringsStream.beg);
				char *buffer = new char[size];
				stringsStream.read(buffer, size);
				int readChars = (int)stringsStream.gcount();
				buffer[readChars] = '\0';
				stringsStream.close();

				std::cout << "Compiling " << languageFilename << "..." << std::endl;

				languageFile.CompileToDAT(buffer);
				delete[] buffer;

				std::stack<TRLEScript::Message> languageErrors = languageFile.GetErrorStack();
				while (!languageErrors.empty())
				{
					std::cout << "ERROR: " << languageErrors.top().text;
					languageErrors.pop();
				}

				std::stack<TRLEScript::Message> languageWarnings = languageFile.GetWarningStack();
				while (!languageWarnings.empty())
				{
					std::cout << "WARNING: " << languageWarnings.top().text;
					languageWarnings.pop();
				}
			}

			std::cout << std::endl << "Compilation has completed" << std::endl;
		}

		system("pause");
	}

	//else if (argc == 2)
	//{
	//	inFile = argv[1];
	//	outFile = "script.dat";
	//}

	//else// if (argc == 3)
	//{
	//	inFile = argv[1];
	//	outFile = argv[2];
	//}
	return 0;
}
