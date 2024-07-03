/*
 * Copyright (c) 2020 Nanjing Xiaoxiongpai Intelligent Technology Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cmsis_os2.h"
#include "ohos_init.h"
#include "lwip/sockets.h"
// #include "wifi_connect.h"

#include "MyE53Interface.h"
#include "motor_control.h"

#include "iot_watchdog.h"
#include "OLED.h"
#include "UserI2C.h"
#include "nfc_configNet.h"

#include "oc_mqtt.h"
#include <cJSON.h>

#define MSGQUEUE_COUNT 32 // number of Message Queue Objects
#define MSGQUEUE_SIZE 25
#define TASK_DELAY_3S 3

#define Car_Speed1   38100
#define Car_Speed2   45200

typedef struct { // object data type
    char *Buf;
    uint8_t Idx;
} MSGQUEUE_OBJ_t;

MSGQUEUE_OBJ_t msg;
osMessageQueueId_t mid_MsgQueue;

typedef enum {
    en_msg_cmd = 0,
    en_msg_report,
} en_msg_type_t;

typedef struct {
    char *request_id;
    char *payload;
} cmd_t;

typedef struct {
    int temp;
    int humi;
    int co2;
    int tvoc;
    int smoke;
    int light;
} report_t;

typedef struct {
    en_msg_type_t msg_type;
    union {
        cmd_t cmd;
        report_t report;
    } msg;
} app_msg_t;

typedef struct {
    int connected;
    int led;
    int motor;
    int Car_Status;
    int Work_Mode;
    int Take_Picture;

} app_cb_t;

#define CLIENT_ID       "663ddea16bc31504f069b342_12138178_0_0_2024051012"
#define USERNAME        "663ddea16bc31504f069b342_12138178"
#define PASSWORD        "3662fc75eca0cce1e611d2c0394d8056060d2e7e5407a830d27e61635a2d0d80"

const char *SSID =      "iKunFans";
const char *PWD =       "qiuguoquan";

static app_cb_t g_app_cb;
osThreadId_t cloudTaskID;
osThreadId_t sensorTaskID;
osThreadId_t TraceTaskID;

static void ReportMsg(report_t *report)
{
    oc_mqtt_profile_service_t service;
    oc_mqtt_profile_kv_t temperature;
    oc_mqtt_profile_kv_t humidity;
    oc_mqtt_profile_kv_t CO2;
    oc_mqtt_profile_kv_t TVOC;
    oc_mqtt_profile_kv_t Smoke;
    oc_mqtt_profile_kv_t Light;

    service.event_time = NULL;
    service.service_id = "qq123";
    service.service_property = &temperature;
    service.nxt = NULL;

    temperature.key = "Temperature";
    temperature.value = &report->temp;
    temperature.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    temperature.nxt = &humidity;

    humidity.key = "Humidity";
    humidity.value = &report->humi;
    humidity.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    humidity.nxt = &CO2;

    CO2.key = "CO2";
    CO2.value = &report->co2;
    CO2.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    CO2.nxt = &TVOC;

    TVOC.key = "TVOC";
    TVOC.value = &report->tvoc;
    TVOC.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    TVOC.nxt = &Smoke;

    Smoke.key = "Smoke";
    Smoke.value = &report->smoke;
    Smoke.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    Smoke.nxt = &Light;

    Light.key = "Light";
    Light.value = &report->light;
    Light.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    Light.nxt = NULL;

    oc_mqtt_profile_propertyreport(USERNAME, &service);
    return;
}

//接收云端数据后进行响应
static char aa[50];
void MsgRcvCallback(uint8_t *tpc_dt, size_t tpc_size,uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size)
{
    app_msg_t *app_msg;
    int ret = 0;
    app_msg = malloc(sizeof(app_msg_t));
    app_msg->msg_type = en_msg_cmd;
    app_msg->msg.cmd.payload = (char *)recv_data;
    printf("recv data is %s\n", recv_data);

    int c='=';
    char strg[tpc_size];
    
    memset(strg, '\0', sizeof(strg));
    strncat(strg, (char *)tpc_dt,tpc_size);

    char *zzpp;
    char *pos=strchr(strg,c);
    zzpp = strchr(strg,c);
    memset(aa, '\0', sizeof(aa));
    strncpy(aa,zzpp+1,strlen(zzpp+1));

    app_msg->msg.cmd.request_id=aa;
    printf("cmd-request_id:%s\r\n",aa);
    ret = osMessageQueuePut(mid_MsgQueue, &app_msg, 0U, 0U);
    if (ret != 0) {
        free(recv_data);
        free(tpc_dt);
    }
    *resp_data = NULL;
    *resp_size = 0;
}

static void oc_cmdresp(cmd_t *cmd, int cmdret)
{
    oc_mqtt_profile_cmdresp_t cmdresp;
    int ret;
    ///< do the response
    cmdresp.paras = NULL;
    cmdresp.request_id = cmd->request_id;
    cmdresp.ret_code = cmdret;
    cmdresp.ret_name = NULL;
    ret = oc_mqtt_profile_cmdresp(USERNAME, &cmdresp);
}


static void DealCmdMsg(cmd_t *cmd)
{
    cJSON *obj_root, *obj_cmdname, *obj_paras, *obj_para;

    int cmdret = 1;

    obj_root = cJSON_Parse(cmd->payload);
    if(obj_root == NULL)
        oc_cmdresp(cmd, cmdret);
    

    obj_cmdname = cJSON_GetObjectItem(obj_root, "command_name");
    if (obj_cmdname == NULL) {
        cJSON_Delete(obj_root);
    }
    if (strcmp(cJSON_GetStringValue(obj_cmdname), "Work_Mode") == 0) {
        obj_paras = cJSON_GetObjectItem(obj_root, "paras");
        if (obj_paras == NULL) {
            cJSON_Delete(obj_root);
        }
        obj_para = cJSON_GetObjectItem(obj_paras, "Mode_cmd");
        if (obj_para == NULL) {
            cJSON_Delete(obj_root);
        }
        ///< operate the LED here
        if (strcmp(cJSON_GetStringValue(obj_para), "AUTO") == 0) {
            g_app_cb.Work_Mode = 1;
            //执行自动循迹
            osThreadResume(TraceTaskID);
            printf("Work_Mode:AUTO!\r\n");
        } else {
            g_app_cb.Work_Mode = 0;
            //挂起自动循迹任务，执行联网控制
            osThreadSuspend(TraceTaskID);
            printf("Work_Mode:CONTROL!\r\n");
        }
        cmdret = 0;
        oc_cmdresp(cmd, cmdret);
    }

    else if (strcmp(cJSON_GetStringValue(obj_cmdname), "Car_Controller") == 0) {
        obj_paras = cJSON_GetObjectItem(obj_root, "Paras");
        if (obj_paras == NULL) {
            cJSON_Delete(obj_root);
        }
        obj_para = cJSON_GetObjectItem(obj_paras, "Control_cmd");
        if (obj_para == NULL) {
            cJSON_Delete(obj_root);
        }
        //直行
        if (strcmp(cJSON_GetStringValue(obj_para), "FORWARD") == 0) {
            g_app_cb.Car_Status = 1;
            car_forward(Car_Speed1,Car_Speed2);

        }
        //左转
        if (strcmp(cJSON_GetStringValue(obj_para), "LEFT") == 0) {
            g_app_cb.Car_Status = 2;
            car_left(Car_Speed1);
            hi_udelay(300000);
            car_stop();

        }
        //右转
        if (strcmp(cJSON_GetStringValue(obj_para), "RIGHT") == 0) {
            g_app_cb.Car_Status = 3;
            car_right(Car_Speed1);
            hi_udelay(300000);
            car_stop();

        }
        //后退
        if (strcmp(cJSON_GetStringValue(obj_para), "BACKWARD") == 0) {
            g_app_cb.Car_Status = 4;
            car_backward(Car_Speed1,Car_Speed2);

        }
        //停止
        if (strcmp(cJSON_GetStringValue(obj_para), "STOP") == 0) {
            g_app_cb.Car_Status = 5;
            car_stop();
        }
        cmdret = 0;
        oc_cmdresp(cmd, cmdret);
    }

    free(cmd);
    cJSON_Delete(obj_root);
}

int CloudMainTaskEntry(void)
{
    app_msg_t *app_msg;

    printf("循迹挂起\r\n");
    osThreadSuspend(TraceTaskID);
    // uint32_t ret = WifiConnect(SSID, PWD);

    if(NFC_CONFIG_WIFI() == 0){
        
        printf("NFC WIFI Config Succ!!!\r\n");
    }else{
        
        WifiConnect(SSID, PWD);
        printf("WIFI Config Succ!!!\r\n");
    }
    OLED_Clear();
    OLED_ShowStr(2,25,"Connect AP Name:                             iKunFans",TEXT_SIZE_8);
    hi_udelay(1000000);


    device_info_init(CLIENT_ID, USERNAME, PASSWORD);
    oc_mqtt_init();
    oc_set_cmd_rsp_cb(MsgRcvCallback);

    while (1) {
        app_msg = NULL;
        (void)osMessageQueueGet(mid_MsgQueue, (void **)&app_msg, NULL, 0U);
        if (app_msg != NULL) {
            switch (app_msg->msg_type) {
                case en_msg_cmd:
                    DealCmdMsg(&app_msg->msg.cmd);
                    break;
                case en_msg_report:
                    ReportMsg(&app_msg->msg.report);
                    break;
                default:
                    break;
            }
            free(app_msg);
        }
    }
    return 0;
}

int SensorTaskEntry(void)
{
    int ret;
    app_msg_t *app_msg;
    MyE53IA1Data data = {0};
    MyE53Interface_Init();

    char temp_str[20];
    char humi_str[20];
    char co2_str[20];
    char tvoc_str[20];
    char smoke_str[20];
    char light_str[20];

    OLED_Clear();
    OLED_DrawBMP(32,0,64,64,kkData);
    

    while (1) {
        MyE53A1GetData(&data);
        app_msg = malloc(sizeof(app_msg_t));
        printf("SENSOR: [light:%.2f temp:%.d\thumi:%.d\tco2:%d\ttvoc:%d\tsmoke:%.2f]\r\n", data.Light,
         data.Temperature, data.Humidity,data.CO2,data.TVOC,data.Smoke);


        OLED_Clear();
        sprintf(temp_str,"Temp:%d C",data.Temperature);
        OLED_ShowStr(0,0,temp_str,TEXT_SIZE_8);
        sprintf(humi_str,"Humi:%d",data.Humidity);
        OLED_ShowStr(62,0,humi_str,TEXT_SIZE_8);
        sprintf(tvoc_str,"TVOC:%d ppm",data.TVOC);
        OLED_ShowStr(5,2,tvoc_str,TEXT_SIZE_8);        
        sprintf(co2_str,"CO2:%d ppm",data.CO2);
        OLED_ShowStr(5,3,co2_str,TEXT_SIZE_8);
        sprintf(smoke_str,"Smoke:%.2f ppd",data.Smoke);
        OLED_ShowStr(5,5,smoke_str,TEXT_SIZE_8);
        sprintf(light_str,"light:%.2f lux",data.Light);
        OLED_ShowStr(5,6,light_str,TEXT_SIZE_8);

        if (app_msg != NULL) {
            app_msg->msg_type = en_msg_report;

            app_msg->msg.report.humi = data.Humidity;
            app_msg->msg.report.light = (int)data.Light;
            app_msg->msg.report.temp = data.Temperature;
            app_msg->msg.report.co2 = data.CO2;
            app_msg->msg.report.smoke = (int)data.Smoke;
            app_msg->msg.report.tvoc = data.TVOC;

            if (osMessageQueuePut(mid_MsgQueue, &app_msg, 0U, 0U) != 0) {
                free(app_msg);
            }
        }
        sleep(TASK_DELAY_3S);
    }
    return 0;
}


static void IotMainTaskEntry(void)
{
    User_I2C1_Init();
    TB6612FNGInit();
    trace_module_init();
    init_ctrl_algo();
    OLED_Init();
    IoTWatchDogDisable();

    // WifiConnect(SSID, PWD);
    // printf("WIFI Config Succ!!!\r\n");
    // OLED_Clear();
    // OLED_ShowStr(2,25,"Connect AP Name:                             iKunFans",TEXT_SIZE_8);

    mid_MsgQueue = osMessageQueueNew(MSGQUEUE_COUNT, MSGQUEUE_SIZE, NULL);
    if (mid_MsgQueue == NULL) {
        printf("Failed to create Message Queue!\n");
    }

    osThreadAttr_t attr;

    attr.name = "CloudMainTaskEntry";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 10;
    attr.priority = 24;

    cloudTaskID = osThreadNew((osThreadFunc_t)CloudMainTaskEntry, NULL, &attr);
    if (cloudTaskID == NULL) {
        printf("Failed to create CloudMainTaskEntry!\n");
    }

    attr.stack_size = 1024 * 3;
    attr.priority = 25;
    attr.name = "SensorTaskEntry";
    sensorTaskID = osThreadNew((osThreadFunc_t)SensorTaskEntry, NULL, &attr);
    if (sensorTaskID == NULL) {
        printf("Failed to create SensorTaskEntry!\n");
    }

    attr.stack_size = 1024 * 2;
    attr.priority = 25;
    attr.name = "TraceTaskEntry";
    TraceTaskID = osThreadNew((osThreadFunc_t)TraceTask, NULL, &attr);
    if(TraceTaskID == NULL){
        printf("Failed to create TraceTask!\n");
    }

}

SYS_RUN(IotMainTaskEntry);