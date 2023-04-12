#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <iostream>
#include <algorithm>

namespace msg
{
	// convert string to wstring
	std::wstring wtos(std::string& value);
	// encrypt message using simple ceasar's cipher
	void encrypt(std::string& s);
	// decrypt message using simple ceasar's cipher
	void decrypt(std::string& s);
}

#endif