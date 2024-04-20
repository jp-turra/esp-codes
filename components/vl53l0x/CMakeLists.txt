idf_component_register(SRCS "src/VL53L0X.cpp" "src/i2c.cpp"
                    INCLUDE_DIRS "include" 
                    REQUIRES "driver" "esp_timer"
                    )
