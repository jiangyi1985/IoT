/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "gpio.h"
#include "usbd_hid.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//typedef struct
//{
//	uint8_t MODIFIER;
//	uint8_t RESERVED;
//	uint8_t KEYCODE1;
//	uint8_t KEYCODE2;
//	uint8_t KEYCODE3;
//	uint8_t KEYCODE4;
//	uint8_t KEYCODE5;
//	uint8_t KEYCODE6;
//} keyboardHID;
//keyboardHID keyboardhid = {0,0,0,0,0,0,0,0};

extern USBD_HandleTypeDef hUsbDeviceFS;

typedef struct 
{
    uint8_t report_id;
    uint8_t modifier;
    uint8_t reserved;
    uint8_t keycode[6];
} keyboard_report_t;

typedef struct 
{
    uint8_t report_id;
    uint8_t buttons;
    int8_t x;
    int8_t y;
		int8_t wheel;
		int8_t pan;
} mouse_report_t;

typedef struct 
{
    uint8_t report_id;
    uint8_t buttons;
} consumer_report_t;

keyboard_report_t keyboardRpt = {1,0,0,0,0,0,0,0,0};
mouse_report_t mouseRpt = {2,0,0,0,0,0};
consumer_report_t consumerRpt = {3,0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

	uint8_t isPressed = 0;//rotary between clicks or clicked
	uint8_t status = 0;// rotary rotating status:		1:clockwise		2:counter-clockwise

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		GPIO_PinState s1 = HAL_GPIO_ReadPin(PA3_GPIO_Port,PA3_Pin);
		GPIO_PinState s2 = HAL_GPIO_ReadPin(PA4_GPIO_Port,PA4_Pin);
		
		//led indicator
		if(s1==0 || s2==0)//rotary moved
			HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);//low = led on
		else
			HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);
		
		if(s1==0 && s2==0)
		{
			isPressed=1;
		}
		else if(s1==1&&s2==0)//clockwise (s1:clk s2:dt)
			status=1;
		else if(s1==0&&s2==1)//counter-clockwise (s1:clk s2:dt)
			status=2;
		else if(s1==1&&s2==1)//released
		{
			if(isPressed==1)//release from pressed
			{
//				//Keyboard
//				if(status==1)
//					keyboardRpt.keycode[0]=0x4f;//RightArrow
//				else if(status==2)
//					keyboardRpt.keycode[0]=0x50;//LeftArrow
//				//press key and release
//				USBD_HID_SendReport(&hUsbDeviceFS,(uint8_t*)&keyboardRpt,sizeof(keyboardRpt));
//				HAL_Delay(50);
//				keyboardRpt.keycode[0]=0;
//				USBD_HID_SendReport(&hUsbDeviceFS,(uint8_t*)&keyboardRpt,sizeof(keyboardRpt));
				
//				//Consumer Device
//				if(status==1)
//					consumerRpt.buttons=0b01000000;//VolumeIncrement
//				else if(status==2)
//					consumerRpt.buttons=0b10000000;//VolumeDecrement
//				//press key and release
//				USBD_HID_SendReport(&hUsbDeviceFS,(uint8_t*)&consumerRpt,sizeof(consumerRpt));
//				HAL_Delay(50);
//				consumerRpt.buttons=0;
//				USBD_HID_SendReport(&hUsbDeviceFS,(uint8_t*)&consumerRpt,sizeof(consumerRpt));
				
				//Mouse
				if(status==1)
				{
//					mouseRpt.buttons=0b00000001;
//					mouseRpt.x=10;
//					mouseRpt.y=10;
//					mouseRpt.wheel=1;
					mouseRpt.pan=1;
				}
				else if(status==2)
				{
//					mouseRpt.buttons=0b00000010;
//					mouseRpt.x=-10;
//					mouseRpt.y=-10;
//					mouseRpt.wheel=-1;
					mouseRpt.pan=-1;
				}
				//press key and release
				USBD_HID_SendReport(&hUsbDeviceFS,(uint8_t*)&mouseRpt,sizeof(mouseRpt));
//				HAL_Delay(50);
//				mouseRpt.buttons=0b00000000;
//				USBD_HID_SendReport(&hUsbDeviceFS,(uint8_t*)&mouseRpt,sizeof(mouseRpt));
			}
			
			isPressed=0;
		}
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
