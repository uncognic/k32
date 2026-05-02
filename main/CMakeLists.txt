idf_component_register(
    SRCS "kernel.c" "context_switch.S" "main.c"
    INCLUDE_DIRS "."
)

target_compile_options(${COMPONENT_LIB} PRIVATE -mabi=call0)