#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_errno.h"
#include "hi_i2c.h"
#include "wifi_connect.h"

#include "nfc_configNet.h"
#include <cJSON.h>


#define NDEF_PROTOCOL_DATA_LENGTH_OFFSET    4
#define NDEF_PROTOCOL_VALID_DATA_OFFSET     20
#define NDEF_PROTOCOL_DATA_TYPE_OFFSET      6
#define NDEF_START_BYTE                     0x03
#define NDEF_END_BYTE 	                    0xFE
#define NDEF_HEADER_SIZE                    0x2
#define NTAG_ERASED                         0xD0
#define MAX_BUFF                            64

uint8_t     nfcPageBuffer[NFC_PAGE_SIZE];
NT3HerrNum errNo;

uint8_t wechat_GetWiFi_ssid_passwd(const char *nfc_buff, char *wifi_name, char *wifi_passwd)
{
    uint8_t payload_len = nfc_buff[NDEF_PROTOCOL_DATA_LENGTH_OFFSET]; // 获取数据长度
    uint8_t *payload = (uint8_t *)malloc(payload_len + 1);
    uint8_t ret = 0;

    if (payload == NULL) {
        printf("payload malloc failed.\r\n");
        return 0;
    }
    memset_s(payload, payload_len + 1, 0, payload_len + 1);
    memcpy_s(payload, payload_len + 1, nfc_buff + NDEF_PROTOCOL_VALID_DATA_OFFSET, payload_len);
    printf("Use Wechat system..........\n");
    printf("payload = %s\r\n", payload);

    cJSON *root = cJSON_Parse(payload);
    cJSON *ssid = cJSON_GetObjectItem(root, "ssid");
    cJSON *password = cJSON_GetObjectItem(root, "pwd");
    if (root != NULL && ssid != NULL && password != NULL) {
        printf("ssid = %s, password = %s", ssid->valuestring, password->valuestring);
        if (strcpy_s(wifi_name, strlen(ssid->valuestring) + 1, ssid->valuestring) == 0) {
            ret = 1;
        } else {
            ret = 0;
        }

        if (strcpy_s(wifi_passwd, strlen(password->valuestring) + 1, password->valuestring) == 0) {
            ret = 1; // 成功获取到WiFi名称和密码
        } else {
            ret = 0;
        }
    }
    cJSON_Delete(root);
    free(payload);
    ssid = NULL;
    password = NULL;
    root = NULL;
    payload = NULL;

    return ret;
}

/**
 * @brief  使用NFC进行配网
 * @note   驱动NDEF协议中的第一个标签数据，然后进行配网
 *         配合《NFC调试文档.xlsx》，看下面的程序，更容易看懂
 * @param  *ndefBuff: 标签数据的缓冲区
 * @retval
 */
uint32_t NFC_configuresWiFiNetwork(uint8_t *ndefBuff)
{
    if (ndefBuff == NULL) {
        printf("NFC_configuresWiFiNetwork to ndefBuff is NULL\r\n");
        return (uint32_t)(-1);
    }
    uint8_t ret = 0;
    uint8_t wifi_name[MAX_BUFF] = {0}; // WiFi名称
    uint8_t wifi_passwd[MAX_BUFF] = {0}; // WiFi密码

    // 使用微信小程序进行配网
    if (ndefBuff[NDEF_PROTOCOL_DATA_TYPE_OFFSET] == 'T') {
        ret = wechat_GetWiFi_ssid_passwd(ndefBuff, wifi_name, wifi_passwd);
    }

    if (ret == 1) {
        printf("wifi_name: %s\n", wifi_name);
        printf("wifi_passwd: %s\n", wifi_passwd);
        // 连接wifi
        if (0 == WifiConnect(wifi_name, wifi_passwd)) {
            printf("thongth to nfc connect wifi is success.\r\n");
            ret = 1;
        }
    }
    return ret;
}

// int NFC_CONFIG_WIFI(void)
// {
//     uint8_t ret = 1 , num = 0;
//     uint8_t ndefLen = 0;      // ndef包的长度
//     uint8_t ndef_Header = 0;  // ndef消息开始标志位-用不到
//     uint32_t result_code = 0; // 函数的返回值
//     uint8_t *ndefBuff = NULL;

