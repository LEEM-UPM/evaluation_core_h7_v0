/* This is a small demo of the high-performance FileX FAT file system with LevelX 
and the NAND simulated driver. */
#include "demo_filex_nand_flash.h"
#include "gpio.h"
#include "usart.h"
#include "tx_api.h"

/* Buffer for FileX FX_MEDIA sector cache. This must be large enough for at least one
sector, which are typically 512 bytes in size. */
UCHAR media_memory[4096];
     
/* Define thread prototypes.  */
VOID thread_0_entry(ULONG thread_input);
UCHAR thread_0_stack[DEMO_STACK_SIZE];

/* Define FileX global data structures. */
FX_MEDIA nand_disk;
FX_FILE my_file;

/* Define ThreadX global data structures.  */
TX_THREAD thread_0;
ULONG thread_0_counter;

extern void SystemClock_Config(void);
extern UINT  _lx_nand_flash_simulator_erase_all(VOID);

int  main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_UART4_Init();

    tx_kernel_enter();

    while(1)
    {
        HAL_GPIO_TogglePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
        HAL_GPIO_TogglePin(RED_LED_GPIO_Port, RED_LED_Pin);
        HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_1);
        HAL_Delay(1000);
    }
}

/* Define what the initial system looks like. */
void tx_application_define(void *first_unused_memory)
{
    TX_PARAMETER_NOT_USED(first_unused_memory);

    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,
                     thread_0_stack, DEMO_STACK_SIZE,
                     1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);
    
    fx_system_initialize();
}

void thread_0_entry(ULONG thread_input)
{
    UINT status;
    ULONG actual;
    CHAR local_buffer[30];

    TX_PARAMETER_NOT_USED(thread_input);

    _lx_nand_flash_simulator_erase_all();
        
    /* Format the NAND disk - the memory for the NAND flash disk is setup in 
       the NAND simulator. Note that for best performance, the format of the
       NAND flash should be less than one full NAND flash block of sectors. */
    status = fx_media_format(&nand_disk, 
                    fx_stm32_levelx_nand_driver, // Driver entry
                    FX_NULL,                     // Unused
                    media_memory,                // Media buffer pointer
                    sizeof(media_memory),        // Media buffer size
                    "NAND_DISK",                 // Volume Name
                    1,                           // Number of FATs
                    32,                          // Directory Entries
                    0,                           // Hidden sectors
                    TOTAL_SECTORS,               // Total sectors 
                    SECTOR_SIZE,                 // Sector size
                    1,                           // Sectors per cluster
                    1,                           // Heads
                    1);                          // Sectors per track

    if (status != FX_SUCCESS)
    {
        return;
    }

    do
    {
        status =  fx_media_open(&nand_disk, "NAND_DISK", fx_stm32_levelx_nand_driver, FX_NULL, media_memory, sizeof(media_memory));
        if (status != FX_SUCCESS)
        {
            break;
        }

        status = fx_file_create(&nand_disk, "TEST.TXT");
        if (status != FX_SUCCESS && status != FX_ALREADY_CREATED)
        {
            break;
        }

        status =  fx_file_open(&nand_disk, &my_file, "TEST.TXT", FX_OPEN_FOR_WRITE);
        if (status != FX_SUCCESS)
        {
            break;
        }

        status =  fx_file_seek(&my_file, 0);
        if (status != FX_SUCCESS)
        {
            break;
        }

        status =  fx_file_write(&my_file, " ABCDEFGHIJKLMNOPQRSTUVWXYZ\r\n", 29);
        if (status != FX_SUCCESS)
        {
            break;
        }

        status =  fx_file_seek(&my_file, 0);
        if (status != FX_SUCCESS)
        {
            break;
        }

        status =  fx_file_read(&my_file, local_buffer, 29, &actual);
        if ((status != FX_SUCCESS) || (actual != 29))
        {
            break;
        }

        HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);

        if (HAL_UART_Transmit(&huart4, (uint8_t *)local_buffer, 29, HAL_MAX_DELAY) != HAL_OK)
        {
            break;
        }

        status =  fx_file_close(&my_file);
        if (status != FX_SUCCESS)
        {
            break;
        }

        status =  fx_file_delete(&nand_disk, "TEST.TXT");
        if (status != FX_SUCCESS)
        {
            break;
        }

        status =  fx_media_close(&nand_disk);
        if (status != FX_SUCCESS)
        {
            break;
        }

        thread_0_counter++;
        tx_thread_sleep(100);
        HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_RESET);
        tx_thread_sleep(100);
    } while (1);

    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1, GPIO_PIN_SET);
        
    return;
}