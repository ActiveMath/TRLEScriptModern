#include "StringHelper.h"
#include "Gameflow.h"
#include <unordered_map>
#include <forward_list>
//#include <iostream>
#include <algorithm>
#include <bitset>
#include <cstdio>
//#include <fstream>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <stack>

using namespace TRLEScript;

	Gameflow::Gameflow(const char *filename) : ScriptFile(filename)
	{
		isLanguageScript = false;
		isGameflowScript = true;
	}


	Gameflow::~Gameflow()
	{
	}

	void Gameflow::Parse()
	{
	//	throw notImplemented;
	}

	std::stack<std::string> Gameflow::GetLanguages()
	{
		std::stack<std::string> languageStack;
		if (!created)
		{
			AddNewMessage(MessageTypes::Error, "Could not complete request: GetLanguages (the gameflow script was not compiled yet)!");
			return languageStack;
		}

		for (auto language : *gameflowScript->languages)
		{
			if (language->index != 0)
				languageStack.push(std::string(language->langName));
		}

		return languageStack;
	}

	const GameflowScriptHeader* Gameflow::GetGameflowScript() const
	{
		if (!created)
			return nullptr;

		return gameflowScript;
	}

	uint8_t Gameflow::GetNumUniqueLevelPaths() const
	{
		return gameflowScript->levelPathMap.size();
	}

	uint16_t Gameflow::GetLevelBlockLength(const TitleLevelData *title)
	{
		uint16_t length = 0;

		length += titleBlockLength;

		if (title->fog != nullptr)
			length += fogLength;

		if (title->layer2 != nullptr)
			length += layer2Length;

		if (title->layer1 != nullptr)
			length += layer1Length;

		if (title->lensFlare != nullptr)
			length += lensFlareLength;

		if (title->loadCam != nullptr)
			length += loadCameraLength;

		if (title->mips != nullptr)
		{
			for (AnimatingMIP *tmp : *title->mips)
				length += animatingMipLength;
		}

		if (title->mirrors != nullptr)
		{
			for (Mirror *tmp : *title->mirrors)
				length += mirrorLength;
		}

		if (title->resCuts != nullptr)
		{
			for (ResidentCut *tmp : *title->resCuts)
				length += residentCutLength;
		}

		if (title->uv != nullptr)
			length += UVrotateLength;

		return length;
	}

	uint16_t Gameflow::GetLevelBlockLength(const LevelData *level)
	{
		uint16_t length = 0;

		length += levelBlockLength;	//level tag, 1 per byte: 0x81 (cmd code)-stringIndex-bitfield(16)-pathIndex-audio-levelDataEnd

		if (level->legend != nullptr)
			length += legendLength;

		if (level->loadCam != nullptr)
			length += loadCameraLength;

		if (level->layer2 != nullptr)
			length += layer2Length;

		if (level->layer1 != nullptr)
			length += layer1Length;

		if (level->mirrors != nullptr)
		{
			for (Mirror *tmp : *level->mirrors)
				length += mirrorLength;
		}

		if (level->fog != nullptr)
			length += fogLength;

		if (level->uv != nullptr)
			length += UVrotateLength;

		if (level->cut != nullptr)
			length += cutLength;

		if (level->resetHUB != nullptr)
			length += resetHubLength;

		if (level->lensFlare != nullptr)
			length += lensFlareLength;

		if (level->resCuts != nullptr)
		{
			for (ResidentCut *tmp : *level->resCuts)
				length += residentCutLength;
		}

		if (level->fmvs != nullptr)
		{
			for (FMV *tmp : *level->fmvs)
				length += FMVLength;
		}

		if (level->mips != nullptr)
		{
			for (AnimatingMIP *tmp : *level->mips)
				length += animatingMipLength;
		}

		if (level->keys != nullptr)
		{
			for (Key *tmp : *level->keys)
				length += keyLength;					//warning: could be 14 if stringIndex is bitu16
		}

		if (level->puzzles != nullptr)
		{
			for (Puzzle *tmp : *level->puzzles)
				length += puzzleLength;
		}

		if (level->pickups != nullptr)
		{
			for (Pickup *tmp : *level->pickups)
				length += pickupLength;
		}

		if (level->puzzleCombos != nullptr)
		{
			for (PuzzleCombo *tmp : *level->puzzleCombos)
				length += puzzleComboLength;
		}

		if (level->pickupCombos != nullptr)
		{
			for (PickupCombo *tmp : *level->pickupCombos)
				length += pickupComboLength;
		}

		if (level->keyCombos != nullptr)
		{
			for (KeyCombo *tmp : *level->keyCombos)
				length += keyComboLength;
		}

		if (level->examines != nullptr)
		{
			for (Examine *tmp : *level->examines)
				length += examineLength;
		}

		return length;
	}

