#ifndef __NFC_CONFIGNET_H
#define __NFC_CONFIGNET_H


#define NT3H1X_ADDRESS 0x55

#define MANUFACTORING_DATA_REG 0x0
#define USER_START_REG 0x1

#define NT3H1X_WRITE_TIMEOUT_US 300000

//  NT3H1201             // for th 2K
#define USER_END_REG   0x77
#define CONFIG_REG	   0x7A

// NT3H1101                     // for th 1K
// #define USER_END_REG   0x38 // just the first 8 bytes for th 1K
// #define CONFIG_REG	   0x3A

#define SERIAL_NUM_LEN 8

#define SRAM_START_REG 0xF8
#define SRAM_END_REG   0xFB // just the first 8 bytes

#define SESSION_REG	   0xFE

#define NFC_PAGE_SIZE 16



typedef enum {
    NT3HERROR_NO_ERROR,
    NT3HERROR_READ_HEADER,
    NT3HERROR_WRITE_HEADER,
    NT3HERROR_INVALID_USER_MEMORY_PAGE,
    NT3HERROR_READ_USER_MEMORY_PAGE,
    NT3HERROR_WRITE_USER_MEMORY_PAGE,
    NT3HERROR_ERASE_USER_MEMORY_PAGE,
    NT3HERROR_READ_NDEF_TEXT,
    NT3HERROR_WRITE_NDEF_TEXT,
    NT3HERROR_TYPE_NOT_SUPPORTED
}NT3HerrNum;

typedef enum {
    EndRecordsPtr = 1,
    NDEFHeader = 2,
} HeardPosEnu;

int NFC_CONFIG_WIFI(void);
uint32_t NFC_configuresWiFiNetwork(uint8_t *ndefBuff);
uint32_t get_NDEFDataPackage(uint8_t *dataBuff, const uint16_t dataBuff_MaxSize);
uint8_t wechat_GetWiFi_ssid_passwd(const char *nfc_buff, char *wifi_name, char *wifi_passwd);

static bool readTimeout(uint8_t address, uint8_t *block_data);
bool NT3HReadUserData(uint8_t page);
uint32_t get_NDEFDataPackage(uint8_t *dataBuff, const uint16_t dataBuff_MaxSize);
bool NT3HReadHeaderNfc(uint8_t *endRecordsPtr, uint8_t *ndefHeader);

void NT3H1101_Read_Userpages(uint8_t pagenumber,uint8_t *outbuf);
uint8_t nfc_get_devived_data(char *wifi_name,char *wifi_passwd);

#endif  /*__NFC_CONFIGNET_H*/