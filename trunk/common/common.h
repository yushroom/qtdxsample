/*!
	@brief DirectX common header
	@author Shintaro Takemura
*/

#pragma once

#define _BOOL
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define _USE_MATH_DEFINES

#include <vector>
#include <valarray>
#include <utility>
#include <memory>
#include <numeric>
#include <algorithm>
#include <functional>

#include <stack>
#include <list>
#include <map>
#include <cassert>
#include <cmath>

#include <sstream>
#include <string>
#include <iostream>

#include <windows.h>

#ifndef D3D_VERSION
#define D3D_VERSION	9
#endif

#if D3D_VERSION >= 9

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib,"d3d9.lib")
#ifdef _DEBUG
#pragma comment(lib,"d3dx9d.lib")
#else
#pragma comment(lib,"d3dx9.lib")
#endif

#endif

#ifndef V
#define V(x)           { hr = (x); }
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
