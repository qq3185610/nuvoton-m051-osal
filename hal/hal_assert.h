/******************************************************************************

 @file  hal_assert.h

 @brief Describe the purpose and contents of the file.

 Group: 
 Target Device: 

 ******************************************************************************

 ******************************************************************************
 Release Name: 
 Release Date: 
 *****************************************************************************/

#ifndef __HAL_ASSERT_H__
#define __HAL_ASSERT_H__

#include "hal_config.h"

/* ------------------------------------------------------------------------------------------------
 *                                           Macros
 * ------------------------------------------------------------------------------------------------
 */

/*
 *  HAL_ASSERT( expression ) - The given expression must evaluate as "true" or else the assert
 *  handler is called.  From here, the call stack feature of the debugger can pinpoint where
 * the problem occurred.
 *
 *  HAL_ASSERT_FORCED( ) - If asserts are in use, immediately calls the assert handler.
 *
 *  HAL_ASSERT_STATEMENT( statement ) - Inserts the given C statement but only when asserts
 *  are in use.  This macros allows debug code that is not part of an expression.
 *
 *  HAL_ASSERT_DECLARATION( declaration ) - Inserts the given C declaration but only when asserts
 *  are in use.  This macros allows debug code that is not part of an expression.
 *
 *  Asserts can be disabled for optimum performance and minimum code size (ideal for
 *  finalized, debugged production code).  To disable, define the preprocessor
 *  symbol HALNODEBUG at the project level.
 */


#if ( HAL_ASSERT_EN == 0 )
#define HAL_ASSERT(expr)
#define HAL_ASSERT_FORCED()
#define HAL_ASSERT_STATEMENT(statement)
#define HAL_ASSERT_DECLARATION(declaration)
#else
#define HAL_ASSERT(expr)                        do{ if (!( expr )) HalAssertHandler(); } while(__LINE__ == -1)
#define HAL_ASSERT_FORCED()                     HalAssertHandler()
#define HAL_ASSERT_STATEMENT(statement)         st( statement )
#define HAL_ASSERT_DECLARATION(declaration)     declaration
#endif

/* ------------------------------------------------------------------------------------------------
 *                                          Prototypes
 * ------------------------------------------------------------------------------------------------
 */
extern void HalAssertHandler(void);


/**************************************************************************************************
 */

/**************************************************************************************************
 *                                        FUNCTIONS - API
 **************************************************************************************************/

#endif

