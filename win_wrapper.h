#pragma once


#include "inc.h"


#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)



namespace wapi {

	__forceinline static HMODULE k32( ) {
		static auto kernel32 = GetModuleHandleW( L"kernel32.dll" );
		return kernel32;
	}

	__forceinline static uintptr_t* loadlib( ) {
		static auto ll = ( uintptr_t* )GetProcAddress( k32( ), "LoadLibraryA" );
		return ll;
	}

	__forceinline static bool p32_first( HANDLE snap, PROCESSENTRY32W* proc_entry ) {
		static auto fn = ( decltype( &Process32FirstW ) )GetProcAddress( k32( ), "Process32FirstW" );
		return fn( snap, proc_entry );
	}

	__forceinline static bool p32_next( HANDLE snap, PROCESSENTRY32W* proc_entry ) {
		static auto fn = ( decltype( &Process32NextW ) )GetProcAddress( k32( ), "Process32NextW" );
		return fn( snap, proc_entry );
	}

	__forceinline static HANDLE open_proc( uint32_t access, uint32_t pid ) {
		static auto fn = ( decltype( &OpenProcess ) )GetProcAddress( k32( ), "OpenProcess" );
		return fn( access, false, pid );
	}

	__forceinline static uintptr_t* valloc_ex( HANDLE proc, size_t size ) {
		static auto fn = ( decltype( &VirtualAllocEx ) )GetProcAddress( k32( ), "VirtualAllocEx" );
		return ( uintptr_t* )fn( proc, NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	}

	__forceinline static bool wpm( HANDLE proc, uintptr_t* addr, const uintptr_t* buf, size_t size ) {
		static auto fn = ( decltype( &WriteProcessMemory ) )GetProcAddress( k32( ), "WriteProcessMemory" );
		return fn( proc, addr, buf, size, NULL );
	}

	__forceinline static bool create_thread( HANDLE proc, HANDLE* thread_handle, uintptr_t* entry_fn ) {
		using NtCreateThreadEx_t = NTSTATUS( NTAPI* )(
			HANDLE*						out_thread_handle,
			ACCESS_MASK 				access_mask,
			uintptr_t*					object_attributes,
			HANDLE 						process_handle,
			uintptr_t*					entry_fn,
			uintptr_t*					entry_args,
			uint32_t 					thread_flags,
			size_t 						zero_bits,
			size_t 						stack_size,
			size_t 						max_stack_size,
			void*						out_attribute_list
			);

		static auto fn = ( NtCreateThreadEx_t )GetProcAddress( GetModuleHandleW( L"ntdll.dll" ), "NtCreateThreadEx" );

		return NT_SUCCESS( fn( thread_handle, THREAD_ALL_ACCESS, NULL, proc, entry_fn, NULL, 0x4, 0, 0, 0, 0 ) );
	}

};