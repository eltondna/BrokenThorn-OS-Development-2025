#ifndef _HAL_H
#define _HAL_H

//****************************************************************************
//**
//**    Hal.h
//**		Hardware Abstraction Layer Interface
//**
//**	The Hardware Abstraction Layer (HAL) provides an abstract interface
//**	to control the basic motherboard hardware devices. This is accomplished
//**	by abstracting hardware dependencies behind this interface.
//**
//**	All routines and types are declared extern and must be defined within
//**	external libraries to define specific hal implimentations.
//**
//****************************************************************************

#ifndef ARCH_X86
#pragma error "HAL not implimented for this platform"
#endif

#include "stdint.h"

#ifdef _MSC_VER
#define interrupt __declspec (naked)
#else
#define interrupt
#endif

extern int              _cdecl hal_initialize();
extern int              _cdecl hal_shutdown();
extern void              _cdecl geninterrupt(int n);
extern void             _cdecl interruptdone(unsigned int intno);
extern void             _cdecl sound(unsigned frequency);
// Read byte from device using port mapped io
extern unsigned char    _cdecl inportb(unsigned short portid);
// Write byte to device through port mapped io
extern void             _cdecl outportb(unsigned short portid, unsigned char value);
// Enable all hardware interrupt
extern void             _cdecl enable();
// Disable all hardware interrupt
extern void             _cdecl disable();
// Set new Interrupt vector
extern void _cdecl setvect(int intno, void (__cdecl & handler)());
// Return current Interrupt vector
extern void *           _cdecl getvect(int intno);
//! returns cpu vender
extern const char*		_cdecl get_cpu_vender ();   
//! returns current tick count (only for demo)
extern int				_cdecl get_tick_count ();
extern bool				 get_pit_state();

#endif

