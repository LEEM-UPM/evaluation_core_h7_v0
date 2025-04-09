#ifndef LX_STM32_NAND_SIMULATOR_DRIVER_H
#define LX_STM32_NAND_SIMULATOR_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lx_api.h"

#define TOTAL_BLOCKS                        8
#define PHYSICAL_PAGES_PER_BLOCK            16          /* Min value of 2                                               */
#define BYTES_PER_PHYSICAL_PAGE             2048        /* 2048 bytes per page                                          */ 
#define WORDS_PER_PHYSICAL_PAGE             512         /* Words per page                                               */ 
#define SPARE_BYTES_PER_PAGE                64          /* 64 "spare" bytes per page                                    */
                                                        /* For 2048 byte block spare area:                              */ 
#define BAD_BLOCK_POSITION                  0           /*      0 is the bad block byte postion                         */ 
#define EXTRA_BYTE_POSITION                 2           /*      2 is the extra bytes starting byte postion              */ 
#define ECC_BYTE_POSITION                   40          /*      40 is the ECC starting byte position                    */ 

UINT lx_stm32_nand_simulator_initialize(LX_NAND_FLASH *nand_flash);

#ifdef __cplusplus
}
#endif

#endif /* LX_STM32_NAND_SIMULATOR_DRIVER_H */

