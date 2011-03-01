/*!
	@brief DirectX common header
	@author Shintaro Takemura
*/

#pragma once

#define _BOOL
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <vector>
#include <valarray>
#include <stack>
#include <list>
#include <set>
#include <map>

#ifdef HAS_TR1
#include <unordered_set>
#include <unordered_map>
#endif

#include <utility>
#include <memory>
#include <numeric>
#include <algorithm>
#include <functional>
#include <cassert>
#include <cmath>
#include <cfloat>

#include <sstream>
#include <string>
#include <iostream>

#ifdef _WIN32
	#if defined(__MINGW32__) || defined(__CYGWIN__) || (defined (_MSC_VER) && _MSC_VER < 1300)

		#define SIMD_FORCE_INLINE inline
		#define ATTRIBUTE_ALIGNED16(a) a
		#define ATTRIBUTE_ALIGNED64(a) a
		#define ATTRIBUTE_ALIGNED128(a) a
		#define btAlignedAlloc(size,alignment) malloc(size)
		#define btAlignedFree(ptr) free(ptr)
	#else

		#include <windows.h>

		#define SIMD_FORCE_INLINE __forceinline
		#define ATTRIBUTE_ALIGNED16(a) __declspec(align(16)) a
		#define ATTRIBUTE_ALIGNED64(a) __declspec(align(64)) a
		#define ATTRIBUTE_ALIGNED128(a) __declspec (align(128)) a
		#define btAlignedAlloc(size,alignment) _aligned_malloc(size, (size_t)alignment)
		#define btAlignedFree(ptr) _aligned_free(ptr)

		#if ((_MSC_VER) && _MSC_VER >= 1400) && (!defined (BT_USE_DOUBLE_PRECISION))
			#define BT_USE_SSE
			#include <emmintrin.h>
		#endif

	#endif //__MINGW32__
#else
	//non-windows systems

	#define BT_USE_SSE
	#include <emmintrin.h>

	#define SIMD_FORCE_INLINE inline
	#define ATTRIBUTE_ALIGNED16(a) a __attribute__ ((aligned (16)))
	#define ATTRIBUTE_ALIGNED64(a) a __attribute__ ((aligned (64)))
	#define ATTRIBUTE_ALIGNED128(a) a __attribute__ ((aligned (128)))
	#define btAlignedAlloc(size,alignment) memalign(size, (size_t)alignment)
	#define btAlignedFree(ptr) free(ptr)
#endif


#define BT_DECLARE_ALIGNED_ALLOCATOR() \
   SIMD_FORCE_INLINE void* operator new(size_t sizeInBytes)   { return btAlignedAlloc(sizeInBytes,16); }   \
   SIMD_FORCE_INLINE void  operator delete(void* ptr)         { btAlignedFree(ptr); }   \
   SIMD_FORCE_INLINE void* operator new(size_t, void* ptr)   { return ptr; }   \
   SIMD_FORCE_INLINE void  operator delete(void*, void*)      { }   \
   SIMD_FORCE_INLINE void* operator new[](size_t sizeInBytes)   { return btAlignedAlloc(sizeInBytes,16); }   \
   SIMD_FORCE_INLINE void  operator delete[](void* ptr)         { btAlignedFree(ptr); }   \
   SIMD_FORCE_INLINE void* operator new[](size_t, void* ptr)   { return ptr; }   \
   SIMD_FORCE_INLINE void  operator delete[](void*, void*)      { }   \

///The btScalar type abstracts floating point numbers, to easily switch between double and single floating point precision.
#if defined(BT_USE_DOUBLE_PRECISION)
typedef double btScalar;
//this number could be bigger in double precision
#define BT_LARGE_FLOAT 1e30
#else
typedef float btScalar;
//keep BT_LARGE_FLOAT*BT_LARGE_FLOAT < FLT_MAX
#define BT_LARGE_FLOAT 1e18f
#endif

