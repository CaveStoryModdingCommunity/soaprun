#include <StdAfx.h>

#include "dt_Stage.h"

#define _CODE_SIZE 4
static const char* _unit_code = "UNIT";

dt_FldObj* dt_Stage::Unit_GetPointer     ( void ){ return _unit_main; }
dt_FldObj* dt_Stage::Unit_GetPointer_Copy( void ){ return _unit_copy; }

unsigned char dt_Stage::Unit_copy_GetUnitNo ( void )
{
	const DT_FLDOBJ* p_unit;
	if( p_unit = _unit_copy->Get( 0 ) ) return p_unit->param1;
	return 0;
}

bool dt_Stage::Unit_copy_Copy( int x1, int y1, int x2, int y2, BOOL bMapParts )
{
	bool b_ret = false;
	int x, y, w, l, work, u;
	DT_FLDOBJ          unit;
	const DT_FLDOBJ* p_unit;

	Map_GetSize( &w, &l );

	_unit_copy->Clear();

	// ‘å¬“ü‚ê‘Ö‚¦
	if( x1 > x2 ){ work = x1; x1 = x2; x2 = work; }
	if( y1 > y2 ){ work = y1; y1 = y2; y2 = work; }

	if( bMapParts )
	{
		if( x1 < 0 ) x1 = 0; if( x1 > MAPPARTS_NUM_H - 1 ) x1 = MAPPARTS_NUM_H;
		if( y1 < 0 ) y1 = 0; if( y1 > MAPPARTS_NUM_V - 1 ) y1 = MAPPARTS_NUM_V;
		if( x2 < 0 ) x2 = 0; if( x2 > MAPPARTS_NUM_H - 1 ) x2 = MAPPARTS_NUM_H;
		if( y2 < 0 ) y2 = 0; if( y2 > MAPPARTS_NUM_V - 1 ) y2 = MAPPARTS_NUM_V;

		for( y = y1; y <= y2; y++ )
		{
			for( x = x1; x <= x2; x++ )
			{
				memset( &unit, 0, sizeof(DT_FLDOBJ) );
				unit.flags  = UNITRECORD_FLAG_VALID;
				unit.param1 = (unsigned char)( y * MAPPARTS_NUM_H + x );
				unit.x      = (short)( x - x1 );
				unit.y      = (short)( y - y1 );
				if( !_unit_copy->Add( &unit ) ) goto End;
			}
		}

	}
	else
	{
		if( x1 < 0 ) x1 = 0; if( x1 > w*2     - 1 ) x1 = w*2 - 1;
		if( y1 < 0 ) y1 = 0; if( y1 > l*2     - 1 ) y1 = l*2 - 1;
		if( x2 < 0 ) x2 = 0; if( x2 > w*2     - 1 ) x2 = w*2 - 1;
		if( y2 < 0 ) y2 = 0; if( y2 > l*2     - 1 ) y2 = l*2 - 1;


		for( u = 0; p_unit = _unit_main->Get( u ); u++ )
		{
			if( p_unit->x >= x1 && p_unit->x <= x2 &&
				p_unit->y >= y1 && p_unit->y <= y2 )
			{
				memcpy( &unit, p_unit, sizeof(DT_FLDOBJ) );
				unit.x -= (short)x1;
				unit.y -= (short)y1;
				if( !_unit_copy->Add( &unit ) ) goto End;
			}
		}
	}

	b_ret = true;
End:

	if( !b_ret ) _unit_copy->Clear();

	return b_ret;
}


void dt_Stage::Unit_Sort_UP( void )
{
	b_unit_changed = true;
	_unit_main->Sort_Up();
}

bool dt_Stage::Unit_Delete( int x1, int y1, int x2, int y2 )
{
	const DT_FLDOBJ* p_unit;

	b_unit_changed = true;

	for( int u = 0; p_unit = _unit_main->Get( u ); u++ )
	{
		if( p_unit->x >= x1 && p_unit->x <= x2 && p_unit->y >= y1 && p_unit->y <= y2 ){ _unit_main->Delete( u ); u--; }
	}

	return true;
}

int dt_Stage::Unit_Search( int x, int y )
{
	const DT_FLDOBJ* p_unit;

	for( int u = 0; p_unit = _unit_main->Get( u ); u++ )
	{
		if( p_unit->x == x && p_unit->y == y ) return u;
	}
	return -1;
}

bool dt_Stage::Unit_Get( int u, DT_FLDOBJ* p_unit )
{
	const DT_FLDOBJ* p_unit_src = _unit_main->Get( u );
	if( !p_unit_src ) return false;
	*p_unit = *p_unit_src;
	return true;
}

bool dt_Stage::Unit_Set( int u, const DT_FLDOBJ* p_unit )
{
	b_unit_changed = true;
	return _unit_main->Set( p_unit, u );
}

bool dt_Stage::Unit_Add( const DT_FLDOBJ* p_unit )
{
	b_unit_changed = true;
	return _unit_main->Add( p_unit );
}

void dt_Stage::Unit_Shift( int x, int y )
{
	b_unit_changed = true;
	_unit_main->Shift( (short)x, (short)y );
}

void dt_Stage::Unit_Clear()
{
	b_unit_changed = true;
	_unit_main->Clear();
}

bool dt_Stage::Unit_SaveNow()
{
	bool b_ret = false;
	FILE *fp   = NULL;

//	unsigned short size;
//	int            sk1, sk2;

	char path[ MAX_PATH ];

	sprintf( path, "%s\\%s.%s\\%s", _dir_data, _stage_name, _stage_ext, _unit_fname );

	if( !( fp = fopen( path, "wb" ) ) ) goto End;

	// unit ==== ====
//	fwrite( _unit_code, sizeof(char), _CODE_SIZE, fp );
//	size = 0; fwrite( &size, sizeof(unsigned short), 1, fp ); sk1  = ftell( fp );
//	{
		if( !_unit_main->Write( fp ) ) goto End;
//	}
//	sk2  = ftell( fp ); size = sk2 - sk1;
//	fseek ( fp, sk1 - sizeof(unsigned short), SEEK_SET );
//	fwrite( &size,    sizeof(unsigned short), 1, fp    );
//	fseek( fp, sk2, SEEK_SET );

	b_unit_changed = false;

	b_ret = true;
End:
	
	if( fp ) fclose( fp );

	return b_ret;
}

bool dt_Stage::Unit_Load( void )
{
	bool           b_ret = false;
	FILE           *fp   = NULL ;
	char           path[ MAX_PATH ];
//	char           code[ 10 ];
//	unsigned short size      ;

	_unit_main->Clear();

	sprintf( path, "%s\\%s.%s\\%s", _dir_data, _stage_name, _stage_ext, _unit_fname );
	if( !( fp = fopen( path, "rb" ) ) ) goto End;

	// unit ==== ====
//	if( fread( &code, sizeof(char          ), _CODE_SIZE, fp ) != _CODE_SIZE ) goto End;
//	if( fread( &size, sizeof(unsigned short),          1, fp ) !=          1 ) goto End;
	if( !_unit_main->Read( fp ) )goto End;
	b_unit_changed = false;

	b_ret = true  ;
End:

	if( fp ) fclose( fp );

	return b_ret;
}
