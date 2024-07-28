#include <stdio.h>     // FILE / printf() / perror() / fopen() / fclose() / sprintf() / fprintf() / fread()
#include <stdlib.h>    // mode_t / exit() / malloc() / atoi()
#include <fcntl.h>     // O_WRONLY / S_IRWXU etc..
#include <sys/wait.h>  // waitpid() / signal() / SIG_XXX etc..
#include <sys/stat.h>  // umask() / stat() / chmod()

#include <unistd.h>    // _exit() / readlink() / unlink() / sleep() / setsid() / close() / dup2()
#include <pthread.h>  // pthread_xxx()

#include "DebugLog.h"



bool Daemon_Set( void )
{
	switch( fork() )
	{
		case -1: perror( "fork" ); return false;
		case  0: setsid(); break;  // 子プロセスはターミナルから切り離す
		default: exit(0);          // 親プロセスは終了させる
	}

	// 標準(エラー)入出力を閉じて、/dev/nullで埋めておく
	close( 0 ); // STDIN_FILENO ?
	close( 1 ); // STDOUT_FILENO?
	close( 2 ); // STDERR_FILENO?

	open( "/dev/null", O_RDWR );
	dup2( 0, 1 ); // STDOUT_FILENO?
	dup2( 0, 2 ); // STDERR_FILENO?

	// 不要なシグナルを無効にする
	signal( SIGCHLD, SIG_IGN ); //ゾンビ止め

	umask( 0 ); // 親のMASKを引き継がない

	return true;
}