#define SIMD_2_PI         btScalar(6.283185307179586232)
#define SIMD_PI           (SIMD_2_PI * btScalar(0.5))
#define SIMD_HALF_PI      (SIMD_2_PI * btScalar(0.25))
#define SIMD_RADS_PER_DEG (SIMD_2_PI / btScalar(360.0))
#define SIMD_DEGS_PER_RAD  (btScalar(360.0) / SIMD_2_PI)
#define SIMDSQRT12 btScalar(0.7071067811865475244008443621048490)

#ifdef BT_USE_DOUBLE_PRECISION
#define SIMD_EPSILON      DBL_EPSILON
#define SIMD_INFINITY     DBL_MAX
#else
#define SIMD_EPSILON      FLT_EPSILON
#define SIMD_INFINITY     FLT_MAX
#endif

SIMD_FORCE_INLINE bool      btFuzzyZero(btScalar x) { return fabs(x) < SIMD_EPSILON; }

SIMD_FORCE_INLINE bool	btEqual(btScalar a, btScalar eps) {
	return (((a) <= eps) && !((a) < -eps));
}
SIMD_FORCE_INLINE bool	btGreaterEqual (btScalar a, btScalar eps) {
	return (!((a) <= eps));
}

SIMD_FORCE_INLINE int       btIsNegative(btScalar x) {
    return x < btScalar(0.0) ? 1 : 0;
}

SIMD_FORCE_INLINE btScalar btRadians(btScalar x) { return x * SIMD_RADS_PER_DEG; }
SIMD_FORCE_INLINE btScalar btDegrees(btScalar x) { return x * SIMD_DEGS_PER_RAD; }

#if 1
	// SIMD ver.
	#include "../vectormath/sse/vectormath_aos.h"
#else
	#include "../vectormath/scalar/vectormath_aos.h"
#endif

typedef Vectormath::Aos::Vector3    vmVector3;
typedef Vectormath::Aos::Vector4    vmVector4;
typedef Vectormath::Aos::Quat       vmQuat;
typedef Vectormath::Aos::Matrix3    vmMatrix3;
typedef Vectormath::Aos::Matrix4    vmMatrix4;
typedef Vectormath::Aos::Transform3 vmTransform3;
typedef Vectormath::Aos::Point3     vmPoint3;


#ifdef USE_D2D
	#include <d2d1.h>
	#include <d2d1helper.h>
	#include <dwrite.h>
	#include <wincodec.h>

	#pragma comment(lib,"d2d1.lib")
	#pragma comment(lib,"dwrite.lib")
	#pragma comment(lib,"windowscodecs.lib")
	#pragma comment(lib,"dxgi.lib")
#endif

#ifdef USE_D3D
#include <DxErr.h>
#pragma comment(lib,"DxErr.lib")

#include <d3dcompiler.h>
#pragma comment(lib,"d3dcompiler.lib")
#endif

#if USE_D3D==11
	#include <d3d11.h>
	#include <d3dx11.h>
	#include <d3dx10math.h>

	#pragma comment(lib,"d3d11.lib")
	#ifdef _DEBUG
		#pragma comment(lib,"d3dx10d.lib")
		#pragma comment(lib,"d3dx11d.lib")
	#else
		#pragma comment(lib,"d3dx10.lib")
		#pragma comment(lib,"d3dx11.lib")
	#endif
#endif

#if USE_D3D==10
	#include <d3d10_1.h>
	#include <d3dx10.h>

	#pragma comment(lib,"d3d10_1.lib")
	#ifdef _DEBUG
		#pragma comment(lib,"d3dx10d.lib")
	#else
		#pragma comment(lib,"d3dx10.lib")
	#endif
#endif

#if USE_D3D==9
	#include <d3d9.h>
	#include <d3dx9.h>

	#pragma comment(lib,"d3d9.lib")
	#ifdef _DEBUG
		#pragma comment(lib,"d3dx9d.lib")
	#else
		#pragma comment(lib,"d3dx9.lib")
	#endif
#endif


#ifndef V_RETURN
#define V_RETURN(x)    { hr = (x); if( FAILED(hr) ) { return hr; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

#ifndef DIRECT3D_VERSION
#define DIRECT3D_VERSION ((D3D_VERSION << 8) + D3D_MINOR_VERSION)
#endif
