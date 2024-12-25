#include "common.hpp"

#include <unistd.h>

#define charSpriteSize 0x100000

static char bgSpriteMem[4][0x200000];
static char charSpriteMem[5][charSpriteSize];

static C2D_SpriteSheet sprites;
static C2D_SpriteSheet bgSprite;
static C2D_SpriteSheet chracterSprite;
static bool chracterSpriteLoaded = false;
static bool chracterSpriteFound[5] = {false};
static bool bgSpriteLoaded = false;

extern int studioBg;
extern u8 settingBits;
extern bool cinemaWide;
extern int iFps;

extern bool showCursor;
extern int cursorAlpha;

bool animateBg = false;
bool bgCanAnimate = false;
static int bgAnimationFrame = 0;
static int bgAnimationCurrent = 0;
static int bgAnimationTime = 0;
static int bgAnimationDelay = 0;
static int bgAnimation[8] = {100};

static int timeOutside = 0;	// 0 == Day, 1 == Sunset, 2 == Night

bool shiftBySubPixel = false;

void GFX::resetCharStatus(int num) {
	if (num > -1) {
		chracterSpriteFound[num] = false;
		return;
	}

	chracterSpriteFound[0] = false;
	chracterSpriteFound[1] = false;
	chracterSpriteFound[2] = false;
	chracterSpriteFound[3] = false;
	chracterSpriteFound[4] = false;
}

Result GFX::loadSheets() {
	sprites			= C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	GFX::loadBgSprite();
	return 0;
}

Result GFX::unloadSheets() {
	C2D_SpriteSheetFree(sprites);
	if (bgSpriteLoaded) {
		C2D_SpriteSheetFree(bgSprite);
	}
	if (chracterSpriteLoaded) {
		C2D_SpriteSheetFree(chracterSprite);
	}
	return 0;
}

static inline bool isDaytime(int hour, int minutes) {
	if ((hour >= 7 && hour < 20) || (hour == 20 && minutes >= 0 && minutes < 45)) {
		timeOutside = 0;
		return true;
	}
	return false;
}

static inline bool isEvening(int hour, int minutes) {
	if ((hour == 20 && minutes >= 45 && minutes < 60) || (hour == 21 && minutes >= 0 && minutes < 45)) {
		timeOutside = 1;
		return true;
	}
	return false;
}

