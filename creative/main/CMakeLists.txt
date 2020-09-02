idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "include"
                       REQUIRES accelerometer oled sound_amplifier dht
                      )