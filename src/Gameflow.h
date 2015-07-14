#ifndef GAMEFLOW_H_INCLUDED
#define GAMEFLOW_H_INCLUDED

#include "ScriptFile.h"
#include <cstdint>
#include <forward_list>
#include <unordered_set>
#include <cstdint>
#include <utility>
#include <string>
#include <stack>

namespace TRLEScript
{

	class Gameflow :
		public ScriptFile
	{
	public:
		Gameflow(const char *filename);
		virtual ~Gameflow();

		void Parse() override;
		void CompileToDAT(const char *data) override;
		const GameflowScriptHeader *GetGameflowScript() const;
		std::stack<std::string> GetLanguages();// const;
		static std::unordered_set<int> *Get1ToNSet(const int n);

		static uint16_t GetLevelBlockLength(const LevelData *level);
		static uint16_t GetLevelBlockLength(const TitleLevelData *level);
		//char *Create(const char *data, int length);

	private:
		uint16_t GetPathStringOffset(int lastIndex);
		uint16_t GetNumUniqueLevelPaths() const;
		//uint8_t GetNumUniqueLevelPaths() const;
		int GetPathIndex(const char *path);// const;
		bool created;

		static const uint16_t TRClickUnit = 256;

		static const int levelOpcode = 0x81;
		static const int titleOpcode = 0x82;

		static const int legendOpcode = 0x8c;
		static const int loadCameraOpcode = 0x91;
		static const int layer1Opcode = 0x89;
		static const int layer2Opcode = 0x8a;
		static const uint8_t mirrorOpcode = 0x8e;

		static const int key1Opcode = 0x93;
		static const int key2Opcode = 0x94;
		static const int key3Opcode = 0x95;
		static const int key4Opcode = 0x96;
		static const int key5Opcode = 0x97;
		static const int key6Opcode = 0x98;
		static const int key7Opcode = 0x99;
		static const int key8Opcode = 0x9a;
		static const int key9Opcode = 0x9b;
		static const int key10Opcode = 0x9c;
		static const int key11Opcode = 0x9d;
		static const int key12Opcode = 0x9e;

		static const int puzzle1Opcode = 0x9f;
		static const int puzzle2Opcode = 0xa0;
		static const int puzzle3Opcode = 0xa1;
		static const int puzzle4Opcode = 0xa2;
		static const int puzzle5Opcode = 0xa3;
		static const int puzzle6Opcode = 0xa4;
		static const int puzzle7Opcode = 0xa5;
		static const int puzzle8Opcode = 0xa6;
		static const int puzzle9Opcode = 0xa7;
		static const int puzzle10Opcode = 0xa8;
		static const int puzzle11Opcode = 0xa9;
		static const int puzzle12Opcode = 0xaa;

		static const int pickup1Opcode = 0xab;
		static const int pickup2Opcode = 0xac;
		static const int pickup3Opcode = 0xad;
		static const int pickup4Opcode = 0xae;

		static const int keyCombo11Opcode = 0xb2;
		static const int keyCombo12Opcode = 0xb3;
		static const int keyCombo21Opcode = 0xb4;
		static const int keyCombo22Opcode = 0xb5;
		static const int keyCombo31Opcode = 0xb6;
		static const int keyCombo32Opcode = 0xb7;
		static const int keyCombo41Opcode = 0xb8;
		static const int keyCombo42Opcode = 0xb9;
		static const int keyCombo51Opcode = 0xba;
		static const int keyCombo52Opcode = 0xbb;
		static const int keyCombo61Opcode = 0xbc;
		static const int keyCombo62Opcode = 0xbd;
		static const int keyCombo71Opcode = 0xbe;
		static const int keyCombo72Opcode = 0xbf;
		static const int keyCombo81Opcode = 0xc0;
		static const int keyCombo82Opcode = 0xc1;
		
		static const int puzzleCombo11Opcode = 0xc2;
		static const int puzzleCombo12Opcode = 0xc3;
		static const int puzzleCombo21Opcode = 0xc4;
		static const int puzzleCombo22Opcode = 0xc5;
		static const int puzzleCombo31Opcode = 0xc6;
		static const int puzzleCombo32Opcode = 0xc7;
		static const int puzzleCombo41Opcode = 0xc8;
		static const int puzzleCombo42Opcode = 0xc9;
		static const int puzzleCombo51Opcode = 0xca;
		static const int puzzleCombo52Opcode = 0xcb;
		static const int puzzleCombo61Opcode = 0xcc;
		static const int puzzleCombo62Opcode = 0xcd;
		static const int puzzleCombo71Opcode = 0xce;
		static const int puzzleCombo72Opcode = 0xcf;
		static const int puzzleCombo81Opcode = 0xd0;
		static const int puzzleCombo82Opcode = 0xd1;

		static const int pickupCombo11Opcode = 0xd2;
		static const int pickupCombo12Opcode = 0xd3;
		static const int pickupCombo21Opcode = 0xd4;
		static const int pickupCombo22Opcode = 0xd5;
		static const int pickupCombo31Opcode = 0xd6;
		static const int pickupCombo32Opcode = 0xd7;
		static const int pickupCombo41Opcode = 0xd8;
		static const int pickupCombo42Opcode = 0xd9;

		static const int examine1Opcode = 0xaf;
		static const int examine2Opcode = 0xb0;
		static const int examine3Opcode = 0xb1;

		static const int fogOpcode = 0x8f;
		static const int UVrotateOpcode = 0x8b;
		static const int cutOpcode = 0x84;

		static const int residentCut1Opcode = 0x85;
		static const int residentCut2Opcode = 0x86;
		static const int residentCut3Opcode = 0x87;
		static const int residentCut4Opcode = 0x88;

		static const int FMVOpcode = 0x80;
		static const int resetHubOpcode = 0x92;
		static const int animatingMipOpcode = 0x90;
		static const int lensFlareOpcode = 0x8d;
		static const int levelDataEndOpcode = 0x83;

		static const uint16_t levelBlockLength = 7;
		static const uint16_t titleBlockLength = 6;

		static const uint16_t legendLength = 2;
		static const uint16_t loadCameraLength = 26;
		static const uint16_t layer1Length = 5;
		static const uint16_t layer2Length = 5;
		static const uint16_t mirrorLength = 6;

		static const uint16_t keyLength = 15;
		static const uint16_t puzzleLength = 15;
		static const uint16_t pickupLength = 15;
		static const uint16_t keyComboLength = 15;
		static const uint16_t puzzleComboLength = 15;
		static const uint16_t pickupComboLength = 15;
		static const uint16_t examineLength = 15;

		static const uint16_t fogLength = 4;
		static const uint16_t UVrotateLength = 2;
		static const uint16_t cutLength = 2;
		static const uint16_t residentCutLength = 2;
		static const uint16_t FMVLength = 2;
		static const uint16_t resetHubLength = 2;
		static const uint16_t animatingMipLength = 2;
		static const uint16_t lensFlareLength = 10;
	};
}


#endif // GAMEFLOW_H_INCLUDED
