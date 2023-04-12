#include "include/global_functions.h"

namespace msg
{
	std::wstring wtos(std::string& text)
	{
		const size_t cSize = text.size() + 1;
		std::wstring wtext;
		wtext.resize(cSize);
		size_t cSize1;
		mbstowcs_s(&cSize1, (wchar_t*)&wtext[0], cSize, text.c_str(), cSize);
		wtext.pop_back();
		return wtext;
	}

	void encrypt(std::string& s) { for_each(s.begin(), s.end(), [](char& c) { c += 1; }); }

	void decrypt(std::string& s) { for_each(s.begin(), s.end(), [](char& c) { c -= 1; }); }
}