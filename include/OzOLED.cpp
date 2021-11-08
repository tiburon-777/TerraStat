/*
  OzOLED.cpp - 0.96' I2C 128x64 OLED Driver Library
  2014 Copyright (c) OscarLiang.net  All right reserved.
 
  Author: Oscar Liang
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

*/




#include "OzOLED.h"
#include <Wire.h>
#include <avr/pgmspace.h>


// 8x8 Font ASCII 32 - 127 Implemented
// Users can modify this to support more characters(glyphs)
// BasicFont is placed in code memory.

// This font be freely used without any restriction(It is placed in public domain)
const byte BasicFont[][8] PROGMEM = {
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x00,0x5F,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x00,0x07,0x00,0x07,0x00,0x00,0x00},
	{0x00,0x14,0x7F,0x14,0x7F,0x14,0x00,0x00},
	{0x00,0x24,0x2A,0x7F,0x2A,0x12,0x00,0x00},
	{0x00,0x23,0x13,0x08,0x64,0x62,0x00,0x00},
	{0x00,0x36,0x49,0x55,0x22,0x50,0x00,0x00},
	{0x00,0x00,0x05,0x03,0x00,0x00,0x00,0x00},
	{0x00,0x1C,0x22,0x41,0x00,0x00,0x00,0x00},
	{0x00,0x41,0x22,0x1C,0x00,0x00,0x00,0x00},
	{0x00,0x08,0x2A,0x1C,0x2A,0x08,0x00,0x00},
	{0x00,0x08,0x08,0x3E,0x08,0x08,0x00,0x00},
	{0x00,0xA0,0x60,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x08,0x08,0x08,0x08,0x08,0x00,0x00},
	{0x00,0x60,0x60,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x20,0x10,0x08,0x04,0x02,0x00,0x00},
	{0x00,0x3E,0x51,0x49,0x45,0x3E,0x00,0x00},
	{0x00,0x00,0x42,0x7F,0x40,0x00,0x00,0x00},
	{0x00,0x62,0x51,0x49,0x49,0x46,0x00,0x00},
	{0x00,0x22,0x41,0x49,0x49,0x36,0x00,0x00},
	{0x00,0x18,0x14,0x12,0x7F,0x10,0x00,0x00},
	{0x00,0x27,0x45,0x45,0x45,0x39,0x00,0x00},
	{0x00,0x3C,0x4A,0x49,0x49,0x30,0x00,0x00},
	{0x00,0x01,0x71,0x09,0x05,0x03,0x00,0x00},
	{0x00,0x36,0x49,0x49,0x49,0x36,0x00,0x00},
	{0x00,0x06,0x49,0x49,0x29,0x1E,0x00,0x00},
	{0x00,0x00,0x36,0x36,0x00,0x00,0x00,0x00},
	{0x00,0x00,0xAC,0x6C,0x00,0x00,0x00,0x00},
	{0x00,0x08,0x14,0x22,0x41,0x00,0x00,0x00},
	{0x00,0x14,0x14,0x14,0x14,0x14,0x00,0x00},
	{0x00,0x41,0x22,0x14,0x08,0x00,0x00,0x00},
	{0x00,0x02,0x01,0x51,0x09,0x06,0x00,0x00},
	{0x00,0x32,0x49,0x79,0x41,0x3E,0x00,0x00},
	{0x00,0x7E,0x09,0x09,0x09,0x7E,0x00,0x00},
	{0x00,0x7F,0x49,0x49,0x49,0x36,0x00,0x00},
	{0x00,0x3E,0x41,0x41,0x41,0x22,0x00,0x00},
	{0x00,0x7F,0x41,0x41,0x22,0x1C,0x00,0x00},
	{0x00,0x7F,0x49,0x49,0x49,0x41,0x00,0x00},
	{0x00,0x7F,0x09,0x09,0x09,0x01,0x00,0x00},
	{0x00,0x3E,0x41,0x41,0x51,0x72,0x00,0x00},
	{0x00,0x7F,0x08,0x08,0x08,0x7F,0x00,0x00},
	{0x00,0x41,0x7F,0x41,0x00,0x00,0x00,0x00},
	{0x00,0x20,0x40,0x41,0x3F,0x01,0x00,0x00},
	{0x00,0x7F,0x08,0x14,0x22,0x41,0x00,0x00},
	{0x00,0x7F,0x40,0x40,0x40,0x40,0x00,0x00},
	{0x00,0x7F,0x02,0x0C,0x02,0x7F,0x00,0x00},
	{0x00,0x7F,0x04,0x08,0x10,0x7F,0x00,0x00},
	{0x00,0x3E,0x41,0x41,0x41,0x3E,0x00,0x00},
	{0x00,0x7F,0x09,0x09,0x09,0x06,0x00,0x00},
	{0x00,0x3E,0x41,0x51,0x21,0x5E,0x00,0x00},
	{0x00,0x7F,0x09,0x19,0x29,0x46,0x00,0x00},
	{0x00,0x26,0x49,0x49,0x49,0x32,0x00,0x00},
	{0x00,0x01,0x01,0x7F,0x01,0x01,0x00,0x00},
	{0x00,0x3F,0x40,0x40,0x40,0x3F,0x00,0x00},
	{0x00,0x1F,0x20,0x40,0x20,0x1F,0x00,0x00},
	{0x00,0x3F,0x40,0x38,0x40,0x3F,0x00,0x00},
	{0x00,0x63,0x14,0x08,0x14,0x63,0x00,0x00},
	{0x00,0x03,0x04,0x78,0x04,0x03,0x00,0x00},
	{0x00,0x61,0x51,0x49,0x45,0x43,0x00,0x00},
	{0x00,0x7F,0x41,0x41,0x00,0x00,0x00,0x00},
	{0x00,0x02,0x04,0x08,0x10,0x20,0x00,0x00},
	{0x00,0x41,0x41,0x7F,0x00,0x00,0x00,0x00},
	{0x00,0x04,0x02,0x01,0x02,0x04,0x00,0x00},
	{0x00,0x80,0x80,0x80,0x80,0x80,0x00,0x00},
	{0x00,0x01,0x02,0x04,0x00,0x00,0x00,0x00},
	{0x00,0x20,0x54,0x54,0x54,0x78,0x00,0x00},
	{0x00,0x7F,0x48,0x44,0x44,0x38,0x00,0x00},
	{0x00,0x38,0x44,0x44,0x28,0x00,0x00,0x00},
	{0x00,0x38,0x44,0x44,0x48,0x7F,0x00,0x00},
	{0x00,0x38,0x54,0x54,0x54,0x18,0x00,0x00},
	{0x00,0x08,0x7E,0x09,0x02,0x00,0x00,0x00},
	{0x00,0x18,0xA4,0xA4,0xA4,0x7C,0x00,0x00},
	{0x00,0x7F,0x08,0x04,0x04,0x78,0x00,0x00},
	{0x00,0x00,0x7D,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x80,0x84,0x7D,0x00,0x00,0x00,0x00},
	{0x00,0x7F,0x10,0x28,0x44,0x00,0x00,0x00},
	{0x00,0x41,0x7F,0x40,0x00,0x00,0x00,0x00},
	{0x00,0x7C,0x04,0x18,0x04,0x78,0x00,0x00},
	{0x00,0x7C,0x08,0x04,0x7C,0x00,0x00,0x00},
	{0x00,0x38,0x44,0x44,0x38,0x00,0x00,0x00},
	{0x00,0xFC,0x24,0x24,0x18,0x00,0x00,0x00},
	{0x00,0x18,0x24,0x24,0xFC,0x00,0x00,0x00},
	{0x00,0x00,0x7C,0x08,0x04,0x00,0x00,0x00},
	{0x00,0x48,0x54,0x54,0x24,0x00,0x00,0x00},
	{0x00,0x04,0x7F,0x44,0x00,0x00,0x00,0x00},
	{0x00,0x3C,0x40,0x40,0x7C,0x00,0x00,0x00},
	{0x00,0x1C,0x20,0x40,0x20,0x1C,0x00,0x00},
	{0x00,0x3C,0x40,0x30,0x40,0x3C,0x00,0x00},
	{0x00,0x44,0x28,0x10,0x28,0x44,0x00,0x00},
	{0x00,0x1C,0xA0,0xA0,0x7C,0x00,0x00,0x00},
	{0x00,0x44,0x64,0x54,0x4C,0x44,0x00,0x00},
	{0x00,0x08,0x36,0x41,0x00,0x00,0x00,0x00},
	{0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00},
	{0x00,0x41,0x36,0x08,0x00,0x00,0x00,0x00},
	{0x00,0x02,0x01,0x01,0x02,0x01,0x00,0x00},
	{0x00,0x02,0x05,0x05,0x02,0x00,0x00,0x00} 
};


