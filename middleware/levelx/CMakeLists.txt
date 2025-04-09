cmake_minimum_required(VERSION 3.22)

project(levelx)
add_library(levelx INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

target_compile_definitions(levelx INTERFACE 
    LX_INCLUDE_USER_DEFINE_FILE
)

target_include_directories(levelx INTERFACE
    common/inc
    ${CMAKE_CURRENT_SOURCE_DIR}

# Threadx headers
    ../threadx/common/inc
    ../threadx/ports/cortex_m7/gnu/inc
)

target_sources(levelx INTERFACE
    common/src/lx_nand_flash_256byte_ecc_check.c
    common/src/lx_nand_flash_256byte_ecc_compute.c
    common/src/lx_nand_flash_block_full_update.c
    common/src/lx_nand_flash_block_obsoleted_check.c
    common/src/lx_nand_flash_block_reclaim.c
    common/src/lx_nand_flash_close.c
    common/src/lx_nand_flash_defragment.c
    common/src/lx_nand_flash_driver_block_erase.c
    common/src/lx_nand_flash_driver_block_erased_verify.c
    common/src/lx_nand_flash_driver_block_status_get.c
    common/src/lx_nand_flash_driver_block_status_set.c
    common/src/lx_nand_flash_driver_extra_bytes_get.c
    common/src/lx_nand_flash_driver_extra_bytes_set.c
    common/src/lx_nand_flash_driver_page_erased_verify.c
    common/src/lx_nand_flash_driver_read.c
    common/src/lx_nand_flash_driver_write.c
    common/src/lx_nand_flash_extended_cache_enable.c
    common/src/lx_nand_flash_initialize.c
    common/src/lx_nand_flash_logical_sector_find.c
    common/src/lx_nand_flash_next_block_to_erase_find.c
    common/src/lx_nand_flash_open.c
    common/src/lx_nand_flash_page_ecc_check.c
    common/src/lx_nand_flash_page_ecc_compute.c
    common/src/lx_nand_flash_partial_defragment.c
    common/src/lx_nand_flash_physical_page_allocate.c
    common/src/lx_nand_flash_sector_mapping_cache_invalidate.c
    common/src/lx_nand_flash_sector_read.c
    common/src/lx_nand_flash_sector_release.c
    common/src/lx_nand_flash_sector_write.c
    common/src/lx_nand_flash_system_error.c
)

target_link_directories(levelx INTERFACE
)

target_link_libraries(levelx INTERFACE
    threadx
)

# Validate that STM32CubeMX code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()
