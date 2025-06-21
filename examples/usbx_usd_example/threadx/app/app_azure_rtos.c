#include "app_azure_rtos.h"
// #include "stm32h7xx.h"

#if (USE_STATIC_ALLOCATION == 1)

#if defined ( __ICCARM__ )
#pragma data_alignment=4
#endif
__ALIGN_BEGIN static UCHAR tx_byte_pool_buffer[TX_APP_MEM_POOL_SIZE] __ALIGN_END;
static TX_BYTE_POOL tx_app_byte_pool;

__ALIGN_BEGIN static UCHAR ux_device_byte_pool_buffer[UX_DEVICE_APP_MEM_POOL_SIZE] __ALIGN_END;
static TX_BYTE_POOL ux_device_app_byte_pool;

#endif

/**
  * @brief  Define the initial system.
  * @param  first_unused_memory : Pointer to the first unused memory
  * @retval None
  */
VOID tx_application_define(VOID *first_unused_memory)
{
#if (USE_STATIC_ALLOCATION == 1)
  UINT status = TX_SUCCESS;
  VOID *memory_ptr;

  if (tx_byte_pool_create(&tx_app_byte_pool, "Tx App memory pool", tx_byte_pool_buffer, TX_APP_MEM_POOL_SIZE) != TX_SUCCESS)
  {
    /* TODO */
  }
  else
  {
    memory_ptr = (VOID *)&tx_app_byte_pool;
    status = app_threadx_init(memory_ptr);
    if (status != TX_SUCCESS)
    {
      while(1) {}
    }
  }

  if (tx_byte_pool_create(&ux_device_app_byte_pool, "Ux App memory pool", ux_device_byte_pool_buffer, UX_DEVICE_APP_MEM_POOL_SIZE) != TX_SUCCESS)
  {
    /* TODO */
  }
  else
  {
    memory_ptr = (VOID *)&ux_device_app_byte_pool;
    status = usbx_device_init(memory_ptr);
    if (status != UX_SUCCESS)
    {
      while(1) {}
    }
  }

#else
  /*
   * Using dynamic memory allocation requires to apply some changes to the linker file.
   * ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
   * using the "first_unused_memory" argument.
   * This require changes in the linker files to expose this memory location.
   * For EWARM add the following section into the .icf file:
       place in RAM_region    { last section FREE_MEM };
   * For MDK-ARM
       - either define the RW_IRAM1 region in the ".sct" file
       - or modify the line below in "tx_initialize_low_level.S to match the memory region being used
          LDR r1, =|Image$$RW_IRAM1$$ZI$$Limit|

   * For STM32CubeIDE add the following section into the .ld file:
       ._threadx_heap :
         {
            . = ALIGN(8);
            __RAM_segment_used_end__ = .;
            . = . + 64K;
            . = ALIGN(8);
          } >RAM_D1 AT> RAM_D1
      * The simplest way to provide memory for ThreadX is to define a new section, see ._threadx_heap above.
      * In the example above the ThreadX heap size is set to 64KBytes.
      * The ._threadx_heap must be located between the .bss and the ._user_heap_stack sections in the linker script.
      * Caution: Make sure that ThreadX does not need more than the provided heap memory (64KBytes in this example).
      * Read more in STM32CubeIDE User Guide, chapter: "Linker script".

   * The "tx_initialize_low_level.S" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.
   */

  (void)first_unused_memory;
#endif
}
