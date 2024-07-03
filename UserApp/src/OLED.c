#include "OLED.h"

#define SEC_MAX 60
#define MIN_MAX 60
#define HOUR_MAX 24


void OLEDTask(void)
{

    OLED_Init();
    OLED_Clear();
    

    while (1)
    {
        OLED_ShowStr(0,0,"Hello world!!",TEXT_SIZE_8);
    }

}


void OLED_WriteCommand(uint8_t byte)
{
    uint8_t buffer[] = {0x00, byte};
    User_I2C1_Write(OLED_SLAVE_ADDRESS,0,buffer, sizeof(buffer));
}

void OLED_WriteData(uint8_t byte)
{
    uint8_t buffer[] = {0x40, byte};
    User_I2C1_Write(OLED_SLAVE_ADDRESS,0,buffer, sizeof(buffer));   
}


void OLED_Fill(uint8_t fill_Data) // 全屏填充
{
    uint8_t m, n;
    uint8_t number_of_rows = 8;
    uint8_t number_of_columns = 128;

    for (m = 0; m < number_of_rows; m++) {
        OLED_WriteCommand(0xb0 + m); // page0-page1
        OLED_WriteCommand(0x00);     // low column start address
        OLED_WriteCommand(0x10);     // high column start address
        for (n = 0; n < number_of_columns; n++) {
            OLED_WriteData(fill_Data);
        }
    }
}


void OLED_SetPos(uint8_t x, uint8_t y) // 设置起始点坐标
{
    uint8_t illegal_keyword = 4;  // 右移4位
    OLED_WriteCommand(0xb0 + y);
    OLED_WriteCommand(((x & 0xf0) >> illegal_keyword) | 0x10);
    OLED_WriteCommand((x & 0x0f) | 0x01);
}


void OLED_Clear(void) // 清屏
{
    OLED_Fill(0x00);
}


/**
 * @brief  垂直滚动函数
 * @retval None
 */
void OLED_Set_Vertical_Rol(void)
{
    OLED_WriteCommand(0x2e); // 停止滚动
    OLED_WriteCommand(0xa3); //
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x40);
    OLED_WriteCommand(0x2a);
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x01);
    OLED_WriteCommand(0x00);
    OLED_WriteCommand(0x04);
    OLED_WriteCommand(0x2f);
}


// --------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
// --------------------------------------------------------------
void OLED_ON(void)
{
    OLED_WriteCommand(0X8D); // 设置电荷泵
    OLED_WriteCommand(0X14); // 开启电荷泵
    OLED_WriteCommand(0XAF); // OLED唤醒
}

// --------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
// --------------------------------------------------------------
void OLED_OFF(void)
{
    OLED_WriteCommand(0X8D); // 设置电荷泵
    OLED_WriteCommand(0X10); // 关闭电荷泵
    OLED_WriteCommand(0XAE); // OLED休眠
}

/**
 * @brief  显示字符串
 */
void OLED_ShowStr(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize)
{
    uint8_t c = 0, i = 0, j = 0;
    uint8_t number_of_letter_lattice = 6;
    uint8_t pos_x = x;
    uint8_t pos_y = y;

    switch (TextSize) {
        case TEXT_SIZE_8:
            while (ch[j] != '\0') {
                c = ch[j] - ' ';
                if (pos_x > TEXT_8_MAX_ROW_POS) {
                    pos_x = 0;
                    pos_y++;
                }
                OLED_SetPos(pos_x, pos_y);
                for (i = 0; i < number_of_letter_lattice; i++) {
                    OLED_WriteData(F6x8[c][i]);
                }
                pos_x += TEXT_SIZE_8_WIDTH_6;
                j++;
            }
            break;

        case TEXT_SIZE_16:
            pos_y *= OFFSET_2;
            while (ch[j] != '\0') {
                c = ch[j] - ' ';
                if (pos_x > TEXT_16_MAX_ROW_POS) {
                    pos_x = 0;
                    pos_y++;
                }
                OLED_SetPos(pos_x, pos_y);
                for (i = 0; i < TEXT_SIZE_16_WIDTH_8; i++) {
                    OLED_WriteData(F8X16[c * OFFSET_16 + i]);
                }
                OLED_SetPos(pos_x, pos_y + 1);

                for (i = 0; i < TEXT_SIZE_16_WIDTH_8; i++) {
                    OLED_WriteData(F8X16[c * OFFSET_16 + i + OFFSET_8]);
                }
                pos_x += TEXT_SIZE_16_WIDTH_8;
                j++;
            }
            break;

        default:
            break;
    }
}

/**
 * @brief  显示一张图片
 * @note   取模方式： 阳码、列行式、逆向、十六进制数、输出索引格、C51格式
 * @param  xMove: x坐标
 * @param  yMove: y坐标
 * @param  width: 宽
 * @param  height: 高
 */
void OLED_DrawBMP(uint8_t xMove, uint8_t yMove,
                     uint8_t width, uint8_t height,
                     uint8_t *BMP)
{
    unsigned short j = 0, x = 0, y = 0;
    uint8_t x0 = xMove;
    uint8_t y0 = yMove / 8;
    uint8_t x1 = xMove + width;
    uint8_t y1 = height / 8 + y0;

    for (y = y0; y < y1; y++) {
        OLED_SetPos(x0, y);
        for (x = x0; x < x1; x++) {
            OLED_WriteData(BMP[j]);
            j++;
        }
    }
}



void OLED_Init(void)
{
    OLED_WriteCommand(0xAE); // display off
    OLED_WriteCommand(0x20); // Set Memory Addressing Mode
    OLED_WriteCommand(0x10); // 10,Page Addressing Mode (RESET)
    OLED_WriteCommand(0xb0); // Set Page Start Address for Page Addressing Mode,0-7
    OLED_WriteCommand(0xc8); // Set COM Output Scan Direction
    OLED_WriteCommand(0x00); // ---set low column address
    OLED_WriteCommand(0x10); // ---set high column address
    OLED_WriteCommand(0x40); // --set start line address
    OLED_WriteCommand(0x81); // --set contrast control register
    OLED_WriteCommand(0xff); //  亮度调节 0x00~0xff
    OLED_WriteCommand(0xa1); // --set segment re-map 0 to 127
    OLED_WriteCommand(0xa6); // --set normal display
    OLED_WriteCommand(0xa8); // --set multiplex ratio(1 to 64)
    OLED_WriteCommand(0x3F); //
    OLED_WriteCommand(0xa4); // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    OLED_WriteCommand(0xd3); // -set display offset
    OLED_WriteCommand(0x00); // -not offset
    OLED_WriteCommand(0xd5); // --set display clock divide ratio/oscillator frequency
    OLED_WriteCommand(0xf0); // --set divide ratio
    OLED_WriteCommand(0xd9); // --set pre-charge period
    OLED_WriteCommand(0x22); //
    OLED_WriteCommand(0xda); // --set com pins hardware configuration
    OLED_WriteCommand(0x12);
    OLED_WriteCommand(0xdb); // --set vcomh
    OLED_WriteCommand(0x20); //  0x20,0.77xVcc
    OLED_WriteCommand(0x8d); // --set DC-DC enable
    OLED_WriteCommand(0x14); //
    OLED_WriteCommand(0xaf); // --turn on oled panel
    OLED_WriteCommand(0xAF); // display ON
    OLED_SetPos(0, 0);
}




