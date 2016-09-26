/*
 $License:
    Copyright (C) 2011 InvenSense Corporation, All Rights Reserved.
 $
 */

/*
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * C/C++ logging functions.  See the logging documentation for API details.
 *
 * We'd like these to be available from C code (in case we import some from
 * somewhere), so this has a C interface.
 *
 * The output will be correct when the log file is shared between multiple
 * threads and/or multiple processes so long as the operating system
 * supports O_APPEND.  These calls have mutex-protected data structures
 * and so are NOT reentrant.  Do not use MPL_LOG in a signal handler.
 */
#ifndef _LIBS_CUTILS_MPL_LOG_H
#define _LIBS_CUTILS_MPL_LOG_H

#include <stdlib.h>
#include <stdarg.h>


#ifdef __cplusplus
extern "C" {
#endif


/* --------------------------------------------------------------------- */

/*
 * Normally we strip MPL_LOGV (VERBOSE messages) from release builds.
 * You can modify this (for example with "#define MPL_LOG_NDEBUG 0"
 * at the top of your source file) to change that behavior.
 */
#ifndef MPL_LOG_NDEBUG
#ifdef NDEBUG
#define MPL_LOG_NDEBUG 1
#else
#define MPL_LOG_NDEBUG 0
#endif
#endif


#define MPL_LOG_UNKNOWN		(0)
#define MPL_LOG_DEFAULT		(1)
#define MPL_LOG_VERBOSE		(2)
#define MPL_LOG_DEBUG		(3)
#define MPL_LOG_INFO		(4)
#define MPL_LOG_WARN		(5)
#define MPL_LOG_ERROR		(6)
#define MPL_LOG_SILENT		(8)


#define MPL_LOG_TAG NULL


/* --------------------------------------------------------------------- */

/*
 * Simplified macro to send a verbose log message using the current MPL_LOG_TAG.
 */
#ifndef MPL_LOGV
#define MPL_LOGV(fmt, ...) MPL_LOG(LOG_VERBOSE, MPL_LOG_TAG, fmt, ##__VA_ARGS__)
#endif

#ifndef CONDITION
#define CONDITION(cond)     ((cond) != 0)
#endif

#ifndef MPL_LOGV_IF
#if MPL_LOG_NDEBUG
#define MPL_LOGV_IF(cond, fmt, ...)  \
	do { if (0) MPL_LOG(fmt, ##__VA_ARGS__); } while (0)
#else
#define MPL_LOGV_IF(cond, fmt, ...) \
	((CONDITION(cond))						\
		? MPL_LOG(LOG_VERBOSE, MPL_LOG_TAG, fmt, ##__VA_ARGS__) \
		: (void)0)
#endif
#endif

/*
 * Simplified macro to send a debug log message using the current MPL_LOG_TAG.
 */
#ifndef MPL_LOGD
#define MPL_LOGD(fmt, ...) MPL_LOG(LOG_DEBUG, MPL_LOG_TAG, fmt, ##__VA_ARGS__)
#endif

#ifndef MPL_LOGD_IF
#define MPL_LOGD_IF(cond, fmt, ...) \
	((CONDITION(cond))					       \
		? MPL_LOG(LOG_DEBUG, MPL_LOG_TAG, fmt, ##__VA_ARGS__)  \
		: (void)0)
#endif

/*
 * Simplified macro to send an info log message using the current MPL_LOG_TAG.
 */
#ifndef MPL_LOGI
#define MPL_LOGI(fmt, ...) MPL_LOG(LOG_INFO, MPL_LOG_TAG, fmt, ##__VA_ARGS__)
#endif

#ifndef MPL_LOGI_IF
#define MPL_LOGI_IF(cond, fmt, ...) \
	((CONDITION(cond))                                              \
		? MPL_LOG(LOG_INFO, MPL_LOG_TAG, fmt, ##__VA_ARGS__)   \
		: (void)0)
#endif

/*
 * Simplified macro to send a warning log message using the current MPL_LOG_TAG.
 */
#ifndef MPL_LOGW
#define MPL_LOGW(fmt, ...) MPL_LOG(LOG_WARN, MPL_LOG_TAG, fmt, ##__VA_ARGS__)
#endif

#ifndef MPL_LOGW_IF
#define MPL_LOGW_IF(cond, fmt, ...) \
	((CONDITION(cond))					       \
		? MPL_LOG(LOG_WARN, MPL_LOG_TAG, fmt, ##__VA_ARGS__)   \
		: (void)0)
#endif

/*
 * Simplified macro to send an error log message using the current MPL_LOG_TAG.
 */
#ifndef MPL_LOGE
#define MPL_LOGE(fmt, ...) MPL_LOG(LOG_ERROR, MPL_LOG_TAG, fmt, ##__VA_ARGS__)
#endif

#ifndef MPL_LOGE_IF
#define MPL_LOGE_IF(cond, fmt, ...) \
	((CONDITION(cond))					       \
		? MPL_LOG(LOG_ERROR, MPL_LOG_TAG, fmt, ##__VA_ARGS__)  \
		: (void)0)
#endif

/* --------------------------------------------------------------------- */



/* --------------------------------------------------------------------- */

/*
 * Basic log message macro.
 *
 * Example:
 *  MPL_LOG(MPL_LOG_WARN, NULL, "Failed with error %d", errno);
 *
 * The second argument may be NULL or "" to indicate the "global" tag.
 */
#ifndef MPL_LOG
#ifdef REMOVE_LOGGING
#define MPL_LOG(priority, tag, fmt, ...) do {} while (0)
#else
#define MPL_LOG(priority, tag, fmt, ...)		\
	MPL_LOG_PRI(priority, tag, fmt, ##__VA_ARGS__)
#endif
#endif

/*
 * Log macro that allows you to specify a number for the priority.
 */
#ifndef MPL_LOG_PRI
#define MPL_LOG_PRI(priority, tag, fmt, ...) \
	_MLPrintLog(MPL_##priority, tag, fmt, ##__VA_ARGS__)
#endif



/* --------------------------------------------------------------------- */

/*
 * ===========================================================================
 *
 * The stuff in the rest of this file should not be used directly.
 */


int _MLPrintLog(int priority, const char *tag, const char *fmt,	...);


static inline void __print_result_location(int result,
					   const char *file,
					   const char *func, int line)
{
	MPL_LOGE("%s|%s|%d returning %d\n", file, func, line, result);
}

#define LOG_RESULT_LOCATION(condition) \
    do {								\
		__print_result_location((int)(condition), __FILE__,	\
					__func__, __LINE__);		\
	} while (0)


#define INV_ERROR_CHECK(r_1329) \
    if (r_1329) { \
        LOG_RESULT_LOCATION(r_1329); \
        return r_1329; \
    }


#ifdef __cplusplus
}
#endif
#endif				/* _LIBS_CUTILS_MPL_LOG_H */