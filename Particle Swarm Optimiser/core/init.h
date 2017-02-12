#pragma once
/**
 * Mostly inspired by the linux kernel
 */

typedef void (*initcall_t)(void);

#ifdef __GNUC__

extern const initcall_t* __start_initcalls;
extern const initcall_t* __stop_initcalls;

/**
 * Using __start_initcalls and __stop_initcalls instead of creating a linker script to get pointers to the
 * user defined initcalls section
 * Very likely to be gcc specific behaviour
 * http://www.airs.com/blog/archives/56
 */
#define module_init(fn) \
	static initcall_t __initcall_##fn __attribute__((__used__)) __attribute__((__section__("initcalls"))) = fn

#elif _MSC_VER

#pragma section(".initmod$a", read)
#pragma section(".initmod$u", read)
#pragma section(".initmod$z", read)

//#pragma comment( linker, "/merge:.initmod_a=.initmod" )
//#pragma comment( linker, "/merge:.initmod_u=.initmod" )
//#pragma comment( linker, "/merge:.initmod_z=.initmod" )

__declspec(allocate(".initmod$a")) static initcall_t __start_initcalls_seg;
__declspec(allocate(".initmod$z")) static initcall_t __stop_initcalls_seg;

#define __start_initcalls (&__start_initcalls_seg)
#define __stop_initcalls (&__stop_initcalls_seg)

#define module_init(fn) \
	__declspec(allocate(".initmod$u")) static initcall_t __initcall_##fn = fn;

#endif