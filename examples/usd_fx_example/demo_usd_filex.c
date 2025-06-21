#include "demo_usd_filex.h"
#include "gpio.h"
#include "sdmmc.h"
#include "tx_api.h"

#define DEFAULT_QUEUE_LENGTH 16

#define MEDIA_CLOSED 1UL
#define MEDIA_OPENED 0UL

typedef enum
{
    CARD_STATUS_CHANGED = 99,
    CARD_STATUS_DISCONNECTED = 88,
    CARD_STATUS_CONNECTED = 77
} SD_ConnectionStateTypeDef;

TX_THREAD fx_app_thread;
UCHAR fx_app_thread_stack[FX_APP_THREAD_STACK_SIZE];

TX_QUEUE tx_msg_queue;
ULONG tx_queue_stack[DEFAULT_QUEUE_LENGTH];

FX_MEDIA sdio_disk;
FX_FILE fx_file;
// Buffer for Filex FX_MEDIA sector cache
ALIGN_32BYTES(uint32_t fx_sd_media_memory[FX_STM32_SD_DEFAULT_SECTOR_SIZE / sizeof(uint32_t)]);

static UINT media_status = MEDIA_CLOSED;

void fx_app_thread_entry(ULONG thread_input);
extern void SystemClock_Config(void);

static UINT sd_is_detected(uint32_t instance);
static VOID media_close_callback(FX_MEDIA *media_ptr);

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SDMMC1_SD_Init();

    tx_kernel_enter();

    while(1)
    {
        HAL_GPIO_TogglePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
        HAL_GPIO_TogglePin(RED_LED_GPIO_Port, RED_LED_Pin);
        HAL_GPIO_TogglePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin);
        HAL_Delay(1000);
    }
}

/* Define what the initial system looks like. */
void tx_application_define(void *first_unused_memory)
{
    TX_PARAMETER_NOT_USED(first_unused_memory);

    tx_thread_create(&fx_app_thread, "filex_usd_app_thread", fx_app_thread_entry, 0,
                     fx_app_thread_stack, FX_APP_THREAD_STACK_SIZE,
                     10, 10, TX_NO_TIME_SLICE, TX_AUTO_START);

    tx_queue_create(&tx_msg_queue, "sd_event_queue", 1, tx_queue_stack, DEFAULT_QUEUE_LENGTH * sizeof(ULONG));
    
    fx_system_initialize();
}