//	struct SumPathLengths
//	{
//		SumPathLengths() {
//			sum = 0;
//		}
//		void operator()(uint16_t n) {
//			sum += n + 1;	//1?
//		}
//
//		uint16_t sum;
//	};

	std::unordered_set<int> *Gameflow::Get1ToNSet(const int n)
	{
		std::unordered_set<int> *out = new std::unordered_set < int > ;

		for (int i = 1; i <= n; ++i)
			out->insert(i);

		return out;
	}

	uint16_t Gameflow::GetPathStringOffset(int lastIndex)
	{
		uint16_t offset = 0;
		int i = 0;

		for (int j = 0; j < gameflowScript->numTotalLevels; ++j)
		{
			for (auto it : gameflowScript->levelPathMap)	//new 11-7-15
			{
				if (it.second == j)
				{
					if (lastIndex == i)
						return offset;

					offset += it.first.size() + 1;
					++i;
				}
			}
		}
//can't get here
	}

	int Gameflow::GetPathIndex(const char *path)// const
	{
		int j = 0;
		for (int i = 0; i < gameflowScript->numTotalLevels; ++i)
		{
			//int j = 0;
			for (auto it : gameflowScript->levelPathMap)
			{
				if (it.second == i)
				{
					if (strcmp(it.first.c_str(), path) == 0)
						return j;

					++j;
				}
			}
		}

		AddNewMessage(MessageTypes::Error, "Could not match an index with path '" + std::string(path) + "'!");
		return -1;
	}

	void Gameflow::CompileToDAT(const char *sourceData)
		//void Gameflow::CompileToDAT(const char *sourceData, int length)
	{
		compilingToDAT = true;

		GameflowScriptHeader *gameflow = SourceParse(sourceData);
		//GameflowScriptHeader *gameflow = SourceParse(sourceData, length);
		gameflowScript = gameflow;
		if (gameflow == nullptr)
			return;

		//created = true;

		FILE *output = fopen(filename, "wb");
		//FILE *output = fopen("script.dat", "wb");
		if (output == nullptr)
		{
			AddNewMessage(MessageTypes::Error, "Could not open " + std::string(filename) + " for writing!");
			return;
		}

		if (gameflow->levels != nullptr)
			gameflow->levels->reverse();

		std::bitset<8> globalOptionsBitfield;

		globalOptionsBitfield.set(0, gameflow->options->FlyCheat);
		globalOptionsBitfield.set(1, gameflow->options->LoadSave);
		globalOptionsBitfield.set(2, gameflow->options->Title);
		globalOptionsBitfield.set(3, gameflow->options->PlayAnyLevel);
		globalOptionsBitfield.set(4, 0);
		globalOptionsBitfield.set(5, 0);
		globalOptionsBitfield.set(6, 0);
		globalOptionsBitfield.set(7, gameflow->options->DemoDisc);

		uint8_t tmp = static_cast<uint8_t>(globalOptionsBitfield.to_ulong());
		fwrite(&tmp, sizeof(uint8_t), 1, output);
		fputc(0, output);
		fputc(0, output);
		fputc(0, output);

		fwrite(&gameflow->options->InputTimeout, sizeof(uint32_t), 1, output);
		fwrite(&gameflow->options->Security, sizeof(uint8_t), 1, output);

		uint8_t tmp2 = GetNumUniqueLevelPaths();
		fwrite(&tmp2, sizeof(uint8_t), 1, output);
		fwrite(&gameflow->numTotalLevels, sizeof(uint16_t), 1, output);

		uint16_t levelPathStringLen = 0;
		for (auto elem : gameflow->levelPathMap)
		{
			levelPathStringLen += elem.first.size() + 1;
		}
		//		SumPathLengths lengthSum = std::for_each(gameflow->levelPathSet.begin(), gameflow->levelPathSet.end(), SumPathLengths());
		//		uint16_t levelPathStringLen = lengthSum.sum;
		fwrite(&levelPathStringLen, sizeof(uint16_t), 1, output);

		uint16_t levelBlockLen = 0;

		if (gameflow->title != nullptr)
		{
			uint16_t titleBlockLen = Gameflow::GetLevelBlockLength(gameflow->title);
			gameflow->title->blockLength = titleBlockLen;
			gameflow->title->isBlockLenCached = true;

			levelBlockLen += titleBlockLen;
		}

		if (gameflow->levels != nullptr)
		{
			for (LevelData *level : *gameflow->levels)
			{
				uint16_t currentLen = Gameflow::GetLevelBlockLength(level);
				level->blockLength = currentLen;
				level->isBlockLenCached = true;

				levelBlockLen += Gameflow::GetLevelBlockLength(level);
			}
		}

		fwrite(&levelBlockLen, sizeof(uint16_t), 1, output);

		if (gameflow->definitions->PSXLevel != nullptr)
		{
			fputs(gameflow->definitions->PSXLevel, output);
			fputc('\0', output);
		}

		else
		{
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
		}

		if (gameflow->definitions->PSXFmv != nullptr)
		{
			fputs(gameflow->definitions->PSXFmv, output);
			fputc('\0', output);
		}

		else
		{
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
		}

		if (gameflow->definitions->PSXCut != nullptr)
		{
			fputs(gameflow->definitions->PSXCut, output);
			fputc('\0', output);
		}

		else
		{
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
		}

		fputc(0, output);
		fputc(0, output);
		fputc(0, output);
		fputc(0, output);
		fputc(0, output);

		if (gameflow->definitions->PCLevel != nullptr)
		{
			fputs(gameflow->definitions->PCLevel, output);
			fputc('\0', output);
		}

		else
		{
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
		}

		if (gameflow->definitions->PCFmv != nullptr)
		{
			fputs(gameflow->definitions->PCFmv, output);
			fputc('\0', output);
		}

		else
		{
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
		}

		if (gameflow->definitions->PCCut != nullptr)
		{
			fputs(gameflow->definitions->PCCut, output);
			fputc('\0', output);
		}

		else
		{
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
			fputc(0, output);
		}

		fputc(0, output);	//unknown
		fputc(0, output);
		fputc(0, output);
		fputc(0, output);
		fputc(0, output);


		if (gameflow->title != nullptr)
			gameflow->title->pathIndex = 0;	//new 11-7-15

		for (int i = 0; i < GetNumUniqueLevelPaths(); ++i)	//new 11-7-15
		{
			uint16_t tmp = GetPathStringOffset(i);
			fwrite(&tmp, sizeof(uint16_t), 1, output);
		}

		for (int i = 0; i < gameflow->numTotalLevels; ++i)
		{
			int j = 0;
			for (auto pair : gameflow->levelPathMap)
			{
				if (pair.second == i)
				{
					fputs(pair.first.c_str(), output);
					fputc('\0', output);

					break;
				}
				++j;
			}
		}

		for (auto it : *gameflow->levels)
		{
			it->pathIndex = GetPathIndex(it->path);
		}

		uint16_t currentBlockOffset = 0;
		int i = 0;

		if (gameflow->title != nullptr)
		{
			fwrite(&currentBlockOffset, sizeof(uint16_t), 1, output);
			currentBlockOffset += gameflow->title->blockLength;
			i = 1;
		}

		if (gameflow->levels != nullptr)
		{
			for (auto it : *gameflow->levels)	//new 11-7-15
			{
				fwrite(&currentBlockOffset, sizeof(uint16_t), 1, output);
				currentBlockOffset += it->blockLength;
			}
		}

		//main level block writing		-todo: consider function-ising everything into chunk-writing
		i = 0;
		if (gameflow->title != nullptr)
		{
			i = 1;
			fputc(titleOpcode, output);
			fwrite(&gameflow->title->pathIndex, sizeof(uint8_t), 1, output);

			std::bitset<16> titleOptions;
			titleOptions.set(0, gameflow->title->options.YoungLara);
			titleOptions.set(1, gameflow->title->options.Weather);
			titleOptions.set(2, gameflow->title->options.Horizon);
			titleOptions.set(3, gameflow->title->options.Horizon);	//ok
			titleOptions.set(4, gameflow->title->options.Layer2used);
			titleOptions.set(5, gameflow->title->options.Starfield);
			titleOptions.set(6, gameflow->title->options.Lightning);
			titleOptions.set(7, gameflow->title->options.Train);
			titleOptions.set(8, gameflow->title->options.Pulse);
			titleOptions.set(9, gameflow->title->options.ColAddHorizon);
			titleOptions.set(10, gameflow->title->options.ResetHUBused);
			titleOptions.set(11, gameflow->title->options.ColAddHorizon);
			titleOptions.set(12, gameflow->title->options.Timer);
			titleOptions.set(13, gameflow->title->options.Mirrorused);
			titleOptions.set(14, gameflow->title->options.RemoveAmulet);
			titleOptions.set(15, gameflow->title->options.NoLevel);

			uint16_t tmp = static_cast<uint16_t>(titleOptions.to_ulong());
			fwrite(&tmp, sizeof(uint16_t), 1, output);
			fwrite(&gameflow->title->audio, sizeof(uint8_t), 1, output);

			if (gameflow->title->fog != nullptr)
			{
				fputc(fogOpcode, output);
				fwrite(&gameflow->title->fog->red, sizeof(uint8_t), 1, output);
				fwrite(&gameflow->title->fog->green, sizeof(uint8_t), 1, output);
				fwrite(&gameflow->title->fog->blue, sizeof(uint8_t), 1, output);
			}

			if (gameflow->title->layer2 != nullptr)
			{
				fputc(layer2Opcode, output);
				fwrite(&gameflow->title->layer2->red, sizeof(uint8_t), 1, output);
				fwrite(&gameflow->title->layer2->green, sizeof(uint8_t), 1, output);
				fwrite(&gameflow->title->layer2->blue, sizeof(uint8_t), 1, output);
				fwrite(&gameflow->title->layer2->speed, sizeof(int8_t), 1, output);
			}

			if (gameflow->title->layer1 != nullptr)
			{
				fputc(layer1Opcode, output);
				fwrite(&gameflow->title->layer1->red, sizeof(uint8_t), 1, output);
				fwrite(&gameflow->title->layer1->green, sizeof(uint8_t), 1, output);
				fwrite(&gameflow->title->layer1->blue, sizeof(uint8_t), 1, output);
				fwrite(&gameflow->title->layer1->speed, sizeof(int8_t), 1, output);
			}

			if (gameflow->title->lensFlare != nullptr)
			{
				fputc(lensFlareOpcode, output);
				uint16_t yCoordinate = gameflow->title->lensFlare->yClicks / TRClickUnit;
				int16_t zCoordinate = gameflow->title->lensFlare->zClicks / TRClickUnit;
				uint16_t xCoordinate = gameflow->title->lensFlare->xClicks / TRClickUnit;
				fwrite(&yCoordinate, sizeof(uint16_t), 1, output);
				fwrite(&zCoordinate, sizeof(int16_t), 1, output);
				fwrite(&xCoordinate, sizeof(uint16_t), 1, output);
				fwrite(&gameflow->title->lensFlare->red, sizeof(uint8_t), 1, output);
				fwrite(&gameflow->title->lensFlare->green, sizeof(uint8_t), 1, output);
				fwrite(&gameflow->title->lensFlare->blue, sizeof(uint8_t), 1, output);
			}

			if (gameflow->title->loadCam != nullptr)
			{
				fputc(loadCameraOpcode, output);
				fwrite(&gameflow->title->loadCam->srcX, sizeof(int32_t), 1, output);
				fwrite(&gameflow->title->loadCam->srcY, sizeof(int32_t), 1, output);
				fwrite(&gameflow->title->loadCam->srcZ, sizeof(int32_t), 1, output);
				fwrite(&gameflow->title->loadCam->targetX, sizeof(int32_t), 1, output);
				fwrite(&gameflow->title->loadCam->targetY, sizeof(int32_t), 1, output);
				fwrite(&gameflow->title->loadCam->targetZ, sizeof(int32_t), 1, output);
				fwrite(&gameflow->title->loadCam->room, sizeof(uint8_t), 1, output);
			}

			if (gameflow->title->mips != nullptr)
			{
				for (AnimatingMIP *mip : *gameflow->title->mips)
				{
					fputc(animatingMipOpcode, output);
					uint32_t argument = 0;	//todo: try just uint8_t
					argument = (static_cast<uint8_t>(mip->distance) << 4) | ((static_cast<uint8_t>(mip->objIndex - 1)));
					uint8_t tmp = static_cast<uint8_t>(argument);
					fwrite(&tmp, sizeof(uint8_t), 1, output);
				}
			}

			if (gameflow->title->mirrors != nullptr)
			{
				for (Mirror *mirror : *gameflow->title->mirrors)
				{
					fputc(mirrorOpcode, output);
					fwrite(&mirror->room, sizeof(uint8_t), 1, output);
					fwrite(&mirror->xAxis, sizeof(int32_t), 1, output);
				}
			}

			if (gameflow->title->resCuts != nullptr)
			{
				gameflow->title->resCuts->reverse();

				for (ResidentCut *resCut : *gameflow->title->resCuts)
				{
					if (resCut->number == 1)
						fputc(residentCut1Opcode, output);
					else if (resCut->number == 2)
						fputc(residentCut2Opcode, output);
					else if (resCut->number == 3)
						fputc(residentCut3Opcode, output);
					else if (resCut->number == 4)
						fputc(residentCut4Opcode, output);

					fwrite(&resCut->cutIndex, sizeof(uint8_t), 1, output);
				}
			}

			if (gameflow->title->uv != nullptr)
			{
				fputc(UVrotateOpcode, output);
				fwrite(&gameflow->title->uv->speed, sizeof(int8_t), 1, output);
			}

			fputc(levelDataEndOpcode, output);
			//write level-data-end
		}

		if (gameflow->levels != nullptr)
		{
			for (auto level : *gameflow->levels)
			{
				if (level->keys != nullptr)
				{
					for (Key *key : *level->keys)
					{
						if (key->index == 1)
							fputc(key10Opcode, output);
						else if (key->index == 2)
							fputc(key2Opcode, output);
						else if (key->index == 3)
							fputc(key3Opcode, output);
						else if (key->index == 4)
							fputc(key4Opcode, output);
						else if (key->index == 5)
							fputc(key5Opcode, output);
						else if (key->index == 6)
							fputc(key6Opcode, output);
						else if (key->index == 7)
							fputc(key7Opcode, output);
						else if (key->index == 8)
							fputc(key8Opcode, output);
						else if (key->index == 9)
							fputc(key9Opcode, output);
						else if (key->index == 10)
							fputc(key10Opcode, output);
						else if (key->index == 11)
							fputc(key11Opcode, output);
						else if (key->index == 12)
							fputc(key12Opcode, output);

						fwrite(&key->stringIndex, sizeof(uint16_t), 1, output);
						fwrite(&key->appearance.height, sizeof(uint16_t), 1, output);
						fwrite(&key->appearance.size, sizeof(uint16_t), 1, output);
						fwrite(&key->appearance.yAngle, sizeof(uint16_t), 1, output);
						fwrite(&key->appearance.zAngle, sizeof(uint16_t), 1, output);
						fwrite(&key->appearance.xAngle, sizeof(uint16_t), 1, output);
						fwrite(&key->appearance.something, sizeof(uint16_t), 1, output);
					}
				}

				if (level->keyCombos != nullptr)
				{
					for (KeyCombo *keyCombo : *level->keyCombos)
					{
						if (keyCombo->firstIndex == 1 && keyCombo->secondIndex == 1)
							fputc(keyCombo11Opcode, output);
						else if (keyCombo->firstIndex == 1 && keyCombo->secondIndex == 2)
							fputc(keyCombo12Opcode, output);
						else if (keyCombo->firstIndex == 2 && keyCombo->secondIndex == 1)
							fputc(keyCombo21Opcode, output);
						else if (keyCombo->firstIndex == 2 && keyCombo->secondIndex == 2)
							fputc(keyCombo22Opcode, output);
						else if (keyCombo->firstIndex == 3 && keyCombo->secondIndex == 1)
							fputc(keyCombo31Opcode, output);
						else if (keyCombo->firstIndex == 3 && keyCombo->secondIndex == 2)
							fputc(keyCombo32Opcode, output);
						else if (keyCombo->firstIndex == 4 && keyCombo->secondIndex == 1)
							fputc(keyCombo41Opcode, output);
						else if (keyCombo->firstIndex == 4 && keyCombo->secondIndex == 2)
							fputc(keyCombo42Opcode, output);
						else if (keyCombo->firstIndex == 5 && keyCombo->secondIndex == 1)
							fputc(keyCombo51Opcode, output);
						else if (keyCombo->firstIndex == 5 && keyCombo->secondIndex == 2)
							fputc(keyCombo52Opcode, output);
						else if (keyCombo->firstIndex == 6 && keyCombo->secondIndex == 1)
							fputc(keyCombo61Opcode, output);
						else if (keyCombo->firstIndex == 6 && keyCombo->secondIndex == 2)
							fputc(keyCombo62Opcode, output);
						else if (keyCombo->firstIndex == 7 && keyCombo->secondIndex == 1)
							fputc(keyCombo71Opcode, output);
						else if (keyCombo->firstIndex == 7 && keyCombo->secondIndex == 2)
							fputc(keyCombo72Opcode, output);
						else if (keyCombo->firstIndex == 8 && keyCombo->secondIndex == 1)
							fputc(keyCombo81Opcode, output);
						else if (keyCombo->firstIndex == 8 && keyCombo->secondIndex == 2)
							fputc(keyCombo82Opcode, output);

						fwrite(&keyCombo->stringIndex, sizeof(uint16_t), 1, output);
						fwrite(&keyCombo->appearance.height, sizeof(uint16_t), 1, output);
						fwrite(&keyCombo->appearance.size, sizeof(uint16_t), 1, output);
						fwrite(&keyCombo->appearance.yAngle, sizeof(uint16_t), 1, output);
						fwrite(&keyCombo->appearance.zAngle, sizeof(uint16_t), 1, output);
						fwrite(&keyCombo->appearance.xAngle, sizeof(uint16_t), 1, output);
						fwrite(&keyCombo->appearance.something, sizeof(uint16_t), 1, output);
					}
				}

				if (level->puzzles != nullptr)
				{
					for (Puzzle *puzzle : *level->puzzles)
					{
						if (puzzle->index == 1)
							fputc(puzzle1Opcode, output);
						else if (puzzle->index == 2)
							fputc(puzzle2Opcode, output);
						else if (puzzle->index == 3)
							fputc(puzzle3Opcode, output);
						else if (puzzle->index == 4)
							fputc(puzzle4Opcode, output);
						else if (puzzle->index == 5)
							fputc(puzzle5Opcode, output);
						else if (puzzle->index == 6)
							fputc(puzzle6Opcode, output);
						else if (puzzle->index == 7)
							fputc(puzzle7Opcode, output);
						else if (puzzle->index == 8)
							fputc(puzzle8Opcode, output);
						else if (puzzle->index == 9)
							fputc(puzzle9Opcode, output);
						else if (puzzle->index == 10)
							fputc(puzzle10Opcode, output);
						else if (puzzle->index == 11)
							fputc(puzzle11Opcode, output);
						else if (puzzle->index == 12)
							fputc(puzzle12Opcode, output);

						fwrite(&puzzle->stringIndex, sizeof(uint16_t), 1, output);
						fwrite(&puzzle->appearance.height, sizeof(uint16_t), 1, output);
						fwrite(&puzzle->appearance.size, sizeof(uint16_t), 1, output);
						fwrite(&puzzle->appearance.yAngle, sizeof(uint16_t), 1, output);
						fwrite(&puzzle->appearance.zAngle, sizeof(uint16_t), 1, output);
						fwrite(&puzzle->appearance.xAngle, sizeof(uint16_t), 1, output);
						fwrite(&puzzle->appearance.something, sizeof(uint16_t), 1, output);
					}
				}

				if (level->puzzleCombos != nullptr)
				{
					for (PuzzleCombo *puzzleCombo : *level->puzzleCombos)
					{
						if (puzzleCombo->firstIndex == 1 && puzzleCombo->secondIndex == 1)
							fputc(puzzleCombo11Opcode, output);
						else if (puzzleCombo->firstIndex == 1 && puzzleCombo->secondIndex == 2)
							fputc(puzzleCombo12Opcode, output);
						else if (puzzleCombo->firstIndex == 2 && puzzleCombo->secondIndex == 1)
							fputc(puzzleCombo21Opcode, output);
						else if (puzzleCombo->firstIndex == 2 && puzzleCombo->secondIndex == 2)
							fputc(puzzleCombo22Opcode, output);
						else if (puzzleCombo->firstIndex == 3 && puzzleCombo->secondIndex == 1)
							fputc(puzzleCombo31Opcode, output);
						else if (puzzleCombo->firstIndex == 3 && puzzleCombo->secondIndex == 2)
							fputc(puzzleCombo32Opcode, output);
						else if (puzzleCombo->firstIndex == 4 && puzzleCombo->secondIndex == 1)
							fputc(puzzleCombo41Opcode, output);
						else if (puzzleCombo->firstIndex == 4 && puzzleCombo->secondIndex == 2)
							fputc(puzzleCombo42Opcode, output);
						else if (puzzleCombo->firstIndex == 5 && puzzleCombo->secondIndex == 1)
							fputc(puzzleCombo51Opcode, output);
						else if (puzzleCombo->firstIndex == 5 && puzzleCombo->secondIndex == 2)
							fputc(puzzleCombo52Opcode, output);
						else if (puzzleCombo->firstIndex == 6 && puzzleCombo->secondIndex == 1)
							fputc(puzzleCombo61Opcode, output);
						else if (puzzleCombo->firstIndex == 6 && puzzleCombo->secondIndex == 2)
							fputc(puzzleCombo62Opcode, output);
						else if (puzzleCombo->firstIndex == 7 && puzzleCombo->secondIndex == 1)
							fputc(puzzleCombo71Opcode, output);
						else if (puzzleCombo->firstIndex == 7 && puzzleCombo->secondIndex == 2)
							fputc(puzzleCombo72Opcode, output);
						else if (puzzleCombo->firstIndex == 8 && puzzleCombo->secondIndex == 1)
							fputc(puzzleCombo81Opcode, output);
						else if (puzzleCombo->firstIndex == 8 && puzzleCombo->secondIndex == 2)
							fputc(puzzleCombo82Opcode, output);

						fwrite(&puzzleCombo->stringIndex, sizeof(uint16_t), 1, output);
						fwrite(&puzzleCombo->appearance.height, sizeof(uint16_t), 1, output);
						fwrite(&puzzleCombo->appearance.size, sizeof(uint16_t), 1, output);
						fwrite(&puzzleCombo->appearance.yAngle, sizeof(uint16_t), 1, output);
						fwrite(&puzzleCombo->appearance.zAngle, sizeof(uint16_t), 1, output);
						fwrite(&puzzleCombo->appearance.xAngle, sizeof(uint16_t), 1, output);
						fwrite(&puzzleCombo->appearance.something, sizeof(uint16_t), 1, output);
					}
				}

				if (level->pickups != nullptr)
				{
					for (Pickup *pickup : *level->pickups)
					{
						if (pickup->index == 1)
							fputc(pickup1Opcode, output);
						else if (pickup->index == 2)
							fputc(pickup2Opcode, output);
						else if (pickup->index == 3)
							fputc(pickup3Opcode, output);
						else if (pickup->index == 4)
							fputc(pickup4Opcode, output);

						fwrite(&pickup->stringIndex, sizeof(uint16_t), 1, output);
						fwrite(&pickup->appearance.height, sizeof(uint16_t), 1, output);
						fwrite(&pickup->appearance.size, sizeof(uint16_t), 1, output);
						fwrite(&pickup->appearance.yAngle, sizeof(uint16_t), 1, output);
						fwrite(&pickup->appearance.zAngle, sizeof(uint16_t), 1, output);
						fwrite(&pickup->appearance.xAngle, sizeof(uint16_t), 1, output);
						fwrite(&pickup->appearance.something, sizeof(uint16_t), 1, output);
					}
				}

				if (level->pickupCombos != nullptr)
				{
					for (PickupCombo *pickupCombo : *level->pickupCombos)
					{
						if (pickupCombo->firstIndex == 1 && pickupCombo->secondIndex == 1)
							fputc(pickupCombo11Opcode, output);
						else if (pickupCombo->firstIndex == 1 && pickupCombo->secondIndex == 2)
							fputc(pickupCombo12Opcode, output);
						else if (pickupCombo->firstIndex == 2 && pickupCombo->secondIndex == 1)
							fputc(pickupCombo21Opcode, output);
						else if (pickupCombo->firstIndex == 2 && pickupCombo->secondIndex == 2)
							fputc(pickupCombo22Opcode, output);
						else if (pickupCombo->firstIndex == 3 && pickupCombo->secondIndex == 1)
							fputc(pickupCombo31Opcode, output);
						else if (pickupCombo->firstIndex == 3 && pickupCombo->secondIndex == 2)
							fputc(pickupCombo32Opcode, output);
						else if (pickupCombo->firstIndex == 4 && pickupCombo->secondIndex == 1)
							fputc(pickupCombo41Opcode, output);
						else if (pickupCombo->firstIndex == 4 && pickupCombo->secondIndex == 2)
							fputc(pickupCombo42Opcode, output);

						fwrite(&pickupCombo->stringIndex, sizeof(uint16_t), 1, output);
						fwrite(&pickupCombo->appearance.height, sizeof(uint16_t), 1, output);
						fwrite(&pickupCombo->appearance.size, sizeof(uint16_t), 1, output);
						fwrite(&pickupCombo->appearance.yAngle, sizeof(uint16_t), 1, output);
						fwrite(&pickupCombo->appearance.zAngle, sizeof(uint16_t), 1, output);
						fwrite(&pickupCombo->appearance.xAngle, sizeof(uint16_t), 1, output);
						fwrite(&pickupCombo->appearance.something, sizeof(uint16_t), 1, output);
					}
				}

				if (level->examines != nullptr)
				{
					for (Examine *examine : *level->examines)
					{
						if (examine->index == 1)
							fputc(examine1Opcode, output);
						else if (examine->index == 2)
							fputc(examine2Opcode, output);
						else if (examine->index == 3)
							fputc(examine3Opcode, output);

						fwrite(&examine->stringIndex, sizeof(uint16_t), 1, output);
						fwrite(&examine->appearance.height, sizeof(uint16_t), 1, output);
						fwrite(&examine->appearance.size, sizeof(uint16_t), 1, output);
						fwrite(&examine->appearance.yAngle, sizeof(uint16_t), 1, output);
						fwrite(&examine->appearance.zAngle, sizeof(uint16_t), 1, output);
						fwrite(&examine->appearance.xAngle, sizeof(uint16_t), 1, output);
						fwrite(&examine->appearance.something, sizeof(uint16_t), 1, output);
					}
				}

				if (level->fmvs != nullptr)
				{
					level->fmvs->reverse();

					for (FMV *fmv : *level->fmvs)
					{
						fputc(FMVOpcode, output);
						uint32_t argument;
						std::bitset<4> triggerTimer;

						triggerTimer.set(3, fmv->triggerTimer & 1);
						triggerTimer.set(2, fmv->triggerTimer & 2);
						triggerTimer.set(1, fmv->triggerTimer & 4);
						triggerTimer.set(0, fmv->triggerTimer & 8);
						argument = (static_cast<uint8_t>(triggerTimer.to_ulong()) << 4) | (static_cast<uint8_t>(fmv->index));
						uint8_t tmp = static_cast<uint8_t>(argument);
						fwrite(&tmp, sizeof(uint8_t), 1, output);
					}
				}

				if (level->fog != nullptr)
				{
					fputc(fogOpcode, output);
					fwrite(&level->fog->red, sizeof(uint8_t), 1, output);
					fwrite(&level->fog->green, sizeof(uint8_t), 1, output);
					fwrite(&level->fog->blue, sizeof(uint8_t), 1, output);
				}

				if (level->mips != nullptr)
				{
					for (AnimatingMIP *mip : *level->mips)
					{
						fputc(animatingMipOpcode, output);
						uint32_t argument = (static_cast<uint8_t>(mip->distance) << 4) | ((static_cast<uint8_t>(mip->objIndex - 1)));
						uint8_t tmp = static_cast<uint8_t>(argument);
						fwrite(&tmp, sizeof(uint8_t), 1, output);
					}
				}

				if (level->legend != nullptr)
				{
					fputc(legendOpcode, output);
					fwrite(&level->legend->stringIndex, sizeof(uint8_t), 1, output);
				}

				if (level->resCuts != nullptr)
				{
					for (ResidentCut *resCut : *level->resCuts)
					{
						if (resCut->number == 1)
							fputc(residentCut1Opcode, output);
						else if (resCut->number == 2)
							fputc(residentCut2Opcode, output);
						else if (resCut->number == 3)
							fputc(residentCut3Opcode, output);
						else if (resCut->number == 4)
							fputc(residentCut4Opcode, output);

						fwrite(&resCut->cutIndex, sizeof(uint8_t), 1, output);
					}
				}

				if (level->cut != nullptr)
				{
					fputc(cutOpcode, output);
					fwrite(&level->cut->cutIndex, sizeof(uint8_t), 1, output);
				}

				if (level->layer1 != nullptr)
				{
					fputc(layer1Opcode, output);
					fwrite(&level->layer1->red, sizeof(uint8_t), 1, output);
					fwrite(&level->layer1->green, sizeof(uint8_t), 1, output);
					fwrite(&level->layer1->blue, sizeof(uint8_t), 1, output);
					fwrite(&level->layer1->speed, sizeof(int8_t), 1, output);
				}

				if (level->layer2 != nullptr)
				{
					fputc(layer2Opcode, output);
					fwrite(&level->layer2->red, sizeof(uint8_t), 1, output);
					fwrite(&level->layer2->green, sizeof(uint8_t), 1, output);
					fwrite(&level->layer2->blue, sizeof(uint8_t), 1, output);
					fwrite(&level->layer2->speed, sizeof(int8_t), 1, output);
				}

				if (level->lensFlare != nullptr)
				{
					fputc(lensFlareOpcode, output);
					uint16_t yCoordinate = level->lensFlare->yClicks / TRClickUnit;
					int16_t zCoordinate = level->lensFlare->zClicks / TRClickUnit;
					uint16_t xCoordinate = level->lensFlare->xClicks / TRClickUnit;

					fwrite(&yCoordinate, sizeof(uint16_t), 1, output);
					fwrite(&zCoordinate, sizeof(int16_t), 1, output);
					fwrite(&xCoordinate, sizeof(uint16_t), 1, output);
					fwrite(&level->lensFlare->red, sizeof(uint8_t), 1, output);
					fwrite(&level->lensFlare->green, sizeof(uint8_t), 1, output);
					fwrite(&level->lensFlare->blue, sizeof(uint8_t), 1, output);
				}

				if (level->mirrors != nullptr)
				{
					for (Mirror *mirror : *level->mirrors)
					{
						fputc(mirrorOpcode, output);
						fwrite(&mirror->room, sizeof(uint8_t), 1, output);
						fwrite(&mirror->xAxis, sizeof(int32_t), 1, output);
					}
				}

				if (level->loadCam != nullptr)
				{
					fputc(loadCameraOpcode, output);
					fwrite(&level->loadCam->srcX, sizeof(int32_t), 1, output);
					fwrite(&level->loadCam->srcY, sizeof(int32_t), 1, output);
					fwrite(&level->loadCam->srcZ, sizeof(int32_t), 1, output);

					fwrite(&level->loadCam->targetX, sizeof(int32_t), 1, output);
					fwrite(&level->loadCam->targetY, sizeof(int32_t), 1, output);
					fwrite(&level->loadCam->targetZ, sizeof(int32_t), 1, output);
					fwrite(&level->loadCam->room, sizeof(uint8_t), 1, output);
				}

				if (level->uv != nullptr)
				{
					fputc(UVrotateOpcode, output);
					fwrite(&level->uv->speed, sizeof(int8_t), 1, output);
				}

				if (level->resetHUB != nullptr)
				{
					fputc(resetHubOpcode, output);
					fwrite(&level->resetHUB->levelIndex, sizeof(uint8_t), 1, output);
				}

				fputc(levelOpcode, output);
				fwrite(&level->stringIndex, sizeof(uint8_t), 1, output);

				std::bitset<16> levelOptions;
				levelOptions.set(0, level->options.YoungLara);
				levelOptions.set(1, level->options.Weather);
				levelOptions.set(2, level->options.Horizon);
				levelOptions.set(3, level->options.Horizon);	//ok
				levelOptions.set(4, level->options.Layer2used);
				levelOptions.set(5, level->options.Starfield);
				levelOptions.set(6, level->options.Lightning);
				levelOptions.set(7, level->options.Train);
				levelOptions.set(8, level->options.Pulse);
				levelOptions.set(9, level->options.ColAddHorizon);
				levelOptions.set(10, level->options.ResetHUBused);
				levelOptions.set(11, level->options.ColAddHorizon);
				levelOptions.set(12, level->options.Timer);
				levelOptions.set(13, level->options.Mirrorused);
				levelOptions.set(14, level->options.RemoveAmulet);
				levelOptions.set(15, level->options.NoLevel);

				uint16_t tmp = static_cast<uint16_t>(levelOptions.to_ulong());
				fwrite(&tmp, sizeof(uint16_t), 1, output);
				fwrite(&level->pathIndex, sizeof(uint8_t), 1, output);
				fwrite(&level->audio, sizeof(uint8_t), 1, output);

				fputc(levelDataEndOpcode, output);
			}
		}

		//write language filenames
		char *fname = StringHelper::ToUpper(StringHelper::ReplaceThreeLetterExtension(gameflow->primaryLanguage->langName, "dat"));
		fputs(fname, output);
		fputc('\0', output);
		delete[] fname;

		for (int i = 1; i < gameflow->numLanguages; ++i)
		{
			for (auto it : *gameflow->languages)
			{
				if (it->index == i)
				{
					char *fname = StringHelper::ToUpper(StringHelper::ReplaceThreeLetterExtension(it->langName, "dat"));
					//char *fname = StringHelper::ReplaceThreeLetterExtension(it->langName, "dat");
					fputs(fname, output);
					fputc('\0', output);
					delete[] fname;
				}
			}
		}

		fclose(output);
		created = true;	//new 13-7-15
	}
