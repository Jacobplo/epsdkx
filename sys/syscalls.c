#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/times.h>
#include <sys/errno.h>

#include <epsdkx/drivers/uart.h>
#include <epsdkx/common/uart.h>

#include <epsdkx/generated/config.h>

char **environ;

void _exit(int status) {
  (void)status;

  while (1) (void)0;
}

int _close(int file) {
  (void)file;
  
  errno = ENOSYS;
  return -1;
}

int _execve(char *name, char **argv, char **env) {
  (void)name;
  (void)argv;
  (void)env;

  errno = ENOSYS;
  return -1;
}

int _fork() {
  errno = ENOSYS;
  return -1;
}

int _fstat(int file, struct stat *st) {
  (void)file;
  (void)st;
  
  errno = ENOSYS;
  return -1;
}

int _getpid() {
  errno = ENOSYS;
  return -1;
}

int _isatty(int file) {
  switch (file) {
    case 0: // stdin
    case 1: // stdout
    case 2: // stderr
      return 1;
    default:
      errno = EBADF;
      return 0;
  }
}

int _kill(int pid, int sig) {
  (void) pid;
  (void) sig;

  errno = ENOSYS;
  return -1;
}

int _link(char *old, char *new) {
  (void)old;
  (void)new;

  errno = ENOSYS;
  return -1;
}

int _lseek(int file, int ptr, int dir) {
  (void)file;
  (void)ptr;
  (void)dir;
  
  errno = ENOSYS;
  return -1;
}

int _open(const char *name, int flags, ...) {
  (void)name;
  (void)flags;

  errno = ENOSYS;
  return -1;
}

int _read(int file, char *ptr, int len) {
  (void)file;
  (void)ptr;
  (void)len;
  
  errno = ENOSYS;
  return -1;
}

caddr_t _sbrk(int incr) {
  extern char _end;
  static char *heap_end = 0;
  char *prev_heap_end;

  if (heap_end == 0) heap_end = &_end;
  prev_heap_end = heap_end;
  heap_end += incr;
  return (caddr_t) prev_heap_end;
}

int _stat(const char *file, struct stat *st) {
  (void)file;
  (void)st;

  errno = ENOSYS;
  return -1;
}

clock_t _times(struct tms *buf) {
  (void)buf;

  errno = ENOSYS;
  return -1;
}

int _unlink(char *name) {
  (void)name;

  errno = ENOSYS;
  return -1;
}

int _wait(int *status) {
  (void)status;
  
  errno = ENOSYS;
  return -1;
}


#ifdef CONFIG_UART
static bool uart_ready = false;
#endif
int _write(int file, char *ptr, int len) {
  (void)file;

#ifdef CONFIG_UART
  if (!uart_ready) {
    uart_init(UART(1), 115200);
    uart_ready = true;
  }

  for (int i = 0; i < len; i++) {
    if (ptr[i] == '\n') {
      uart_put(UART(1), '\r');
    }

    uart_put(UART(1), ptr[i]);
  }
#endif
    
  return len;
}
