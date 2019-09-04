#pragma once

#define GFX_MEM_1KB             1024
#define GFX_MEM_KB(capacity)    (capacity                                     * GFX_MEM_1KB)
#define GFX_MEM_MB(capacity)    (GFX_MEM_KB(capacity)                         * GFX_MEM_1KB)
#define GFX_MEM_GB(capacity)    ((GFX_MEM_MB(static_cast<uint64_t>(capacity)) * GFX_MEM_1KB))

#define GFX_CONVERT_TO_POINTER(value)                       ((void*)(value))
#define GFX_ADVANCE_POINTER_BY_OFFSET(pointer, offset)      ((void*)((uintptr_t)GFX_CONVERT_TO_POINTER(pointer) + offset))
#define GFX_POINTER_TO_UINT(pointer)                        ((uintptr_t)(pointer))
#define GFX_ALIGN_POINTER(pointer, base)                    GFX_CONVERT_TO_POINTER(((GFX_POINTER_TO_UINT(pointer))+((base)-1L)) & ~((base)-1L))
#define GFX_IS_ALIGNED(pointer, alignment)                  (((uintptr_t)pointer & (uintptr_t)(alignment - 1L)) == 0)
#define GFX_IS_VALID_RANGE(value, min_value, max_value)     (value >= min_value && value <= max_value)
#define GFX_NEXT_POWER_OF_2(v)                              { (v)--; (v) |= (v) >> 1; (v) |= (v) >> 2; (v) |= (v) >> 4; (v) |= (v) >> 8; (v) |= (v) >> 16; (v)++; }

#if (defined(UINTPTR_MAX) && UINTPTR_MAX == UINT32_MAX)
    #define GFX_SYSTEM_DEFAULT_ALIGNMENT 4
#elif defined(UINTPTR_MAX) && UINTPTR_MAX == UINT64_MAX
    #define GFX_SYSTEM_DEFAULT_ALIGNMENT 8
#else
#error "Invalid Darwin platform architecture"
#endif


// A simplified logging system using macros for C/C++ and Objective-C, from https://github.com/dmcrodrigues/macro-logger
#ifndef __MACROLOGGER_H__
#define __MACROLOGGER_H__
#include <stdio.h>

#ifdef __OBJC__
#import <Foundation/Foundation.h>
#else
#include <time.h>
#include <string.h>
#endif

// === auxiliar functions
static inline char *timenow();

#define _FILE strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__

#define NO_LOG          0x00
#define ERROR_LEVEL     0x01
#define INFO_LEVEL      0x02
#define DEBUG_LEVEL     0x03

#ifndef LOG_LEVEL
#define LOG_LEVEL   DEBUG_LEVEL
#endif

#ifdef __OBJC__

#if __has_feature(objc_arc)
#define AUTORELEASEPOOL_BEGIN   @autoreleasepool {
#define AUTORELEASEPOOL_END     }
#define RELEASE(OBJ)            OBJ = nil
#else
#define AUTORELEASEPOOL_BEGIN   NSAutoreleasePool *_pool = [[NSAutoreleasePool alloc] init];
#define AUTORELEASEPOOL_END     [_pool release];
#define RELEASE(OBJ)            [OBJ release];
#endif

#define PRINTFUNCTION(format, ...)      objc_print(@format, __VA_ARGS__)
#else
#define PRINTFUNCTION(format, ...)      fprintf(stderr, format, __VA_ARGS__)

#endif

#define LOG_FMT             "%s | %-7s | %-15s | %s:%d | "
#define LOG_ARGS(LOG_TAG)   timenow(), LOG_TAG, _FILE, __FUNCTION__, __LINE__

#define NEWLINE     "\n"

#define ERROR_TAG   "ERROR"
#define INFO_TAG    "INFO"
#define DEBUG_TAG   "DEBUG"

#if LOG_LEVEL >= DEBUG_LEVEL
#define LOG_DEBUG(message, args...)     PRINTFUNCTION(LOG_FMT message NEWLINE, LOG_ARGS(DEBUG_TAG), ## args)
#else
#define LOG_DEBUG(message, args...)
#endif

#if LOG_LEVEL >= INFO_LEVEL
#define LOG_INFO(message, args...)      PRINTFUNCTION(LOG_FMT message NEWLINE, LOG_ARGS(INFO_TAG), ## args)
#else
#define LOG_INFO(message, args...)
#endif

#if LOG_LEVEL >= ERROR_LEVEL
#define LOG_ERROR(message, args...)     PRINTFUNCTION(LOG_FMT message NEWLINE, LOG_ARGS(ERROR_TAG), ## args)
#else
#define LOG_ERROR(message, args...)
#endif

#if LOG_LEVEL >= NO_LOGS
#define LOG_IF_ERROR(condition, message, args...) if (condition) PRINTFUNCTION(LOG_FMT message NEWLINE, LOG_ARGS(ERROR_TAG), ## args)
#else
#define LOG_IF_ERROR(condition, message, args...)
#endif

static inline char *timenow() {
    static char buffer[64];
    time_t rawtime;
    struct tm *timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", timeinfo);
    
    return buffer;
}

#ifdef __OBJC__

static inline void objc_print(NSString *format, ...) {
    AUTORELEASEPOOL_BEGIN
    va_list args;
    va_start(args, format);
    NSString *logStr = [[NSString alloc] initWithFormat:format arguments:args];
    fprintf(stderr, "%s", [logStr UTF8String]);
    RELEASE(logStr);
    va_end(args);
    AUTORELEASEPOOL_END
}

#endif

#endif

/********************** C++ Usage ************************
 
 LOG_DEBUG("Maybe i can touch this button...");
 
 LOG_INFO("Pressure is dropping...");
 
 LOG_ERROR("Houston we have a problem!");
 
 int going_down = 1;
 LOG_IF_ERROR(going_down, "i'm going down... if only i had used macro-logger...");
 
 *********************** C++ Usage ************************/

/********************** Objective-C Usage ************************
 
 LOG_DEBUG("Maybe i can touch this button...");
 
 LOG_INFO("Pressure is dropping...");
 
 NSString *baseStation = @"Houston";
 LOG_ERROR(@"%@ we have a problem!", baseStation);
 
 int going_down = 1;
 LOG_IF_ERROR(going_down, "i'm going down... if only i had used macro-logger...");
 
 *********************** Objective-C Usage ************************/
