#include "StringHelper.h"
#include <cstdlib>
#include <cstring>
#include <locale>
#include <cctype>

char *StringHelper::ToUpper(char *string)
{
	int length = strlen(string);
	std::locale loc;

	for (int i = 0; i < length; ++i)
		string[i] = std::toupper(string[i], loc);

	return string;
}

char *StringHelper::ToLower(char *string)
{
	int length = strlen(string);
	std::locale loc;

	for (int i = 0; i < length; ++i)
		string[i] = std::tolower(string[i], loc);

	return string;
}

//A helper function used in parsing the script commands. E.g. Name=... - "Name" is a command, but since Core's script compiler isn't case sensitive, this function ignores cases
//and also ignores trailing whitespace. Return value: true (if the strings are equivalent given the aforementioned exceptions) or false otherwise
bool StringHelper::IsStatement(const char *cmd, const char *str)
{
	//int ret;
	std::locale loc;

	const char *startPos = str;
	while (isblank(*startPos))
		startPos++;

	const char *endPos = str + strlen(str);
	while (isblank(*endPos))
	//while (*endPos == ' ' || *startPos == '\v' || *startPos == '\t')
		endPos--;

	const char *ptr1 = cmd;
	const char *ptr2 = startPos;

	while (*ptr1 != '\0')
	{
		if (std::tolower(*ptr1, loc) == std::tolower(*ptr2, loc))
		{
			ptr1++;
			ptr2++;
		}

		else
			return false;
	}

	return true;
}

char *StringHelper::ReplaceLiteralWhitespace(const char *str)
{
	int nNewlines = 0;
	int nTabs = 0;

	const char *ptr1 = str;
	while (*ptr1 != '\0')
	{
		if (*ptr1 == '\\' && *(ptr1 + 1) == 'n')
		{
			++nNewlines;
			ptr1 += 2;
		}

		else if (*ptr1 == '\\' && *(ptr1 + 1) == 't')
		{
			++nTabs;
			ptr1 += 2;
		}

		else
			++ptr1;
	}

	char *replaced = new char[strlen(str) - nNewlines - nTabs + 1];
	if (nNewlines == nTabs && nTabs == 0)
	{
		strcpy(replaced, str);
		return replaced;
	}

	char *ptr2 = replaced;

	ptr1 = str;
	while (*ptr1 != '\0')
	{
		if (*ptr1 == '\\' && *(ptr1 + 1) == 'n')
		{
			*ptr2 = '\n';
			++ptr1;
		}

		else if (*ptr1 == '\\' && *(ptr1 + 1) == 't')
		{
			*ptr2 = '\t';
			++ptr1;
		}

		else
			*ptr2 = *ptr1;

		++ptr2;
		++ptr1;
	}
	*ptr2 = '\0';

	return replaced;
}

char *StringHelper::ReplaceLiteralHexCode(const char *str)
{
	int nTokens = 0;

	const char *ptr1 = strstr(str, "\\x");
	while (ptr1 != nullptr)
	{
		//ptr = strstr(str, "\\x");
		//*ptr = '0';
		++nTokens;
		ptr1 = strstr(ptr1 + 2, "\\x");
	}

	char *replaced = new char[strlen(str) - 3 * nTokens + 1];
	if (nTokens == 0)
	{
		strcpy(replaced, str);
		return replaced;
	}

	char *ptr2 = replaced;
	ptr1 = str;
	const char *pos = strstr(str, "\\x");
	//ptr1 = strstr(str, "\\x");

	int i = 0;	//new 11-7-15 just a test
	while (pos != nullptr)
	{
		while (ptr1 != pos)
		{
			*ptr2 = *ptr1;
			++ptr2;
			++ptr1;
		}

		char numberString[2];
		numberString[0] = *(ptr1 + 2);
		numberString[1] = *(ptr1 + 3);
		char number = (char)strtol(numberString, nullptr, 16);
		*ptr2 = number;

		pos = strstr(pos + 2, "\\x");
		ptr1 += 4;
		++ptr2;

		++i;
	}

	while (*ptr1 != '\0')
	{
		*ptr2 = *ptr1;
		++ptr2;
		++ptr1;
	}

	*ptr2 = '\0';
	return replaced;
}

