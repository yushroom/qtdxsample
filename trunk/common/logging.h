/** @file
 * @brief python-like logging
 * @author Shintaro Takemura(stakemura@gmail.com)
 *
 * This code is public domain, no warranty expressed or implied, 
 * Functionality is thought to be correct, but it's up to you to make
 * sure it does what you want.
 */

#pragma once
#include <vector>
#include <sstream>

#ifdef  ERROR
#undef  ERROR
#endif

//! Python-like logging class
template<typename T>
class	logging
{
public:
	//! Logging Level
	enum Level
	{
		TRACE = 0, NOTSET = 0,
		DEBUG = 10,
		INFO = 20,
		WARN = 30, WARNING = 30,
		ERROR = 40,
		CRITICAL = 50, FATAL = 50
	};

	//! Configuration
	struct Config
	{
		//! level
		Level cntLevel;

		//! log file
		std::basic_string<T> logFile;
	};

	//! log_stringbuf
	template<Level myLevel>
	class log_stringbuf : public std::basic_stringbuf<T>
	{
	public:
		log_stringbuf(const Config &_p) : config(_p) {}

		virtual ~log_stringbuf()
		{
			sync();
		}

		//! equal to logger.setLevel
		void setLevel(Level _level)
		{
			cntLevel = _level;
		}
	protected:
		virtual int sync()
		{
			logging<T>::trace( config, myLevel, str().c_str() );
			pbump((int)(pbase() - pptr()));
			return 0;
		}//sync

		//! Configuration info
		const Config	&config;
	};//class log_stringbuf

	//! log_stream
	template<Level myLevel>
	class log_stream : public std::basic_ostream<T>
	{
	public:
		log_stream(const Config &_p) :
			stringbuf(_p),
			std::basic_ostream<T>(&stringbuf),
			config(_p) {}

		virtual ~log_stream()
		{
			//delete rdbuf();
		}

		//! Enable logger.xxx call
		void operator()(const T * fmt, ... )
		{
			va_list ap;
			va_start(ap, fmt);

			temp.resize( logging<T>::_vscprintf(fmt, ap)+1 );
			
			logging<T>::_vsnprintf_s(&temp[0], temp.size(), temp.size(), fmt, ap);
			va_end(ap);

			logging<T>::trace( config, myLevel, &temp[0] );
		}

	protected:
		//! Configuration info
		const Config	&config;

		//! String Buffer
		log_stringbuf<myLevel>	stringbuf;

		// Temporary Data
		std::vector<T> temp;
	};//class log_stream

	logging() :
		debug(config),
		info(config),
		warn(config),
		error(config),
		fatal(config)
	{
#ifdef _DEBUG
		setLevel(DEBUG);
#else
		setLevel(INFO);
#endif
	}

	virtual ~logging() {}

	//! equal to logger.setLevel
	virtual void setLevel(Level _level)
	{
		config.cntLevel = _level;
	}

	//! limited implement
	virtual void startConfig(const T* _fname)
	{
		if(_fname!=0)
		{
			config.logFile = _fname;
		}
		else
		{
			config.logFile.clear();
		}
	}

	//! common output function
	static void trace(const Config &config, Level myLevel, const T *buf)
	{
		if(myLevel<config.cntLevel) return;

		if(config.logFile.empty())
		{
			logging<T>::OutputDebugString(buf);
		}
		else
		{
			FILE *fp = 0;
			logging<T>::fopen_s(&fp,config.logFile.data());
			logging<T>::fputs(buf,fp);
			fclose(fp);
		}
	}

	static void OutputDebugString(const T *);
	static int  _vscprintf( const T * _Format, va_list _ArgList);
	static void _vsnprintf_s(T * _DstBuf, size_t _SizeInBytes, size_t _MaxCount, const T * _Format, va_list _ArgList);
	static void fopen_s(FILE ** _File, const T * _FileName);
	static void fputs(const T * _DstBuf, FILE * _File);

	log_stream<DEBUG>		debug;
	log_stream<INFO>		info;
	log_stream<WARN>		warn;
	log_stream<ERROR>		error;
	log_stream<FATAL>		fatal;

protected:
	Config config;
};//class logging

template <>
inline void logging<char>::OutputDebugString(const char *str)
{
	::OutputDebugStringA(str);
}

template <>
inline void logging<wchar_t>::OutputDebugString(const wchar_t *str)
{
	::OutputDebugStringW(str);
}

template <>
inline int  logging<char>::_vscprintf( const char * _Format, va_list _ArgList)
{
	return ::_vscprintf(_Format, _ArgList);
}

template <>
inline int  logging<wchar_t>::_vscprintf( const wchar_t * _Format, va_list _ArgList)
{
	return ::_vscwprintf(_Format, _ArgList);
}

template <>
inline void logging<char>::_vsnprintf_s(char * _DstBuf, size_t _SizeInBytes, size_t _MaxCount, const char * _Format, va_list _ArgList)
{
	::_vsnprintf_s(_DstBuf,_SizeInBytes,_MaxCount,_Format,_ArgList);
}

template <>
inline void logging<wchar_t>::_vsnprintf_s(wchar_t * _DstBuf, size_t _SizeInBytes, size_t _MaxCount, const wchar_t * _Format, va_list _ArgList)
{
	::_vsnwprintf_s(_DstBuf,_SizeInBytes,_MaxCount,_Format,_ArgList);
}

template <>
inline void logging<char>::fopen_s(FILE ** _File, const char * _FileName)
{
	::fopen_s(_File,_FileName,"at");
}

template <>
inline void logging<wchar_t>::fopen_s(FILE ** _File, const wchar_t * _FileName)
{
	::_wfopen_s(_File,_FileName, L"at");
}

template <>
inline void logging<char>::fputs(const char * _DstBuf, FILE * _File)
{
	::fputs(_DstBuf,_File);
}

template <>
inline void logging<wchar_t>::fputs(const wchar_t * _DstBuf, FILE * _File)
{
	::fputws(_DstBuf,_File);
}

static logging<char> logger;
static logging<wchar_t> loggerw;
