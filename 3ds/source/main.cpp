﻿#ifdef _3DS

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <3ds.h>
#include <malloc.h>
#include <unistd.h>		// access
#include <sys/stat.h>

#include "common.hpp"
#include "screenMode.h"
#include "dumpdsp.h"
#include "photoStudio.hpp"
#include "inifile.h"
#include "productIdent.hpp"
#include "rocketRobz.hpp"
#include "screen.hpp"
#include "screenshot.h"
#include "sound.h"
#include "thread.h"

bool isInit = true;
bool rocketRobzScreen = false;
int delay = 0;
Handle threadRequest;

#define settingsIni "sdmc:/3ds/SuperPhotoStudio/settings.ini"

char verText[32];
int studioBg = 0;
bool cinemaWide = false;
int iFps = 60;

u8 consoleModel = 0;

sound *music = NULL;
sound *music_loop = NULL;
//sound *mus_logos = NULL;
sound *sfx_select = NULL;
sound *sfx_back = NULL;
sound *sfx_highlight = NULL;

bool horiHd = true;
bool dspfirmfound = false;
bool exiting = false;
bool musicPlayStarted = false;
static bool musicPlaying = false;
static bool musicLoopPlaying = false;
static int musicLoopDelay = 0;
bool clearTop = true;	// Disable in order to render a second character
bool renderTop = true;	// Disable to prevent second character from flickering
bool doScreenshot = false;

void loadSettings(void) {
	bool setting = false;

	CIniFile settingsini(settingsIni);

	//studioBg = settingsini.GetInt("SuperPhotoStudio", "STUDIO_BG", studioBg);
	setting = settingsini.GetInt("SuperPhotoStudio", "CINEMA_WIDE", false);
	if (setting > 0) cinemaWide = true;
	iFps = settingsini.GetInt("SuperPhotoStudio", "FRAME_RATE", iFps);
	horiHd = settingsini.GetInt("SAVVY-MANAGER", "HORI_HD", horiHd);
}

void saveSettings(void) {
	CIniFile settingsini(settingsIni);

	//settingsini.SetInt("SuperPhotoStudio", "STUDIO_BG", studioBg);
	settingsini.SetInt("SuperPhotoStudio", "CINEMA_WIDE", (cinemaWide) ? true : false);
	settingsini.SetInt("SuperPhotoStudio", "FRAME_RATE", iFps);

	settingsini.SaveIniFileModified(settingsIni);
}

void Play_Music(void) {
	if (musicPlaying && !musicLoopPlaying) {
		musicLoopDelay++;
		if (musicLoopDelay>5 && !ndspChnIsPlaying(0)) {
			music_loop->play();
			musicLoopPlaying = true;
		}
	}
	if (!musicPlaying && dspfirmfound) {
		music->play();
		musicPlaying = true;
	}
}

void Stop_Music(void) {
	if (dspfirmfound) {
		music->~sound();
		music_loop->~sound();
	}
	musicPlayStarted = false;
	musicPlaying = false;
	musicLoopPlaying = false;
	musicLoopDelay = 0;
}

/*void musLogos(void) {
	if (!dspfirmfound) return;
	mus_logos->stop();
	mus_logos->play();
}*/

void sndSelect(void) {
	if (!dspfirmfound) return;
	sfx_select->stop();
	sfx_select->play();
}

void sndBack(void) {
	if (!dspfirmfound) return;
	sfx_back->stop();
	sfx_back->play();
}

void sndHighlight(void) {
	if (!dspfirmfound) return;
	sfx_highlight->stop();
	sfx_highlight->play();
}

u8 sysRegion = CFG_REGION_USA;
u64 appID = 0;

bool showCursor = false;
int cursorAlpha = 0;

u32 hDown = 0;
u32 hHeld = 0;
touchPosition touch;

bool touchingBackButton(void) {
	return (touch.px >= 7 && touch.px < 7+40 && touch.py >= 197 && touch.py < 197+44);
}

void getFrameForScreenshot(void) {
	// Get current frame
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	Gui::clearTextBufs();
	Gui::ScreenDraw(Top);
	C3D_FrameEnd(0);
}

void renderTopScreenSubPixels(void) {
	shiftBySubPixel = true;
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(Top, TRANSPARENT);
	Gui::clearTextBufs();
	Gui::DrawScreen();
	C3D_FrameEnd(0);
	shiftBySubPixel = false;
}

static bool runThreads = true;

void controlThread(void) {
	while (runThreads) {
		svcWaitSynchronization(threadRequest, U64_MAX);
		Gui::ScreenLogic(hDown, hHeld, touch, false); // Call the logic of the current screen here.
		svcClearEvent(threadRequest);
	}
}

