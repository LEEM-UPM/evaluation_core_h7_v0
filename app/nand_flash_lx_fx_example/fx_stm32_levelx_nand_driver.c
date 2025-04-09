#include "fx_stm32_levelx_nand_driver.h"
#include "lx_stm32_nand_simulator_driver.h"
#include "lx_api.h"

struct fx_lx_nand_driver_instance
{
    LX_NAND_FLASH flash_instance;
    CHAR name[32];
    UINT id;
    UINT (*nand_driver_initialize)(LX_NAND_FLASH *);
    UINT initialized;
};

static struct fx_lx_nand_driver_instance flash_driver = 
{
    .flash_instance = {0},
    .name = "MyNANDDriver",
    .id = 1,
    .nand_driver_initialize = lx_stm32_nand_simulator_initialize,
    .initialized = FX_FALSE
};

VOID fx_stm32_levelx_nand_driver(FX_MEDIA *media_ptr)
{
    ULONG i;
    UINT status;
    UCHAR *source_buffer;
    UCHAR *destination_buffer;
    ULONG logical_sector;

    switch (media_ptr->fx_media_driver_request)
    {
        case FX_DRIVER_INIT:
        {
            if (flash_driver.initialized == FX_FALSE)
            {
                status = lx_nand_flash_open(&flash_driver.flash_instance, flash_driver.name, flash_driver.nand_driver_initialize);

                if (status == LX_SUCCESS)
                {
                    flash_driver.initialized = FX_TRUE;
                    media_ptr->fx_media_driver_status = FX_SUCCESS;

                    media_ptr->fx_media_driver_free_sector_update = FX_TRUE;
                    break;
                }
                else
                {
                    media_ptr->fx_media_driver_status = FX_IO_ERROR;
                }
            }
            else
            {
                media_ptr->fx_media_driver_status = FX_SUCCESS;
            }

            break;
        }

        case FX_DRIVER_UNINIT:
        {
            status = lx_nand_flash_close(&flash_driver.flash_instance);

            if (status == LX_SUCCESS)
            {
                media_ptr->fx_media_driver_status = FX_SUCCESS;
            }
            else
            {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
            }

            break;
        }

        case FX_DRIVER_READ:
        {
            logical_sector = media_ptr->fx_media_driver_logical_sector;
            destination_buffer = (UCHAR *)media_ptr->fx_media_driver_buffer;

            for (i = 0; i < media_ptr->fx_media_driver_sectors; i++)
            {
                status = lx_nand_flash_sector_read(&flash_driver.flash_instance, logical_sector, destination_buffer);
                if (status != LX_SUCCESS)
                {
                    media_ptr->fx_media_driver_status = FX_IO_ERROR;
                    return;
                }

                /* Move to the next entries.  */
                logical_sector++;
                destination_buffer = destination_buffer + media_ptr->fx_media_bytes_per_sector;
            }

            media_ptr->fx_media_driver_status = FX_SUCCESS;

            break;
        }

        case FX_DRIVER_BOOT_READ:
        {
            destination_buffer = (UCHAR *)media_ptr->fx_media_driver_buffer;

            status = lx_nand_flash_sector_read(&flash_driver.flash_instance, 0, destination_buffer);
            if (status != LX_SUCCESS)
            {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
                return;
            }

            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }

        case FX_DRIVER_WRITE:
        {
            logical_sector = media_ptr->fx_media_driver_logical_sector;
            source_buffer = (UCHAR *)media_ptr->fx_media_driver_buffer;

            for (i = 0; i < media_ptr->fx_media_driver_sectors; i++)
            {
                status = lx_nand_flash_sector_write(&flash_driver.flash_instance, logical_sector, source_buffer);
                if (status != LX_SUCCESS)
                {
                    media_ptr->fx_media_driver_status = FX_IO_ERROR;
                    return;
                }

                logical_sector++;
                source_buffer = source_buffer + media_ptr->fx_media_bytes_per_sector;
            }

            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }

        case FX_DRIVER_BOOT_WRITE:
        {
            source_buffer = (UCHAR *)media_ptr->fx_media_driver_buffer;

            status = lx_nand_flash_sector_write(&flash_driver.flash_instance, 0, source_buffer);
            if (status != LX_SUCCESS)
            {
                media_ptr->fx_media_driver_status = FX_IO_ERROR;
                return;
            }

            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }

        case FX_DRIVER_RELEASE_SECTORS:
        {
            logical_sector = media_ptr->fx_media_driver_logical_sector;

            for (i = 0; i < media_ptr->fx_media_driver_sectors; i++)
            {
                status = lx_nand_flash_sector_release(&flash_driver.flash_instance, logical_sector);
                if (status != LX_SUCCESS)
                {
                    media_ptr->fx_media_driver_status = FX_IO_ERROR;
                    return;
                }

                logical_sector++;
            }

            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }

        case FX_DRIVER_FLUSH:
        {
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }

        case FX_DRIVER_ABORT:
        {
            media_ptr->fx_media_driver_status = FX_SUCCESS;
            break;
        }

        default:
        {
            media_ptr->fx_media_driver_status = FX_IO_ERROR;
            break;
        }
    }
}
