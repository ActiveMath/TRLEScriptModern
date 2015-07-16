#ifndef SCRIPTFILE_H_INCLUDED
#define SCRIPTFILE_H_INCLUDED

#include "Notifier.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <forward_list>
#include <stack>
#include <unordered_set>
#include <vector>
//#include <utility>

namespace TRLEScript
{

    class LanguageScript;

	enum class ParseMode
	{
		Undefined,
		PSXExtensions,
		PCExtensions,
		Language,
		Options,
		Title,
		Level,
		Strings,
		PSXStrings,
		PCStrings
	};

	enum class MacroType
	{
		None,
		Include,
		Define
	};

	struct PlatformDefinitions
	{
		char *PSXLevel;
		char *PSXCut;
		char *PSXFmv;

		char *PCLevel;
		char *PCCut;
		char *PCFmv;
	};

	struct LanguageFilename
	{
		char *langName;
		int index;
	};

	struct Options
	{
		bool LoadSave = true;
		bool Title = true;
		bool PlayAnyLevel = false;
		int InputTimeout = 18000;
		bool FlyCheat = false;
		uint8_t Security = 0x55;
		bool DemoDisc = false;
	};

	struct LoadCamera
	{
		int32_t srcX, srcY, srcZ, targetX, targetY, targetZ;
		uint8_t room;
	};

	struct LevelOptions
	{
		bool YoungLara, Weather, Horizon, Layer2used, Starfield, Lightning, Train, Pulse, ColAddHorizon, ResetHUBused, Timer, Mirrorused, RemoveAmulet, NoLevel;
		//bool LoadCamUsed, Layer1Used, LegendUsed, FogUsed, UVUsed, CutUsed, ResidentUsed, FMVUsed, LensFlareUsed;	//auxiliary
	};

	struct Level
	{
		uint8_t stringIndex;
		LevelOptions options;
		//uint16_t bitfield;
		uint8_t pathIndex;
		uint8_t audio;
	};

	struct Legend
	{
		//char *string;
		uint8_t stringIndex;
	};

	struct Layer1
	{
		uint8_t red, green, blue;
		int8_t speed;
	};

	struct Layer2
	{
		uint8_t red, green, blue;
		int8_t speed;
	};

	struct Mirror
	{
		uint8_t room;
		int32_t xAxis;
	};

	struct Fog
	{
		uint8_t red, green, blue;
	};

	struct UVRotate
	{
		int8_t speed;
	};

	struct Cut
	{
		uint8_t cutIndex;
	};

	struct ResidentCut
	{
		int number;	//1-4
		uint8_t cutIndex;
	};

	struct FMV
	{
		int index;
		uint8_t triggerTimer;
		//int triggerTimer;
	};

	struct ResetHUB
	{
		uint8_t levelIndex;
	};

	struct AnimatingMIP
	{
		int objIndex;
		int distance;
	};

	struct LensFlare
	{
		uint32_t xClicks, yClicks;
		int32_t zClicks;
		uint8_t red, green, blue;
	};

	struct ItemModifiers
	{
		uint16_t height, size, yAngle, zAngle, xAngle, something;
	};

	struct PuzzleCombo
	{
		int firstIndex, secondIndex;
		//char *name;
		uint16_t stringIndex;//?
		ItemModifiers appearance;
	};

	struct Puzzle
	{
		int index;
		//char *name;
		uint16_t stringIndex;
		ItemModifiers appearance;
	};

	struct Examine
	{
		int index;	//1-3
		//char *name;
		uint16_t stringIndex;
		ItemModifiers appearance;
	};

	struct PickupCombo
	{
		int firstIndex, secondIndex;
		uint16_t stringIndex;
		ItemModifiers appearance;
	};

	struct Pickup
	{
		int index;	//1-4
		//char *name;
		uint16_t stringIndex;
		ItemModifiers appearance;
	};

	struct KeyCombo
	{
		int firstIndex, secondIndex;
		uint16_t stringIndex;
		ItemModifiers appearance;
	};