void GFX::loadBgSprite(void) {
	if (bgSpriteLoaded) return;

	timeOutside = 2;	// Default is Nighttime
	int aniFrames = 0;

	const char* bgPath;
	time_t t = time(0);
	int hour = localtime(&t)->tm_hour;
	int minutes = localtime(&t)->tm_min;

	switch (studioBg) {
		case 0:
		default:
			bgPath = "romfs:/gfx/bg_blue.t3x";
			break;
		case 1:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_loversBell.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_loversBell.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_loversBell.t3x";
			}
			break;
		case 2:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_bougainville.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_bougainville.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_bougainville.t3x";
			}
			break;
		case 3:
			bgPath = "romfs:/gfx/bg_nin10Pro.t3x";
			break;
		case 4:
			bgPath = "romfs:/gfx/bg_beautician4.t3x";
			break;
		case 5:
			bgPath = "romfs:/gfx/bg_hairSalon4.t3x";
			break;
		case 6:
			bgPath = "romfs:/gfx/bg_celestialHotel.t3x";
			break;
		case 7:
			bgPath = "romfs:/gfx/bg_liveMusicClub4.t3x";
			break;
		case 8:
			bgPath = "romfs:/gfx/bg_menswearShop.t3x";
			break;
		case 9:
			bgPath = "romfs:/gfx/bg_VIP.t3x";
			break;
		case 10:
			bgPath = "romfs:/gfx/bg_restauraunt4.t3x";
			break;
		case 11:
			bgPath = "romfs:/gfx/bg_cinema.t3x";
			break;
		case 12:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_tropicaBeach_0.t3x";
				aniFrames = 2;
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_tropicaBeach_0.t3x";
				aniFrames = 2;
			} else {
				bgPath = "romfs:/gfx/bgNight_tropicaBeach.t3x";
			}
			break;
		case 13:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_primrosePark.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_primrosePark.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_primrosePark.t3x";
			}
			break;
		case 14:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_cafe3.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_cafe3.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_cafe3.t3x";
			}
			break;
		case 15:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_mapleCrescent.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_mapleCrescent.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_mapleCrescent.t3x";
			}
			break;
		case 16:
			bgPath = "romfs:/gfx/bg_white.t3x";
			break;
		case 17:
			bgPath = "romfs:/gfx/bg_yellow.t3x";
			break;
		case 18:
			bgPath = "romfs:/gfx/bg_snowflakes.t3x";
			break;
		case 19:
			bgPath = "romfs:/gfx/bg_pinkShine.t3x";
			break;
		case 20:
			bgPath = "romfs:/gfx/bg_beigeWithCircles.t3x";
			break;
		case 21:
			bgPath = "romfs:/gfx/bg_gradientPurple.t3x";
			break;
		case 22:
			bgPath = "romfs:/gfx/bg_greenBirds.t3x";
			break;
		case 23:
			bgPath = "romfs:/gfx/bg_circlePattern.t3x";
			break;
		case 24:
			bgPath = "romfs:/gfx/bg_circleWindows.t3x";
			break;
		case 25:
			bgPath = "romfs:/gfx/bg_bigHearts.t3x";
			break;
		case 26:
			bgPath = "romfs:/gfx/bg_smallHearts.t3x";
			break;
		case 27:
			bgPath = "romfs:/gfx/bg_park2Spring.t3x";
			break;
		case 28:
			bgPath = "romfs:/gfx/bg_park2Summer.t3x";
			break;
		case 29:
			bgPath = "romfs:/gfx/bg_park2Fall.t3x";
			break;
		case 30:
			bgPath = "romfs:/gfx/bg_park2Winter.t3x";
			break;
		case 31:
			bgPath = "romfs:/gfx/bg_downtownSpring.t3x";
			break;
		case 32:
			bgPath = "romfs:/gfx/bg_downtownSummer.t3x";
			break;
		case 33:
			bgPath = "romfs:/gfx/bg_downtownFall.t3x";
			break;
		case 34:
			bgPath = "romfs:/gfx/bg_downtownWinter.t3x";
			break;
		case 35:
			bgPath = "romfs:/gfx/bg_cafe2Spring.t3x";
			break;
		case 36:
			bgPath = "romfs:/gfx/bg_cafe2Summer.t3x";
			break;
		case 37:
			bgPath = "romfs:/gfx/bg_cafe2Fall.t3x";
			break;
		case 38:
			bgPath = "romfs:/gfx/bg_cafe2Winter.t3x";
			break;
		case 39:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_exhibitionHall2.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_exhibitionHall2.t3x";
			}
			break;
		case 40:
			bgPath = "romfs:/gfx/bg_beautician2.t3x";
			break;
		case 41:
			bgPath = "romfs:/gfx/bg_afterParty.t3x";
			break;
		case 42:
			bgPath = "romfs:/gfx/bg_graceShop.t3x";
			break;
		case 43:
			bgPath = "romfs:/gfx/bg_hairSalon1.t3x";
			break;
		case 44:
			bgPath = "romfs:/gfx/bg_beautician1.t3x";
			break;
		case 45:
			bgPath = "romfs:/gfx/bg_lifestyleShop2.t3x";
			break;
		case 46:
			bgPath = "romfs:/gfx/bg_liveMusicClub2.t3x";
			break;
		case 47:
			bgPath = "romfs:/gfx/bg_roseGarden2.t3x";
			break;
		case 48:
			bgPath = "romfs:/gfx/bg_carringtonInstitute.t3x";
			break;
		case 49:
			bgPath = "romfs:/gfx/bg_peachCastleOutside.t3x";
			break;
		case 50:
			bgPath = "romfs:/gfx/bg_peachCastle.t3x";
			break;
		case 51:
			bgPath = "romfs:/gfx/bg_liveMusicClub3_0.t3x";
			aniFrames = 3;
			break;
		case 52:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_outdoorStage.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_outdoorStage.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_outdoorStage.t3x";
			}
			break;
		case 53:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_park4.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_park4.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_park4.t3x";
			}
			break;
		case 54:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_cafe4.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_cafe4.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_cafe4.t3x";
			}
			break;
		case 55:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_seaside.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_seaside.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_seaside.t3x";
			}
			break;
		case 56:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_countrysideSpring.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_countrysideSpring.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_countrysideSpring.t3x";
			}
			break;
		case 57:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_countrysideSummer.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_countrysideSummer.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_countrysideSummer.t3x";
			}
			break;
		case 58:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_countrysideFall.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_countrysideFall.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_countrysideFall.t3x";
			}
			break;
		case 59:
			if (isDaytime(hour, minutes)) {
				bgPath = "romfs:/gfx/bgDay_countrysideWinter.t3x";
			} else if (isEvening(hour, minutes)) {
				bgPath = "romfs:/gfx/bgSunset_countrysideWinter.t3x";
			} else {
				bgPath = "romfs:/gfx/bgNight_countrysideWinter.t3x";
			}
			break;
		case 60:
			bgPath = "romfs:/gfx/bg_lifestyleShop4.t3x";
			break;
		case 61:
			bgPath = "romfs:/gfx/bg_florist.t3x";
			break;
		case 62:
			bgPath = "romfs:/gfx/bg_cakeShop4.t3x";
			break;
		case 63:
			bgPath = "romfs:/gfx/bg_livelyStage_0.t3x";
			aniFrames = 3;
			break;
		case 64:
			bgPath = "romfs:/gfx/bg_girlyStage_0.t3x";
			aniFrames = 1;
			break;
		case 65:
			bgPath = "romfs:/gfx/bg_concertHall.t3x";
			break;
		case 66:
			bgPath = "romfs:/gfx/bg_charityStage.t3x";
			break;
		case 67:
			bgPath = "romfs:/gfx/bg_RocketRobz.t3x";
			break;
		case 68:
			bgPath = "romfs:/gfx/bg_Nintendo.t3x";
			break;
		case 69:
			bgPath = "romfs:/gfx/bg_segaGen.t3x";
			break;
		case 70:
			bgPath = "romfs:/gfx/bg_twlMenu.t3x";
			break;
		case 71:
			bgPath = "romfs:/gfx/bg_hiyaCFW.t3x";
			break;
	}
	FILE* bgFile = fopen(bgPath, "rb");
	fread((void*)bgSpriteMem[0], 1, 0x200000, bgFile);
	fclose(bgFile);

	bgSprite		= C2D_SpriteSheetLoadFromMem(bgSpriteMem[0], 0x200000);
	bgSpriteLoaded = true;
	bgAnimationFrame = 0;
	bgAnimationCurrent = 0;
	bgAnimationTime = 0;
	bgCanAnimate = false;

	// Load animated parts
	if (aniFrames > 0) {
		switch (studioBg) {
			case 12:
				if (timeOutside == 0) {
					bgPath = "romfs:/gfx/bgDay_tropicaBeach_%i.t3x";
				} else {
					bgPath = "romfs:/gfx/bgSunset_tropicaBeach_%i.t3x";
				}
				break;
			case 51:
				bgPath = "romfs:/gfx/bg_liveMusicClub3_%i.t3x";
				break;
			case 63:
				bgPath = "romfs:/gfx/bg_livelyStage_%i.t3x";
				break;
			case 64:
				bgPath = "romfs:/gfx/bg_girlyStage_%i.t3x";
				break;
		}
		char bgAniPath[64];
		for (int i = 1; i <= aniFrames; i++) {
			sprintf(bgAniPath, bgPath, i);
			bgFile = fopen(bgAniPath, "rb");
			fread((void*)bgSpriteMem[i], 1, 0x200000, bgFile);
			fclose(bgFile);
		}
		if (studioBg == 64) {
			bgAnimationDelay = iFps/2;
			bgAnimation[0] = 0;
			bgAnimation[1] = 1;
			bgAnimation[2] = 100;
		} else if (studioBg == 51 || studioBg == 63) {
			bgAnimationDelay = iFps/2;
			bgAnimation[0] = 0;
			bgAnimation[1] = 1;
			bgAnimation[2] = 2;
			bgAnimation[3] = 3;
		} else {
			bgAnimationDelay = iFps;
			bgAnimation[0] = 0;
			bgAnimation[1] = 1;
			bgAnimation[2] = 2;
			bgAnimation[3] = 1;
		}
		bgAnimation[4] = 100;
		bgCanAnimate = true;
	}

	bgSpriteLoaded = true;
}

