#include "KeyTranslate.hpp"
#include "Crypt.hpp"

#include <array>
#include <string>

namespace ImGui
{
	const std::string key_array[ 124 ] =
	{
		"",
		"M1",
		"M2",
		"",
		"M3",
		"M4",
		"M5",
		"",
		"BS",
		"Tab",
		"",
		"",
		"",
		"Enter",
		"",
		"",
		"Shift",
		"Ctrl",
		"Alt",
		"Pause",
		"Caps",
		"",
		"",
		"",
		"",
		"",
		"",
		"Escape",
		"",
		"",
		"",
		"",
		"Space",
		"PU",
		"PD",
		"End",
		"Home",
		"Left",
		"Up",
		"Right",
		"Down",
		"",
		"",
		"",
		"Print",
		"Insert",
		"Delete",
		"",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",
		"",
		"",
		"",
		"",
		"",
		"Num 0",
		"Num 1",
		"Num 2",
		"Num 3",
		"Num 4",
		"Num 5",
		"Num 6",
		"Num 7",
		"Num 8",
		"Num 9",
		"Multiply",
		"Add",
		"",
		"Subtract",
		"Decimal",
		"Divide",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12"
	};

	uint32_t GetCodeFromName( const std::string& name )
	{
		for( int i = 0; i < 124; i++ )
		{
			if( name.compare( key_array[ i ] ) == 0 )
				return i;
		}

		return NULL;
	}

	const std::string& GetNameFromCode( uint32_t code )
	{
		if( code > 124 )
			return key_array[ 0 ];

		return key_array[ code ];
	}
}