#include "Flash.hpp"
#include "CRC8.h"

using namespace Flash;
cFlashCore flashCore;
Flash_t flash_base = {.bytes=0x00, .address=(uint8_t *) 0x080E0000};

bool cFlashCore::init() {
    Flash_t flash_struct{};
    bool fault_flag = false;
    auto *address = (uint8_t *) FLASH_STORAGE_BASE_ADDRESS;

    for (auto &data: _flash_list) {
        // When error happened, stop search
        if (fault_flag) {
            data.address = nullptr;
            continue;
        }

        // Read head
        for (uint32_t i = 0; i < sizeof(Flash_t); i++) {
            ((uint8_t *) (&flash_struct))[i] = *(__IO uint8_t *) (address + i);
        }
        // Check head
        if (flash_struct.crc[0] == cal_crc8_table(address, sizeof(Flash_t) - FLASH_STORAGE_CRC_FILED_SIZE)) {
            memcpy(&data, &flash_struct, sizeof(flash_struct));
            address += sizeof(flash_struct);
        } else {
            data.address = nullptr;
            fault_flag = true;
        }
    }

    return fault_flag;
}

bool cFlashCore::config_data(Flash_Element_ID element_id, uint8_t *data, uint32_t len) {
    //Check ID
    if ((element_id == Element_ID_BASE) || (element_id >= Element_ID_MAX)) {
        return false;
    }
    _flash_list[element_id].address = data;
    _flash_list[element_id].bytes = len;
    _flash_list[element_id].crc[1] = cal_crc8_table(data, len);
    _rebuild_status[element_id] = true;
    return true;
}

bool cFlashCore::rebuild() {
    uint32_t error_msg;
    auto *address = (uint8_t *) FLASH_STORAGE_BASE_ADDRESS;
    FLASH_EraseInitTypeDef EraseInitStruct{.TypeErase=FLASH_TYPEERASE_SECTORS, .Sector=FLASH_STORAGE_SECTOR, .NbSectors=1, .VoltageRange=FLASH_VOLTAGE_RANGE_3};
    uint32_t en_address = FLASH_STORAGE_BASE_ADDRESS + Element_ID_MAX * sizeof(Flash_t);

    /*Check if all data elements are ready*/
    for (uint32_t i = 0; i < Element_ID_MAX; i++) {
        if (!_rebuild_status[i]) {
            return false;
        }
        en_address += _flash_list[i].bytes;
    }
    /*Check if data is larger than flash*/
    if (en_address > FLASH_STORAGE_BASE_ADDRESS + FLASH_STORAGE_SIZE) {
        return false;
    }

    /*Start program flash*/
    HAL_FLASH_Unlock();
    /*Erase Flash*/
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &error_msg) != HAL_OK) {
        HAL_FLASH_Lock();
        return false;
    }
    for (auto &data: _flash_list) {
        //Write data to flash
        for (uint32_t len = 0; len < data.bytes; len++) {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, (uint32_t) (address + sizeof(Flash_t) + len),
                                  (uint64_t) data.address[len]) != HAL_OK) {
                HAL_FLASH_Lock();
                return false;
            }
        }
        //Reset head
        data.address = address + sizeof(Flash_t);
        data.crc[0] = cal_crc8_table((uint8_t *) &data, sizeof(Flash_t) - FLASH_STORAGE_CRC_FILED_SIZE);
        //Write head to flash
        for (uint32_t len = 0; len < sizeof(data); len++) {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, (uint32_t) (address + len),
                                  (uint64_t) (((uint8_t*)(&data))[len])) != HAL_OK) {
                HAL_FLASH_Lock();
                return false;
            }
        }
        //Recursive new address
        address += sizeof(Flash_t) + data.bytes;
    }
    HAL_FLASH_Lock();
    return true;
}

bool cFlashCore::get_element(Flash_Element_ID element_id, Flash_t *flash_element) {
    if (element_id >= Element_ID_MAX) {
        return false;
    }
    if (_flash_list[element_id].address == nullptr) {
        return false;
    }
    memcpy(flash_element, &_flash_list[element_id], sizeof(Flash_t));
    return true;
}

bool cFlashCore::flash_memcpy(Flash_Element_ID element_id, uint8_t *dst, bool en_crc) {
    if (element_id >= Element_ID_MAX) {
        return false;
    }
    
    if ( _flash_list[element_id].address == nullptr ){
        return false;
    }

    flash_memcpy(dst, _flash_list[element_id].address, _flash_list[element_id].bytes);

    if (en_crc) {
        if (cal_crc8_table(dst, _flash_list[element_id].bytes) != _flash_list[element_id].crc[1]) {
            return false;
        }
    }

    return true;
}