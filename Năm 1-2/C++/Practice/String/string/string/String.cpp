#include <iostream>
#include <cstring>
#include <string>
#include <string.h>
#include <sstream>

using namespace std;

// Problem 4
void reverseOrderString(string &name)
{
	string temp[20];
	int len = name.length(), j = 0;
	
	for (int i = 0; i < len; i++)
	{
		if (name[i] != ' ')
			temp[j] += name[i];
		else
			j++;
	}

	name = temp[j];
	for (int i = j - 1; i >= 0; i--)
		name += " " + temp[i];
}

// Problem 5
string nString(string s, unsigned int n)
{
	int len = s.length();
	if (n > len)
		return s;

	return s.substr(0, n);
}

// Problem 6
bool isAlpha(int chr)
{
	if (chr >= 65 && chr <= 90)
		return true;

	if (chr >= 97 && chr <= 122)
		return true;

	return false;
}

void titleString(string& s)
{
	int len = s.length();

	int i = 0;
	while (s[i] == ' ')
		i++;

	s[i] = toupper(s[i]);
	i++;

	for (; i < len - 1; i++)
	{
		if (s[i] == ' ' && isalpha(char(s[i + 1])))
		{
			s[i + 1] = toupper(s[i + 1]);
			i++;
		}

		else if (isalpha(char(s[i])))
			s[i] = tolower(s[i]);
	}

	s[i] = tolower(s[i]);
}

// Problem 7
void deleteSpace(string& s)
{
	string temp = "";
	for (int i = 0; i < s.size(); i++)
		if (s[i] != ' ')
			temp += s[i];
	s = temp;
}

bool isPalindrome(string s)
{
	deleteSpace(s);
	int len = s.length();
	string temp = "";
	for (int i = len - 1; i >= 0; i--)
		temp.push_back(s[i]);

	if (temp == s)
		return true;

	return false;
}

// Problem 8
void frequencyOfWords(string s)
{
	string words[100];
	int freq[100] = {};
	string word;
	int n = 0;

	stringstream ss(s);

	while (ss >> word)
	{
		bool found = false;
		
		for( int i = 0; i < n; i++ )
			if (words[i] == word)
			{
				found = true;
				freq[i]++;
				break;
			}

		if (!found)
		{
			words[n] = word;
			freq[n]++;
			n++;
		}
	}

	for (int i = 0; i < n; i++)
		cout << words[i] << ' ' << freq[i] << ' ';
}

// Problem 9
bool isPangram(string s)
{
	int freqChar[100] = {};
	int len = s.length();
	for (int i = 0; i < len; i++)
		freqChar[toupper(s[i])]++;

	for (int i = 65; i <= 90; i++)
		if (freqChar[i] == 0)
			return false;

	return true;
}

// Problem 10
bool ishavingLowerAlpha(string s)
{
	for (int i = 0; i < s.length(); i++)
		if (s[i] >= 'a' && s[i] <= 'z')
			return true;

	return false;
}

bool ishavingUpperAlpha(string s)
{
	for (int i = 0; i < s.length(); i++)
		if (s[i] >= 'A' && s[i] <= 'Z')
			return true;

	return false;
}

bool ishavingDigit(string s)
{
	for (int i = 0; i < s.length(); i++)
		if (s[i] >= '0' && s[i] <= '9')
			return true;

	return false;
}

bool ishavingSymbol(string s)
{
	for (int i = 0; i < s.length(); i++)
	{
		if (s[i] >= 33 && s[i] <= 47)
			return true;
		if (s[i] >= 58 && s[i] <= 64)
			return true;
		if (s[i] >= 91 && s[i] <= 96)
			return true;
		if (s[i] >= 123 && s[i] <= 126)
			return true;
	}

	return false;
}

bool isStrongPassword(string password)
{
	if (password.length() < 15)
		return false;
	if (!ishavingUpperAlpha(password))
		return false;
	if (!ishavingLowerAlpha(password))
		return false;
	if (!ishavingDigit(password))
		return false;
	if (!ishavingSymbol(password))
		return false;

	return true;
}

// Problem 11
void sortDescendingString(string& s)
{
	string words[100];
	int len = s.length();
	int n = 0;

	for (int i = 0; i < len; i++)
	{
		if (isAlpha(s[i]))
			words[n] += s[i];
		else
			n++;
	}

	for (int i = 0; i < n - 1; i++)
		for (int j = i + 1; j < n; j++)
			if (words[i] < words[j])
				swap(words[i], words[j]);

	s = words[0];
	for (int i = 1; i < n; i++)
		s += ' ' + words[i];
}

// Problem 12
void deleteAt(string &s, int pos)
{
	int len = s.length();
	for (int i = pos; i < len - 1; i++)
		s[i] = s[i + 1];
	
	s[len - 1] = '\0';
}

void deleteUnecessaryBlank(string &s)
{
	int len = s.length();

	while (s[0] == ' ')
		deleteAt(s, 0);

	for (int i = 0; i < len - 1; i++)
		if (s[i] == ' ' && s[i + 1] == ' ')
		{
			deleteAt(s, i);
			i--;
		}

	if (s[s.size() - 1] == ' ')
		deleteAt(s, s.size() - 1);
	
}

// Problem 13
void deleteRepeatWord(string& s, string word)
{
	stringstream ss(s);
	string result = "";
	string temp;
	int freq = 0;

	while (ss >> temp)
	{
		if (temp == word && freq == 0)
		{
			result += temp + ' ';
			freq++;
		}

		else if (temp != word)
			result += temp + ' ';
	}

	result[result.size() - 1] = '\0';
	s = result;
}

int main()
{
	string s = "hello two guy on the board with two two two bags on two rooms";
	deleteRepeatWord(s, "two");
	cout << s;
}