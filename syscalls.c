#include <errno.h>
#include <sys/stat.h>

extern int __io_putchar(int ch) __attribute__ ((weak));
extern int __io_getchar(void) __attribute__ ((weak));

int _getpid()
{
    return -1;
}

int _kill(int /*pid*/, int /*signal*/)
{
    errno = EINVAL;
    return -1;
}

void _exit(int /*status*/)
{
    errno = EINVAL;
    while (1) {}
}

int _close(int /*file*/)
{
    return -1;
}


int _fstat(int /*file*/, struct stat* st)
{
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int /*file*/)
{
    return 1;
}

int _lseek(int /*file*/, int /*ptr*/, int /*dir*/)
{
    return 0;
}

__attribute__ ((weak)) int _write(int /*file*/, char *ptr, int len)
{
    for (int i = 0; i < len; ++i)
        __io_putchar(*ptr++);
    return len;
}

__attribute__ ((weak)) int _read(int /*file*/, char *ptr, int len)
{
    for (int DataIdx = 0; DataIdx < len; DataIdx++)
        *ptr++ = (char)__io_getchar();

    return len;
}
