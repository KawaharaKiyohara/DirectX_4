/*!
 *@brief		ÉçÉO
 */

#ifndef _TKLOG_H_
#define _TKLOG_H_


namespace tkEngine2{
	/*!
	 *@brief	ÉçÉOèoóÕ
	 */
	static inline void Log( const char* format, ... )
	{
		static char log[1024*10];
		va_list va;
		va_start( va, format );
		vsprintf( log, format, va );
		OutputDebugString( log );
		va_end( va );
	}
}

#if BUILD_LEVEL != BUILD_LEVEL_MASTER
	#define TK_LOG( format, ... )	tkEngine2::Log(format, __VA_ARGS__)
#else // _DEBUG
#define TK_LOG( format, ... )
#endif // _DEBUG
#endif // _TKLOG_H_