#ifndef APP_AZURE_RTOS_CONFIG_H
#define APP_AZURE_RTOS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Using static memory allocation via threadX Byte memory pools */
#define USE_STATIC_ALLOCATION                    1
#define TX_APP_MEM_POOL_SIZE                     1024
#define UX_DEVICE_APP_MEM_POOL_SIZE              15*1024

#ifdef __cplusplus
}
#endif

#endif /* APP_AZURE_RTOS_CONFIG_H */
