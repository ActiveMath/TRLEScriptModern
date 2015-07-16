#include "LanguageScript.h"
#include "ScriptFile.h"
#include "StringHelper.h"
//#include <unordered_set>
#include <unordered_map>
#include <forward_list>
#include <fstream>
#include <cstdlib>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <stack>
//#include<iostream>

using namespace TRLEScript;

//class StringParseException stringParseException;
//
//class ParseArgumentException :public std::exception
//	{
//		virtual const char *what() const throw()
//		{
//			return "Invalid number of arguments!";
//		}
//	} parseArgException;
//
//	class ParseContextException :public std::exception
//	{
//		virtual const char *what() const throw()
//		{
//			return "Command out of context";
//		}
//	} parseContextException;
//
//	class ParseExtensionException :public std::exception
//	{
//		virtual const char *what() const throw()
//		{
//			return "Extension must be 4 chars!";
//		}
//	} parseExtensionException;
//
//	class ParseModeException :public std::exception
//	{
//		virtual const char *what() const throw()
//		{
//			return "Could not locate a script section start!";
//		}
//	} parseModeException;
//
//	class ParseErrorException :public std::exception
//	{
//		virtual const char *what() const throw()
//		{
//			return "Meaningless entry!";
//		}
//	} parseError;
//
//	class NotFoundException : public std::exception
//	{
//		virtual const char *what() const throw()
//		{
//			return "File does not exist!";
//		}
//	} notFound;
//
//	class NotImplementedException :public std::exception
//	{
//		virtual const char *what() const throw()
//		{
//			return "This method is not yet implemented!";
//		}
//	} notImplemented;

