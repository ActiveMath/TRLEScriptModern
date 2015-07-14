#ifndef STRINGHELPER_H_INCLUDED
#define STRINGHELPER_H_INCLUDED

class StringHelper
{
public:
	static char *ToUpper(char *string);
	static char *ToLower(char *string);
	//static void ToLower(char *string);
	static bool IsStatement(const char *cmd, const char *str);
	static char *ReplaceLiteralWhitespace(const char *str);
	static char *ReplaceLiteralHexCode(const char *str);
	static char *ReplaceHexDollars(const char *str);
	static char *IsolateStringBetweenCommas(int firstComma, int secondComma, const char *str);
	static char *ExtractFilenameWithoutExtension(const char *filename);
	static char *ReplaceThreeLetterExtension(char *filename, const char *newExtension);
	static int GetCurrentLineNumber(const char *startPos, const char *currentPos);
};


#endif // STRINGHELPER_H_INCLUDED
