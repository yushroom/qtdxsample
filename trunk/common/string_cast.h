/** @file
 * @brief python-like string cast module
 * @author Shintaro Takemura(stakemura@gmail.com)
 *
 * This code is public domain, no warranty expressed or implied, 
 * Functionality is thought to be correct, but it's up to you to make
 * sure it does what you want.
 */

#pragma once

#include <vector>
#include <set>
#include <string>
#include <algorithm>

template<typename Td,typename Ts>
class string_cast
{
public:
	typedef std::basic_string<Td> result_type;

	result_type operator()(const Ts *src,UINT codepage=CP_UTF8);
};

template <>
inline std::basic_string<char> string_cast<char,char>::operator()(const char *pSource,UINT codepage)
{
	std::basic_string<char> dst;
	dst.assign(pSource);
	return dst;
}

template <>
inline std::basic_string<char> string_cast<char,wchar_t>::operator()(const wchar_t *pSource,UINT codepage)
{
	// convert unicode to string
	const int nSizeUTF8 = ::WideCharToMultiByte( codepage, 0, pSource, -1, NULL, 0, NULL, NULL );

	char* buffUTF8 = new char[ nSizeUTF8 + 1 ];
	ZeroMemory( buffUTF8, nSizeUTF8 + 1 );
	::WideCharToMultiByte( codepage, 0, pSource, -1, buffUTF8, nSizeUTF8, NULL, NULL );
 
	std::basic_string<char> dst;
	dst.assign(buffUTF8);
	delete[] buffUTF8;
	return dst;
}

template <>
inline std::basic_string<wchar_t> string_cast<wchar_t,wchar_t>::operator()(const wchar_t *pSource,UINT codepage)
{
	std::basic_string<wchar_t> dst;
	dst.assign(pSource);
	return dst;
}

template <>
inline std::basic_string<wchar_t> string_cast<wchar_t,char>::operator()(const char *pSource,UINT codepage)
{
	// convert string to unicode
	const int nSize = ::MultiByteToWideChar( codepage, 0, pSource, -1, NULL, 0 );

	wchar_t* buffUtf16 = new wchar_t[ nSize + 1 ];
	::MultiByteToWideChar( codepage, 0, pSource, -1, buffUtf16, nSize );

	std::basic_string<wchar_t> dst;
	dst.assign(buffUtf16); 
	delete[] buffUtf16;
	return dst;
}


template <class T>
inline std::basic_string<char> str(const T *src,UINT codepage=CP_UTF8)
{
	return string_cast<char,T>()(src,codepage);
}

template <class T>
inline std::basic_string<char> str(const std::basic_string<T> &src,UINT codepage=CP_UTF8)
{
	return string_cast<char,T>()(src.data(),codepage);
}

template <class T>
inline std::basic_string<wchar_t> unicode(const T *src,UINT codepage=CP_UTF8)
{
	return string_cast<wchar_t,T>()(src,codepage);
}

template <class T>
inline std::basic_string<wchar_t> unicode(const std::basic_string<T> &src,UINT codepage=CP_UTF8)
{
	return string_cast<wchar_t,T>()(src.data(),codepage);
}

template<typename T>
inline std::basic_string<T> join(int argc, const T* argv[], const std::basic_string<T>& join)
{
	std::basic_string<T> result;
	for (int i = 0; i < argc;)
	{
		result += argv[i];
		if (++i < argc)
			result += join;
	}
	return result;
}

template<typename T>
inline std::basic_string<T> join(const std::vector<std::basic_string<T> >& input, const std::basic_string<T>& join)
{
	std::basic_string<T> result;
	std::vector<std::basic_string<T>>::const_iterator i;
	for (i = input.begin(); i != input.end();)
	{
		result += *i;
		if (++i != input.end())
			result += join;
	}
	return result;
}

template<typename T,typename Iterator>
inline std::basic_string<T> join(Iterator _First, Iterator _Last, const std::basic_string<T>& join)
{
	std::basic_string<T> result;
	Iterator i;
	for (i = _First; i != _Last;)
	{
		result += *i;
		if (++i != _Last)
			result += join;
	}
	return result;
}

//! given a std::basic_string, split it into components, at the delimiters character.
template<typename T,typename OutputIterator>
OutputIterator  split(const std::basic_string<T>& str,const std::basic_string<T>& delimiters,OutputIterator out)
{
	std::vector<std::basic_string<T> > tokens;

    // Skip delimiters at beginning.
    std::basic_string<T>::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::basic_string<T>::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::basic_string<T>::npos != pos || std::basic_string<T>::npos != lastPos)
    {
        // Found a token, add it to the vector.
        *out++ = str.substr(lastPos, pos - lastPos);
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
	return out;
}

// split string to set<string>
template<typename T>
inline std::set<std::basic_string<T> >  split(const std::basic_string<T>& src,const std::basic_string<T>& delimiters)
{
	std::set<std::basic_string<T> > dst;
	split(src,delimiters,std::inserter(dst,dst.end()) );
	return dst;
}

//! given a std::basic_string, split it into components, at the splitter characters.
template<typename T,typename Iterator>
inline std::vector<std::basic_string<T> > split(const std::basic_string<T>& input, Iterator _First, Iterator _Last)
{
	std::vector<std::basic_string<T> > output;
	size_t curr = 0;
	size_t start = 0;
	size_t end = input.length();
	while (curr < end)
	{
		if (std::find(_First,_Last,input[curr])!=_Last)
		{
			if (curr>start)
			{
				output.push_back(input.substr(start, curr-start));
			}
			else if (curr == 0)
			{
				output.push_back(std::basic_string<T>());
			}
			start = curr+1;
			if (std::find(_First,_Last,input[start])!=_Last || start == end)
			{
				output.push_back(std::basic_string<T>());
				++start;
				++curr;
			}
		}
		++curr;
	}
	return output;
}