	struct Key
	{
		int index;	//1-12
		//char *name;
		uint16_t stringIndex;
		ItemModifiers appearance;
	};

	struct TitleLevelData
	{
		bool isBlockLenCached = false;
		uint16_t blockLength;

		uint8_t pathIndex;
		char *path;
		LevelOptions options;
		uint8_t audio;

		LoadCamera *loadCam;
		LensFlare *lensFlare;
		int numMIPs;
		std::forward_list<AnimatingMIP *> *mips;
		UVRotate *uv;
		Layer1 *layer1;
		Layer2 *layer2;
		int numMirrors = 0;
		std::forward_list<Mirror *> *mirrors;
		Fog *fog;

		int numResCuts = 0;
		std::forward_list<ResidentCut *> *resCuts;
	};

	struct LevelData
	{
		bool isBlockLenCached = false;
		uint16_t blockLength;

		uint8_t stringIndex;
		uint8_t pathIndex;
		char *path;
		//char *name;

		LevelOptions options;
		uint8_t audio;

		LoadCamera *loadCam;
		LensFlare *lensFlare;
		int numMIPs;
		std::forward_list<AnimatingMIP *> *mips;
		UVRotate *uv = nullptr;
		Layer1 *layer1 = nullptr;
		Layer2 *layer2 = nullptr;
		int numMirrors;
		std::forward_list<Mirror *> *mirrors;
		Fog *fog;
		Legend *legend = nullptr;
		Cut *cut;
		int numFMVs;
		std::forward_list<FMV *> *fmvs;
		//FMV fmv;
		int numResCuts;
		std::forward_list<ResidentCut *> *resCuts;
		ResetHUB *resetHUB;

		int numPuzzleCombos = 0;
		std::forward_list<PuzzleCombo *> *puzzleCombos = nullptr;

		int numPickupCombos = 0;
		std::forward_list<PickupCombo *> *pickupCombos = nullptr;

		int numKeyCombos = 0;
		std::forward_list<KeyCombo *> *keyCombos = nullptr;

		int numPuzzles = 0;
		std::forward_list<Puzzle *> *puzzles = nullptr;

		int numExamines = 0;
		std::forward_list<Examine *> *examines = nullptr;

		int numPickups = 0;
		std::forward_list<Pickup *> *pickups = nullptr;

		int numKeys = 0;
		std::forward_list<Key *> *keys = nullptr;
	};

	struct LanguageString
	{
		char *string;
		uint16_t stringLength;
		int index;
	};

	struct GameflowScriptHeader
	{
		int numStrings = 0;
		int numPSXStrings = 0;
		int numPCStrings = 0;

		std::forward_list<LanguageString *> *Strings = nullptr;
		std::forward_list<LanguageString *> *PSXStrings = nullptr;
		std::forward_list<LanguageString *> *PCStrings = nullptr;

		//std::unordered_map<int, std::string> levelPathMap;

		//std::unordered_set<std::pair<std::string, int>> levelPathSet;
		//std::unordered_set<std::string> levelPathSet;



		//std::unordered_map<std::string, int> pathIndices;

		std::unordered_map<std::string, int> levelPathMap;

		uint16_t levelPathStringLen = 0;
		uint16_t levelBlockLen = 0;

		Options *options = nullptr;
		uint16_t numTotalLevels;
		uint8_t numLevelPaths;

		uint16_t levelPathsLen;
		//uint16_t levelBlockLen;

		PlatformDefinitions *definitions = nullptr;

		int numLanguages;
		std::forward_list<LanguageFilename *> *languages = nullptr;
		LanguageFilename *primaryLanguage = nullptr;
		LanguageScript *primaryLanguageScript = nullptr;			//new 12-7-15

		bool titleUsed;
		TitleLevelData *title;

		std::forward_list<LevelData *> *levels = nullptr;
	};

	class ScriptFile : public Notifier
	{
	public:
		ScriptFile(const char* filename);
		virtual ~ScriptFile();

