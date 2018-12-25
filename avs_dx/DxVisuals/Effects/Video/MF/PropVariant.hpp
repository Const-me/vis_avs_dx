#pragma once

class PropVariant
{
	PROPVARIANT var;

public:

	PropVariant()
	{
		PropVariantInit( &var );
	}
	PropVariant( __int64 i8 )
	{
		PropVariantInit( &var );
		var.vt = VT_I8;
		var.hVal.QuadPart = i8;
	}

	~PropVariant()
	{
		PropVariantClear( &var );
	}

	operator const PROPVARIANT *( ) const { return &var; }
};