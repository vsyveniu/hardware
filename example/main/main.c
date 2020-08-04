#include "main.h"
#include "module.h"
 

#define PIN1 4
//#define PIN2 25


void app_main(void)
{

//   static const adc_atten_t atten = ADC_ATTEN_DB_11;
//     static const adc_unit_t unit = ADC_UNIT_1;
//     #define DEFAULT_VREF    1100
//    esp_adc_cal_characteristics_t *adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));

//  esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF,adc_chars);
  
   gpio_pad_select_gpio(PIN1);
   gpio_set_direction(PIN1, GPIO_MODE_OUTPUT);
//   gpio_pad_select_gpio(PIN2);
//   gpio_set_direction(PIN2, GPIO_MODE_OUTPUT);

//   //xTaskCreate(&task1, "temperature", 2048, NULL, 2, NULL);
//   //xTaskCreate(&task2, "humidity", 2048, NULL, 2, NULL);

//   int read_raw;
//   adc2_config_channel_atten( ADC2_CHANNEL_7, ADC_ATTEN_0db);

//   while (true)
//   {

//     esp_err_t r = adc2_get_raw( ADC2_CHANNEL_7, ADC_WIDTH_12Bit, &read_raw);

//     if (r == ESP_OK) {

//       gpio_set_level(PIN1, esp_adc_cal_raw_to_voltage(read_raw, adc_chars));
//       printf("%d\n", esp_adc_cal_raw_to_voltage(read_raw, adc_chars));
//     }g
//     else if(r == ESP_ERR_TIMEOUT){
//       printf("ADC2 fucked up\n");
//     }
//   }
  while (true){
    gpio_set_level(PIN1, 1);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    gpio_set_level(PIN1, 0);
     vTaskDelay(100 / portTICK_PERIOD_MS);
     gpio_set_level(PIN1, 1);
    
    printf("lighted on\n");
  }

}

