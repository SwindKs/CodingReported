#ifndef __MyE53INTERFACE_H
#define __MyE53INTERFACE_H



/* E53_IA1传感器数据类型定义 ------------------------------------------------------------*/
typedef struct {
    float   Light;			  // 光照强度
    int   Humidity;        // 湿度
    int   Temperature;     // 温度
    float   Smoke;
    int     CO2;
    int     TVOC;
} MyE53IA1Data;

void MyE53Interface_Init(void);
void MyE53A1GetData(MyE53IA1Data *ReadData);



#endif /*__MyE53INTERFACE_H*/