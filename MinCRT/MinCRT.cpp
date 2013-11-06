// MinCRT.cpp : Defines the entry point for the console application.
//
#ifdef WIN32
  #include <Windows.h>
#endif


extern int main(int argc, char *argv[]);
void exit(int);

static void crt_fatal_error(const char *msg)
{
	//printf("fatal error: %s\n", msg);
	exit(1);
}


void spch_start()
{
	int ret = 0;

#ifdef WIN32
	const int NParms = 16;
	char *p = GetCommandLineA();
	int argc = 0;
	char *argv[NParms];
	
	//first, *p can not be '\0'，at lease the name of program
	while( true )
	{
		//eat the white space
		while( *p == ' ')
			++p;
		//handle "MinCRT.exe  aa bb    \0"
		if( *p != '\0' )
			argv[argc++] = p;

		//find the white space as separator
		while( *p != '\0' && *p != ' ')
			++p;
		if( *p == '\0') break;

		//one param end
		*p++ = '\0';
	}
#else
	int argc;
	char **argv;

	char *ebp_reg = 0;
	asm("movl %%ebp, %0 \n" : "=r"(ebp_reg));
	//汇编理解 http://argcv.com/articles/84.c

	argc =  *(int*)(ebp_req+4);
	argv = (char**)(ebp_req + 8);
	// ebp_req point to ebp

#endif
    /*
	if( !crt_heap_init() )
		crt_fatal_error("heap initialize failed");

	if( !crt_io_init() )
		crt_fatal_error("IO initialize failed");
    */
	ret = main(argc, argv);
	exit(ret);
}

void exit(int exitCode)
{
#ifdef WIN32
	ExitProcess(exitCode);
#else
	asm( "movl %0, %%ebx \n\t"
		 "movl $1, %%eax \n\t"
		 "int $ox80 \n\t"
		 "hlt       \n\t" :: "m"(exitCode));

	// \n\t 解释： http://blog.csdn.net/robin_qiu/article/details/613904
#endif
}

//注意：
//windows平台下，字符串通过GetCommandLineA得到，
//而linux平台下，加载器会自动放于栈上