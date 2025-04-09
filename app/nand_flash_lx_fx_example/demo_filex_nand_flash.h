#ifndef __DEMO_FILEX_NAND_FLASH_H__
#define __DEMO_FILEX_NAND_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "fx_api.h"
#include "fx_stm32_levelx_nand_driver.h"
#include "lx_stm32_nand_simulator_driver.h"

#define DEMO_STACK_SIZE 4096

#define SECTOR_SIZE     BYTES_PER_PHYSICAL_PAGE /* sector size is always the page size of the underlying NAND hardware */

#define TOTAL_SECTORS   (TOTAL_BLOCKS * PHYSICAL_PAGES_PER_BLOCK)

#ifdef __cplusplus
}
#endif

#endif /* __DEMO_FILEX_NAND_FLASH_H__ */