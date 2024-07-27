#include <StdAfx.h>

#include "../Fixture/DebugLog.h"
#include "../Fixture/MessageBox.h"

#include "Scene.h"
#include "loop.h"

Scene::Scene( SCENE scene )
{
	memset( _err_msg, 0, 260 );
	_b_error = false;
	_b_reset = true ;
	_scene   = scene;
}

void  Scene::Set( SCENE scene )
{
//	Loop_Reset();
	if( !_b_error ) _scene = scene;
}
SCENE Scene::Get()
{
	return _scene;
}

bool Scene:: IsError()
{
	return _b_error;
}

void Scene::SetError( const char* fmt, ... )
{
	if( _b_error ) return;
	_b_error = true;
	_scene   = SCENE_Exit;
	va_list ap; va_start( ap, fmt ); vsprintf( _err_msg, fmt, ap ); va_end( ap );
	dlog( "ERR: %s", _err_msg );

//	mbox_ERR( _err_msg );
}

void Scene::Reset_Set()
{
	_b_reset = true;
}

bool Scene::Reset_Is()
{
	return _b_reset;
}
void Scene::Reset_Remove()
{
	_b_reset = false;
}