//     //读整个数据的包头部分，读出整个数据的长度
//     result_code = NT3HReadHeaderNfc(&ndefLen, &ndef_Header);
//     if (result_code != true) {
//         printf("NT3HReadHeaderNfc is failed. result_code = %d\r\n", result_code);
//         ret = 1;
//         return ret;
//     }
//     printf("%d\r\n",ndefLen);
//     ndefLen += NDEF_HEADER_SIZE; // 加上头部字节
//     if (ndefLen <= NDEF_HEADER_SIZE) {
//         printf("ndefLen <= 2\r\n");
//         ret = 1;
//         return ret;
//     }

//     ndefBuff = (uint8_t *)malloc(ndefLen + 1);
//     if (ndefBuff == NULL) {
//         printf("ndefBuff malloc is Falied!\r\n");
//         ret = 1;
//         return ret;
//     }

//     if (result_code = get_NDEFDataPackage(ndefBuff, ndefLen) != HI_ERR_SUCCESS) {
//         printf("get_NDEFDataPackage is failed. result_code = %d\r\n", result_code);
//         ret = 1;
//         return ret;
//     }

//     printf("start print ndefBuff.\r\n");
//     for (size_t i = 0; i < ndefLen; i++) {
//         printf("0x%x ", ndefBuff[i]);
//     }
  
//     if(NFC_configuresWiFiNetwork(ndefBuff) == 1){
//         ret = 0;
//         return ret;
//     }

    
//     return ret;
// }

int NFC_CONFIG_WIFI(void)
{

    int ret = 1;
    uint8_t wifi_name[MAX_BUFF] = {0}; // WiFi名称
    uint8_t wifi_passwd[MAX_BUFF] = {0}; // WiFi密码

    ret = nfc_get_devived_data(wifi_name,wifi_passwd);
    
    if(ret==0)
    {
        printf("wifi_name: %s\n", wifi_name);
        printf("wifi_passwd: %s\n", wifi_passwd);
        // 连接wifi
        if (0 == WifiConnect(wifi_name, wifi_passwd)) {
            printf("thongth to nfc connect wifi is success.\r\n");
            ret = 0;
        }
    }


    return ret;
}


void NT3H1101_Read_Userpages(uint8_t pagenumber,uint8_t *outbuf)
{
    for (int i = 0; i < pagenumber; i++)
    {
        NT3HReadUserData(i);
        for (int j = 0; j < 16; j++)
        {
            outbuf[16 * i + j] = nfcPageBuffer[j];
        }
    }
}

/**************************************************
 * get_devived_data
 * 获取WiFi的ssid和密钥及设备DevicesID和密钥
 * ***********************************************/
uint8_t nfc_get_devived_data(char *wifi_name,char *wifi_passwd)
{
    uint8_t memary_buf[16 * 15];
    cJSON *json, *jsonTemp; // *jsonArray,
    uint8_t jsonbuf[512] = {0};
    char jsonbuf_string[512] = {0};
    uint8_t payload_len = 0;
    uint8_t json_len = 0;
    uint8_t ret = 1;

    NT3H1101_Read_Userpages(15, memary_buf);

    payload_len = memary_buf[4];
    json_len = payload_len - 3;

    //取出真实的json数据包，往后再偏移3位
    for (uint8_t i = 0; i < json_len; i++)
    {
        jsonbuf[i] = memary_buf[9 + i];
    }
    memset(jsonbuf_string, 0x00, sizeof(jsonbuf_string));
    sprintf(jsonbuf_string, "%s", jsonbuf);
    
    // printf("\r\n nfc json: [%s]\r\n", jsonbuf_string);

    //解析json数据
    json = cJSON_Parse(jsonbuf_string);
    if (!json)
    {
        ret = 1;
        printf("Error before: [%s]\n", cJSON_GetErrorPtr());
    }
    else
    {
        ret = 0;
        jsonTemp = cJSON_GetObjectItem(json, "ssid");
        memset(wifi_name, 0, sizeof(wifi_name));
        snprintf(wifi_name, strlen(jsonTemp->valuestring) + 1, "%s", jsonTemp->valuestring);

        jsonTemp = cJSON_GetObjectItem(json, "pwd");
        memset(wifi_passwd, 0, sizeof(wifi_passwd));
        snprintf(wifi_passwd, strlen(jsonTemp->valuestring) + 1, "%s", jsonTemp->valuestring);

        cJSON_Delete(json);
        free(json); // isequal
    }
    return ret;
}