char *StringHelper::ReplaceHexDollars(const char *str)
{
	int nDollars = 0;
	int len = 0;

	const char *ptr1 = str;
	while (*ptr1 != '\0')
	{
		if (*ptr1 == '$')
			++nDollars;

		++ptr1;
		++len;
	}

	char *replaced = new char[nDollars + len + 1];
	if (nDollars == 0)
	{
		strcpy(replaced,str);
		return replaced;
	}

	char *ptr2 = replaced;

	ptr1 = str;
	while (*ptr1 != '\0')
	{
		if (*ptr1 == '$')
		{
			*ptr2 = '0';
			++ptr2;
			*ptr2 = 'x';
		}

		else
			*ptr2 = *ptr1;
		++ptr2;
		++ptr1;
	}

	*ptr2 = '\0';

	return replaced;
}

char *StringHelper::IsolateStringBetweenCommas(int firstComma, int secondComma, const char *str)
{
	int nCommas = 0;
	const char *startPos;
	const char *endPos;

	const char *ptr = str;
	while (nCommas < firstComma && *ptr != '\0')
	{
		ptr = strpbrk(ptr, ",");
		if (ptr != nullptr)
		{
			++nCommas;
			++ptr;
		}
	}

	startPos = ptr;

	ptr = str + strlen(str);
	nCommas = 0;
	while (nCommas < secondComma && ptr != startPos)
	{
		if (*ptr == ',')
			++nCommas;

		ptr--;
	}

	endPos = ptr + 1;

	ptrdiff_t length = endPos - startPos;
	//int length = endPos - startPos;
	char *out = new char[length + 1];
	strncpy(out, startPos, length);
	out[length] = '\0';

	return out;
}

char *StringHelper::ExtractFilenameWithoutExtension(const char *filename)
{
	if (strpbrk(filename, ".") == nullptr)
	{
		char *out = new char[strlen(filename) + 1];
		strcpy(out, filename);
		out[strlen(filename)] = '\0';
		return out;
	}

	const char *ptr = filename + strlen(filename);  //new 10-7-15
	//const char *ptr = filename + strlen(filename);
	const char *start = filename;

	while (*ptr != '.' && ptr != filename)
		ptr--;

	ptrdiff_t length = ptr - start;
	//int length = ptr - start;
	char *out = new char[length + 1];
	strncpy(out, filename, length);
	out[length] = '\0';

	return out;
}

char *StringHelper::ReplaceThreeLetterExtension(char *filename, const char *newExtension)
{
	char *tmp = ExtractFilenameWithoutExtension(filename);
	char *newFilename = new char[strlen(tmp) + strlen(newExtension) + 2];

	strcpy(newFilename, tmp);
	strcpy(newFilename + strlen(tmp), ".");
	strcpy(newFilename + strlen(tmp) + 1, newExtension);
	newFilename[strlen(tmp) + strlen(newExtension) + 1] = '\0';
	delete[] tmp;

	return newFilename;

	//char *tmp = new char[strlen(filename) + 1];
	//strcpy(tmp, filename);
	//if (strpbrk(filename, ".") == nullptr)
	//	return tmp;

	//char *end = tmp + strlen(tmp);
	//*(end - 3) = newExtension[0];
	//*(end - 2) = newExtension[1];
	//*(end - 1) = newExtension[2];

	//return tmp;
}

int StringHelper::GetCurrentLineNumber(const char *startPos, const char *currentPos)
{
	int lineNumber = 0;
	const char *ptr = startPos;

	while (ptr != currentPos)
	{
		if (*ptr == '\n')
			++lineNumber;

		++ptr;
	}

	return lineNumber + 1;
}
