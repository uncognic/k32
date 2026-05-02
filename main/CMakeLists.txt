idf_component_register(SRCS "main.c" "context_switch.S" "kernel.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE -mabi=call0)