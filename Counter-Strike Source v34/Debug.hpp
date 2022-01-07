#pragma once

namespace Debug
{
	extern void Print( const char* fmt, ... );
}

#define DPRINT( fmt, ... ) Debug::Print( fmt, __VA_ARGS__ )
