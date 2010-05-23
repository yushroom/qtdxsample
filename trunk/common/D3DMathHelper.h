/*!
	@brief Direct3D Math Helper
	@author Shintaro Takemura
*/

#pragma once

inline std::ostream& operator << (std::ostream& os, const D3DXVECTOR2& value)
{
	return os << value.x << " " << value.y;
}

inline std::istream& operator >> (std::istream& is, D3DXVECTOR2& value)
{
	D3DXVECTOR2 tmp;
	if(is >> tmp.x >> tmp.y)
	{
		value = tmp;
	}
	return is;
}

inline std::ostream& operator << (std::ostream& os, const D3DXVECTOR3& value)
{
	return os << value.x << " " << value.y << " " << value.z;
}

inline std::istream& operator >> (std::istream& is, D3DXVECTOR3& value)
{
	D3DXVECTOR3 tmp;
	if(is >> tmp.x >> tmp.y >> tmp.y)
	{
		value = tmp;
	}
	return is;
}

inline std::ostream& operator << (std::ostream& os, const D3DXVECTOR4& value)
{
	return os << value.x << " " << value.y << " " << value.z << " " << value.w;
}

inline std::istream& operator >> (std::istream& is, D3DXVECTOR4& value)
{
	D3DXVECTOR4 tmp;
	if(is >> tmp.x >> tmp.y >> tmp.z >> tmp.w)
	{
		value = tmp;
	}
	return is;
}

inline std::ostream& operator << (std::ostream& os, const D3DXQUATERNION& value)
{
	return os << value.x << " " << value.y << " " << value.z << " " << value.w;
}

inline std::istream& operator >> (std::istream& is, D3DXQUATERNION& value)
{
	D3DXQUATERNION tmp;
	if(is >> tmp.x >> tmp.y >> tmp.z >> tmp.w)
	{
		value = tmp;
	}
	return is;
}

inline std::ostream& operator << (std::ostream& os, const D3DXCOLOR& value)
{
	return os << value.r << " " << value.g << " " << value.b << " " << value.a;
}

inline std::istream& operator >> (std::istream& is, D3DXCOLOR& value)
{
	D3DXCOLOR tmp;
	if(is >> tmp.r >> tmp.g >> tmp.b >> tmp.a)
	{
		value = tmp;
	}
	return is;
}

inline std::ostream& operator << (std::ostream& os, const D3DXMATRIX& value)
{
	for(UINT i=0; i<4; ++i)
	{
		for(UINT j=0; j<4; ++j)
		{
			os << value(i, j);

			if(i < 3 || j < 3)
			{
				os << " ";
			}
		}
	}
	return os;
}

inline std::istream& operator >> (std::istream& is, D3DXMATRIX& value)
{
	D3DXMATRIX tmp;
	for(UINT i=0; i<4; ++i)
	{
		for(UINT j=0; j<4; ++j)
		{
			is >> tmp(i, j);
		}
	}

	if(is)
	{
		value = tmp;
	}
	return is;
}

// ----------------------------------------------------------------------
// wide-char

inline std::wostream& operator << (std::wostream& os, const D3DXVECTOR2& value)
{
	return os << value.x << L" " << value.y;
}

inline std::wistream& operator >> (std::wistream& is, D3DXVECTOR2& value)
{
	D3DXVECTOR2 tmp;
	if(is >> tmp.x >> tmp.y)
	{
		value = tmp;
	}
	return is;
}

inline std::wostream& operator << (std::wostream& os, const D3DXVECTOR3& value)
{
	return os << value.x << L" " << value.y << L" " << value.z;
}

inline std::wistream& operator >> (std::wistream& is, D3DXVECTOR3& value)
{
	D3DXVECTOR3 tmp;
	if(is >> tmp.x >> tmp.y >> tmp.y)
	{
		value = tmp;
	}
	return is;
}

inline std::wostream& operator << (std::wostream& os, const D3DXVECTOR4& value)
{
	return os << value.x << L" " << value.y << L" " << value.z << L" " << value.w;
}

inline std::wistream& operator >> (std::wistream& is, D3DXVECTOR4& value)
{
	D3DXVECTOR4 tmp;
	if(is >> tmp.x >> tmp.y >> tmp.z >> tmp.w)
	{
		value = tmp;
	}
	return is;
}

inline std::wostream& operator << (std::wostream& os, const D3DXQUATERNION& value)
{
	return os << value.x << L" " << value.y << L" " << value.z << L" " << value.w;
}

inline std::wistream& operator >> (std::wistream& is, D3DXQUATERNION& value)
{
	D3DXQUATERNION tmp;
	if(is >> tmp.x >> tmp.y >> tmp.z >> tmp.w)
	{
		value = tmp;
	}
	return is;
}

inline std::wostream& operator << (std::wostream& os, const D3DXCOLOR& value)
{
	return os << value.r << L" " << value.g << L" " << value.b << L" " << value.a;
}

inline std::wistream& operator >> (std::wistream& is, D3DXCOLOR& value)
{
	D3DXCOLOR tmp;
	if(is >> tmp.r >> tmp.g >> tmp.b >> tmp.a)
	{
		value = tmp;
	}
	return is;
}

inline std::wostream& operator << (std::wostream& os, const D3DXMATRIX& value)
{
	for(UINT i=0; i<4; ++i)
	{
		for(UINT j=0; j<4; ++j)
		{
			os << value(i, j);

			if(i < 3 || j < 3)
			{
				os << L" ";
			}
		}
	}
	return os;
}

inline std::wistream& operator >> (std::wistream& is, D3DXMATRIX& value)
{
	D3DXMATRIX tmp;
	for(UINT i=0; i<4; ++i)
	{
		for(UINT j=0; j<4; ++j)
		{
			is >> tmp(i, j);
		}
	}

	if(is)
	{
		value = tmp;
	}
	return is;
}