void fx_app_thread_entry(ULONG thread_input)
{
    TX_PARAMETER_NOT_USED(thread_input);

    UINT status = FX_SUCCESS;
    ULONG r_msg;
    ULONG s_msg = CARD_STATUS_CHANGED;
    ULONG last_status = CARD_STATUS_DISCONNECTED;
    ULONG bytes_read;
    CHAR read_buffer[32];
    CHAR data[] = "This is Filex working on STM32";

    fx_media_close_notify_set(&sdio_disk, media_close_callback);

    if (sd_is_detected(FX_STM32_SD_INSTANCE) == HAL_OK)
    {
        /* SD card is already inserted, place the info into the queue */
        tx_queue_send(&tx_msg_queue, &s_msg, TX_NO_WAIT);
    }
    else
    {
        /* Indicate that SD card is not inserted from start */
        HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
    }

    /* Infinite Loop */
    while (1)
    {
        /* We wait here for a valid SD card insertion event, if it is not inserted already */
        while (1)
        {
            while (tx_queue_receive(&tx_msg_queue, &r_msg, TX_TIMER_TICKS_PER_SECOND / 2) != TX_SUCCESS)
            {
                /* Toggle GREEN LED to indicate idle state after a successful operation */
                if (last_status == CARD_STATUS_CONNECTED)
                {
                    HAL_GPIO_TogglePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
                }
            }

            /* check if we received the correct event message */
            if (r_msg == CARD_STATUS_CHANGED)
            {
                /* reset the status */
                r_msg = 0;

                /* for debouncing purpose we wait a bit till it settles down */
                tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);

                if (sd_is_detected(FX_STM32_SD_INSTANCE) == HAL_OK)
                {
                    /* We have a valid SD insertion event, start processing.. */
                    /* Update last known status */
                    last_status = CARD_STATUS_CONNECTED;
                    HAL_GPIO_WritePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin, GPIO_PIN_SET);
                    break;
                }
                else
                {
                    /* Update last known status */
                    last_status = CARD_STATUS_DISCONNECTED;
                    HAL_GPIO_WritePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_RESET);
                }
            }
        }

        /* Create a file called STM32.TXT in the root directory.  */
        if (media_status == MEDIA_CLOSED)
        {
            status = fx_media_open(&sdio_disk, "usd_disk", fx_stm32_sd_driver, (VOID *)FX_NULL, (VOID *)fx_sd_media_memory, sizeof(fx_sd_media_memory));
            if (status != FX_SUCCESS)
            {
                return;
            }
            
            HAL_GPIO_TogglePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
            media_status = MEDIA_OPENED;
        }
        
        status = fx_file_create(&sdio_disk, "STM32.TXT");
        if (status != FX_SUCCESS && status != FX_ALREADY_CREATED)
        {
            return;
        }

        status = fx_file_open(&sdio_disk, &fx_file, "STM32.TXT", FX_OPEN_FOR_WRITE);
        if (status != FX_SUCCESS)
        {
            return;
        }

        status = fx_file_seek(&fx_file, 0);
        if (status != FX_SUCCESS)
        {
            return;
        }

        status = fx_file_write(&fx_file, data, strlen(data));
        if (status != FX_SUCCESS)
        {
            return;
        }

        status = fx_file_close(&fx_file);
        if (status != FX_SUCCESS)
        {
            return;
        }

        status = fx_media_flush(&sdio_disk);
        if (status != FX_SUCCESS)
        {
            return;
        }

        status = fx_file_open(&sdio_disk, &fx_file, "STM32.TXT", FX_OPEN_FOR_READ);
        if (status != FX_SUCCESS)
        {
            break;
        }

        status = fx_file_seek(&fx_file, 0);
        if (status != FX_SUCCESS)
        {
            break;
        }

        status = fx_file_read(&fx_file, read_buffer, sizeof(data), &bytes_read);
        if ((status != FX_SUCCESS) || (bytes_read != sizeof(data)))
        {
            break;
        }

        status = fx_file_close(&fx_file);
        if (status != FX_SUCCESS)
        {
            break;
        }

        status = fx_media_close(&sdio_disk);
        if (status != FX_SUCCESS)
        {
            break;
        }
    }
}

/**
 * @brief  Detects if SD card is correctly plugged in the memory slot or not.
 * @param Instance  SD Instance
 * @retval Returns if SD is detected or not
 */
static UINT sd_is_detected(uint32_t instance)
{
    UINT ret;

    if (instance >= 1)
    {
        ret = HAL_ERROR;
    }
    else
    {
        /* Check SD card detect pin */
        if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_Port, SD_DETECT_Pin) == GPIO_PIN_SET)
        {
            ret = HAL_ERROR;
        }
        else
        {
            ret = HAL_OK;
        }
    }

    return ret;
}

/**
 * @brief  EXTI line detection callback.
 * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    ULONG s_msg = CARD_STATUS_CHANGED;

    if (GPIO_Pin == SD_DETECT_Pin)
    {
        tx_queue_send(&tx_msg_queue, &s_msg, TX_NO_WAIT);
    }
}

/**
 * @brief  Media close notify callback function.
 * @param  media_ptr: Media control block pointer
 * @retval None
 */
static VOID media_close_callback(FX_MEDIA *media_ptr)
{
    TX_PARAMETER_NOT_USED(media_ptr);
    media_status = MEDIA_CLOSED;
}
