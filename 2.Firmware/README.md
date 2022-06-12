# Project

* board: ESP32
* flash size: 4MB
* esp-idf: v4.4.1

## About LCD TFT

* LCD driver: ST7789
* LCD module: 1.3 TFT SPI 240x320

<img src="assets/lcd.PNG" width="75%" > </img>

## About LVGL

* LVGL : v8.0.0-194-gd79ca388
* LVGL commit : d79ca38
* LVGL esp32 drivers commit: a68ce89 

## Wiring

| module TFT   | ESP32    |
| ---          | ---      |
| SDA (MOSI)   |  gpio 23 |
| SCK          |  gpio 18 |
| BLK          |  gpio 12 |
| DC           |  gpio 2  |
| RESET        |  gpio 4  |
| CS           |  gpio NC |


##  其他记录
    -   增加timer task stack到4096.
    -   lvgl颜色还没有配置正确 BGR->RGB, 初始化添加 `{ST7789_RAMCTRL,{0x00, 0xC8}, 2},` 开启颜色反转
    -   lvgl demo.
