#include "rocketRobz.hpp"
#include "screenvars.h"
#ifdef NDS
#include "lodepng.h"
#include "myDSiMode.h"
#endif

static int subMode = 0;
#ifdef __3DS__
static int prevSubMode = -1;
#endif
extern int delay;
static int rr_fadeAlpha = 0;
static int rr_fadeType = true;

#ifdef __3DS__
static int robzXpos = 235+220;
static int robzYpos = 15+220;
static float robzXscale = 0.5;
static float robzYscale = 1;
static int rrFlashFade = 255;
#endif

void RocketRobz::Draw(void) const {
	fadecolor = 0;	// Always use black color for fading effects

	#ifdef __3DS__
	Gui::ScreenDraw(Top);

	bool robzInPos = (robzXpos == 235 && robzYpos == 15);
	GFX::DrawSpriteLinear(sprites_rr_spaceBG_idx, 0, 0, 0.5, 1);

	if (robzInPos) {
		GFX::DrawSpriteLinear(sprites_logo_rocketrobz_idx, 115, 41, 0.5, 1);
		if (rrFlashFade > 0) {
			GFX::DrawSpriteLinearBlend(sprites_logo_rocketrobz_idx, 115, 41, C2D_Color32(255, 255, 255, rrFlashFade), 0.5, 1);
		}
		switch (iFps) {
			default:
				rrFlashFade -= 16;
				break;
			case 30:
				rrFlashFade -= 32;
				break;
			case 24:
				rrFlashFade -= 40;
				break;
		}
		if (rrFlashFade < 0) {
			rrFlashFade = 0;
		}
	} else {
		switch (iFps) {
			default:
				robzXpos -= 2;
				robzYpos -= 2;
				robzXscale -= 0.0024;
				robzYscale -= 0.005;
				break;
			case 30:
				robzXpos -= 4;
				robzYpos -= 4;
				robzXscale -= 0.0048;
				robzYscale -= 0.010;
				break;
			case 24:
				robzXpos -= 6;
				robzYpos -= 6;
				robzXscale -= 0.0072;
				robzYscale -= 0.015;
				break;
		}
		if (robzXpos < 235) robzXpos = 235;
		if (robzYpos < 15) robzYpos = 15;
	}
	GFX::DrawSpriteLinear(sprites_rr_robz_idx, robzXpos, robzYpos, robzXscale, robzYscale);
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect

	if (shiftBySubPixel) return;
	Gui::ScreenDraw(Bottom);
	if (subMode == 0) {
		if (robzInPos) {
			GFX::DrawSprite(sprites_logo_UniversalCore_idx, 0, 26);
			if (rrFlashFade > 0) {
				GFX::DrawSpriteBlend(sprites_logo_UniversalCore_idx, 0, 26, C2D_Color32(255, 255, 255, rrFlashFade));
			}
		}
	} else {
		Gui::DrawStringCentered(0, 72, 0.50, WHITE, this->presentedText);
		GFX::DrawSprite(sprites_logo_horizonHD_idx, 16, 95);
	}
	if (rr_fadeAlpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(0, 0, 0, rr_fadeAlpha)); // Fade in/out effect
	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 320, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	#endif
	
	int fadeFPS;
	switch (iFps) {
		default:
			fadeFPS = 8;
			break;
		case 30:
			fadeFPS = 16;
			break;
		case 24:
			fadeFPS = 20;
			break;
	}
	if (subMode == 2) {
		fadeFPS += fadeFPS;
	}
	
	if (rr_fadeType) {
		rr_fadeAlpha -= fadeFPS;
		if (rr_fadeAlpha < 0) rr_fadeAlpha = 0;
	} else {
		rr_fadeAlpha += fadeFPS;
		if (rr_fadeAlpha > 255) rr_fadeAlpha = 255;
	}

	#ifdef __3DS__
	if (delay > iFps*10 && gfxIsWide()) {
		if (prevSubMode != 0) {
			rr_fadeType = false;
		}
		if (rr_fadeAlpha == 255) {
			subMode = 1;
			rr_fadeType = true;
			prevSubMode++;
		}
	}
	#endif
}


void RocketRobz::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	#ifdef NDS
	if (!graphicLoaded) {
		extern int bg2Main;
		extern int bg3Main;
		extern void applyColorLutBitmap(u16 *palette, int size);

		std::vector<unsigned char> image;
		unsigned width, height;
		lodepng::decode(image, width, height, "nitro:/graphics/logos/rocketRobz.png");
		bool alternatePixel = false;
		for(unsigned i=0;i<image.size()/4;i++) {
			image[(i*4)+3] = 0;
			if (alternatePixel) {
				if (image[(i*4)] >= 0x4) {
					image[(i*4)] -= 0x4;
					image[(i*4)+3] |= BIT(0);
				}
				if (image[(i*4)+1] >= 0x4) {
					image[(i*4)+1] -= 0x4;
					image[(i*4)+3] |= BIT(1);
				}
				if (image[(i*4)+2] >= 0x4) {
					image[(i*4)+2] -= 0x4;
					image[(i*4)+3] |= BIT(2);
				}
			}
			bgGetGfxPtr(bg2Main)[i] = image[i*4]>>3 | (image[(i*4)+1]>>3)<<5 | (image[(i*4)+2]>>3)<<10 | BIT(15);
		  if (dsiFeatures()) {
			if (alternatePixel) {
				if (image[(i*4)+3] & BIT(0)) {
					image[(i*4)] += 0x4;
				}
				if (image[(i*4)+3] & BIT(1)) {
					image[(i*4)+1] += 0x4;
				}
				if (image[(i*4)+3] & BIT(2)) {
					image[(i*4)+2] += 0x4;
				}
			} else {
				if (image[(i*4)] >= 0x4) {
					image[(i*4)] -= 0x4;
				}
				if (image[(i*4)+1] >= 0x4) {
					image[(i*4)+1] -= 0x4;
				}
				if (image[(i*4)+2] >= 0x4) {
					image[(i*4)+2] -= 0x4;
				}
			}
			bgGetGfxPtr(bg3Main)[i] = image[i*4]>>3 | (image[(i*4)+1]>>3)<<5 | (image[(i*4)+2]>>3)<<10 | BIT(15);
		  }
			if ((i % 256) == 255) alternatePixel = !alternatePixel;
			alternatePixel = !alternatePixel;
		}
		applyColorLutBitmap(bgGetGfxPtr(bg2Main), 0x18000/sizeof(u16));
		if (dsiFeatures()) {
			applyColorLutBitmap(bgGetGfxPtr(bg3Main), 0x18000/sizeof(u16));
		}
		graphicLoaded = true;
	}
	#endif
}