/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2010
 *
 *    File name   : low_level_init.c
 *    Description : Low level init procedure
 *
 *    History :
 *    1. Date        : 08, September 2010
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *
 *    $Revision: #2 $
 **************************************************************************/
#include "types.h"
#include <derivative.h>

/*************************************************************************
 * Function Name: low_level_init
 * Parameters: none
 *
 * Return: none
 *
 * Description: This function is used for low level initialization
 *
 *************************************************************************/
int32_t __low_level_init(void)
{
  uint32_t reg;
  /*Disable WWDT*/
  reg = HW_WDOG_STCTRLH_RD();
  reg &= ~1UL;
  /*Unlock sequence*/
  HW_WDOG_UNLOCK_WR(0xC520);
  HW_WDOG_UNLOCK_WR(0xD928);
  HW_WDOG_STCTRLH_WR(reg);
  /* perform initialization*/
  return 1;
}