void GFX::unloadBgSprite() {
	if (!bgSpriteLoaded) return;

	animateBg = false;
	bgCanAnimate = false;

	C2D_SpriteSheetFree(bgSprite);
	bgSpriteLoaded = false;
}

void GFX::reloadBgSprite() {
	unloadBgSprite();
	loadBgSprite();
}

bool GFX::loadCharSprite(int num, const char* t3xPathPose, const char* t3xPathAllSeasons, const char* t3xPathOneSeason) {
	if (chracterSpriteLoaded) {
		C2D_SpriteSheetFree(chracterSprite);
		chracterSpriteLoaded = false;
	}
	chracterSpriteFound[num] = false;
	bool allSeasons = true;
	bool fileFound = false;
	bool poseable = false;
	fileFound = (access(t3xPathPose, F_OK) == 0);
	if (fileFound) {
		poseable = true;
	} else {
		fileFound = (access(t3xPathAllSeasons, F_OK) == 0);
	}
	if (!fileFound) {
		allSeasons = false;
		fileFound = (access(t3xPathOneSeason, F_OK) == 0);
	}

	if (!fileFound) {
		return false;
	}

	FILE* charFile = fopen(poseable ? t3xPathPose : (allSeasons ? t3xPathAllSeasons : t3xPathOneSeason), "rb");
	fread((void*)charSpriteMem[num], 1, charSpriteSize, charFile);
	fclose(charFile);

	chracterSpriteFound[num] = true;

	return true;
}

