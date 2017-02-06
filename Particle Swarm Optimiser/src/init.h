#pragma once
/**
 * Mostly inspired by the linux kernel
 */

typedef void (*initcall_t)(void);

#ifdef __GNUC__

/**
 * Using __start_initcalls and __stop_initcalls instead of creating a linker script to get pointers to the
 * user defined initcalls section
 * Very likely to be gcc specific behaviour
 * http://www.airs.com/blog/archives/56
 */
extern initcall_t __start_initcalls[], __stop_initcalls[];

#define module_init(fn) \
	static initcall_t __initcall_##fn __attribute__((__section__("initcalls"))) __attribute__((__used__)) = fn

#elif _MSC_VER

#pragma section(".initmod$a", read)
#pragma section(".initmod$u")
#pragma section(".initmod$z", read)

__declspec(allocate(".init$a")) initcall_t __start_initcalls_seg;
__declspec(allocate(".init$z")) initcall_t __stop_initcalls_seg;

initcall_t* __start_initcalls = &__start_initcalls_seg;
initcall_t* __stop_initcalls = &__stop_initcalls_seg;

#define module_init(fn) \
	__declspec(allocate(".init$u")) static initcall_t __initcall_##fn = fn;

#endif