		virtual void CompileToDAT(const char *data) = 0;
		virtual void Parse() = 0;
		virtual const GameflowScriptHeader *GetGameflowScript() const = 0;

		const LanguageScript *GetPrimaryLanguageScript() const;

		ScriptFile& operator= (const ScriptFile& a);
		ScriptFile(const ScriptFile& a);

		ScriptFile(ScriptFile&&);
		ScriptFile& operator= (ScriptFile&&);

	protected:
		GameflowScriptHeader *gameflowScript = nullptr;
		PlatformDefinitions *definitions = nullptr;
		Options *globalOptions = nullptr;
		LanguageScript *primaryLanguage = nullptr;

		virtual GameflowScriptHeader* SourceParse(const char *data);
		virtual void PreprocessorPass(const char *data);
		bool isLanguageScript;
		bool isGameflowScript;
		bool compilingToDAT;
		char *filename = nullptr;

	private:

		bool InterpretLanguageStrings(int &stringIndex, char *token, GameflowScriptHeader *gameflow, ParseMode mode, int lineNumber);
		void ClearArgumentStack();

		std::vector<char> PreprocessorIncludePass(const char *);
		std::vector<char> PreprocessorDefinePass(const char *);
		//std::vector<char> PreprocessorIncludePass(std::vector<char>);
	//	std::vector<char> PreprocessorDefinePass(std::vector<char>);

		std::string EvaluateMacro(std::unordered_map<std::string, std::string> &vData, const std::string const &toReplace);
		//std::string EvaluateMacro(std::vector<std::pair<std::string, std::string>> &vData, std::string &toReplace);

		std::stack<char *> *ParseArguments(char *token);

		char *StripTrailingLeadingWhitespace(const char *string);
		char *StripTrailingLeadingChar(const char *string, char c);
		char *ParseLanguageString(const char *token);
		char *ParseString(const char *token);
		long int ParseNumber(const char *token);
		bool ParseBool(const char *token);
		void DefaultTitle(TitleLevelData *title);
		void DefaultLevel(LevelData *level);

		std::vector<char> preprocessedData;
		static int MacroPointerCompare(const void *, const void*);
		void MacroSubstitute(std::unordered_map<std::string, std::string> &vMacro, std::vector<char> &definedData, const char *& lastPos, const char *endPos);
		//void MacroSubstitute(std::vector<std::pair<std::string, std::string>> &vMacro, std::vector<char> &definedData, const char *& lastPos, const char *endPos);

		char *command = nullptr;
		char *wholeLine = nullptr;	//new 13-7-15
		char *line = nullptr;
		std::stack<char *> *argStack = nullptr;

		void PlotNumberExpectedError(const char *startPos, const char *currentPos);
		void PlotOutOfContextError(const char *startPos, const char *currentPos);
		void PlotOutOfContextError(int lineNumber);
		bool ArgumentsContainNumber(const char *startPos, const char *currentPos);
		bool LineContainsNumber(const char *startPos, const char *currentPos);
		void AddNewWarning(const char *text, int lineNumber);
		void AddNewError(const char *text, int lineNumber);

		void AddNewWarning(const char *text);
		void AddNewError(const char *text);

		void AddNewWarning(std::string text, int lineNumber);
		void AddNewError(std::string text, int lineNumber);

		void AddNewWarning(std::string text);
		void AddNewError(std::string text);

		void Clear();
		void Copy(const ScriptFile& a);

	//private:
		/*GameflowScriptHeader* SourceParse(const char *data, int length);
		std::stack<char *, std::forward_list<char *>> *ParseArguments(const char *token);
		char *StripTrailingWhitespace(const char *string);
		char *ParseLanguageString(const char *token);
		char *ParseString(const char *token);
		long int ParseNumber(const char *token);
		bool ParseBool(const char *token);*/
	};

}

//#endif


#endif // SCRIPTFILE_H_INCLUDED