ScriptFile::ScriptFile(const char *filename)
{
	this->filename = new char[strlen(filename) + 1];
	strcpy(this->filename, filename);
	//this->filename = filename;
	primaryLanguage = nullptr;
	compilingToDAT = false;

	gameflowScript = nullptr;
	globalOptions = nullptr;
	definitions = nullptr;
	argStack = nullptr;
	command = nullptr;
	//arg = nullptr;
}

	char *ScriptFile::StripTrailingLeadingWhitespace(const char *string)
	{
		const char *ptr = string;
		const char *startPos;
		const char *endPos;
		int length = strlen(string);
		startPos = ptr;
		endPos = ptr + length - 1;	//new 11-7-15 -1

		while (isblank(*startPos))
			++startPos;

		while (isblank(*endPos))
			endPos--;

		ptrdiff_t newLength = endPos - startPos + 1;
		//int newLength = endPos - startPos + 1;	//new 11-7-15
        char *out = new char[newLength + 1];    //new 10-7-15

		out = strncpy(out, startPos, newLength);
		out[newLength] = '\0';
		return out;
	}

	char *ScriptFile::StripTrailingLeadingChar(const char *string, char c)
	{
		const char *ptr = string;
		const char *startPos;
		const char *endPos;
		int length = strlen(string);
		startPos = ptr;
		endPos = ptr + length - 1;

		while (*startPos == c)
			++startPos;

		while (*endPos == c)
			endPos--;

		ptrdiff_t newLength = endPos - startPos + 1;
		char *out = new char[newLength + 1];

		out = strncpy(out, startPos, newLength);
		out[newLength] = '\0';
		return out;
	}

	std::stack<char *> *ScriptFile::ParseArguments(char *token)
	{
		if (argStack == nullptr)
			argStack = new std::stack < char * > ;

		token = strtok(token, ",");
		while (token != nullptr)
		{
			char *tmpArg = new char[strlen(token) + 1];
			strcpy(tmpArg, token);
			tmpArg[strlen(token)] = '\0';

			argStack->push(tmpArg);
			token = strtok(nullptr, ",");
		}

		return argStack;
	}

	char *ScriptFile::ParseLanguageString(const char *token)
	{
		const char *ptr2 = strchr(token, ':');
		//const char *ptr2 = strpbrk(token, ":");
		const char *ptr1 = ptr2 + 1;
		if (ptr2 == nullptr)
			ptr1 = token;

		char *whitespaceString = StringHelper::ReplaceLiteralWhitespace(ptr1);
		char *correctedString = StringHelper::ReplaceLiteralHexCode(whitespaceString);
		delete[] whitespaceString;

		return correctedString;	//new 11-7-15
	}

	char *ScriptFile::ParseString(const char *token)
	{
		return StripTrailingLeadingWhitespace(token);
	}

	long int ScriptFile::ParseNumber(const char *token)
	{
		char *numString = StringHelper::ReplaceHexDollars(token);
		long int n = strtol(numString, nullptr, 0);
		delete[] numString;

		return n;
	}

	bool ScriptFile::ParseBool(const char *token)
	{
		if (StringHelper::IsStatement("DISABLED", token))
			return false;

		else if (StringHelper::IsStatement("ENABLED", token))
			return true;

		AddNewWarning("Expected bool statement (disabled/enabled) : '" + std::string(wholeLine) + "'; defaulting to 'enabled'");
		return true;
	}

	void ScriptFile::DefaultLevel(LevelData *level)
	{
		level->cut = nullptr;
		level->examines = nullptr;
		level->fmvs = nullptr;
		level->fog = nullptr;
		level->keys = nullptr;
		level->layer1 = nullptr;
		level->layer2 = nullptr;
		level->legend = nullptr;
		level->lensFlare = nullptr;
		level->loadCam = nullptr;
		level->mips = nullptr;
		level->mirrors = nullptr;
		//level->name = nullptr;
		level->path = nullptr;
		level->pickups = nullptr;
		level->puzzleCombos = nullptr;
		level->puzzles = nullptr;
		level->resCuts = nullptr;
		level->resetHUB = nullptr;
		level->uv = nullptr;

		level->options.ColAddHorizon = false;
		level->options.Horizon = false;
		level->options.Layer2used = false;
		level->options.Lightning = false;
		level->options.Mirrorused = false;
		level->options.NoLevel = false;
		level->options.Pulse = false;
		level->options.RemoveAmulet = false;
		level->options.ResetHUBused = false;
		level->options.Starfield = false;
		level->options.Timer = false;
		level->options.Train = false;
		level->options.Weather = false;
		level->options.YoungLara = false;

		level->numExamines = 0;
		level->numFMVs = 0;
		level->numKeys = 0;
		level->numMIPs = 0;
		level->numPickups = 0;
		level->numPuzzleCombos = 0;
		level->numPuzzles = 0;
		level->numResCuts = 0;
	}

	void ScriptFile::DefaultTitle(TitleLevelData *title)
	{
		title->fog = nullptr;
		title->layer1 = nullptr;
		title->layer2 = nullptr;
		title->lensFlare = nullptr;
		title->loadCam = nullptr;
		title->mips = nullptr;
		title->mirrors = nullptr;
		title->path = nullptr;
		title->resCuts = nullptr;
		title->uv = nullptr;

		title->options.ColAddHorizon = false;
		title->options.Horizon = false;
		title->options.Layer2used = false;
		title->options.Lightning = false;
		title->options.Mirrorused = false;
		title->options.NoLevel = false;
		title->options.Pulse = false;
		title->options.RemoveAmulet = false;
		title->options.ResetHUBused = false;
		title->options.Starfield = false;
		title->options.Timer = false;
		title->options.Train = false;
		title->options.Weather = false;
		title->options.YoungLara = false;

		title->numResCuts = 0;
		title->numMIPs = 0;
	}

	bool ScriptFile::InterpretLanguageStrings(int &stringIndex, char *token, GameflowScriptHeader *gameflow, ParseMode mode, int lineNumber)
	{
		char *langString = ParseString(token);	//new 11-7-15
		LanguageString *newString = new LanguageString;
		char *finalString = ParseLanguageString(langString);
		delete[] langString;

		newString->string = finalString;
		newString->stringLength = (uint16_t)strlen(finalString);
		newString->index = stringIndex;

		switch (mode)
		{
		case ParseMode::Strings:
			gameflow->Strings->push_front(newString);
			++gameflow->numStrings;
			break;

		case ParseMode::PSXStrings:
			gameflow->PSXStrings->push_front(newString);
			++gameflow->numPSXStrings;
			break;

		case ParseMode::PCStrings:
			gameflow->PCStrings->push_front(newString);
			++gameflow->numPCStrings;
			break;

		default:
			PlotOutOfContextError(lineNumber);
			return false;
		}

		++stringIndex;
		return true;
	}

	//std::stack<std::string> ScriptFile::GetMessageStack() const
	//{
	//	return messageStack;
	//}

	//std::string *ScriptFile::GetLastMessage() const
	//{
	//	if (messageStack.empty())
	//		return nullptr;

	//	std::string message = messageStack.top();
	//	return &message;
	//}

	void ScriptFile::PlotNumberExpectedError(const char *startPos, const char *currentPos)
	{
		char lineNumber[1024];
		sprintf(lineNumber, "%d", StringHelper::GetCurrentLineNumber(startPos, currentPos));

		AddNewMessage(MessageTypes::Error, "Number expected : \"" + std::string(line) + "\"; line" + std::string(lineNumber));
	}

	bool ScriptFile::ArgumentsContainNumber(const char *startPos, const char *currentPos)
	{
		if (strpbrk(argStack->top(), "$ABCDEFabcdef9876543210") == nullptr)
		{
			PlotNumberExpectedError(startPos, currentPos);
			return false;
		}

		return true;
	}

	void ScriptFile::PlotOutOfContextError(const char *startPos, const char *currentPos)
	{
		char lineNumber[1024];
		sprintf(lineNumber, "%d", StringHelper::GetCurrentLineNumber(startPos, currentPos));

		AddNewMessage(MessageTypes::Warning, "Command out of context : \"" + std::string(line) + "\"; line" + std::string(lineNumber));
	}

	void ScriptFile::PlotOutOfContextError(int lineNumber)
	{
		char lineString[1024];
		sprintf(lineString, "%d", lineNumber);

		AddNewMessage(MessageTypes::Warning, "Command out of context : \"" + std::string(line) + "\"; line" + std::string(lineString));
	}

	bool ScriptFile::LineContainsNumber(const char *startPos, const char *currentPos)
	{
		if (strpbrk(line, "$ABCDEFabcdef9876543210") == nullptr)
		{
			PlotNumberExpectedError(startPos, currentPos);
			return false;
		}

		return true;
	}

	void ScriptFile::Copy(const ScriptFile& a)
	{
		isLanguageScript = a.isLanguageScript;
		isGameflowScript = a.isGameflowScript;

		if (a.gameflowScript != nullptr)
		{
			gameflowScript = new GameflowScriptHeader;
			if (a.gameflowScript->definitions != nullptr)
			{
				gameflowScript->definitions = new PlatformDefinitions;
				gameflowScript->definitions->PCCut = new char[4 + 1];
				gameflowScript->definitions->PCFmv = new char[4 + 1];
				gameflowScript->definitions->PCLevel = new char[4 + 1];
				gameflowScript->definitions->PSXCut = new char[4 + 1];
				gameflowScript->definitions->PSXFmv = new char[4 + 1];
				gameflowScript->definitions->PSXLevel = new char[4 + 1];

				strcpy(gameflowScript->definitions->PCCut, a.gameflowScript->definitions->PCCut);
				strcpy(gameflowScript->definitions->PCFmv, a.gameflowScript->definitions->PCFmv);
				strcpy(gameflowScript->definitions->PCLevel, a.gameflowScript->definitions->PCLevel);
				strcpy(gameflowScript->definitions->PSXCut, a.gameflowScript->definitions->PSXCut);
				strcpy(gameflowScript->definitions->PSXFmv, a.gameflowScript->definitions->PSXFmv);
				strcpy(gameflowScript->definitions->PSXLevel, a.gameflowScript->definitions->PSXLevel);
			}

			if (a.gameflowScript->languages != nullptr)
			{
				gameflowScript->languages = new std::forward_list < LanguageFilename * >;
				for (LanguageFilename *x : *a.gameflowScript->languages)
				{
					LanguageFilename *newLanguage = new LanguageFilename;
					newLanguage->langName = new char[strlen(x->langName) + 1];
					newLanguage->index = x->index;
					strcpy(newLanguage->langName, x->langName);

					gameflowScript->languages->push_front(newLanguage);
				}
			}

			if (a.gameflowScript->options != nullptr)
			{
				gameflowScript->options = new Options;
				gameflowScript->options->DemoDisc = a.gameflowScript->options->DemoDisc;
				gameflowScript->options->FlyCheat = a.gameflowScript->options->FlyCheat;
				gameflowScript->options->InputTimeout = a.gameflowScript->options->InputTimeout;
				gameflowScript->options->LoadSave = a.gameflowScript->options->LoadSave;
				gameflowScript->options->PlayAnyLevel = a.gameflowScript->options->PlayAnyLevel;
				gameflowScript->options->Security = a.gameflowScript->options->Security;
				gameflowScript->options->Title = a.gameflowScript->options->Title;
			}

			if (a.gameflowScript->levels != nullptr)
			{
				for (LevelData *x : *a.gameflowScript->levels)
				{
					LevelData *newLevel = new LevelData;
					newLevel->audio = newLevel->audio;

					if (x->cut != nullptr)
					{
						newLevel->cut = new Cut;
						newLevel->cut->cutIndex = x->cut->cutIndex;
					}

					if (x->examines != nullptr)
					{
						newLevel->examines = new std::forward_list < Examine * >;
						for (Examine *examine : *x->examines)
						{
							Examine *newExamine = new Examine;
							newExamine->appearance = examine->appearance;
							newExamine->index = examine->index;
							newExamine->stringIndex = examine->stringIndex;

							newLevel->examines->push_front(newExamine);
						}
					}

					if (x->fmvs != nullptr)
					{
						newLevel->fmvs = new std::forward_list < FMV * >;
						for (FMV *fmv : *x->fmvs)
						{
							FMV *newFMV = new FMV;
							newFMV->index = fmv->index;
							newFMV->triggerTimer = fmv->triggerTimer;

							newLevel->fmvs->push_front(newFMV);
						}

						newLevel->fmvs->reverse();
					}

					if (x->fog != nullptr)
					{
						newLevel->fog = new Fog;
						newLevel->fog->blue = x->fog->blue;
						newLevel->fog->green = x->fog->green;
						newLevel->fog->red = x->fog->red;
					}

					if (x->keys != nullptr)
					{
						newLevel->keys = new std::forward_list < Key * >;
						for (Key *key : *x->keys)
						{
							Key *newKey = new Key;
							newKey->appearance = key->appearance;
							newKey->index = key->index;
							newKey->stringIndex = key->index;

							newLevel->keys->push_front(newKey);
						}
					}

					if (x->layer2 != nullptr)
					{
						newLevel->layer2 = new Layer2;
						newLevel->layer2->blue = x->layer2->blue;
						newLevel->layer2->green = x->layer2->green;
						newLevel->layer2->red = x->layer2->red;
						newLevel->layer2->speed = x->layer2->speed;
					}

					if (x->layer1 != nullptr)
					{
						newLevel->layer1 = new Layer1;
						newLevel->layer1->blue = x->layer1->blue;
						newLevel->layer1->green = x->layer1->green;
						newLevel->layer1->red = x->layer1->red;
						newLevel->layer1->speed = x->layer1->speed;
					}

					if (x->legend != nullptr)
					{
						newLevel->legend = new Legend;
						newLevel->legend->stringIndex = x->legend->stringIndex;
					}

					if (x->lensFlare != nullptr)
					{
						newLevel->lensFlare = new LensFlare;
						newLevel->lensFlare->blue = x->lensFlare->blue;
						newLevel->lensFlare->green = x->lensFlare->green;
						newLevel->lensFlare->red = x->lensFlare->red;
						newLevel->lensFlare->xClicks = x->lensFlare->yClicks;
						newLevel->lensFlare->yClicks = x->lensFlare->xClicks;
						newLevel->lensFlare->zClicks = x->lensFlare->zClicks;
					}

					if (x->loadCam != nullptr)
					{
						newLevel->loadCam = new LoadCamera;
						newLevel->loadCam->room = x->loadCam->room;
						newLevel->loadCam->srcX = x->loadCam->srcX;
						newLevel->loadCam->srcY = x->loadCam->srcY;
						newLevel->loadCam->srcZ = x->loadCam->srcZ;
						newLevel->loadCam->targetX = x->loadCam->targetX;
						newLevel->loadCam->targetY = x->loadCam->targetY;
						newLevel->loadCam->targetZ = x->loadCam->targetZ;
					}

					if (x->mips != nullptr)
					{
						newLevel->mips = new std::forward_list < AnimatingMIP * >;
						for (AnimatingMIP *mip : *x->mips)
						{
							AnimatingMIP *newMip = new AnimatingMIP;
							newMip->distance = mip->distance;
							newMip->objIndex = mip->objIndex;

							newLevel->mips->push_front(newMip);
						}
					}

					if (x->mirrors != nullptr)
					{
						newLevel->mirrors = new std::forward_list < Mirror * >;
						for (Mirror *mirror : *x->mirrors)
						{
							Mirror *newMirror = new Mirror;
							newMirror->room = mirror->room;
							newMirror->xAxis = mirror->xAxis;

							newLevel->mirrors->push_front(newMirror);
						}
					}

					//if (x->name != nullptr)
					//{
					//	newLevel->name = new char[strlen(x->name) + 1];
					//	strcpy(newLevel->name, x->name);
					//}

					newLevel->options.ColAddHorizon = x->options.ColAddHorizon;
					newLevel->options.Horizon = x->options.Horizon;
					newLevel->options.Layer2used = x->options.Layer2used;
					newLevel->options.Lightning = x->options.Layer2used;
					newLevel->options.Mirrorused = x->options.Mirrorused;
					newLevel->options.NoLevel = x->options.NoLevel;
					newLevel->options.Pulse = x->options.Pulse;
					newLevel->options.RemoveAmulet = x->options.RemoveAmulet;
					newLevel->options.ResetHUBused = x->options.ResetHUBused;
					newLevel->options.Starfield = x->options.Starfield;
					newLevel->options.Timer = x->options.Timer;
					newLevel->options.Train = x->options.Train;
					newLevel->options.Weather = x->options.Weather;
					newLevel->options.YoungLara = x->options.YoungLara;

					if (x->pickups != nullptr)
					{
						newLevel->pickups = new std::forward_list < Pickup * >;
						for (Pickup *pickup : *x->pickups)
						{
							Pickup *newPickup = new Pickup;
							newPickup->appearance = pickup->appearance;
							newPickup->index = pickup->index;
							newPickup->stringIndex = pickup->stringIndex;

							newLevel->pickups->push_front(newPickup);
						}
					}

					if (x->puzzleCombos != nullptr)
					{
						newLevel->puzzleCombos = new std::forward_list < PuzzleCombo * >;
						for (PuzzleCombo *puzzleCombo : *x->puzzleCombos)
						{
							PuzzleCombo *newPuzzleCombo = new PuzzleCombo;
							newPuzzleCombo->appearance = puzzleCombo->appearance;
							newPuzzleCombo->firstIndex = puzzleCombo->firstIndex;
							newPuzzleCombo->secondIndex = puzzleCombo->secondIndex;
							newPuzzleCombo->stringIndex = puzzleCombo->stringIndex;

							newLevel->puzzleCombos->push_front(newPuzzleCombo);
						}
					}

					if (x->pickupCombos != nullptr)
					{
						newLevel->pickupCombos = new std::forward_list < PickupCombo * > ;
						for (auto combo : *x->pickupCombos)
						{
							PickupCombo *newPickupCombo = new PickupCombo;
							newPickupCombo->appearance = combo->appearance;
							newPickupCombo->firstIndex = combo->firstIndex;
							newPickupCombo->secondIndex = combo->secondIndex;
							newPickupCombo->stringIndex = combo->stringIndex;

							newLevel->pickupCombos->push_front(newPickupCombo);
						}
					}

					if (x->keyCombos != nullptr)
					{
						newLevel->keyCombos = new std::forward_list < KeyCombo * > ;
						for (auto combo : *x->keyCombos)
						{
							KeyCombo *newKeyCombo = new KeyCombo;
							newKeyCombo->appearance = combo->appearance;
							newKeyCombo->firstIndex = combo->firstIndex;
							newKeyCombo->secondIndex = combo->secondIndex;
							newKeyCombo->stringIndex = combo->stringIndex;

							newLevel->keyCombos->push_front(newKeyCombo);
						}
					}

					if (x->puzzles != nullptr)
					{
						newLevel->puzzles = new std::forward_list < Puzzle * >;
						for (Puzzle *puzzle : *x->puzzles)
						{
							Puzzle *newPuzzle = new Puzzle;
							newPuzzle->appearance = puzzle->appearance;
							newPuzzle->index = puzzle->index;
							newPuzzle->stringIndex = puzzle->stringIndex;

							newLevel->puzzles->push_front(newPuzzle);
						}
					}

					if (x->resCuts != nullptr)
					{
						newLevel->resCuts = new std::forward_list < ResidentCut * >;
						for (ResidentCut *resCut : *x->resCuts)
						{
							ResidentCut *newResCut = new ResidentCut;
							newResCut->cutIndex = resCut->cutIndex;
							newResCut->number = resCut->number;

							newLevel->resCuts->push_front(newResCut);
						}
					}

					if (x->resetHUB != nullptr)
					{
						newLevel->resetHUB = new ResetHUB;
						newLevel->resetHUB->levelIndex = x->resetHUB->levelIndex;
					}

					newLevel->stringIndex = x->stringIndex;

					if (x->uv != nullptr)
					{
						newLevel->uv = new UVRotate;
						newLevel->uv->speed = x->uv->speed;
					}

					gameflowScript->levels->push_front(newLevel);
				}

				gameflowScript->levels->reverse();
			}

			gameflowScript->numLanguages = a.gameflowScript->numLanguages;
			gameflowScript->numLevelPaths = a.gameflowScript->numLevelPaths;
			gameflowScript->numPCStrings = a.gameflowScript->numPCStrings;
			gameflowScript->numPSXStrings = a.gameflowScript->numPSXStrings;
			gameflowScript->numStrings = a.gameflowScript->numStrings;
			gameflowScript->numTotalLevels = a.gameflowScript->numTotalLevels;

			if (a.gameflowScript->title != nullptr)
			{
				gameflowScript->title = new TitleLevelData;
				gameflowScript->title->audio = a.gameflowScript->title->audio;

				if (a.gameflowScript->title->fog != nullptr)
				{
					gameflowScript->title->fog = new Fog;
					gameflowScript->title->fog->blue = a.gameflowScript->title->fog->blue;
					gameflowScript->title->fog->green = a.gameflowScript->title->fog->green;
					gameflowScript->title->fog->red = a.gameflowScript->title->fog->red;
				}

				if (a.gameflowScript->title->layer2 != nullptr)
				{
					gameflowScript->title->layer2 = new Layer2;
					gameflowScript->title->layer2->blue = a.gameflowScript->title->layer2->blue;
					gameflowScript->title->layer2->green = a.gameflowScript->title->layer2->green;
					gameflowScript->title->layer2->red = a.gameflowScript->title->layer2->red;
					gameflowScript->title->layer2->speed = a.gameflowScript->title->layer2->speed;
				}

				if (a.gameflowScript->title->layer1 != nullptr)
				{
					gameflowScript->title->layer1 = new Layer1;
					gameflowScript->title->layer1->blue = a.gameflowScript->title->layer1->blue;
					gameflowScript->title->layer1->green = a.gameflowScript->title->layer1->green;
					gameflowScript->title->layer1->red = a.gameflowScript->title->layer1->red;
					gameflowScript->title->layer1->speed = a.gameflowScript->title->layer1->speed;
				}

				if (a.gameflowScript->title->lensFlare != nullptr)
				{
					gameflowScript->title->lensFlare = new LensFlare;
					gameflowScript->title->lensFlare->blue = a.gameflowScript->title->lensFlare->blue;
					gameflowScript->title->lensFlare->green = a.gameflowScript->title->lensFlare->green;
					gameflowScript->title->lensFlare->red = a.gameflowScript->title->lensFlare->red;
					gameflowScript->title->lensFlare->xClicks = a.gameflowScript->title->lensFlare->xClicks;
					gameflowScript->title->lensFlare->yClicks = a.gameflowScript->title->lensFlare->yClicks;
					gameflowScript->title->lensFlare->zClicks = a.gameflowScript->title->lensFlare->zClicks;
				}

				if (a.gameflowScript->title->loadCam != nullptr)
				{
					gameflowScript->title->loadCam = new LoadCamera;
					gameflowScript->title->loadCam->room = a.gameflowScript->title->loadCam->room;
					gameflowScript->title->loadCam->srcX = a.gameflowScript->title->loadCam->srcX;
					gameflowScript->title->loadCam->srcY = a.gameflowScript->title->loadCam->srcY;
					gameflowScript->title->loadCam->srcZ = a.gameflowScript->title->loadCam->srcZ;
					gameflowScript->title->loadCam->targetX = a.gameflowScript->title->loadCam->targetX;
					gameflowScript->title->loadCam->targetY = a.gameflowScript->title->loadCam->targetY;
					gameflowScript->title->loadCam->targetZ = a.gameflowScript->title->loadCam->targetZ;
				}

				if (a.gameflowScript->title->mips != nullptr)
				{
					gameflowScript->title->mips = new std::forward_list < AnimatingMIP * >;
					for (AnimatingMIP *mip : *a.gameflowScript->title->mips)
					{
						AnimatingMIP *newMIP = new AnimatingMIP;
						newMIP->distance = mip->distance;
						newMIP->objIndex = mip->objIndex;

						gameflowScript->title->mips->push_front(newMIP);
					}
				}

				if (a.gameflowScript->title->mirrors != nullptr)
				{
					gameflowScript->title->mirrors = new std::forward_list < Mirror * >;
					for (Mirror *mirror : *a.gameflowScript->title->mirrors)
					{
						Mirror *newMirror = new Mirror;
						newMirror->room = mirror->room;
						newMirror->xAxis = mirror->xAxis;

						gameflowScript->title->mirrors->push_front(newMirror);
					}
				}

				gameflowScript->title->options.ColAddHorizon = a.gameflowScript->title->options.ColAddHorizon;
				gameflowScript->title->options.Horizon = a.gameflowScript->title->options.Horizon;
				gameflowScript->title->options.Layer2used = a.gameflowScript->title->options.Layer2used;
				gameflowScript->title->options.Lightning = a.gameflowScript->title->options.Lightning;
				gameflowScript->title->options.Mirrorused = a.gameflowScript->title->options.Mirrorused;
				gameflowScript->title->options.NoLevel = a.gameflowScript->title->options.NoLevel;
				gameflowScript->title->options.Pulse = a.gameflowScript->title->options.Pulse;
				gameflowScript->title->options.RemoveAmulet = a.gameflowScript->title->options.RemoveAmulet;
				gameflowScript->title->options.ResetHUBused = a.gameflowScript->title->options.ResetHUBused;
				gameflowScript->title->options.Starfield = a.gameflowScript->title->options.Starfield;
				gameflowScript->title->options.Timer = a.gameflowScript->title->options.Timer;
				gameflowScript->title->options.Train = a.gameflowScript->title->options.Train;
				gameflowScript->title->options.Weather = a.gameflowScript->title->options.Weather;
				gameflowScript->title->options.YoungLara = a.gameflowScript->title->options.YoungLara;

				if (a.gameflowScript->title->resCuts != nullptr)
				{
					gameflowScript->title->resCuts = new std::forward_list < ResidentCut * >;
					for (ResidentCut *resCut : *a.gameflowScript->title->resCuts)
					{
						ResidentCut *newResCut = new ResidentCut;
						newResCut->cutIndex = resCut->cutIndex;
						newResCut->number = resCut->number;

						gameflowScript->title->resCuts->push_front(newResCut);
					}
				}

				if (a.gameflowScript->title->uv != nullptr)
				{
					gameflowScript->title->uv = new UVRotate;
					gameflowScript->title->uv->speed = a.gameflowScript->title->uv->speed;
				}
			}

			if (a.gameflowScript->Strings != nullptr)
			{
				gameflowScript->Strings = new std::forward_list < LanguageString * >;
				for (LanguageString *string : *a.gameflowScript->Strings)
				{
					LanguageString *newString = new LanguageString;
					newString->string = new char[strlen(string->string) + 1];
					newString->index = string->index;
					newString->stringLength = string->stringLength;
					strcpy(newString->string, string->string);

					gameflowScript->Strings->push_front(newString);
				}
			}

			if (a.gameflowScript->PSXStrings != nullptr)
			{
				gameflowScript->PSXStrings = new std::forward_list < LanguageString * >;
				for (LanguageString *string : *a.gameflowScript->PSXStrings)
				{
					LanguageString *newString = new LanguageString;
					newString->string = new char[strlen(string->string) + 1];
					newString->index = string->index;
					newString->stringLength = string->stringLength;
					strcpy(newString->string, string->string);

					gameflowScript->PSXStrings->push_front(newString);
				}
			}

			if (a.gameflowScript->PCStrings != nullptr)
			{
				gameflowScript->PCStrings = new std::forward_list < LanguageString * >;
				for (LanguageString *string : *a.gameflowScript->PCStrings)
				{
					LanguageString *newString = new LanguageString;
					newString->string = new char[strlen(string->string) + 1];
					newString->index = string->index;
					newString->stringLength = string->stringLength;
					strcpy(newString->string, string->string);

					gameflowScript->PCStrings->push_front(newString);
				}
			}
		}
	}

	ScriptFile::ScriptFile(const ScriptFile& a)
	{
		Copy(a);
	}

	ScriptFile & ScriptFile::operator= (const ScriptFile& a)
	{
		if (this != &a)
		{
			Clear();
			Copy(a);
		}

		return *this;
	}

	ScriptFile::ScriptFile(ScriptFile &&a)
	{
		gameflowScript = a.gameflowScript;

		isLanguageScript = a.isLanguageScript;
		isGameflowScript = a.isGameflowScript;

		a.gameflowScript = nullptr;
		a.isLanguageScript = false;
		a.isGameflowScript = false;
	}

	ScriptFile& ScriptFile::operator= (ScriptFile &&a)
	{
		std::swap(gameflowScript, a.gameflowScript);

		std::swap(isLanguageScript, a.isLanguageScript);
		std::swap(isGameflowScript, a.isGameflowScript);

		//a.Clear();																					13-7-15
		a.gameflowScript = nullptr;
		a.isLanguageScript = false;
		a.isGameflowScript = false;

		return *this;
	}

	const LanguageScript *ScriptFile::GetPrimaryLanguageScript() const
	{
		return primaryLanguage;
	}

	void ScriptFile::AddNewWarning(const char *text, int lineNumber)
	{
		char lineString[1024];
		sprintf(lineString, "%d", lineNumber);

		std::string warningText(std::string(text) + " line " + std::string(lineString));
		AddNewMessage(MessageTypes::Warning, warningText);
	}

	void ScriptFile::AddNewError(const char *text, int lineNumber)
	{
		char lineString[1024];
		sprintf(lineString, "%d", lineNumber);

		std::string warningText(std::string(text) + " line " + std::string(lineString));
		AddNewMessage(MessageTypes::Error, warningText);
	}

	void ScriptFile::AddNewWarning(const char *text)
	{
		AddNewMessage(MessageTypes::Warning, std::string(text));
	}

	void ScriptFile::AddNewError(const char *text)
	{
		AddNewMessage(MessageTypes::Error, std::string(text));
	}

	void ScriptFile::AddNewWarning(std::string text, int lineNumber)
	{
		char lineString[1024];
		sprintf(lineString, "%d", lineNumber);

		std::string warningText(text + " line " + std::string(lineString));
		AddNewMessage(MessageTypes::Warning, warningText);
	}

	void ScriptFile::AddNewError(std::string text, int lineNumber)
	{
		char lineString[1024];
		sprintf(lineString, "%d", lineNumber);

		std::string warningText(text + " line " + std::string(lineString));
		AddNewMessage(MessageTypes::Error, warningText);
	}

	void ScriptFile::AddNewWarning(std::string text)
	{
		AddNewMessage(MessageTypes::Warning, text);
	}

	void ScriptFile::AddNewError(std::string text)
	{
		AddNewMessage(MessageTypes::Error, text);
	}

	void ScriptFile::PreprocessorPass(const char *data)
	{
		preprocessedData = PreprocessorDefinePass(PreprocessorIncludePass(data).data());
	}

	std::vector<char> ScriptFile::PreprocessorIncludePass(const char *data)
	{
		std::vector<char> includeProcessedData;

		const char *lastPos = data;
		while (lastPos != nullptr)
		{
			const char *hashPos;

			if (lastPos == data)
				hashPos = strstr(lastPos, "#include ");

			else
				hashPos = strstr(lastPos + 1, "#include ");

			if (hashPos != nullptr)
			{
				while (lastPos != hashPos)
				{
					includeProcessedData.push_back(*lastPos);
					++lastPos;
				}

				if (StringHelper::PreviousLineCharsIncludeChar(hashPos, ';'))
					continue;

				int restLength = data + strlen(data) - hashPos;
				char *restData = new char[restLength + 1];
				strncpy(restData, hashPos, restLength);
				restData[restLength] = '\0';
				char *filename = strtok(restData, " \t\n;");
				filename = strtok(nullptr, "\n;");
				//filename = strtok(nullptr, " \t\n;");
				if (filename == nullptr)
				{
					delete[] restData;
					//lastPos += 9;
				}

				else
				{
					char *filenameNoSpaces = StripTrailingLeadingWhitespace(filename);
					int macroLength = filename + strlen(filename) - restData;
					char *realFilename = StripTrailingLeadingChar(filenameNoSpaces, '\"');
					//char *realFilename = StripTrailingChar(filename, '\"');
					delete[] restData;

					std::fstream includeFile(realFilename);
					if (includeFile)
					{
						includeFile.seekg(0, includeFile.end);
						int includeFileSize = includeFile.tellg();
						includeFile.seekg(0, includeFile.beg);

						char *buffer = new char[includeFileSize];
						includeFile.read(buffer, includeFileSize);
						int actualSize = includeFile.gcount();
						buffer[actualSize] = '\0';

						std::vector<char> processedData = PreprocessorIncludePass(buffer);
						char *includedBuffer = processedData.data();

						for (int i = 0; i < strlen(includedBuffer); ++i)
						{
							includeProcessedData.push_back(includedBuffer[i]);
						}

						includeFile.close();
						delete[] realFilename;
						delete[] filenameNoSpaces;
						delete[] buffer;
						lastPos = hashPos + macroLength - 1;
					}

					else
					{
						AddNewWarning("Could not open '" + std::string(realFilename) + "' as part of an #include directive");
						delete[] filenameNoSpaces;
						delete[] realFilename;
						//lastPos += 9;;
					}
				}
			}

			else
			{
				while (lastPos != data + strlen(data))
				{
					includeProcessedData.push_back(*lastPos);
					++lastPos;
				}

				includeProcessedData.push_back('\0');
				lastPos = nullptr;
			}
		}

		return includeProcessedData;
	}

	int ScriptFile::MacroPointerCompare(const void *a, const void *b)
	{
		if (*(const char *)a == *(const char *)b)
			return 0;

		if (*(const char *)a > *(const char *)b)
			return 1;

		if (*(const char *)a < *(const char *)b)
			return -1;
	}

	std::string ScriptFile::EvaluateMacro(std::vector<std::pair<std::string, std::string>> &vMacro, std::string &toReplace)
	{
		for (auto macro : vMacro)
		{
			if (toReplace == macro.first)
				return EvaluateMacro(vMacro, macro.second);
		}

		return toReplace;
	}

	void ScriptFile::MacroSubstitute(std::vector<std::pair<std::string, std::string>> &vMacro, std::vector<char> &definedData, const char *& lastPos, const char *endPos)
	{

		while (lastPos != endPos)
		{
			bool needsPush = true;
			for (int i = 0; i < vMacro.size(); ++i)
			{
				const char *replacementStartPos = strstr(lastPos, vMacro[i].first.c_str());
				const char *initialPos = lastPos;
				if (strstr(lastPos, vMacro[i].first.c_str()) == lastPos)
				{
					std::string replaced;
					if (vMacro[i].second == vMacro[i].first)
						replaced = vMacro[i].first;
					else

						replaced = EvaluateMacro(vMacro, vMacro[i].first);

					for (int j = 0; j < replaced.size(); ++j)
					{
						definedData.push_back(replaced[j]);
					}

					lastPos += vMacro[i].first.size();
					needsPush = false;
					break;
				}
			}

			if (needsPush)
			{
				definedData.push_back(*lastPos);
				++lastPos;
			}
		}
	}

	std::vector<char> ScriptFile::PreprocessorDefinePass(const char *data)
	{
		std::vector<char> definedData;

		std::vector < std::pair<std::string, std::string> > vMacro;
		const char *lastPos = data;
		while (lastPos != nullptr)
		{
			const char *hashPos;

			if (lastPos == data)
				hashPos = strstr(lastPos, "#define ");

			else
				hashPos = strstr(lastPos + 1, "#define ");		//7?

			if (hashPos != nullptr)
			{
				if (StringHelper::PreviousLineCharsIncludeChar(hashPos, ';'))
				{
					while (lastPos != hashPos)
					{
						definedData.push_back(*lastPos);
						++lastPos;
					}

					continue;
				}

				MacroSubstitute(vMacro, definedData, lastPos, hashPos);


				int restLength = data + strlen(data) - hashPos;
				char *restData = new char[restLength + 1];
				strncpy(restData, hashPos, restLength);
				restData[restLength] = '\0';
				char *token = strtok(restData, " \t\n;");
				token = strtok(nullptr, " \t\n;");
				if (token == nullptr)
				{
					delete[] restData;
					//lastPos += 9;
				}

				else
				{
					char *target = new char[strlen(token) + 1];
					strcpy(target, token);
					target[strlen(token)] = '\0';

					token = strtok(nullptr, " \t\n;");
					if (token == nullptr)
					{
						delete[] restData;
						delete[] target;
					}

					else
					{
						char *replacement = new char[strlen(token) + 1];
						strcpy(replacement, token);
						replacement[strlen(token)] = '\0';

						int macroLength = token + strlen(replacement) - restData;
						delete[] restData;

						bool replacedExistingMacro = false;
						for (int i = 0; i < vMacro.size(); ++i)
						{
							if (strcmp(vMacro[i].first.c_str(), target) == 0)
							{
								std::pair<std::string, std::string> tmp = std::make_pair<std::string, std::string>(std::string(target), std::string(replacement));
								vMacro[i] = tmp;
								replacedExistingMacro = true;
								break;
							}
						}

						if (!replacedExistingMacro)
							vMacro.push_back(std::make_pair<std::string, std::string>(std::string(target), std::string(replacement)));
						lastPos = hashPos + macroLength;

						delete[] replacement;
						delete[] target;
					}
				}
			}

			else
			{
				MacroSubstitute(vMacro, definedData, lastPos, data + strlen(data));

				definedData.push_back('\0');
				lastPos = nullptr;
			}
		}

		return definedData;
	}

	GameflowScriptHeader *ScriptFile::SourceParse(const char *rawData)
	{
		GameflowScriptHeader *gameflow = new GameflowScriptHeader;

		gameflowScript = gameflow;

		gameflow->languages = nullptr;
		gameflow->levels = nullptr;
		gameflow->numTotalLevels = 0;
		gameflow->numLanguages = 0;
		gameflow->numLevelPaths = 0;
		gameflow->title = nullptr;

		PreprocessorPass(rawData);
		const char *data = preprocessedData.data();

		const char *ptr = data;
		bool isPrimaryLanguageParsed = false;
		bool isComment = false;

		int numPSXExtensionChunks = 0;
		int numPCExtensionChunks = 0;
		int numLanguageChunks = 0;
		int numOptionChunks = 0;

		int stringIndex = 0;	//used only in parsing language scripts

		ParseMode mode = ParseMode::Undefined;

        while (*ptr != '\0')
		{
			int lineLength = 0;
			if (*ptr == ';')
			{
				isComment = true;
				++ptr;
			}

			else if (isblank(*ptr) || *ptr == '\n')
			{
				if (*ptr == '\n')
					isComment = false;
				++ptr;
			}

			else if (isComment == true)
			{
				++ptr;
			}

			/*else if (*ptr == '#')
			{
				isMacro = true;
				++ptr;
			}

			else if (isMacro == true)
			{
				size_t lineLength = strcspn(ptr, "\n;");
				size_t macroLength = strcspn(ptr, "\" ");
				char *macroString = new char[macroLength + 1];
				strncpy(macroString, ptr, macroLength);
				macroString[macroLength] = '\0';

				if (strcmp(macroString, "include") == 0)
					macroType = MacroType::Include;

				else if (strcmp(macroString, "define") == 0)
					macroType = MacroType::Define;
			}*/

			else if (isComment == false)
			{

				lineLength = strcspn(ptr, "\n;");//optimize with strtok only

				line = new char[lineLength + 1];
				line = strncpy(line, ptr, lineLength);
				line[lineLength] = '\0';

				wholeLine = new char[lineLength + 1];
				strcpy(wholeLine, line);
				wholeLine[lineLength] = '\0';

				char *token;
				int numArgs = 0;

				char *test = strpbrk(line, "=");
				if (test == nullptr)
				{
					if (StringHelper::IsStatement("[PSXExtensions]", line) && isGameflowScript)
					{
						if (gameflow->definitions == nullptr)
							gameflow->definitions = new PlatformDefinitions;

						definitions = gameflow->definitions;
						++numPSXExtensionChunks;

						if (numPSXExtensionChunks > 1)
						{
							char numChunksString[5];
							char lineNumber[5];
							sprintf(numChunksString, "%d", numPSXExtensionChunks);
							sprintf(lineNumber, "%d", StringHelper::GetCurrentLineNumber(data, ptr));

							std::string warningText("Multiple (" + std::string(numChunksString) + ") PSX extension chunks");
							AddNewWarning(warningText, StringHelper::GetCurrentLineNumber(data, ptr));
						}

						mode = ParseMode::PSXExtensions;
					}

					else if (StringHelper::IsStatement("[PCExtensions]", line) && isGameflowScript)
					{
						if (gameflow->definitions == nullptr)
							gameflow->definitions = new PlatformDefinitions;

						definitions = gameflow->definitions;
						++numPCExtensionChunks;

						if (numPCExtensionChunks > 1)
						{
							char numChunksString[5];
							char lineNumber[5];
							sprintf(numChunksString, "%d", numPCExtensionChunks);
							sprintf(lineNumber, "%d", StringHelper::GetCurrentLineNumber(data, ptr));

							std::string warningText("Multiple (" + std::string(numChunksString) + ") PC extension chunks");
							AddNewWarning(warningText, StringHelper::GetCurrentLineNumber(data, ptr));
						}

						mode = ParseMode::PCExtensions;
					}

					else if (StringHelper::IsStatement("[Language]", line) && isGameflowScript)
					{
						if (gameflow->languages == nullptr)
							gameflow->languages = new std::forward_list < LanguageFilename * > ;

						++numLanguageChunks;

						mode = ParseMode::Language;
					}

					else if (StringHelper::IsStatement("[Options]", line) && isGameflowScript)
					{
						if (gameflow->options == nullptr)
							gameflow->options = new Options;

						globalOptions = gameflow->options;
						++numOptionChunks;

						if (numOptionChunks > 1)
						{
							char numChunksString[5];
							char lineNumber[5];
							sprintf(numChunksString, "%d", numOptionChunks);
							sprintf(lineNumber, "%d", StringHelper::GetCurrentLineNumber(data, ptr));

							std::string warningText("Multiple (" + std::string(numChunksString) + ") option chunks");
							AddNewWarning(warningText, StringHelper::GetCurrentLineNumber(data, ptr));
						}

						mode = ParseMode::Options;
					}

					else if (StringHelper::IsStatement("[Title]", line) && isGameflowScript)
					{
						mode = ParseMode::Title;

						gameflow->title = new TitleLevelData;
						DefaultTitle(gameflow->title);

						++gameflow->numTotalLevels;		//important!
					}

					else if (StringHelper::IsStatement("[Level]", line) && isGameflowScript)
					{
						mode = ParseMode::Level;

						if (gameflow->levels == nullptr)
							gameflow->levels = new std::forward_list < LevelData * > ;

						LevelData *newLevel = new LevelData;

						DefaultLevel(newLevel);

						gameflow->levels->push_front(newLevel);
						++gameflow->numTotalLevels;
					}

					else if (StringHelper::IsStatement("[Strings]", line) && isLanguageScript)
					{
						mode = ParseMode::Strings;
						gameflow->Strings = new std::forward_list < LanguageString * > ;
					}

					else if (StringHelper::IsStatement("[PSXStrings]", line) && isLanguageScript)
					{
						mode = ParseMode::PSXStrings;
						gameflow->PSXStrings = new std::forward_list < LanguageString * > ;
					}

					else if (StringHelper::IsStatement("[PCStrings]", line) && isLanguageScript)
					{
						mode = ParseMode::PCStrings;
						gameflow->PCStrings = new std::forward_list < LanguageString * > ;
					}

					else if (isLanguageScript)
					{
					    char *tmpString = new char[strlen(line)+1];
					    strcpy(tmpString, line);
                        tmpString[strlen(line)] = '\0';

						InterpretLanguageStrings(stringIndex, tmpString, gameflow, mode, StringHelper::GetCurrentLineNumber(data, ptr));    //new 10-7-15
					    delete[] tmpString;
					}

					else// if (isGameflowScript)new 13-7-15
					//else if (!isGameflowScript)
					{
						std::string warningText("Unknown chunk reached : \"" + std::string(wholeLine) + '\"');
						AddNewWarning(warningText, StringHelper::GetCurrentLineNumber(data, ptr));
						//delete[] line;
						//return nullptr;
					}
				}

				else if (isLanguageScript)
				{
				    char *tmpString = new char[strlen(line)+1];
				    strcpy(tmpString, line);
				    tmpString[strlen(line)] = '\0';

					InterpretLanguageStrings(stringIndex, tmpString, gameflow, mode, StringHelper::GetCurrentLineNumber(data, ptr));
					delete[] tmpString;
				}

				else if (isGameflowScript)
				{
				    token = strtok(line, "=");

					command = new char[lineLength + 1];	//could probably do without the command - test it!
					strcpy(command, token);			//try optimising this; just command=token
					char *argString = test + 1;

					char *tmp = new char[strlen(argString) + 1];
					strcpy(tmp, argString);
					argStack = ParseArguments(tmp);

					delete[] tmp;

					if (StringHelper::IsStatement("Level", command))
					{
						switch (mode)
						{
						case ParseMode::PSXExtensions:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							char *cleanArg = StripTrailingLeadingWhitespace(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							definitions->PSXLevel = cleanArg;//do this here rather than below, so that the allocated memory can be freed upon destruction
							if (strlen(cleanArg) != 4)
							{
								std::string errorText("The extension is not 3 characters long : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}
							break;
						    }

						case ParseMode::PCExtensions:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							char *cleanArg = StripTrailingLeadingWhitespace(argStack->top());

							delete[] argStack->top();
							argStack->pop();
							definitions->PCLevel = cleanArg;
							if (strlen(cleanArg) != 4)
							{
								std::string errorText("The extension is not 3 characters long : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}
							break;
						    }

						case ParseMode::Level:
						    {
							if (argStack->size() != 2)
							{
								std::string errorText("Expected 2 arguments : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							LevelData *currentLevel = gameflow->levels->front();
							currentLevel->audio = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->path = StringHelper::ToUpper(ParseString(argStack->top()));

							gameflow->levelPathMap.insert(std::make_pair<std::string, int>(std::string(currentLevel->path), gameflow->numTotalLevels - 1));
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						case ParseMode::Title:
						    {
							if (argStack->size() != 2)
							{
								std::string errorText("Expected 2 arguments : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							gameflow->title->audio = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							gameflow->title->path = StringHelper::ToUpper(ParseString(argStack->top()));

							gameflow->levelPathMap.insert(std::make_pair<std::string, int>(std::string(gameflow->title->path), gameflow->numTotalLevels - 1));

							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
							PlotOutOfContextError(data, ptr);
							//return nullptr;
						}
					}

					else if (StringHelper::IsStatement("Cut", command))
					{
						switch (mode)
						{
						case ParseMode::PSXExtensions:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							char *cleanArg = StripTrailingLeadingWhitespace(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							definitions->PSXCut = cleanArg;

							if (strlen(cleanArg) != 4)
							{
								std::string errorText("The extension is not 3 characters long : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}
							break;
						    }

						case ParseMode::PCExtensions:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							char *cleanArg = StripTrailingLeadingWhitespace(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							definitions->PCCut = cleanArg;

							if (strlen(cleanArg) != 4)
							{
								std::string errorText("The extension is not 3 characters long : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							break;
						    }

						case ParseMode::Level:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->cut == nullptr)
								currentLevel->cut = new Cut;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->cut->cutIndex = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							//currentLevel->options.CutUsed = true;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("FMV", command))
					{
						switch (mode)
						{
						case ParseMode::PSXExtensions:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							char *cleanArg = StripTrailingLeadingWhitespace(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							definitions->PSXFmv = cleanArg;

							if (strlen(cleanArg) != 4)
							{
								std::string errorText("The extension is not 3 characters long : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}
							break;
						    }

						case ParseMode::PCExtensions:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							char *cleanArg = StripTrailingLeadingWhitespace(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							definitions->PCFmv = cleanArg;

							if (strlen(cleanArg) != 4)
							{
								std::string errorText("The extension is not 3 characters long : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}
							break;
						    }

						case ParseMode::Level:
						    {
							if (argStack->size() != 2)
							{
								std::string errorText("Expected 2 arguments : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->fmvs == nullptr)
								currentLevel->fmvs = new std::forward_list < FMV * > ;

							FMV *newFMV = new FMV;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newFMV->triggerTimer = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newFMV->index = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->fmvs->push_front(newFMV);
							++currentLevel->numFMVs;

							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("File", command))
					{
						switch (mode)
						{
						case ParseMode::Language:
						{
							if (argStack->size() != 2)
							{
								std::string errorText("Expected 2 arguments : \"" + std::string(wholeLine) + '\"');
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							if (strpbrk(argStack->top(), ".") == nullptr)
							{
								char lineNumber[1024];
								sprintf(lineNumber, "%d", StringHelper::GetCurrentLineNumber(data, ptr));

								AddNewMessage(MessageTypes::Error, "Filename expected at line " + std::string(lineNumber));
								return nullptr;
							}

							++gameflow->numLanguages;

							char *langFilename = ParseString(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
							{
								delete[] langFilename;
								return nullptr;
							}

							int index = (int)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							TRLEScript::LanguageFilename *newLanguage = new TRLEScript::LanguageFilename;
							if (gameflow->languages == nullptr)
								gameflow->languages = new std::forward_list < LanguageFilename * > ;

							newLanguage->langName = langFilename;
							newLanguage->index = index;
							gameflow->languages->push_front(newLanguage);
							if (index == 0)
							{
								gameflow->primaryLanguage = newLanguage;
								if (compilingToDAT)
								{
									char *pureName = StringHelper::ExtractFilenameWithoutExtension(gameflow->primaryLanguage->langName);
									primaryLanguage = new LanguageScript(pureName);
									//gameflow->primaryLanguageScript = primaryLanguage;		//new 12-7-15

									std::fstream input(gameflow->primaryLanguage->langName, std::fstream::in);

									char *sourceDataLang = nullptr;
									int sourceLengthLang;
									int readChars = 0;
									if (input)
									{
										input.seekg(0, input.end);
										sourceLengthLang = (int)input.tellg();
										input.seekg(0, input.beg);

										sourceDataLang = new char[sourceLengthLang];

										input.read(sourceDataLang, sourceLengthLang);
										readChars = (int)input.gcount();
										sourceDataLang[readChars] = '\0';

										/*if (!input)
										{
										}*/

										input.close();

									}

									else
									{
										AddNewError("Could not open primary language file: " + std::string(langFilename));
										return nullptr;
									}

									primaryLanguage->CompileToDAT(sourceDataLang);
									//primaryLanguage->CompileToDAT(sourceDataLang, readChars);
									delete[] sourceDataLang;
								}

								/*else
								{
									AddNewError("Unexpected error in reading " + std::string(langFilename));
									return nullptr;
								}*/

								isPrimaryLanguageParsed = true;
							}
							break;
						}

						default:
						{
							PlotOutOfContextError(data, ptr);
						}
						}
					}

					else if (StringHelper::IsStatement("LoadSave", command))
					{
						switch (mode)
						{
						case ParseMode::Options:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							bool loadSave = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							globalOptions->LoadSave = loadSave;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
								//return nullptr;
						    }
						}
					}

					else if (StringHelper::IsStatement("Title", command))
					{
						switch (mode)
						{
						case ParseMode::Options:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							globalOptions->Title = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("PlayAnyLevel", command))
					{
						switch (mode)
						{
						case ParseMode::Options:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							globalOptions->PlayAnyLevel = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("InputTimeout", command))
					{
						switch (mode)
						{
						case ParseMode::Options:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							globalOptions->InputTimeout = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("FlyCheat", command))
					{
						switch (mode)
						{
						case ParseMode::Options:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							globalOptions->FlyCheat = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Security", command))
					{
						switch (mode)
						{
						case ParseMode::Options:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							globalOptions->Security = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("DemoDisc", command))
					{
						switch (mode)
						{
						case ParseMode::Options:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							globalOptions->DemoDisc = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Name", command))//todo:delete stack here!
					{
						switch (mode)
						{
						case ParseMode::Level:
						    {
							if (argStack->size() < 1)
							{
								std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							char *levelName;
							levelName = ParseString(argString);
							LevelData *currentLevel = gameflow->levels->front();
							int stringIndex = primaryLanguage->GetStringIndex(levelName);
							delete[] levelName;

							if (stringIndex < 0)
							{
								return nullptr;
							}

							currentLevel->stringIndex = stringIndex;

							ClearArgumentStack();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Legend", command))//todo:delete stack here!
					{
						switch (mode)
						{
						case ParseMode::Level:
						    {
							if (argStack->size() < 1)
							{
								std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							char *legend = ParseString(argString);
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->legend == nullptr)
								currentLevel->legend = new Legend;

							int stringIndex = primaryLanguage->GetStringIndex(legend);
							//currentLevel->legend->stringIndex = primaryLanguage->GetStringIndex(legend);
							delete[] legend;

							if (stringIndex < 0)
								return nullptr;

							currentLevel->legend->stringIndex = stringIndex;

							ClearArgumentStack();
							//currentLevel->options.LegendUsed = true;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Horizon", command))
					{
						switch (mode)
						{
						case ParseMode::Level:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							LevelData *currentLevel = gameflow->levels->front();
							currentLevel->options.Horizon = ParseBool(argStack->top());

							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						case ParseMode::Title:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							TitleLevelData *title = gameflow->title;
							title->options.Horizon = ParseBool(argStack->top());

							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("ColAddHorizon", command))
					{
						switch (mode)
						{
						case ParseMode::Level:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							LevelData *currentLevel = gameflow->levels->front();
							currentLevel->options.ColAddHorizon = ParseBool(argStack->top());

							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						case ParseMode::Title:
						    {
							if (argStack->size() != 1)
							{
								std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							TitleLevelData *title = gameflow->title;
							title->options.ColAddHorizon = ParseBool(argStack->top());

							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Layer1", command))
					{
						switch (mode)
						{
						case ParseMode::Level:
						    {
							if (argStack->size() != 4)
							{
								std::string errorText("Expected 4 arguments : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->layer1 == nullptr)
								currentLevel->layer1 = new Layer1;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->layer1->speed = (int8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->layer1->blue = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->layer1->green = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->layer1->red = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							//currentLevel->options.Layer1Used = true;
							break;
						    }

						case ParseMode::Title:
						    {
							if (argStack->size() != 4)
							{
								std::string errorText("Expected 4 arguments : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							TitleLevelData *title = gameflow->title;
							if (title->layer1 == nullptr)
								title->layer1 = new Layer1;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->layer1->speed = (int8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->layer1->blue = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->layer1->green = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->layer1->red = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							//title->options.Layer1Used = true;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Layer2", command))
					{
						switch (mode)
						{
							if (argStack->size() != 4)
							{
								std::string errorText("Expected 4 arguments : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

						case ParseMode::Level:
						    {
							/*if (argStack->size() != 4)
							{
								std::string errorText("Expected 4 arguments : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, GetCurrentLineNumber(data, ptr));
								return nullptr;
							}*/

							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->layer2 == nullptr)
								currentLevel->layer2 = new Layer2;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->layer2->speed = (int8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->layer2->blue = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->layer2->green = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->layer2->red = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->options.Layer2used = true;
							break;
						    }

						case ParseMode::Title:
						    {
							/*if (argStack->size() != 4)
							{
								std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + "\" at");
								AddNewError(errorText, GetCurrentLineNumber(data, ptr));
								return nullptr;
							}*/

							TitleLevelData *title = gameflow->title;
							if (title->layer2 == nullptr)
								title->layer2 = new Layer2;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->layer2->speed = (int8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->layer2->blue = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->layer2->green = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->layer2->red = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							title->options.Layer2used = true;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("LensFlare", command))
					{
						if (argStack->size() != 6)
						{
							std::string errorText("Expected 6 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->lensFlare == nullptr)
								currentLevel->lensFlare = new LensFlare;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->lensFlare->blue = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->lensFlare->green = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->lensFlare->red = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->lensFlare->xClicks = (uint32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->lensFlare->zClicks = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->lensFlare->yClicks = (uint32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							//currentLevel->options.LensFlareUsed = true;
							break;
						    }

						case ParseMode::Title:
						    {
							TitleLevelData *title = gameflow->title;
							if (title->lensFlare == nullptr)
								title->lensFlare = new LensFlare;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->lensFlare->blue = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->lensFlare->green = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->lensFlare->red = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->lensFlare->xClicks = (uint32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->lensFlare->zClicks = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->lensFlare->xClicks = (uint32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							//title->options.LensFlareUsed = true;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("UVrotate", command))
					{
						if (argStack->size() != 1)
						{
							std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->uv == nullptr)
								currentLevel->uv = new UVRotate;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->uv->speed = (int8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							//currentLevel->options.UVUsed = true;
							break;
						    }

						case ParseMode::Title:
						    {
							TitleLevelData *title = gameflow->title;
							if (title->uv == nullptr)
								title->uv = new UVRotate;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->uv->speed = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							//title->options.UVUsed = true;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Fog", command))
					{
						if (argStack->size() != 3)
						{
							std::string errorText("Expected 3 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->fog == nullptr)
								currentLevel->fog = new Fog;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->fog->blue = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->fog->green = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->fog->red = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							//currentLevel->options.FogUsed = true;
							break;
						    }

						case ParseMode::Title:
						    {
							TitleLevelData *title = gameflow->title;
							if (title->fog == nullptr)
								title->fog = new Fog;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->fog->blue = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->fog->green = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->fog->red = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							//title->options.FogUsed = true;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("LoadCamera", command))
					{
						if (argStack->size() != 7)
						{
							std::string errorText("Expected 7 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->loadCam == nullptr)
								currentLevel->loadCam = new LoadCamera;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->loadCam->room = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->loadCam->targetZ = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->loadCam->targetY = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->loadCam->targetX = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->loadCam->srcZ = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->loadCam->srcY = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->loadCam->srcX = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							//currentLevel->options.LoadCamUsed = true;
							break;
						    }

						case ParseMode::Title:
						    {
							TitleLevelData *title = gameflow->title;
							if (title->loadCam == nullptr)
								title->loadCam = new LoadCamera;

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->loadCam->room = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->loadCam->targetZ = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->loadCam->targetY = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->loadCam->targetX = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->loadCam->srcZ = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->loadCam->srcY = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							title->loadCam->srcX = (int32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							//title->options.LoadCamUsed = true;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("ResidentCut", command))
					{
						if (argStack->size() != 2)
						{
							std::string errorText("Expected 2 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->resCuts == nullptr)
								currentLevel->resCuts = new std::forward_list<ResidentCut *>;

							ResidentCut *newResCut = new ResidentCut;
							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newResCut->cutIndex = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newResCut->number = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->resCuts->push_front(newResCut);
							++currentLevel->numResCuts;

							break;
						    }

						case ParseMode::Title:
						    {
							TitleLevelData *title = gameflow->title;
							if (title->resCuts == nullptr)
								title->resCuts = new std::forward_list<ResidentCut *>;

							ResidentCut *newResCut = new ResidentCut;
							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newResCut->cutIndex = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newResCut->number = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							title->resCuts->push_front(newResCut);
							++title->numResCuts;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Mirror", command))
					{
						if (argStack->size() != 2)
						{
							std::string errorText("Expected 2 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->mirrors == nullptr)
								currentLevel->mirrors = new std::forward_list<Mirror *>;

							Mirror *newMirror = new Mirror;
							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newMirror->xAxis = (uint32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newMirror->room = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->mirrors->push_front(newMirror);
							currentLevel->options.Mirrorused = true;
							break;
						    }

						case ParseMode::Title:
						    {
							TitleLevelData *title = gameflow->title;
							Mirror *newMirror = new Mirror;
							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newMirror->xAxis = (uint32_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newMirror->room = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							title->mirrors->push_front(newMirror);
							title->options.Mirrorused = true;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("AnimatingMIP", command))
					{
						if (argStack->size() != 2)
						{
							std::string errorText("Expected 2 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->mips == nullptr)
								currentLevel->mips = new std::forward_list<AnimatingMIP *>;

							AnimatingMIP *newMIP = new AnimatingMIP;
							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newMIP->distance = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newMIP->objIndex = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->mips->push_front(newMIP);
							++currentLevel->numMIPs;

							if (newMIP->objIndex > 16 || newMIP->objIndex < 1)
							{
								AddNewError("Illegal animating object number;", StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							break;
						    }

						case ParseMode::Title:
						    {
							TitleLevelData *title = gameflow->title;
							if (title->mips == nullptr)
								title->mips = new std::forward_list<AnimatingMIP *>;

							AnimatingMIP *newMIP = new AnimatingMIP;
							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newMIP->distance = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							newMIP->objIndex = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							title->mips->push_front(newMIP);
							++title->numMIPs;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("ResetHUB", command))
					{
						if (argStack->size() != 1)
						{
							std::string errorText("Expected 1 argument : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							currentLevel->resetHUB = new ResetHUB;
							if (!ArgumentsContainNumber(data, ptr))
								return nullptr;

							currentLevel->resetHUB->levelIndex = (uint8_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->options.ResetHUBused = true;
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("PuzzleCombo", command))
					{
						if (argStack->size() < 9)
						{
							std::string errorText("Expected 9 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						if (!LineContainsNumber(data, ptr))
							return nullptr;

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->puzzleCombos == nullptr)
								currentLevel->puzzleCombos = new std::forward_list < PuzzleCombo * > ;

							PuzzleCombo *newPuzzleCombo = new PuzzleCombo;
							newPuzzleCombo->appearance.something = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPuzzleCombo->appearance.xAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPuzzleCombo->appearance.zAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPuzzleCombo->appearance.yAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPuzzleCombo->appearance.size = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPuzzleCombo->appearance.height = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							char *name = StringHelper::IsolateStringBetweenCommas(2, 6, argString);
							char *nameString = ParseString(name);
							delete[] name;

							int stringIndex = primaryLanguage->GetStringIndex(nameString);
							//newPuzzleCombo->stringIndex = primaryLanguage->GetStringIndex(nameString);
							delete[] nameString;

							if (stringIndex < 0)
								return nullptr;
							//newPuzzleCombo->stringIndex = primaryLanguage->GetStringIndex(ParseString(StringHelper::IsolateStringBetweenCommas(2, 6, argString)));
							newPuzzleCombo->stringIndex = stringIndex;

							while (argStack->size() != 2)
							{
								delete[] argStack->top();
								argStack->pop();			//todo: deletions have to be done here!
							}

							newPuzzleCombo->secondIndex = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPuzzleCombo->firstIndex = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->puzzleCombos->push_front(newPuzzleCombo);
							++currentLevel->numPuzzleCombos;

							if (newPuzzleCombo->secondIndex > 2 || newPuzzleCombo->secondIndex < 1 || newPuzzleCombo->firstIndex>8 || newPuzzleCombo->firstIndex < 1)
							{
								AddNewError("Illegal Puzzle Combo number;", StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("PickupCombo", command))
					{
						if (argStack->size() < 9)
						{
							std::string errorText("Expected 9 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						if (!LineContainsNumber(data, ptr))
							return nullptr;

						switch (mode)
						{
						case ParseMode::Level:
						{
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->pickupCombos == nullptr)
								currentLevel->pickupCombos = new std::forward_list < PickupCombo * > ;

							PickupCombo *newPickupCombo = new PickupCombo;
							newPickupCombo->appearance.something = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPickupCombo->appearance.xAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPickupCombo->appearance.zAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPickupCombo->appearance.yAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPickupCombo->appearance.size = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPickupCombo->appearance.height = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							char *name = StringHelper::IsolateStringBetweenCommas(2, 6, argString);
							char *nameString = ParseString(name);
							delete[] name;

							int stringIndex = primaryLanguage->GetStringIndex(nameString);
							delete[] nameString;

							if (stringIndex < 0)
								return nullptr;
							newPickupCombo->stringIndex = stringIndex;

							while (argStack->size() != 2)
							{
								delete[] argStack->top();
								argStack->pop();			//todo: deletions have to be done here!
							}

							newPickupCombo->secondIndex = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPickupCombo->firstIndex = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->pickupCombos->push_front(newPickupCombo);
							++currentLevel->numPickupCombos;

							if (newPickupCombo->secondIndex > 2 || newPickupCombo->secondIndex < 1 || newPickupCombo->firstIndex>4 || newPickupCombo->firstIndex < 1)
							{
								AddNewError("Illegal Pickup Combo number;", StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							break;
						}

						default:
						{
							PlotOutOfContextError(data, ptr);
						}
						}
					}

					else if (StringHelper::IsStatement("KeyCombo", command))
					{
						if (argStack->size() < 9)
						{
							std::string errorText("Expected 9 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						if (!LineContainsNumber(data, ptr))
							return nullptr;

						switch (mode)
						{
						case ParseMode::Level:
						{
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->keyCombos == nullptr)
								currentLevel->keyCombos = new std::forward_list < KeyCombo * > ;

							KeyCombo *newKeyCombo = new KeyCombo;
							newKeyCombo->appearance.something = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newKeyCombo->appearance.xAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newKeyCombo->appearance.zAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newKeyCombo->appearance.yAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newKeyCombo->appearance.size = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newKeyCombo->appearance.height = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							char *name = StringHelper::IsolateStringBetweenCommas(2, 6, argString);
							char *nameString = ParseString(name);
							delete[] name;

							int stringIndex = primaryLanguage->GetStringIndex(nameString);
							delete[] nameString;

							if (stringIndex < 0)
								return nullptr;

							newKeyCombo->stringIndex = stringIndex;

							//newKeyCombo->stringIndex = primaryLanguage->GetStringIndex(ParseString(StringHelper::IsolateStringBetweenCommas(2, 6, argString)));
							while (argStack->size() != 2)
							{
								delete[] argStack->top();
								argStack->pop();			//todo: deletions have to be done here!
							}

							newKeyCombo->secondIndex = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newKeyCombo->firstIndex = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->keyCombos->push_front(newKeyCombo);
							++currentLevel->numKeyCombos;

							if (newKeyCombo->secondIndex > 2 || newKeyCombo->secondIndex < 1 || newKeyCombo->firstIndex>8 || newKeyCombo->firstIndex < 1)
							{
								AddNewError("Illegal Key Combo number;", StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							break;
						}

						default:
						{
							PlotOutOfContextError(data, ptr);
						}
						}
					}

					else if (StringHelper::IsStatement("Puzzle", command))
					{
						if (argStack->size() < 8)
						{
							std::string errorText("Expected 8 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						if (!LineContainsNumber(data, ptr))
							return nullptr;

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->puzzles == nullptr)
								currentLevel->puzzles = new std::forward_list<Puzzle *>;

							Puzzle *newPuzzle = new Puzzle;
							newPuzzle->appearance.something = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPuzzle->appearance.xAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPuzzle->appearance.zAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPuzzle->appearance.yAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPuzzle->appearance.size = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPuzzle->appearance.height = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							char *name = StringHelper::IsolateStringBetweenCommas(1, 6, argString);
							char *nameString = ParseString(name);
							delete[] name;

							int stringIndex = primaryLanguage->GetStringIndex(nameString);
							delete[] nameString;

							if (stringIndex < 0)
								return nullptr;

							newPuzzle->stringIndex = stringIndex;

							//newPuzzle->stringIndex = primaryLanguage->GetStringIndex(ParseString(StringHelper::IsolateStringBetweenCommas(1, 6, argString)));
							while (argStack->size() != 1)
							{
								delete[] argStack->top();
								argStack->pop();
							}

							newPuzzle->index = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->puzzles->push_front(newPuzzle);
							++currentLevel->numPuzzles;

							if (newPuzzle->index > 12 || newPuzzle->index < 1)
							{
								AddNewError("Illegal Puzzle number;", StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Pickup", command))
					{
						if (argStack->size() < 8)
						{
							std::string errorText("Expected 8 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						if (!LineContainsNumber(data, ptr))
							return nullptr;

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->pickups == nullptr)
								currentLevel->pickups = new std::forward_list<Pickup *>;
								//currentLevel->pickups = new std::stack < Pickup *, std::forward_list<Pickup *> >;

							Pickup *newPickup = new Pickup;
							newPickup->appearance.something = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPickup->appearance.xAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPickup->appearance.zAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPickup->appearance.yAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPickup->appearance.size = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newPickup->appearance.height = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							char *name = StringHelper::IsolateStringBetweenCommas(1, 6, argString);
							char *nameString = ParseString(name);
							delete[] name;

							int stringIndex = primaryLanguage->GetStringIndex(nameString);
							delete[] nameString;

							if (stringIndex < 0)
								return nullptr;

							newPickup->stringIndex = stringIndex;

							//newPickup->stringIndex = primaryLanguage->GetStringIndex(ParseString(StringHelper::IsolateStringBetweenCommas(1, 6, argString)));
							while (argStack->size() != 1)
							{
								delete[] argStack->top();
								argStack->pop();
							}
							//argStack->pop();
							//newPickup->name = ParseString(argStack->top());
							//argStack->pop();
							newPickup->index = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->pickups->push_front(newPickup);
							++currentLevel->numPickups;

							if (newPickup->index > 4 || newPickup->index < 1)
							{
								AddNewError("Illegal Pickup number;", StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Examine", command))
					{
						if (argStack->size() < 8)
						{
							std::string errorText("Expected 8 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						if (!LineContainsNumber(data, ptr))
							return nullptr;

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->examines == nullptr)
								currentLevel->examines = new std::forward_list<Examine *>;
								//currentLevel->examines = new std::stack < Examine *, std::forward_list<Examine *> >;

							Examine *newExamine = new Examine;
							newExamine->appearance.something = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newExamine->appearance.xAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newExamine->appearance.zAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newExamine->appearance.yAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newExamine->appearance.size = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newExamine->appearance.height = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							char *name = StringHelper::IsolateStringBetweenCommas(1, 6, argString);
							char *nameString = ParseString(name);
							delete[] name;

							int stringIndex = primaryLanguage->GetStringIndex(nameString);
							delete[] nameString;

							if (stringIndex < 0)
								return nullptr;

							newExamine->stringIndex = stringIndex;

							//newExamine->stringIndex = primaryLanguage->GetStringIndex(ParseString(StringHelper::IsolateStringBetweenCommas(1, 6, argString)));
							while (argStack->size() != 1)
							{
								delete[] argStack->top();
								argStack->pop();
							}

							newExamine->index = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->examines->push_front(newExamine);
							++currentLevel->numExamines;

							if (newExamine->index > 3 || newExamine->index < 1)
							{
								AddNewError("Illegal Examine number;", StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Key", command))
					{
						if (argStack->size() < 8)
						{
							std::string errorText("Expected 8 arguments : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						if (!LineContainsNumber(data, ptr))
							return nullptr;

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();
							if (currentLevel->keys == nullptr)
								currentLevel->keys = new std::forward_list<Key *>;
								//currentLevel->keys = new std::stack < Key *, std::forward_list<Key *> >;

							Key *newKey = new Key;
							newKey->appearance.something = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newKey->appearance.xAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newKey->appearance.zAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newKey->appearance.yAngle = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newKey->appearance.size = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							newKey->appearance.height = (uint16_t)ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							char *name = StringHelper::IsolateStringBetweenCommas(1, 6, argString);
							char *nameString = ParseString(name);
							delete[] name;

							int stringIndex = primaryLanguage->GetStringIndex(nameString);
							delete[] nameString;

							if (stringIndex < 0)
								return nullptr;
							newKey->stringIndex = stringIndex;

							//newKey->stringIndex = primaryLanguage->GetStringIndex(ParseString(StringHelper::IsolateStringBetweenCommas(1, 6, argString)));
							while (argStack->size() != 1)
							{
								delete[] argStack->top();
								argStack->pop();
							}

							newKey->index = ParseNumber(argStack->top());
							delete[] argStack->top();
							argStack->pop();

							currentLevel->keys->push_front(newKey);
							++currentLevel->numKeys;
							break;

							if (newKey->index > 12 || newKey->index < 1)
							{
								AddNewError("Illegal Key number;", StringHelper::GetCurrentLineNumber(data, ptr));
								return nullptr;
							}

						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Lightning", command))
					{
						if (argStack->size() != 1)
						{
							std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();

							currentLevel->options.Lightning = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						case ParseMode::Title:
						    {
							TitleLevelData *title = gameflow->title;
							title->options.Lightning = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("YoungLara", command))
					{
						if (argStack->size() != 1)
						{
							std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();

							currentLevel->options.YoungLara = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Weather", command))
					{
						if (argStack->size() != 1)
						{
							std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();

							currentLevel->options.Weather = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						case ParseMode::Title:
						    {
							TitleLevelData *title = gameflow->title;
							title->options.Weather = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Train", command))
					{
						if (argStack->size() != 1)
						{
							std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();

							currentLevel->options.Train = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						case ParseMode::Title:
						    {
							TitleLevelData *title = gameflow->title;
							title->options.Train = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Pulse", command))
					{
						if (argStack->size() != 1)
						{
							std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();

							currentLevel->options.Pulse = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						case ParseMode::Title:
						    {
							TitleLevelData *title = gameflow->title;
							title->options.Pulse = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("NoLevel", command))
					{
						if (argStack->size() != 1)
						{
							std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();

							currentLevel->options.NoLevel = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("RemoveAmulet", command))
					{
						if (argStack->size() != 1)
						{
							std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();

							currentLevel->options.RemoveAmulet = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Starfield", command))
					{
						if (argStack->size() != 1)
						{
							std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();

							currentLevel->options.Starfield = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						case ParseMode::Title:
						    {
							TitleLevelData *title = gameflow->title;
							title->options.Starfield = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else if (StringHelper::IsStatement("Timer", command))
					{
						if (argStack->size() != 1)
						{
							std::string errorText("Expected 1 bool (disabled/enabled) argument : \"" + std::string(wholeLine) + "\" at");
							AddNewError(errorText, StringHelper::GetCurrentLineNumber(data, ptr));
							return nullptr;
						}

						switch (mode)
						{
						case ParseMode::Level:
						    {
							LevelData *currentLevel = gameflow->levels->front();

							currentLevel->options.Timer = ParseBool(argStack->top());
							delete[] argStack->top();
							argStack->pop();
							break;
						    }

						default:
						    {
								PlotOutOfContextError(data, ptr);
						    }
						}
					}

					else
					{
						AddNewWarning("Command does not exist : \"" + std::string(command), StringHelper::GetCurrentLineNumber(data, ptr));
					}

					//delete[] argString;
					//delete[]command;
					//delete[] arg;
					while (!argStack->empty())
					{
						char *toErase = argStack->top();
						delete[] toErase;
						argStack->pop();
					}
				}
			}
			ptr += lineLength;
		}

		if (numPSXExtensionChunks == 0 && isGameflowScript)
			AddNewWarning("Could not find '[PSXExtensions]'");

		if (numPCExtensionChunks == 0 && isGameflowScript)
			AddNewWarning("Could not find '[PCExtensions]'");

		if (numOptionChunks == 0 && isGameflowScript)
			AddNewWarning("Could not find '[Options]'");

		if (numLanguageChunks == 0 && isGameflowScript)
			AddNewWarning("Could not find '[Language]'");

		if (gameflow->PSXStrings == nullptr && isLanguageScript)
			AddNewWarning("Could not find '[PSXStrings]'");

		if (gameflow->PCStrings == nullptr && isLanguageScript)
			AddNewWarning("Could not find '[PCStrings]'");

		if (gameflow->Strings == nullptr && isLanguageScript)
			AddNewWarning("Could not find '[Strings]'");

		return gameflow;
	}


    void ScriptFile::ClearArgumentStack()
    {
        while (!argStack->empty())
		{
			char *toDelete = argStack->top();
			delete[] toDelete;
			argStack->pop();
		}
    }

	void ScriptFile::Clear()
	{
		if (filename != nullptr)
			delete[] filename;

		filename = nullptr;

		if (command != nullptr)
			delete[] command;

		if (wholeLine != nullptr)
			delete[] wholeLine;

		if (line != nullptr)
			delete[] line;

		if (primaryLanguage != nullptr)
			delete primaryLanguage;

		if (gameflowScript != nullptr)
		{
			if (gameflowScript->languages != nullptr)
			{
				for (auto it = gameflowScript->languages->begin(); it != gameflowScript->languages->end(); ++it)
					//for (auto it = gameflowScript->languages->begin(); it != gameflowScript->languages->end(); ++it)
				{
					delete[](*it)->langName;
				}

				delete gameflowScript->languages;
			}

			if (gameflowScript->definitions != nullptr)
			{
				delete[] gameflowScript->definitions->PCCut;
				delete[] gameflowScript->definitions->PCFmv;
				delete[] gameflowScript->definitions->PCLevel;
				delete[] gameflowScript->definitions->PSXCut;
				delete[] gameflowScript->definitions->PSXFmv;
				delete[] gameflowScript->definitions->PSXLevel;

				delete gameflowScript->definitions;
			}

			if (gameflowScript->levels != nullptr)
			{
				for (auto it = gameflowScript->levels->begin(); it != gameflowScript->levels->end(); ++it)
				{
					LevelData *currentLevel = *it;

					if (currentLevel->path != nullptr)//new 11-7-15
						delete[] currentLevel->path;

					if (currentLevel->cut != nullptr)
						delete currentLevel->cut;

					if (currentLevel->examines != nullptr)
					{
						for (auto examineIt = currentLevel->examines->begin(); examineIt != currentLevel->examines->end(); ++examineIt)
						{
							Examine *currentExamine = *examineIt;

							//delete[] currentExamine->name;	//is this allocated? check
							delete currentExamine;
						}

						delete currentLevel->examines;
					}

					if (currentLevel->fmvs != nullptr)
					{
						for (auto fmvIt = currentLevel->fmvs->begin(); fmvIt != currentLevel->fmvs->end(); ++fmvIt)
						{
							delete *fmvIt;
						}

						delete currentLevel->fmvs;
					}

					if (currentLevel->fog != nullptr)
					{
						delete currentLevel->fog;
					}

					if (currentLevel->keys != nullptr)
					{
						for (auto keyIt = currentLevel->keys->begin(); keyIt != currentLevel->keys->end(); ++keyIt)
						{
							Key *currentKey = *keyIt;

							//delete[] currentKey->name;
							delete currentKey;
						}

						delete currentLevel->keys;
					}

					if (currentLevel->layer2 != nullptr)
					{
						delete currentLevel->layer2;
					}

					if (currentLevel->layer1 != nullptr)
					{
						delete currentLevel->layer1;
					}

					if (currentLevel->legend != nullptr)
					{
						//delete[] currentLevel->legend->string;
						delete currentLevel->legend;
					}

					if (currentLevel->lensFlare != nullptr)
					{
						delete currentLevel->lensFlare;
					}

					if (currentLevel->loadCam != nullptr)
					{
						delete currentLevel->loadCam;
					}

					if (currentLevel->mips != nullptr)
					{
						for (auto mipIt = currentLevel->mips->begin(); mipIt != currentLevel->mips->end(); ++mipIt)
						{
							delete *mipIt;
						}

						delete currentLevel->mips;
					}

					if (currentLevel->mirrors != nullptr)
					{
						for (auto mirrorIt = currentLevel->mirrors->begin(); mirrorIt != currentLevel->mirrors->end(); ++mirrorIt)
						{
							delete *mirrorIt;
						}

						delete currentLevel->mirrors;
					}

					/*if (currentLevel->name != nullptr)
					{
						delete[] currentLevel->name;
					}*/

					if (currentLevel->pickups != nullptr)
					{
						for (auto pickupIt = currentLevel->pickups->begin(); pickupIt != currentLevel->pickups->end(); ++pickupIt)
						{
							Pickup *currentPickup = *pickupIt;

							//delete[] currentPickup->name;
							delete currentPickup;
						}

						delete currentLevel->pickups;
					}

					if (currentLevel->puzzleCombos != nullptr)
					{
						for (auto puzzleComboIt = currentLevel->puzzleCombos->begin(); puzzleComboIt != currentLevel->puzzleCombos->end(); ++puzzleComboIt)
						{
							PuzzleCombo *currentPuzzleCombo = *puzzleComboIt;

							//delete[] currentPuzzleCombo->name;
							delete currentPuzzleCombo;
						}

						delete currentLevel->puzzleCombos;
					}

					if (currentLevel->pickupCombos != nullptr)
					{
						for (auto combo : *currentLevel->pickupCombos)
							delete combo;

						delete currentLevel->pickupCombos;
					}

					if (currentLevel->keyCombos != nullptr)
					{
						for (auto combo : *currentLevel->keyCombos)
							delete combo;

						delete currentLevel->keyCombos;
					}

					if (currentLevel->puzzles != nullptr)
					{
						for (auto puzzleIt = currentLevel->puzzles->begin(); puzzleIt != currentLevel->puzzles->end(); ++puzzleIt)
						{
							Puzzle *currentPuzzle = *puzzleIt;

							//delete[]currentPuzzle->name;
							delete currentPuzzle;
						}

						delete currentLevel->puzzles;
					}

					if (currentLevel->resCuts != nullptr)
					{
						for (auto cutIt = currentLevel->resCuts->begin(); cutIt != currentLevel->resCuts->end(); ++cutIt)
						{
							delete *cutIt;
						}

						delete currentLevel->resCuts;
					}

					if (currentLevel->resetHUB != nullptr)
					{
						delete currentLevel->resetHUB;
					}

					if (currentLevel->uv != nullptr)
					{
						delete currentLevel->uv;
					}

					delete currentLevel;
				}

				delete gameflowScript->levels;
			}

			if (gameflowScript->PCStrings != nullptr)
			{
				for (auto it = gameflowScript->PCStrings->begin(); it != gameflowScript->PCStrings->end(); ++it)
				{
					LanguageString *currentString = *it;

					delete[] currentString->string;
					delete currentString;
				}

				delete gameflowScript->PCStrings;
			}

			if (gameflowScript->PSXStrings != nullptr)
			{
				for (auto it = gameflowScript->PSXStrings->begin(); it != gameflowScript->PSXStrings->end(); ++it)
				{
					LanguageString *currentString = *it;

					delete[] currentString->string;
					delete currentString;
				}

				delete gameflowScript->PSXStrings;
			}

			if (gameflowScript->Strings != nullptr)
			{
				for (auto it = gameflowScript->Strings->begin(); it != gameflowScript->Strings->end(); ++it)
				{
					LanguageString *currentString = *it;

					delete[] currentString->string;
					delete currentString;
				}

				delete gameflowScript->Strings;
			}

			if (gameflowScript->title != nullptr)
			{
				if (gameflowScript->title->path != nullptr)
					delete[] gameflowScript->title->path;

				if (gameflowScript->title->fog != nullptr)
					delete gameflowScript->title->fog;

				if (gameflowScript->title->layer2 != nullptr)
					delete gameflowScript->title->layer2;

				if (gameflowScript->title->layer1 != nullptr)
					delete gameflowScript->title->layer1;

				if (gameflowScript->title->lensFlare != nullptr)
					delete gameflowScript->title->lensFlare;

				if (gameflowScript->title->loadCam != nullptr)
					delete gameflowScript->title->loadCam;

				if (gameflowScript->title->mips != nullptr)
				{
					for (auto it = gameflowScript->title->mips->begin(); it != gameflowScript->title->mips->end(); ++it)
					{
						delete *it;
					}

					delete gameflowScript->title->mips;
				}

				if (gameflowScript->title->mirrors != nullptr)
				{
					for (auto it = gameflowScript->title->mirrors->begin(); it != gameflowScript->title->mirrors->end(); ++it)
					{
						delete *it;
					}

					delete gameflowScript->title->mirrors;
				}

				if (gameflowScript->title->resCuts != nullptr)
				{
					for (auto it = gameflowScript->title->resCuts->begin(); it != gameflowScript->title->resCuts->end(); ++it)
					{
						delete *it;
					}

					delete gameflowScript->title->resCuts;
				}

				if (gameflowScript->title->uv != nullptr)
				{
					delete gameflowScript->title->uv;
				}

				delete gameflowScript->title;
			}
		}

		delete gameflowScript;

		if (argStack != nullptr)
		{
			while (!argStack->empty())
			{
				char *toDelete = argStack->top();
				delete[] toDelete;
				argStack->pop();
			}

			delete argStack;
		}
		//todo: delete all allocated structs inside gameflow!
	}

	ScriptFile::~ScriptFile()
	{
		Clear();
	}
