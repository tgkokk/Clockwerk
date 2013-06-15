// Compatibility.h
// 
// Copyright (c) 2004, Ingo Weinhold (bonefish@cs.tu-berlin.de)
// 
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// 
// Except as contained in this notice, the name of a copyright holder shall
// not be used in advertising or otherwise to promote the sale, use or other
// dealings in this Software without prior written authorization of the
// copyright holder.

#ifndef USERLAND_FS_COMPATIBILITY_H
#define USERLAND_FS_COMPATIBILITY_H

#ifdef __BEOS__
#	include <BeBuild.h>
#	if B_BEOS_VERSION <= B_BEOS_VERSION_5
		// BONE doesn't seem to define a greater version
#		define BEOS_NETSERVER 1
#		define B_BAD_DATA -2147483632L
#	else
#		ifndef closesocket
#			define closesocket(fd)	close(fd)
#		endif
#		define B_BAD_DATA -2147483632L
#	endif
#elif defined(__HAIKU__)
#	ifndef closesocket
#		define closesocket(fd)	close(fd)
#	endif
#endif


// a Haiku definition
#ifndef B_BUFFER_OVERFLOW
#	define B_BUFFER_OVERFLOW	EOVERFLOW
#endif

// make Zeta R5 source compatible without needing to link against libzeta.so
#ifdef find_directory
#	undef find_directory
#endif

#endif	// USERLAND_FS_COMPATIBILITY_H
