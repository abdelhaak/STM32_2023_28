    /* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lib_lcd.h"
#define _OPEN_SYS_ITOA_EXT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



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

// Adresse de capteur
static const uint8_t CAPTEUR_ADRS = 0x44 << 1;
// Adresse de bit fort
static const uint8_t CAPTEUR_CMD_MSB = 0x24;
// Adresse de bit faible
static const uint8_t CAPTEUR_CMD_LSB = 0x16;
static rgb_lcd lcdData;
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

	// Declaration des variables
	HAL_StatusTypeDef ret;
		uint8_t buf[12];
		volatile uint16_t value;
		volatile uint16_t value1;
		volatile float temp;
		float umid;
		float Decimal_part;
		float Entier_part;

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
  MX_USART2_UART_Init();
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */

  // Initialisation de LCD
    lcd_init(&hi2c1,&lcdData);
    lcd_position(&hi2c1,0,0);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

	  	  	  // Valeur de bit fort stocké dans le buf [0]
	  	  	buf[0] = CAPTEUR_CMD_MSB;
	  	  	  // Valeur de bit faible stocké dans le buf [1]
	  		buf[1] = CAPTEUR_CMD_LSB;
	  		// Debut de transmission de I2C "WRITE" dans l'I2C avec l'adresse de capteur pour recevoir les données de capteur
	  		ret = HAL_I2C_Master_Transmit( &hi2c1, CAPTEUR_ADRS, buf, 2, HAL_MAX_DELAY);

	  		// Echec de communcation
	  		if ( ret != HAL_OK)
	  		{
	  			strcpy((char*)buf, "erreur_T!!\r\n");
	  		}

	  		// Communication reussite
	  			else
	  		{

	  		ret = HAL_I2C_Master_Receive( &hi2c1, CAPTEUR_ADRS, buf, 6, HAL_MAX_DELAY);


	  		// Echec de communcation
	  					if ( ret != HAL_OK)
	  				{
	  					strcpy((char*)buf, "erreur_R!!\r\n");
	  				}
	  		// Communication reussite
	  		else
	  		{
	  			// Decodage de MSB & LSB et socké dans la valeur
	  			value  =   buf[1] | buf[0] << 8;

	  			// Calcul exacte de la valeur de temperature
	  			temp = -45 + 175 * ( (float)value / 65535);

	  			// Transformation de valeur pour la partie entier et decimale
	  			Entier_part = (int) temp;
	  			Decimal_part = temp;
	  			Decimal_part *= 100;
	  			Decimal_part = Decimal_part - (Entier_part * 100);

	  			// On stocke la valeur de temperature dans le byte  buf[4] et buf[3]
	  			value = buf[4] | buf[3] << 8;

	  			// Calcul exacte de la valeur de l'humidité
	  			umid = -49 + 315 *( (float)value / 65535);

	  			//:::::::::::::::::::::::::::::::::::::::::: PARTIE LCD ::::::::::::::::::::::::::::::::::::::::::::::::::://

	  			// Affichage des valeurs dans le LCD
	  			sprintf( (char*)buf, "%u.%u C ; %u D", (unsigned int) Entier_part,(unsigned int) Decimal_part,(unsigned int) umid  );
	  			lcd_position(&hi2c1,0,0);
	  			lcd_print(&hi2c1,"Temp : ");
	  			lcd_position(&hi2c1,7,0);
	  			lcd_print(&hi2c1,buf);
	  			lcd_position(&hi2c1,0,1);
	  			lcd_print(&hi2c1,"Hum: ");
	  			lcd_position(&hi2c1,7,1);
	  			lcd_print(&hi2c1,&buf[10]);

	  		}
	  		}
	  		HAL_UART_Transmit(&huart2, buf, strlen((char*)buf), HAL_MAX_DELAY);
	  		HAL_Delay(1000);

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


  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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