int main()
{
	amInit();
	romfsInit();

	Result res = cfguInit();
	if (R_SUCCEEDED(res)) {
		CFGU_SecureInfoGetRegion(&sysRegion);
		CFGU_GetSystemModel(&consoleModel);
		cfguExit();
	}

	aptInit();
	APT_GetProgramID(&appID);
	aptExit();

	gfxInitDefault();
	gfxSetWide(horiHd && consoleModel != 3);	// Enable 800x240 mode for non-O2DS consoles. Improves clarity in graphics.
	loadSettings();

	Gui::init();
	osSetSpeedupEnable(true);	// Enable speed-up for New 3DS users
	GFX::loadSheets();
	fadein = true;
	fadealpha = 0;

	// make folders if they don't exist
	mkdir("sdmc:/3ds", 0777);
	mkdir("sdmc:/3ds/SuperPhotoStudio", 0777);

	if ( access( "sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) {
		ndspInit();
		dspfirmfound = true;
	} else {
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		Gui::ScreenDraw(Bottom);
		Gui::DrawString(12, 16, 0.5f, WHITE, "Dumping DSP firm...");
		C3D_FrameEnd(0);
		dumpDsp();
		if ( access( "sdmc:/3ds/dspfirm.cdc", F_OK ) != -1 ) {
			ndspInit();
			dspfirmfound = true;
		} else {
			C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
			C2D_TargetClear(Bottom, TRANSPARENT); // clear Bottom Screen to avoid Text overdraw.
			Gui::ScreenDraw(Bottom);
			Gui::DrawString(12, 16, 0.5f, WHITE, "DSP firm dumping failed.\n"
					"Running without sound.");
			C3D_FrameEnd(0);
			for (int i = 0; i < 90; i++) {
				gspWaitForVBlank();
			}
		}
	}

	// Load the sound effects if DSP is available.
	if (dspfirmfound) {
		//mus_logos = new sound("romfs:/sounds/logos.wav", 0, false);
		sfx_select = new sound("romfs:/sounds/select.wav", 2, false);
		sfx_back = new sound("romfs:/sounds/back.wav", 3, false);
		sfx_highlight = new sound("romfs:/sounds/highlight.wav", 4, false);
	}

	sprintf(verText, "Ver. %i.%i.%i", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);

	C3D_FrameRate(iFps);

	Gui::setScreen(std::make_unique<ProductIdent>(), false); // Set screen to product identification.
	svcCreateEvent(&threadRequest,(ResetType)0);
	createThread((ThreadFunc)controlThread);
	//musLogos();

	// Loop as long as the status is not exit
	while(aptMainLoop()) {
		// Scan hid shared memory for input events
		hidScanInput();

		hDown = hidKeysDown();
		hHeld = hidKeysHeld();

		hidTouchRead(&touch);

		if (musicPlayStarted) {
			Play_Music();
		}

		// Here we draw the actual screen.
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		if (clearTop) {
			C2D_TargetClear(Top, TRANSPARENT);
		}
		C2D_TargetClear(Bottom, TRANSPARENT);
		Gui::clearTextBufs();
		Gui::DrawScreen();
		C3D_FrameEnd(0);
		GFX::animateBgSprite();
		if (exiting) {
			if (!fadeout)	break;
		}

		if (isInit) {
			delay++;
			if (rocketRobzScreen) {
				if (delay > iFps*(gfxIsWide() ? 13 : 10)) {
					Gui::setScreen(std::make_unique<PhotoStudio>(), true); // Set after delay to the photo studio.
					isInit = false;
				}
			} else
			if (delay > iFps*6) {
				fadeout = true;
				fadealpha = 255;
				Gui::setScreen(std::make_unique<RocketRobz>(), true); // Set after delay to RocketRobz's screen.
				rocketRobzScreen = true;
			}
		}

		if (hDown) {
			svcSignalEvent(threadRequest);
		}

		if (doScreenshot) {
			getFrameForScreenshot();
			Screenshot_Capture();
			renderTop = true;
			doScreenshot = false;
		}

		if ((hDown & KEY_UP)
		|| (hDown & KEY_DOWN)
		|| (hDown & KEY_LEFT)
		|| (hDown & KEY_RIGHT))
		{
			showCursor = true;
		} else if (hDown & KEY_TOUCH)
		{
			showCursor = false;
		}

		if (showCursor) {
			cursorAlpha += 32;
			if (cursorAlpha > 255) {
				cursorAlpha = 255;
			}
		} else {
			cursorAlpha -= 32;
			if (cursorAlpha < 0) {
				cursorAlpha = 0;
			}
		}

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

		Gui::fadeEffects(fadeFPS, fadeFPS);
	}


	runThreads = false;

	saveSettings();

	//delete music;
	//delete mus_logos;
	delete sfx_select;
	delete sfx_back;
	delete sfx_highlight;
	if (dspfirmfound) {
		ndspExit();
	}

	// signal the thread and wait for it to exit
	svcSignalEvent(threadRequest);
	destroyThreads();

	// close event handle
	svcCloseHandle(threadRequest);

	Gui::exit();
	GFX::unloadSheets();

	gfxExit();
	romfsExit();
	amExit();

	return 0;
}

#endif