//=========================================================================================================================


bool NT3HReadHeaderNfc(uint8_t *endRecordsPtr, uint8_t *ndefHeader)
{
    *endRecordsPtr = 0;
    bool ret = NT3HReadUserData(0);
    // read the first page to see where is the end of the Records.
    if (ret == true) {
        // if the first byte is equals to NDEF_START_BYTE there are some records
        // store theend of that
        if ((NDEF_START_BYTE == nfcPageBuffer[0]) && (NTAG_ERASED != nfcPageBuffer[NDEFHeader])) {
            *endRecordsPtr = nfcPageBuffer[EndRecordsPtr];
            *ndefHeader = nfcPageBuffer[NDEFHeader];
        }
        return true;
    } else {
        errNo = NT3HERROR_READ_HEADER;
    }

    return ret;
}

uint32_t get_NDEFDataPackage(uint8_t *dataBuff, const uint16_t dataBuff_MaxSize)
{
    if (dataBuff == NULL || dataBuff_MaxSize <= 0) {
        printf("dataBuff==NULL or dataBuff_MaxSize<=0\r\n");
        return (uint32_t)(-1);
    }

    uint8_t userMemoryPageNum = 0; // 用户的数据操作页数

    // 算出要取多少页
    if (dataBuff_MaxSize <= NFC_PAGE_SIZE) {
        userMemoryPageNum = 1; // 1页
    } else {
        // 需要访问多少页
        userMemoryPageNum = (dataBuff_MaxSize / NFC_PAGE_SIZE) + \
                            ((dataBuff_MaxSize % NFC_PAGE_SIZE) >= 0 ? 1 : 0);
    }

    // 内存拷贝
    uint8_t *p_buff = (uint8_t *)malloc(userMemoryPageNum * NFC_PAGE_SIZE);
    if (p_buff == NULL) {
        printf("p_buff == NULL.\r\n");
        return (uint32_t)(-1);
    }

    // 读取数据
    for (int i = 0; i < userMemoryPageNum; i++) {
        if (NT3HReadUserData(i) == true) {
            memcpy_s(p_buff + i * NFC_PAGE_SIZE, userMemoryPageNum * NFC_PAGE_SIZE,
                     nfcPageBuffer, NFC_PAGE_SIZE);
        }
    }

    memcpy_s(dataBuff, dataBuff_MaxSize, p_buff, dataBuff_MaxSize);

    free(p_buff);
    p_buff = NULL;

    return HI_ERR_SUCCESS;
}

bool NT3HReadUserData(uint8_t page)
{
    uint8_t reg = USER_START_REG + page;
    // if the requested page is out of the register exit with error
    if (reg > USER_END_REG) {
        errNo = NT3HERROR_INVALID_USER_MEMORY_PAGE;
        return false;
    }
    bool ret = readTimeout(reg, nfcPageBuffer);
    if (ret == false) {
        errNo = NT3HERROR_READ_USER_MEMORY_PAGE;
    }

    return ret;
}

static bool readTimeout(uint8_t address, uint8_t *block_data)
{
    uint32_t status = 0;
    hi_i2c_data nt3h1101_i2c_data = { 0 };
    uint8_t  buffer[1] = { address };
    nt3h1101_i2c_data.send_buf = buffer;
    nt3h1101_i2c_data.send_len = 1;
    nt3h1101_i2c_data.receive_buf = block_data;
    nt3h1101_i2c_data.receive_len = NFC_PAGE_SIZE;
    status = hi_i2c_writeread(HI_I2C_IDX_1, (NT3H1X_ADDRESS << 1) | 0x00, &nt3h1101_i2c_data);
    if (status != 0) {
        printf("===== Error: I2C write status = 0x%x! =====\r\n", status);
        return 0;
    }
    return 1;
}