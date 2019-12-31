#pragma once

#include "inc.h"
#include "win_wrapper.h"


class injector {
private:
	char		m_dll[ MAX_PATH ];

	__forceinline HANDLE get_process( const std::wstring &proc_name ) {
		HANDLE h{}, snapshot{};
		PROCESSENTRY32W entry{};
		entry.dwSize = sizeof( PROCESSENTRY32 );

		snapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, NULL );

		if ( wapi::p32_first( snapshot, &entry ) ) {
			while ( wapi::p32_next( snapshot, &entry ) ) {
				if ( !_wcsnicmp( entry.szExeFile, proc_name.c_str( ), MAX_PATH ) ) {
					h = wapi::open_proc( PROCESS_ALL_ACCESS, entry.th32ProcessID );
				}
			}
		}

		CloseHandle( snapshot );

		return h;
	}

public:
	__forceinline injector( ) : m_dll{} {}
	__forceinline ~injector( ) {}

	__forceinline bool get_dll( const char* binary ) {
		if ( !binary ) {
			printf_s( "Please specify a DLL name!\n" );
			return getchar( );
		}

		GetFullPathNameA( binary, MAX_PATH, m_dll, NULL );

		std::ifstream	f( m_dll );
		return f.good( );
	}

	__forceinline int inject( const char* proc_name ) {

		if ( !proc_name ) {
			printf_s( "Please specify a process name!\n" );
			return getchar( );
		}

		HANDLE h = get_process( std::wstring( std::string( proc_name ).begin( ), std::string( proc_name ).end( ) ) );

		if ( h == INVALID_HANDLE_VALUE ) {
			printf_s( "Process %s not found!\n", proc_name );
			return getchar( );
		}

		const auto writespace = wapi::valloc_ex( h, strlen( m_dll ) );

		if ( !writespace ) {
			printf_s( "Unable to allocate memory in target process.\n" );
			return getchar( );
		}

		if ( !wapi::wpm( h, writespace, ( const uintptr_t* )m_dll, strlen( m_dll ) ) ) {
			printf_s( "Unable to write memory in target process.\n" );
			return getchar( );
		}

		HANDLE thread_handle{};

		wapi::create_thread( h, &thread_handle, wapi::loadlib( ) );

		if ( thread_handle == INVALID_HANDLE_VALUE ) {
			printf_s( "Thread creation failure.\n" );
			return getchar( );
		}

		CloseHandle( thread_handle );
		CloseHandle( h );

		printf_s( "Success!\n" );

		std::this_thread::sleep_for( std::chrono::seconds( 1 ) );

		return 0;
	}
};