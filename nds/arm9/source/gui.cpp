/*
*   This file is part of Universal-Core
*   Copyright (C) 2020 DeadPhoenix8091, Epicpkmn11, Flame, RocketRobz, StackZ, TotallyNotGuy
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "gui.hpp"
#include "screenCommon.hpp"
#include "fontHandler.h"
#include "myDSiMode.h"
#include "getFileSize.h"
#include "tonccpy.h"

#include "arrow_back.h"
#include "cursor.h"
#include "item_button0.h"
#include "item_button1.h"
#include "item_button2.h"
#include "icon_female.h"
#include "icon_male.h"

#include <nds.h>
#include <unistd.h>

std::unique_ptr<Screen> usedScreen, tempScreen; // tempScreen used for "fade" effects.
bool ditherlaceOnVBlank = false;
bool currentScreen = false;
bool secondFrame = false;
bool fadeout = false;
bool fadein = false;
int fadealpha = 0;
int fadecolor = 0;

int bg2Main;
int bg3Main;
int bg2Sub;
int bg3Sub;

u16* gfxSub;

u16* colorTable = NULL;

// Ported from PAlib (obsolete)
void SetBrightness(u8 screen, s8 bright) {
	u16 mode = 1 << 14;

	if (bright < 0) {
		mode = 2 << 14;
		bright = -bright;
	}
	if (bright > 31)
		bright = 31;
	*(vu16 *)(0x0400006C + (0x1000 * screen)) = bright + mode;
}

// Clear Text.
void Gui::clearTextBufs(void) {  }

// Draw a sprite from the sheet.
void Gui::DrawSprite(int imgindex, int x, int y) {
}

void Gui__ChangeBrightness() {
	if (dsiFeatures() && ditherlaceOnVBlank) {
		secondFrame ? bgShow(bg3Main) : bgHide(bg3Main);
		secondFrame = !secondFrame;
	}

	SetBrightness(0, (fadecolor==255 ? fadealpha : -fadealpha)/8);
	SetBrightness(1, (fadecolor==255 ? fadealpha : -fadealpha)/8);
}

void applyColorLut(u16 *palette, int size) {
	if (!colorTable) {
		return;
	}
	for (int i = 0; i < size; i++) {
		palette[i] = colorTable[palette[i] % 0x8000];
	}
}

// Copies a palette and applies color LUT if loaded
void copyPalette16(u16 *dst, const u16 *src, int size) {
	if (colorTable) {
		for (int i = 0; i < size; i++) {
			dst[i] = colorTable[src[i] % 0x8000];
		}
		return;
	}
	tonccpy(dst, src, size*2);
}

// Copies a palette and applies color LUT if loaded
void copyPalette(u16 *dst, const u16 *src, int size) {
	if (colorTable) {
		for (int i = 0; i < size/2; i++) {
			dst[i] = colorTable[src[i] % 0x8000];
		}
		return;
	}
	tonccpy(dst, src, size);
}

// Initialize GUI.
void Gui::init(void) {
	*(vu16*)0x0400006C |= BIT(14);
	*(vu16*)0x0400006C &= BIT(15);
	SetBrightness(0, 31);
	SetBrightness(1, 31);

	videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
	videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);

	vramSetBankA(VRAM_A_MAIN_BG);
	vramSetBankB(VRAM_B_MAIN_BG);
	vramSetBankC(VRAM_C_SUB_BG);
	vramSetBankD(VRAM_D_LCD);
	vramSetBankG(VRAM_G_MAIN_SPRITE);
	vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
	vramSetBankI(VRAM_I_SUB_SPRITE);

	if (access("/_nds/colorLut/currentSetting.txt", F_OK) == 0) {
		// Load color LUT
		char lutName[128] = {0};
		FILE* file = fopen("/_nds/colorLut/currentSetting.txt", "rb");
		fread(lutName, 1, 128, file);
		fclose(file);

		char colorLutPath[256];
		sprintf(colorLutPath, "/_nds/colorLut/%s.lut", lutName);

		if (getFileSize(colorLutPath) == 0x10000) {
			colorTable = new u16[0x10000/sizeof(u16)];

			file = fopen(colorLutPath, "rb");
			fread(colorTable, 1, 0x10000, file);
			fclose(file);

			tonccpy(VRAM_D, colorTable, 0x10000); // Copy LUT to VRAM
			delete[] colorTable; // Free up RAM space
			colorTable = VRAM_D;
		}
	}

	bg3Main = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
	bgSetPriority(bg3Main, 0);

	bg2Main = bgInit(2, BgType_Bmp16, BgSize_B16_256x256, 6, 0);
	bgSetPriority(bg2Main, 1);

	bg3Sub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
	bgSetPriority(bg3Sub, 3);

	bg2Sub = bgInitSub(2, BgType_Bmp8, BgSize_B8_256x256, 3, 0);
	//nocashMessage(std::to_string(bg2Sub).c_str());
	bgSetPriority(bg2Sub, 0);

	oamInit(&oamSub, SpriteMapping_1D_32, false);

	gfxSub = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);

	// Load back button
	tonccpy(gfxSub, arrow_backTiles, arrow_backTilesLen);
	copyPalette(SPRITE_PALETTE_SUB, arrow_backPal, arrow_backPalLen);

	oamSet(&oamSub, 
		0,
		5, 192,	//5, 156,
		0,
		0,
		SpriteSize_64x64,
		SpriteColorFormat_16Color,
		gfxSub, 
		-1, 
		false, 
		false,	
		false, false,
		false
		);

	// Load cursor
	gfxSub = oamAllocateGfx(&oamSub, SpriteSize_32x32, SpriteColorFormat_16Color);

	tonccpy(gfxSub, cursorTiles, cursorTilesLen);
	copyPalette(SPRITE_PALETTE_SUB+16, cursorPal, cursorPalLen);

	oamSet(&oamSub, 
		1,
		0, 192,
		0,
		1,
		SpriteSize_32x32,
		SpriteColorFormat_16Color,
		gfxSub, 
		-1, 
		false, 
		false,	
		false, false,
		false
		);

	// Load item button
	gfxSub = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);

	tonccpy(gfxSub, item_button0Tiles, item_button0TilesLen);
	copyPalette(SPRITE_PALETTE_SUB+32, item_button0Pal, item_button0PalLen);

	for (int i = 2; i <= 4; i++) {
	oamSet(&oamSub, 
		i,
		16, 192,
		2,
		2,
		SpriteSize_64x64,
		SpriteColorFormat_16Color,
		gfxSub, 
		-1, 
		false, 
		false,	
		false, false,
		false
		);
	}

	gfxSub = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);

	tonccpy(gfxSub, item_button1Tiles, item_button1TilesLen);

	for (int i = 5; i <= 10; i++) {
	oamSet(&oamSub, 
		i,
		(i > 7 ? 16+128 : 16+64), 192,
		2,
		2,
		SpriteSize_64x64,
		SpriteColorFormat_16Color,
		gfxSub, 
		-1, 
		false, 
		false,	
		false, false,
		false
		);
	}

	gfxSub = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);

	tonccpy(gfxSub, item_button2Tiles, item_button2TilesLen);

	for (int i = 11; i <= 13; i++) {
	oamSet(&oamSub, 
		i,
		16+(64*3), 192,
		2,
		2,
		SpriteSize_64x64,
		SpriteColorFormat_16Color,
		gfxSub, 
		-1, 
		false, 
		false,	
		false, false,
		false
		);
	}

	// Load female icon
	gfxSub = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);

	tonccpy(gfxSub, icon_femaleTiles, icon_femaleTilesLen);
	copyPalette(SPRITE_PALETTE_SUB+48, icon_femalePal, icon_femalePalLen);

	for (int i = 14; i <= 16; i++) {
	oamSet(&oamSub, 
		i,
		12, 192,
		0,
		3,
		SpriteSize_64x64,
		SpriteColorFormat_16Color,
		gfxSub, 
		-1, 
		false, 
		false,	
		false, false,
		false
		);
	}

	// Load male icon
	gfxSub = oamAllocateGfx(&oamSub, SpriteSize_64x64, SpriteColorFormat_16Color);

	tonccpy(gfxSub, icon_maleTiles, icon_maleTilesLen);
	copyPalette(SPRITE_PALETTE_SUB+64, icon_malePal, icon_malePalLen);

	for (int i = 17; i <= 19; i++) {
	oamSet(&oamSub, 
		i,
		12, 192,
		0,
		4,
		SpriteSize_64x64,
		SpriteColorFormat_16Color,
		gfxSub, 
		-1, 
		false, 
		false,	
		false, false,
		false
		);
	}

	irqSet(IRQ_VBLANK, Gui__ChangeBrightness);
	irqEnable(IRQ_VBLANK);

	fontInit();
}

// Load a Font.
void Gui::loadFont(const char* Path) {
}

// Unload a Font.
void Gui::unloadFont() {
}

// Load a Sheet.
void Gui::loadSheet(const char* Path) {
}

// Unload a Sheet.
void Gui::unloadSheet() {
}

// Exit the GUI.
void Gui::exit(void) {
	if (usedScreen != nullptr)	usedScreen = nullptr;
}

// Draw a Centered String.
void Gui::DrawStringCentered(float x, float y, float size, u32 color, std::string Text, int maxWidth, int maxHeight) {
}

// Draw String or Text.
void Gui::DrawString(float x, float y, float size, u32 color, std::string Text, int maxWidth, int maxHeight) {
}

// Get String or Text Width.
float Gui::GetStringWidth(float size, std::string Text) {
}

// Get String or Text Size.
void Gui::GetStringSize(float size, float *width, float *height, std::string Text) {
}


// Get String or Text Height.
float Gui::GetStringHeight(float size, std::string Text) {
}

// Draw a Rectangle.
bool Gui::Draw_Rect(int x, int y, int w, int h, int color) {
}

// Draw's the current screen's draw.
void Gui::DrawScreen() {
	if (usedScreen != nullptr)	usedScreen->Draw();
	oamUpdate(&oamSub);
}

// Do the current screen's logic.
void Gui::ScreenLogic(u32 hDown, u32 hHeld, touchPosition touch, bool waitFade) {
	if (waitFade) {
		if (!fadein && !fadeout) {
			if (usedScreen != nullptr)	usedScreen->Logic(hDown, hHeld, touch);
		}
	} else {
		if (usedScreen != nullptr)	usedScreen->Logic(hDown, hHeld, touch);
	}
}

// Move's the tempScreen to the used one.
void Gui::transferScreen() {
	if (tempScreen != nullptr)	usedScreen = std::move(tempScreen);
}

// Set the current Screen.
void Gui::setScreen(std::unique_ptr<Screen> screen, bool fade) { 
	tempScreen = std::move(screen);
	// Switch screen without fade.
	if (!fade) {
		Gui::transferScreen();
	} else {
		// Fade, then switch.
		fadeout = true;
	}
}

// Fade's the screen in and out and transfer the screen.
// Credits goes to RocketRobz & SavvyManager.
void Gui::fadeEffects(int fadeoutFrames, int fadeinFrames) {
	if (fadein) {
		fadealpha -= fadeinFrames;
		if (fadealpha < 0) {
			fadealpha = 0;
			fadecolor = 255;
			fadein = false;
		}
	}

	if (fadeout) {
		fadealpha += fadeoutFrames;
		if (fadealpha > 255) {
			fadealpha = 255;
			Gui::transferScreen(); // Transfer Temp screen to the used one.
			fadein = true;
			fadeout = false;
		}
	}
}

// Select, on which Screen should be drawn.
void Gui::ScreenDraw() {
}

void Gui::drawGrid(float xPos, float yPos, float Width, float Height, u32 color) {
}

void Gui::drawAnimatedSelector(float xPos, float yPos, float Width, float Height, float speed, u32 SelectorColor, u32 bgColor) {
}