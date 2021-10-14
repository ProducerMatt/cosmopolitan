#ifndef COSMOPOLITAN_LIBC_LOG_LIBFATAL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_LOG_LIBFATAL_INTERNAL_H_
#include "libc/dce.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/nt/process.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/nr.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern char __fatalbuf[];

void __printf(const char *, ...);

forceinline void __sysv_exit(long rc) {
  asm volatile("call\t__syscall__"
               : /* no outputs */
               : "a"(__NR_exit_group), "D"(rc)
               : "memory", "cc");
}

forceinline int __sysv_close(long fd) {
  long ax;
  asm volatile("call\t__syscall__"
               : "=a"(ax)
               : "0"(__NR_close), "D"(fd)
               : "rdx", "memory", "cc");
  return ax;
}

forceinline int __sysv_open(const char *path, long flags, long mode) {
  long ax, dx;
  asm volatile("call\t__syscall__"
               : "=a"(ax), "=d"(dx)
               : "0"(__NR_open), "D"(path), "S"(flags), "1"(mode)
               : "memory", "cc");
  return ax;
}

forceinline long __sysv_read(long fd, void *data, unsigned long size) {
  long ax, dx;
  asm volatile("call\t__syscall__"
               : "=a"(ax), "=d"(dx)
               : "0"(__NR_read), "D"(fd), "S"(data), "1"(size)
               : "memory", "cc");
  return ax;
}

forceinline long __sysv_write(long fd, const void *data, unsigned long size) {
  long ax, dx;
  asm volatile("call\t__syscall__"
               : "=a"(ax), "=d"(dx)
               : "0"(__NR_write), "D"(fd), "S"(data), "1"(size)
               : "memory", "cc");
  return ax;
}

forceinline long __sysv_mprotect(void *addr, size_t size, long prot) {
  long ax, dx;
  asm volatile("call\t__syscall__"
               : "=a"(ax), "=d"(dx)
               : "0"(__NR_mprotect), "D"(addr), "S"(size), "1"(prot)
               : "memory", "cc");
  return ax;
}

forceinline int __sysv_getpid(void) {
  long ax;
  asm volatile("call\t__syscall__"
               : "=a"(ax)
               : "0"(__NR_getpid)
               : "rdx", "memory", "cc");
  return ax;
}

forceinline int __getpid(void) {
  if (!IsWindows()) {
    return __sysv_getpid();
  } else {
    return GetCurrentProcessId();
  }
}

forceinline ssize_t __write(const void *p, size_t n) {
  char cf;
  ssize_t rc;
  uint32_t wrote;
  if (!IsWindows()) {
    return __sysv_write(2, p, n);
  } else if (WriteFile(GetStdHandle(kNtStdErrorHandle), p, n, &wrote, 0)) {
    return wrote;
  } else {
    return -GetLastError();
  }
}

forceinline size_t __strlen(const char *s) {
  size_t i = 0;
  while (s[i]) ++i;
  return i;
}

forceinline ssize_t __write_str(const char *s) {
  return __write(s, __strlen(s));
}

forceinline int __strcmp(const char *l, const char *r) {
  size_t i = 0;
  while (l[i] == r[i] && r[i]) ++i;
  return (l[i] & 255) - (r[i] & 255);
}

forceinline char *__stpcpy(char *d, const char *s) {
  size_t i;
  for (i = 0;; ++i) {
    if (!(d[i] = s[i])) {
      return d + i;
    }
  }
}

forceinline void *__repstosb(void *di, char al, size_t cx) {
  asm("rep stosb"
      : "=D"(di), "=c"(cx), "=m"(*(char(*)[cx])di)
      : "0"(di), "1"(cx), "a"(al));
  return di;
}

forceinline void *__repmovsb(void *di, void *si, size_t cx) {
  asm("rep movsb"
      : "=D"(di), "=S"(si), "=c"(cx), "=m"(*(char(*)[cx])di)
      : "0"(di), "1"(si), "2"(cx), "m"(*(char(*)[cx])si));
  return di;
}

forceinline void *__mempcpy(void *d, const void *s, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    ((char *)d)[i] = ((const char *)s)[i];
  }
  return (char *)d + n;
}

forceinline char *__uintcpy(char p[hasatleast 21], uint64_t x) {
  char t;
  size_t i, a, b;
  i = 0;
  do {
    p[i++] = x % 10 + '0';
    x = x / 10;
  } while (x > 0);
  p[i] = '\0';
  if (i) {
    for (a = 0, b = i - 1; a < b; ++a, --b) {
      t = p[a];
      p[a] = p[b];
      p[b] = t;
    }
  }
  return p + i;
}

forceinline char *__intcpy(char p[hasatleast 21], int64_t x) {
  if (x < 0) *p++ = '-', x = -(uint64_t)x;
  return __uintcpy(p, x);
}

forceinline char *__fixcpy(char p[hasatleast 17], uint64_t x, uint8_t k) {
  while (k > 0) *p++ = "0123456789abcdef"[(x >> (k -= 4)) & 15];
  *p = '\0';
  return p;
}

forceinline char *__hexcpy(char p[hasatleast 17], uint64_t x) {
  return __fixcpy(p, x, ROUNDUP(x ? bsrl(x) + 1 : 1, 4));
}

forceinline const void *__memchr(const void *s, unsigned char c, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    if (((const unsigned char *)s)[i] == c) {
      return (const unsigned char *)s + i;
    }
  }
  return 0;
}

forceinline char *__strstr(const char *haystack, const char *needle) {
  size_t i;
  for (;;) {
    for (i = 0;; ++i) {
      if (!needle[i]) return (/*unconst*/ char *)haystack;
      if (!haystack[i]) break;
      if (needle[i] != haystack[i]) break;
    }
    if (!*haystack++) break;
  }
  return 0;
}

forceinline char *__getenv(char **p, const char *s) {
  size_t i, j;
  if (p) {
    for (i = 0; p[i]; ++i) {
      for (j = 0;; ++j) {
        if (!s[j]) {
          if (p[i][j] == '=') {
            return p[i] + j + 1;
          }
          break;
        }
        if (s[j] != p[i][j]) {
          break;
        }
      }
    }
  }
  return 0;
}

forceinline unsigned long __atoul(const char *p) {
  int c;
  unsigned long x = 0;
  while ('0' <= (c = *p++) && c <= '9') x *= 10, x += c - '0';
  return x;
}

forceinline long __atol(const char *p) {
  int s = *p;
  unsigned long x;
  if (s == '-' || s == '+') ++p;
  x = __atoul(p);
  if (s == '-') x = -x;
  return x;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_LOG_LIBFATAL_INTERNAL_H_ */