// Big numbers font, from 0 to 9 - 96 bytes each.
const byte bigNumbers [][96] PROGMEM = {
{0x00, 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0xF0, 0xE0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x80, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x0F, 0x0F,
0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00},

{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0,
0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

{0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0xC1, 0xC0, 0xC0, 0xC0,
0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
0x03, 0x03, 0x83, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x0F, 0x0F, 0x0F,
0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00},

{0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0xC1, 0xC0, 0xC0, 0xC0,
0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE1, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x81, 0x83, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x87,
0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x0F, 0x0F, 0x0F,
0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00},

{0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0,
0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x07,
0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00},

{0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1,
0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC1, 0x81, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x81, 0x83, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x87,
0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x0F, 0x0F, 0x0F,
0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00},

{0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1,
0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC1, 0x81, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x87, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x87,
0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x0F, 0x0F, 0x0F,
0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00},

{0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00},

{0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0xFF, 0xE1,
0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE1, 0xFF, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0x87, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x87,
0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0F, 0x0F, 0x0F, 0x0F,
0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00},

{0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
0xF0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xE1,
0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xC0, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x07,
0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0F, 0x0F, 0x0F, 0x07, 0x00, 0x00, 0x00, 0x00},

{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x3C, 0x7E, 0x7E, 0x7E, 0x7E, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0xF8, 0xF8, 0xF8, 0xF0, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};


// SSD1306 Commandset
// ------------------
// Fundamental Commands
#define ASA_DISPLAY_ALL_ON_RESUME            0xA4
// Addressing Setting Commands
#define ASA_MEMORY_ADDR_MODE                0x20
// Hardware Configuration Commands
#define ASA_SET_START_LINE                0x40
#define ASA_SET_SEGMENT_REMAP                0xA0
#define ASA_SET_MULTIPLEX_RATIO                0xA8
#define ASA_COM_SCAN_DIR_DEC            0xC8
#define ASA_SET_DISPLAY_OFFSET            0xD3
#define ASA_SET_COM_PINS                0xDA
#define ASA_CHARGE_PUMP               0x8D
// Timing & Driving Scheme Setting Commands
#define ASA_SET_DISPLAY_CLOCK_DIV_RATIO        0xD5
#define ASA_SET_PRECHARGE_PERIOD        0xD9
#define ASA_SET_VCOM_DESELECT                0xDB



// ====================== LOW LEVEL =========================

void OzOLED::sendCommand(byte command){
	Wire.beginTransmission(OLED_ADDRESS); // begin transmitting
	Wire.write(OzOLED_COMMAND_MODE);//data mode
	Wire.write(command);
	Wire.endTransmission();    // stop transmitting
}


void OzOLED::sendData(byte data){
	
	Wire.beginTransmission(OLED_ADDRESS); // begin transmitting
	Wire.write(OzOLED_DATA_MODE);//data mode
	Wire.write(data);
	Wire.endTransmission();    // stop transmitting

}

void OzOLED::printChar(char C, byte X, byte Y){

	if ( X < 128 )
		setCursorXY(X, Y);

	//Ignore unused ASCII characters. Modified the range to support multilingual characters.
    if(C < 32 || C > 127)
		C='*'; //star - indicate characters that can't be displayed

	
    for(byte i=0; i<8; i++) {
	
       //read bytes from code memory
       sendData(pgm_read_byte(&BasicFont[C-32][i])); //font array starts at 0, ASCII starts at 32. Hence the translation
	 
    }
}

void OzOLED::printString(const char *String, byte X, byte Y, byte numChar){

	if ( X < 128 )
		setCursorXY(X, Y);

	
	byte count=0;
    while(String[count] && count<numChar){
		printChar(String[count++]);  
	}

}


byte OzOLED::printNumber(long long_num, byte X, byte Y){

	if ( X < 128 )
		setCursorXY(X, Y);


	byte char_buffer[10] = "";
	byte i = 0;
	byte f = 0; // number of characters

	if (long_num < 0) {
	
		f++;
		printChar('-');
		long_num = -long_num;
	
	} 
	else if (long_num == 0) {
	
		f++;
		printChar('0');
		return f;
	
	} 

	while (long_num > 0) {
	
		char_buffer[i++] = long_num % 10;
		long_num /= 10;
	
	}

	f += i;
	for(; i > 0; i--) {

		printChar('0'+ char_buffer[i - 1]);

	}
	
	return f;

}

byte OzOLED::printNumber16(long long_num, byte X, byte Y){

	if ( X < 128 )
		setCursorXY(X, Y);


	byte char_buffer[10] = "";
	byte i = 0;
	byte f = 0; // number of characters

	if (long_num < 0) {
	
		f++;
		printChar16('-', X, Y);
		long_num = -long_num;
	
	} 
	else if (long_num == 0) {
	
		f++;
		printChar16('0', X, Y);
		return f;
	
	} 

	while (long_num > 0) {
	
		char_buffer[i++] = long_num % 10;
		long_num /= 10;
	
	}

	f += i;
	for(; i > 0; i--) {

		printChar16('0'+ char_buffer[i - 1], X, Y);
		X += 2;

	}
	
	return f;

}


byte OzOLED::printNumber(float float_num, byte prec, byte X, byte Y){

	if ( X < 128 )
		setCursorXY(X, Y);

// prec - 6 maximum

	byte num_int = 0;
	byte num_frac = 0;
	byte num_extra = 0;
	
	long d = float_num; // get the integer part
	float f = float_num - d; // get the fractional part
	
	
	if (d == 0 && f < 0.0){
	
		printChar('-');
		num_extra++;
		printChar('0');
		num_extra++;
		f *= -1;
		
	}
	else if (d < 0 && f < 0.0){
	
		num_int = printNumber(d); // count how many digits in integer part
		f *= -1;
		
	}
	else{
	
		num_int = printNumber(d); // count how many digits in integer part
	
	}
	
	// only when fractional part > 0, we show decimal point
	if (f > 0.0){
	
		printChar('.');
		num_extra++;
	
		long f_shift = 1;
		
		if (num_int + prec > 8) 
			prec = 8 - num_int;
		
		for (byte j=0; j<prec; j++){
			f_shift *= 10;
		}

		num_frac = printNumber((long)(f*f_shift)); // count how many digits in fractional part
		
	}
	
	return num_int + num_frac + num_extra;

}

unsigned int EnlardeByte2Word(char b)
{
    unsigned int d = 0;
    for (byte i = 0; i < 8; i++)
    {
        unsigned int e = (((unsigned int)b) & (1 << i)) << i;
        d = d | e | (e << 1);
    }
    return d;
}

void OzOLED::printChar16(char C, byte X, byte Y){
    setCursorXY(X, Y);
    if(C < 32 || C > 127)        //Ignore unused ASCII characters.
        C='*';        //star - indicate characters that can't be displayed
    unsigned int m = 0;
    unsigned char n[8];

    for ( byte i=0; i<8; i++) {
        m = EnlardeByte2Word(pgm_read_byte(&BasicFont[C-32][i]));
        sendData(lowByte(m));
        sendData(lowByte(m));
        n[i] = highByte(m);
    }
    setCursorXY(X, Y+1);

    for(byte i=0; i<8; i++) {
        sendData(n[i]); //font array starts at 0, ASCII starts at 32. Hence the translation
        sendData(n[i]); //font array starts at 0, ASCII starts at 32. Hence the translation
    }
}

void OzOLED::printString16(const char *String, byte X, byte Y, byte numChar){
    byte count=0;
    while(String[count] && count<numChar){
        printChar16(String[count++], X, Y);
        X += 2;
    }

}

void OzOLED::printBigNumber(const char *number, byte X, byte Y, byte numChar){
// big number pixels: 24 x 32

 // Y - page
	byte column = 0;
	byte count = 0;

	while(number[count] && count<numChar){
	
	
		setCursorXY(X, Y);
		
		for(byte i=0; i<96; i++) {
		
			// if character is not "0-9" or ':'
			if(number[count] < 48 || number[count] > 58)	
				sendData(0);
			else 				
				sendData(pgm_read_byte(&bigNumbers[number[count]-48][i]));
			
			
			if(column >= 23){
				column = 0;
				setCursorXY(X, ++Y);
			}
			else				
				column++;

		}
		
		count++;
		
		X = X + 3;
		Y = Y - 4;
		
	
	}

	
	
}


void OzOLED::drawBitmap(const byte *bitmaparray, byte X, byte Y, byte width, byte height){

// max width = 16
// max height = 8

	setCursorXY( X, Y );
	
	byte column = 0; 
	for(int i=0; i<width*8*height; i++) {  

		sendData(pgm_read_byte(&bitmaparray[i]));
		
		if(++column == width*8) {
			column = 0;
			setCursorXY( X, ++Y );
		} 
	}

}



// =================== High Level ===========================


void OzOLED::init(){
	Wire.begin();
	
	// upgrade to 400KHz! (only use when your other i2c device support this speed)
	if (I2C_400KHZ){
		// save I2C bitrate (default 100Khz)
		byte twbrbackup = TWBR;
		TWBR = 12; 
		//TWBR = twbrbackup;
		//Serial.println(TWBR, DEC);
		//Serial.println(TWSR & 0x3, DEC);
	}
	
    setPowerOff(); 	//display off
    delay(10);
    setPowerOn();	//display on
    delay(10); 
    setNormalDisplay();  //default Set Normal Display
	setPageMode();	// default addressing mode
	clearDisplay();
	setCursorXY(0,0);

// Additional command
    OzOled.setPowerOff();
    OzOled.sendCommand(ASA_SET_DISPLAY_CLOCK_DIV_RATIO);
    OzOled.sendCommand(0x80);
    OzOled.sendCommand(ASA_SET_MULTIPLEX_RATIO);
    OzOled.sendCommand(0x3F);
    OzOled.sendCommand(ASA_SET_DISPLAY_OFFSET);
    OzOled.sendCommand(0x0);
    OzOled.sendCommand(ASA_SET_START_LINE | 0x0);
    OzOled.sendCommand(ASA_CHARGE_PUMP);
        OzOled.sendCommand(0x14);
    OzOled.sendCommand(ASA_MEMORY_ADDR_MODE);
    OzOled.sendCommand(0x00);
    OzOled.sendCommand(ASA_SET_SEGMENT_REMAP | 0x1);
    OzOled.sendCommand(ASA_COM_SCAN_DIR_DEC);
    OzOled.sendCommand(ASA_SET_COM_PINS);
    OzOled.sendCommand(0x12);
    OzOled.setBrightness(0xCF);
    OzOled.sendCommand(ASA_SET_PRECHARGE_PERIOD);
        OzOled.sendCommand(0xF1);
    OzOled.sendCommand(ASA_SET_VCOM_DESELECT);
    OzOled.sendCommand(0x40);
    OzOled.sendCommand(ASA_DISPLAY_ALL_ON_RESUME);
    OzOled.setNormalDisplay();
    OzOled.setPowerOn();	

}

void OzOLED::setCursorXY(byte X, byte Y){
	// Y - 1 unit = 1 page (8 pixel rows)
	// X - 1 unit = 8 pixel columns

    sendCommand(0x00 + (8*X & 0x0F)); 		//set column lower address
    sendCommand(0x10 + ((8*X>>4)&0x0F)); 	//set column higher address
	sendCommand(0xB0 + Y); 					//set page address
	
}


void OzOLED::clearDisplay()	{


	for(byte page=0; page<8; page++) {	
	
		setCursorXY(0, page);     
		for(byte column=0; column<128; column++){  //clear all columns
			sendData(0);    
		}

	}
	
	setCursorXY(0,0);  
	
}

/*
void OzOLED::clearPage(byte page)	{
	// clear page and set cursor at beginning of that page

	setCursorXY(0, page);    
	for(byte column=0; column<128; column++){  //clear all columns
		sendData(0x00);    
	}
	
}
*/


void OzOLED::setInverseDisplay(){

	sendCommand(OzOLED_CMD_INVERSE_DISPLAY);
	
}

void OzOLED::setNormalDisplay(){

	sendCommand(OzOLED_CMD_NORMAL_DISPLAY);
	
}

void OzOLED::setPowerOff(){

	sendCommand(OzOLED_CMD_DISPLAY_OFF);
	
}

void OzOLED::setPowerOn(){

	sendCommand(OzOLED_CMD_DISPLAY_ON);
	
}

void OzOLED::setBrightness(byte Brightness){

	sendCommand(OzOLED_CMD_SET_BRIGHTNESS);
	sendCommand(Brightness);
   
}

void OzOLED::setPageMode(){
	addressingMode = PAGE_ADDRESSING;
	sendCommand(0x20); 				//set addressing mode
	sendCommand(PAGE_ADDRESSING); 	//set page addressing mode
}

void OzOLED::setHorizontalMode(){
	addressingMode = HORIZONTAL_ADDRESSING;
	sendCommand(0x20); 				//set addressing mode
	sendCommand(HORIZONTAL_ADDRESSING); 	//set page addressing mode
}


// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// scrollRight(0x00, 0x0F)  - start - stop
void OzOLED::scrollRight(byte start, byte end, byte speed){

    sendCommand(OzOLED_RIGHT_SCROLL);  //Horizontal Scroll Setup
    sendCommand(0x00);	// dummy byte 
    sendCommand(start);	// start page address
    sendCommand(speed);	// set time interval between each scroll
    sendCommand(end);	// end page address
	
    sendCommand(0x01);  
    sendCommand(0xFF);
	
    sendCommand(0x2f);  //active scrolling
	
}


// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)   - start - stop
void OzOLED::scrollLeft(byte start, byte end, byte speed){

    sendCommand(OzOLED_LEFT_SCROLL);  //Horizontal Scroll Setup
    sendCommand(0x00);	// dummy byte
    sendCommand(start);	// start page address
    sendCommand(speed);	// set time interval between each scroll
    sendCommand(end);	// end page address
	
    sendCommand(0x01);  
    sendCommand(0xFF);  
	
    sendCommand(0x2f);  //active scrolling
	
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F) 
void OzOLED::scrollDiagRight(){

        sendCommand(OzOLED_SET_VERTICAL_SCROLL_AREA);        
        sendCommand(0X00);
        sendCommand(OzOLED_Max_Y);
        sendCommand(OzOLED_VERTICAL_RIGHT_SCROLL); //Vertical and Horizontal Scroll Setup
        sendCommand(0X00); 	//dummy byte
        sendCommand(0x00);	 //define page0 as startpage address
        sendCommand(0X00);	//set time interval between each scroll ste as 6 frames
        sendCommand(0x07);	//define page7 as endpage address
        sendCommand(0X01);	//set vertical scrolling offset as 1 row
        sendCommand(OzOLED_CMD_ACTIVATE_SCROLL); //active scrolling
	
}

void OzOLED::scrollDiagLeft(){

        sendCommand(OzOLED_SET_VERTICAL_SCROLL_AREA);        
        sendCommand(0X00);
        sendCommand(OzOLED_Max_Y);
        sendCommand(OzOLED_VERTICAL_LEFT_SCROLL); //Vertical and Horizontal Scroll Setup
        sendCommand(0X00); //dummy byte
        sendCommand(0x00);	 //define page0 as startpage address
        sendCommand(0X00);	//set time interval between each scroll ste as 6 frames
        sendCommand(0x07);	//define page7 as endpage address
        sendCommand(0X01);	//set vertical scrolling offset as 1 row
        sendCommand(OzOLED_CMD_ACTIVATE_SCROLL); //active scrolling
	
}


void OzOLED::setActivateScroll(byte direction, byte startPage, byte endPage, byte scrollSpeed){


/*
This function is still not complete, we need more testing also.
Use the following defines for 'direction' :

 Scroll_Left			
 Scroll_Right			

For Scroll_vericle, still need to debug more... 

Use the following defines for 'scrollSpeed' :

 Scroll_2Frames		
 Scroll_3Frames
 Scroll_4Frames
 Scroll_5Frames	
 Scroll_25Frames
 Scroll_64Frames
 Scroll_128Frames
 Scroll_256Frames

*/


	if(direction == Scroll_Right) {
	
		//Scroll Right
		sendCommand(0x26);
		
	}
	else {
	
		//Scroll Left  
		sendCommand(0x27);

	}
	/*
	else if (direction == Scroll_Up ){
	
		//Scroll Up  
		sendCommand(0x29);
	
	}
	else{
	
		//Scroll Down  
		sendCommand(0x2A);
	
	}
	*/
	sendCommand(0x00);//dummy byte
	sendCommand(startPage);
	sendCommand(scrollSpeed);	
	sendCommand(endPage);		// for verticle scrolling, use 0x29 as command, endPage should = start page = 0
	
	/*
	if(direction == Scroll_Up) {
	
		sendCommand(0x01);

	}
	*/

	sendCommand(OzOLED_CMD_ACTIVATE_SCROLL);

}

void OzOLED::setDeactivateScroll(){

	sendCommand(OzOLED_CMD_DEACTIVATE_SCROLL);

}




OzOLED OzOled;  // Preinstantiate Objects
