// This file is only there to forward some of the print functions to ntdll.dll.
#include <stdarg.h>
#include <memory.h>

int __cdecl snprintf(char* buf, size_t buflen, const char* fmt, ...)
{
	int res;
	va_list args;
	va_start(args, fmt);
	extern __declspec(dllimport) int _vsnprintf(char* buf, size_t buflen, const char* fmt, va_list arg);
	res = _vsnprintf(buf, buflen, fmt, args);
	va_end(args);
	return res;
}

int __cdecl vsnprintf(char* buf, size_t buflen, const char* fmt, va_list arg)
{
	extern __declspec(dllimport) int _vsnprintf(char* buf, size_t buflen, const char* fmt, va_list arg);
	return _vsnprintf(buf, buflen, fmt, arg);
}

extern int nt_free(void* ptr);
extern void* nt_malloc(size_t size);

char* nt_strdup(char* str)
{
	int len = 0;
	char* strptr = str;
	while (*strptr++ != 0)
	{
		len++;
	}
	len++;
	strptr = nt_malloc(len);
	if (!strptr) return NULL;
	memcpy(strptr, str, len);
	return strptr;
}