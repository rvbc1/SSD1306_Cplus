/*
 * SSD1306.cpp
 *
 *  Created on: 22.03.2020
 *      Author: danie
 */

#include "SSD1306.h"

#if defined(SSD1306_USE_I2C)
void SSD1306::ssd1306_Reset(void) {
	/* for I2C - do nothing */
}
// Send a byte to the command registers
void SSD1306::registersAndBuffor.buffor.registerss[](uint8_t byte) {
	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
}
// Send data
void SSD1306::ssd1306_WriteData(uint8_t* buffer, size_t buff_size) {
	HAL_I2C_Mem_Write(&SSD1306_I2C_PORT, SSD1306_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
}

#elif defined(SSD1306_USE_SPI)
void SSD1306::ssd1306_Reset(void) {
	// CS = High (not selected)
	HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET);

	// Reset the OLED
	HAL_GPIO_WritePin(SSD1306_Reset_Port, SSD1306_Reset_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(SSD1306_Reset_Port, SSD1306_Reset_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
}
// Send a byte to the command registers
void SSD1306::ssd1306_WriteCommand(uint8_t byte) {
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_RESET); // select OLED
    HAL_GPIO_WritePin(SSD1306_DC_Port, SSD1306_DC_Pin, GPIO_PIN_RESET); // command
    HAL_SPI_Transmit(&SSD1306_SPI_PORT, (uint8_t *) &byte, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET); // un-select OLED
}
// Send data
void SSD1306::ssd1306_WriteData(uint8_t* buffer, size_t buff_size) {
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_RESET); // select OLED
    HAL_GPIO_WritePin(SSD1306_DC_Port, SSD1306_DC_Pin, GPIO_PIN_SET); // data
    HAL_SPI_Transmit(&SSD1306_SPI_PORT, buffer, buff_size, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SSD1306_CS_Port, SSD1306_CS_Pin, GPIO_PIN_SET); // un-select OLED
}

#else
#error "You should define SSD1306_USE_SPI or SSD1306_USE_I2C macro"
#endif

// Screenbuffer

void SSD1306::ssd1306_Init(void) {
	// Reset OLED
	ssd1306_Reset();

    // Wait for the screen to boot
    HAL_Delay(100);

    // Init OLED
    registersAndBuffor.buffor.registers[0]=0xAE; //display off

    registersAndBuffor.buffor.registers[1]=0x20; //Set Memory Addressing Mode
    registersAndBuffor.buffor.registers[2]=0x00; // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                // 10b,Page Addressing Mode (RESET); 11b,Invalid

    registersAndBuffor.buffor.registers[3]=0xB0; //Set Page Start Address for Page Addressing Mode,0-7

	#ifdef SSD1306_MIRROR_VERT
		registersAndBuffor.buffor.registers[4]=0xC0; // Mirror vertically
	#else
		registersAndBuffor.buffor.registers[4]=0xC8; //Set COM Output Scan Direction
	#endif

    registersAndBuffor.buffor.registers[5]=0x00; //---set low column address
    registersAndBuffor.buffor.registers[6]=0x10; //---set high column address

    registersAndBuffor.buffor.registers[7]=0x40; //--set start line address - CHECK

    registersAndBuffor.buffor.registers[8]=0x81; //--set contrast control registers - CHECK
    registersAndBuffor.buffor.registers[9]=0xFF;

	#ifdef SSD1306_MIRROR_HORIZ
		registersAndBuffor.buffor.registers[10]=0xA0; // Mirror horizontally
	#else
		registersAndBuffor.buffor.registers[10]=0xA1; //--set segment re-map 0 to 127 - CHECK
	#endif

	#ifdef SSD1306_INVERSE_COLOR
		registersAndBuffor.buffor.registers[11]=0xA7; //--set inverse color
	#else
		registersAndBuffor.buffor.registers[11]=0xA6; //--set normal color
	#endif

    registersAndBuffor.buffor.registers[12]=0xA8; //--set multiplex ratio(1 to 64) - CHECK
	#if (SSD1306_HEIGHT == 32)
		registersAndBuffor.buffor.registers[13]=0x1F; //
	#elif (SSD1306_HEIGHT == 64)
		registersAndBuffor.buffor.registers[13]=0x3F; //
	#else
	#error "Only 32 or 64 lines of height are supported!"
	#endif

    registersAndBuffor.buffor.registers[14]=0xA4; //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    registersAndBuffor.buffor.registers[15]=0xD3; //-set display offset - CHECK
    registersAndBuffor.buffor.registers[16]=0x00; //-not offset

    registersAndBuffor.buffor.registers[17]=0xD5; //--set display clock divide ratio/oscillator frequency
    registersAndBuffor.buffor.registers[18]=0xF0; //--set divide ratio

    registersAndBuffor.buffor.registers[19]=0xD9; //--set pre-charge period
    registersAndBuffor.buffor.registers[20]=0x22; //

    registersAndBuffor.buffor.registers[21]=0xDA; //--set com pins hardware configuration - CHECK
	#if (SSD1306_HEIGHT == 32)
		registersAndBuffor.buffor.registers[22]=0x02;
	#elif (SSD1306_HEIGHT == 64)
		registersAndBuffor.buffor.registers[22]=0x12;
	#else
	#error "Only 32 or 64 lines of height are supported!"
	#endif

    registersAndBuffor.buffor.registers[23]=0xDB; //--set vcomh
    registersAndBuffor.buffor.registers[24]=0x20; //0x20,0.77xVcc

    registersAndBuffor.buffor.registers[25]=0x8D; //--set DC-DC enable
    registersAndBuffor.buffor.registers[26]=0x14; //
    registersAndBuffor.buffor.registers[27]=0xAF; //--turn on SSD1306 panel

    // Clear screen
    ssd1306_Fill(Black);

    // Flush buffer to screen
    //ssd1306_UpdateScreen();

    // Set default values for screen object
    currentX = 0;
    currentY = 0;

    initialized = 1;
}