void GFX::loadCharSpriteMem(int num) {
	if (chracterSpriteLoaded) {
		C2D_SpriteSheetFree(chracterSprite);
	}

	if (!chracterSpriteFound[num]) return;
	chracterSprite		= C2D_SpriteSheetLoadFromMem(charSpriteMem[num], charSpriteSize);
	chracterSpriteLoaded = true;
}

void GFX::showBgSprite(int zoomIn) {
	if (!bgSpriteLoaded) return;

	int yPos = -(240*zoomIn);
	if (cinemaWide) yPos -= 16;

	C2D_Image image = C2D_SpriteSheetGetImage(bgSprite, 0);
	if (!gfxIsWide()) {
		C3D_TexSetFilter(image.tex, GPU_LINEAR, GPU_LINEAR);
	}

	C2D_DrawImageAt(image, 0, yPos-(shiftBySubPixel ? 0.5f : 0), 0.5f, NULL, 0.5, 1);
}

void GFX::animateBgSprite(void) {
	if (!animateBg) return;

	// Animate background
	bgAnimationTime++;
	if (bgAnimationTime >= bgAnimationDelay) {
		bgAnimationCurrent++;
		if (bgAnimation[bgAnimationCurrent] == 100) {
			// Reset animation
			bgAnimationCurrent = 0;
		}
		bgAnimationFrame = bgAnimation[bgAnimationCurrent];
		C2D_SpriteSheetFree(bgSprite);
		bgSprite = C2D_SpriteSheetLoadFromMem(bgSpriteMem[bgAnimationFrame], 0x200000);
		bgAnimationTime = 0;
	}
}

