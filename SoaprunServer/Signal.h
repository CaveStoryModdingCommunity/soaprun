
#ifdef NDEBUG

extern bool g_exit;

#else

#include <sys/wait.h> // waitpid() / signal() / SIG_XXX etc..

bool Signal_Mask( sigset_t *p_ss );
bool Signal_CreateThread();
bool Signal_JoinThread  ();

extern volatile sig_atomic_t g_exit;

#endif

