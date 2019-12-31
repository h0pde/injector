#include "injector.hpp"



injector g_inj{};


int main( int arg, char *argv[ ] ) {

	if ( !g_inj.get_dll( ( const char * )argv[ 2 ] ) ) {
		printf_s( "Failed to find DLL! Is it in the same directory as the injector?\n" );
		return getchar( );
	}

	return g_inj.inject( argv[ 1 ] );
}