void GFX::showCharSprite(int num, bool flipH, int zoomIn, int fadeAlpha, bool lightingEffects) {
	if (!chracterSpriteLoaded) return;

	int xPos = (cinemaWide ? 60 : 0);
	float xScale = (cinemaWide ? 0.35f : 0.5);
	if (flipH) {
		xScale -= xScale*2;
	}
	int yPos = 0;
	int yPosRefl = 230;
	if (cinemaWide) {
		yPos += 36;
		yPosRefl -= 36;
	}
	if ((num == 3 || num == 4) && chracterSpriteFound[3]) {
		if (zoomIn == 0) {
			return;
		}
		yPos += cinemaWide ? 60 : 80;
		switch (num) {
			case 3:
				xPos -= cinemaWide ? 72 : 80;
				break;
			case 4:
				xPos += cinemaWide ? 72 : 80;
				break;
		}
	} else if (chracterSpriteFound[0] && chracterSpriteFound[1] && chracterSpriteFound[2]) {
		if (zoomIn == 1) {
		switch (num) {
			case 0:
				xPos -= cinemaWide ? 136 : 152;
				break;
			case 2:
				xPos += cinemaWide ? 136 : 152;
				break;
		}
		} else {
		switch (num) {
			case 0:
				xPos -= cinemaWide ? 48 : 64;
				break;
			case 2:
				xPos += cinemaWide ? 48 : 64;
				break;
		}
		}
	} else if (chracterSpriteFound[0] && chracterSpriteFound[1]) {
		if (zoomIn == 1) {
		switch (num) {
			case 0:
				xPos -= cinemaWide ? 72 : 80;
				break;
			case 1:
				xPos += cinemaWide ? 72 : 80;
				break;
		}
		} else {
		switch (num) {
			case 0:
				xPos -= cinemaWide ? 24 : 32;
				break;
			case 1:
				xPos += cinemaWide ? 24 : 32;
				break;
		}
		}
	}

	C2D_Image image = C2D_SpriteSheetGetImage(chracterSprite, zoomIn);
	C3D_TexSetFilter(image.tex, GPU_LINEAR, GPU_LINEAR);

	C2D_ImageTint tint;
	if (fadeAlpha == 255) {
		if (lightingEffects && zoomIn==0) {
			// Reflection on the floor
			switch (studioBg) {
				default:
					break;
				case 43:	// Hair Salon (Girls Mode/Style Savvy/Style Boutique)
				case 48:	// Carrington Institute (Perfect Dark)
					C2D_PlainImageTint(&tint, C2D_Color32(255, 255, 255, 127), 0);
					C2D_DrawImageAt(image, xPos, yPosRefl-(shiftBySubPixel ? 0.5f : 0), 0.5f, &tint, xScale, -(cinemaWide ? 0.7f : 1));
					break;
			}
		}
		C2D_PlainImageTint(&tint, C2D_Color32(255, 255, 255, 255), 0);
		if (lightingEffects) {
			switch (studioBg) {
				default:
					break;
				case 7:
					C2D_PlainImageTint(&tint, C2D_Color32(0, 0, 95, 255), 0.15);	// Tint for Live Music Club 4
					break;
				case 46:
					C2D_PlainImageTint(&tint, C2D_Color32(31, 31, 95, 255), 0.15);	// Tint for Live Music Club 2
					break;
				case 11:
					C2D_PlainImageTint(&tint, C2D_Color32(191, 63, 87, 255), 0.1);	// Tint for Cinema
					break;
				case 1:
				case 12:
				case 13:
				case 14:
				case 15:
				case 52:
				case 53:
				case 55:
				case 56:
				case 57:
				case 58:
				case 59:
					if (timeOutside == 1) {
						C2D_PlainImageTint(&tint, C2D_Color32(95, 47, 0, 255), 0.15);	// Tint for Sunset
					} else if (timeOutside == 2) {
						C2D_PlainImageTint(&tint, C2D_Color32(0, 0, 95, 255), 0.15);	// Tint for Nighttime
					}
					break;
				case 51:
					// Tint for Live Music Club 3
					if (bgAnimationFrame==1 || bgAnimationFrame==3) {
						C2D_PlainImageTint(&tint, C2D_Color32(0, 0, 0, 255), 0.15);
					} else {
						C2D_PlainImageTint(&tint, C2D_Color32(0, 0, 0, 255), 0.30);
					}
					break;
				case 54:
					// Tint for Cafe 4
					if (timeOutside == 1) {
						C2D_PlainImageTint(&tint, C2D_Color32(95, 47, 0, 255), 0.15);	// Tint for Sunset
					} else if (timeOutside == 2) {
						C2D_PlainImageTint(&tint, C2D_Color32(0, 0, 63, 255), 0.15);	// Tint for Nighttime
					}
					break;
			}
		}
		C2D_DrawImageAt(image, xPos, yPos-(shiftBySubPixel ? 0.5f : 0), 0.5f, &tint, xScale, (cinemaWide ? 0.7f : 1));
	} else {
		C2D_PlainImageTint(&tint, C2D_Color32(255, 255, 255, fadeAlpha), 1);
		C2D_DrawImageAt(image, xPos, yPos-(shiftBySubPixel ? 0.5f : 0), 0.5f, &tint, xScale, (cinemaWide ? 0.7f : 1));
	}
}

