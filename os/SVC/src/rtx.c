/* @brief:  rtx.c kernel API implementations, this is a skeleton only
 * @author: Yiqing Huang
 * @date:   2013/01/06
 */

#include "uart_polling.h"
#include "process.h"
#ifdef DEBUG_0
#include <stdio.h>
#endif // DEBUG_0

/* 
  To better strcture your code, you may want to split these functions
  into different files. For example, memory related kernel APIs in one file
  and process related API in another file.
*/

/* 
  This symbol is defined in the scatter file, 
  refer to RVCT Linker User Guide
*/  
extern unsigned int Image$$RW_IRAM1$$ZI$$Limit;  