void SSD1306::process(){
	//components to display


	ssd1306_UpdateScreen();
	HAL_Delay(5);
}

// Fill the whole screen with the given color
void SSD1306::ssd1306_Fill(SSD1306_COLOR color) {
    /* Set memory */
    uint32_t i;
    for(int j=0; j<8; j++){
        registersAndBuffor.buffor.registers[28]=0xB0 + j;
        registersAndBuffor.buffor.registers[29]=0x00;
        registersAndBuffor.buffor.registers[30]=0x10;
		for(i = 0; i < sizeof(registersAndBuffor.buffor.SSD1306_Buffer)/8; i++) {
			registersAndBuffor.buffor.SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
		}
    }
}

// Write the screenbuffer with changed to the screen
void SSD1306::ssd1306_UpdateScreen(void) {
    uint8_t i;
    for(i = 0; i < 8; i++) {
        registersAndBuffor.buffor.registers[28]=0xB0 + i;
        registersAndBuffor.buffor.registers[29]=0x00;
        registersAndBuffor.buffor.registers[30]=0x10;
        ssd1306_WriteData(&registersAndBuffor.buffor.SSD1306_Buffer[SSD1306_WIDTH*i],SSD1306_WIDTH);
    }
}

//    Draw one pixel in the screenbuffer
//    X => X Coordinate
//    Y => Y Coordinate
//    color => Pixel color
void SSD1306::ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        // Don't write outside the buffer
        return;
    }

    // Check if pixel should be inverted
    if(inverted) {
        color = (SSD1306_COLOR)!color;
    }

    // Draw in the right color
    	int j=(int)y/8.;
        registersAndBuffor.buffor.registers[28]=0xB0 + j;
        registersAndBuffor.buffor.registers[29]=0x00;
        registersAndBuffor.buffor.registers[30]=0x10;
		if(color == White) {
			registersAndBuffor.buffor.SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH/8] |= 1 << (y % 8);
		} else {
			registersAndBuffor.buffor.SSD1306_Buffer[x + (y / 8) * SSD1306_WIDTH/8] &= ~(1 << (y % 8));
		}
}

// Draw 1 char to the screen buffer
// ch       => char om weg te schrijven
// Font     => Font waarmee we gaan schrijven
// color    => Black or White
char SSD1306::ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color) {
    uint32_t i, b, j;

    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;

    // Check remaining space on current line
    if (SSD1306_WIDTH < (currentX + Font.FontWidth) ||
        SSD1306_HEIGHT < (currentY + Font.FontHeight))
    {
        // Not enough space on current line
        return 0;
    }

    // Use the font to write
    for(i = 0; i < Font.FontHeight; i++) {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for(j = 0; j < Font.FontWidth; j++) {
            if((b << j) & 0x8000)  {
                ssd1306_DrawPixel(currentX + j, (currentY + i), (SSD1306_COLOR) color);
            } else {
                ssd1306_DrawPixel(currentX + j, (currentY + i), (SSD1306_COLOR)!color);
            }
        }
    }

    // The current space is now taken
    currentX += Font.FontWidth;

    // Return written char for validation
    return ch;
}

// Write full string to screenbuffer
char SSD1306::ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color) {
    // Write until null-byte
    while (*str) {
        if (ssd1306_WriteChar(*str, Font, color) != *str) {
            // Char could not be written
            return *str;
        }

        // Next char
        str++;
    }

    // Everything ok
    return *str;
}

// Position the cursor
void SSD1306::ssd1306_SetCursor(uint8_t x, uint8_t y) {
    currentX = x;
    currentY = y;
}

SSD1306::SSD1306() {
	// TODO Auto-generated constructor stub
}

SSD1306::~SSD1306() {
	// TODO Auto-generated destructor stub
}
