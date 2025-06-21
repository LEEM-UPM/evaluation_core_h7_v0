#include "sdmmc.h"
#include "gpio.h"
#include "tx_api.h"

HAL_SD_CardInfoTypeDef USBD_SD_CardInfo;

extern void SystemClock_Config(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SDMMC1_SD_Init();

  int status;

  status = HAL_SD_GetCardInfo(&hsd1, &USBD_SD_CardInfo);
  if (status != HAL_OK)
  {
    HAL_GPIO_WritePin(RED_LED_GPIO_Port, RED_LED_Pin, GPIO_PIN_SET);
  }

  tx_kernel_enter();

  while (1)
  {
    HAL_GPIO_TogglePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
    HAL_GPIO_TogglePin(RED_LED_GPIO_Port, RED_LED_Pin);
    HAL_GPIO_TogglePin(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin);
    HAL_Delay(1000);
  }
}
