/*
*
* Graphic display driver for LCD with ST7565 controller
*
* Originally by Sebastian, see http://www.mikrocontroller.net/topic/75589
*
* Reorganized and modified by Markus Baertschi
*/

#define g_draw_line_h(x,y,len) g_draw_line(x,y,len,0)
#define g_draw_line_v(x,y,len) g_draw_line(x,y,len,1)

uint8_t g_draw_string(uint8_t,uint8_t, char *,uint8_t);
