#include <stdio.h>    // FILE / printf() / perror()
#include <limits.h>   // PATH_MAX / NAME_MAX
#include <string.h>   // strcmp() / strerror() / strcpy() / memset() / strlen()
#include <unistd.h>   // usleep()


#define MAX_PATH (PATH_MAX+NAME_MAX)

#define _MAXATTENDER 10

#include "DebugLog.h"
#include "svAttender.h"
#include "svListener.h"

#define _PORTNO 1001

static 	svListener *lstn = NULL;//( _PORTNO );

bool Loop_Main()
{
	bool b_ret = false;

	lstn = new svListener( _PORTNO );
//	svAttender atnd( 3 );

//	atnd.WaitExit();

	lstn->WaitExit();

	b_ret = true;
End:

	return b_ret;
}

void Loop_Main_Kill()
{
	lstn->Kill();
}