void GFX::DrawSprite(int img, int x, int y, float ScaleX, float ScaleY) {
	float yPos = y;
	if (shiftBySubPixel /*&& img != sprites_logo_savvymanager_idx*/) {
		yPos -= 0.5f;
	}

	C2D_DrawImageAt(C2D_SpriteSheetGetImage(sprites, img), x, yPos, 0.5f, NULL, ScaleX, ScaleY);
}

void GFX::DrawSpriteLinear(int img, int x, int y, float ScaleX, float ScaleY) {
	float yPos = y;
	if (shiftBySubPixel /*&& img != sprites_logo_savvymanager_idx*/) {
		yPos -= 0.5f;
	}

	C2D_Image image = C2D_SpriteSheetGetImage(sprites, img);
	C3D_TexSetFilter(image.tex, GPU_LINEAR, GPU_LINEAR);

	C2D_DrawImageAt(image, x, yPos, 0.5f, NULL, ScaleX, ScaleY);
}

void GFX::DrawSpriteBlend(int img, float x, float y, u32 color, float ScaleX, float ScaleY) {
	C2D_ImageTint tint;
	C2D_SetImageTint(&tint, C2D_TopLeft, color, 1);
	C2D_SetImageTint(&tint, C2D_TopRight, color, 1);
	C2D_SetImageTint(&tint, C2D_BotLeft, color, 1);
	C2D_SetImageTint(&tint, C2D_BotRight, color, 1);
	C2D_DrawImageAt( C2D_SpriteSheetGetImage(sprites, img), x, y-(shiftBySubPixel ? 0.5f : 0), 0.5f, &tint, ScaleX, ScaleY);
}

void GFX::DrawSpriteLinearBlend(int img, float x, float y, u32 color, float ScaleX, float ScaleY) {
	C2D_Image image = C2D_SpriteSheetGetImage(sprites, img);
	C3D_TexSetFilter(image.tex, GPU_LINEAR, GPU_LINEAR);

	C2D_ImageTint tint;
	C2D_SetImageTint(&tint, C2D_TopLeft, color, 1);
	C2D_SetImageTint(&tint, C2D_TopRight, color, 1);
	C2D_SetImageTint(&tint, C2D_BotLeft, color, 1);
	C2D_SetImageTint(&tint, C2D_BotRight, color, 1);
	C2D_DrawImageAt(image, x, y-(shiftBySubPixel ? 0.5f : 0), 0.5f, &tint, ScaleX, ScaleY);
}

void GFX::drawCursor(int cX, int cY) {
	if (cursorAlpha == 255) {
		DrawSprite(sprites_cursor_idx, cX, cY);
	} else if (cursorAlpha > 0) {
		DrawSpriteBlend(sprites_cursor_idx, cX, cY, C2D_Color32(255, 255, 255, cursorAlpha));
	}
}