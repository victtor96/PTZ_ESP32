idf_component_register(
    SRCS "0.0.1_IEBI.c"
         "iebi_globals.c"
         "wifi_manager.c"
         "network_tasks.c"
         "sensor_tasks.c"
         "nvs_tasks.c"
         "control_tasks.c"
         "adc_main.c"
    INCLUDE_DIRS "."
)
