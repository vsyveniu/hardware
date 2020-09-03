#include "sound_amplifier.h"

void beep(){

int volt;
int beep_count;

		 for (beep_count = 0; beep_count < 50; beep_count++){
            for (volt = 0; volt < 256; volt+=5)
            { 
                dac_output_voltage(DAC_CHANNEL_1, volt);        
            }     
            ets_delay_us(1);
        }

        for (beep_count = 50; beep_count > 0; beep_count--){ 
			for (volt = 256; volt  > 0; volt-=5)
            {
                dac_output_voltage(DAC_CHANNEL_1, volt);
            }      
            ets_delay_us(1);
        }
}