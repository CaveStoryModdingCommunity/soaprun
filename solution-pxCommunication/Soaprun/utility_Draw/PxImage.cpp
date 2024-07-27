#include <StdAfx.h>

#include "../../Dx/DxDraw.h"


BOOL PxImage_Load( const char* name, long surface_no, BOOL bExterior )
{
	char path[ MAX_PATH ];

	if( bExterior )
	{
//#ifdef NDEBUG
//		sprintf( path, "%s.pximg", name );
//#else
		sprintf( path, "%s.bmp",   name );
//#endif
	}
	else
	{
		sprintf( path, "BMP_%s", name );
	}

	DxDraw_SubSurface_Release( surface_no );
	return DxDraw_SubSurface_Load( path, surface_no, bExterior, FALSE );
}

BOOL PxImage_Reload( const char* name, long surface_no, BOOL bExterior )
{
	char path[ MAX_PATH ];

	if( bExterior )
	{
//#ifdef NDEBUG
//		sprintf( path, "%s.pximg", name );
//#else
		sprintf( path, "%s.bmp",   name );
//#endif
	}
	else
	{
		sprintf( path, "BMP_%s", name );
	}

	return DxDraw_SubSurface_Reload( path, surface_no, bExterior );
}
