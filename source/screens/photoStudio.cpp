#include "photoStudio.hpp"
#include "screenvars.h"

#include "file_browse.h"

#include "import_ss1charnames.h"
#include "import_ss2charnames.h"
#include "import_ss3charnames.h"
#include "import_ss4charnames.h"
#include "rocketcharnames.h"
#include "smCharNames.h"
#include "sthCharNames.h"
#include "jfgCharNames.h"
#include "conkerCharNames.h"
#include "banjokCharNames.h"
#include "pacCharNames.h"
#include "swapCharNames.h"
#include "metroidCharNames.h"
#include "sc5CharNames.h"
#include "vvvvvvCharNames.h"
#include "kirbyCharNames.h"
#include "nesCharNames.h"
#include "nightsCharNames.h"

#include "logobgnames.h"
#include "import_ss1bgnames.h"
#include "import_ss2bgnames.h"
#include "import_ss3bgnames.h"
#include "import_ss4bgnames.h"
#include "pdarkBgNames.h"
#include "smBgNames.h"

#include "jimPowerBgmNames.h"
#include "marioGolfAdvanceTourBgmNames.h"
#include "mmBattleAndChaseBgmNames.h"
#include "pkmnStadiumBgmNames.h"
#include "savvyMgrBgmNames.h"
#include "sonicManiaBgmNames.h"
#include "sonicJamBgmNames.h"
#include "superMarioBrosBgmNames.h"
#include "ss1BgmNames.h"
#include "ss2BgmNames.h"
#include "ss3BgmNames.h"
#include "tetrisPartyBgmNames.h"

#include <unistd.h>

extern int savedMusicId;
static bool musicLoaded = false;
extern void loadMusic(int num);

#ifdef NDS
extern bool mepFound;
extern bool dsDebugRam;

#define sysRegion *(u8*)0x02FFFD70

/// Configuration region values.
typedef enum
{
	CFG_REGION_JPN = 0, ///< Japan
	CFG_REGION_USA = 1, ///< USA
	CFG_REGION_EUR = 2, ///< Europe
	CFG_REGION_AUS = 3, ///< Australia
	CFG_REGION_CHN = 4, ///< China
	CFG_REGION_KOR = 5, ///< Korea
} CFG_Region;

#include "fontHandler.h"

#define KEY_CPAD_UP KEY_X
#define KEY_CPAD_DOWN KEY_B
#define KEY_CPAD_LEFT KEY_LEFT
#define KEY_CPAD_RIGHT KEY_RIGHT
#define KEY_DUP KEY_UP
#define KEY_DDOWN KEY_DOWN
#define KEY_DLEFT KEY_LEFT
#define KEY_DRIGHT KEY_RIGHT
#define KEY_ZL KEY_L
#define KEY_ZR KEY_R

void gspWaitForVBlank(void) {
}

static bool redrawText = true;
extern bool ditherlaceOnVBlank;
int characterLimit = 0;
#else
static bool musicPlayOnce = false;
int characterLimit = 4;
#endif

static u8 bgPageOrder[] = {
	4,	// Logos
	5,	// Perfect Dark
	0,	// Style Savvy
	1,	// Style Savvy: Trendsetters
	2,	// Style Savvy: Fashion Forward
	3,	// Style Savvy: Styling Star
	6,	// Super Mario series
};

static u8 charPageOrder[] = {
	4,	// Super Photo Studio (Original Characters)
	9,	// Banjo-Kazooie series
	8,	// Conker series
	7,	// Jet Force Gemini
	15,	// Kirby series
	12,	// Metroid series
	17,	// NiGHTS series
	16,	// Nintendo Entertainment System
	10,	// Pac-Man series
	6,	// Sonic the Hedgehog series
	13,	// Space Channel 5
	0,	// Style Savvy
	1,	// Style Savvy: Trendsetters
	2,	// Style Savvy: Fashion Forward
	3,	// Style Savvy: Styling Star
	5,	// Super Mario series
	11,	// Swapnote/Swapdoodle
	14,	// VVVVVV
	0xFF,	// Custom
};

static u8 bgmPageOrder[] = {
	6,	// Jim Power
	2,	// Mario Golf: Advance Tour
	1,	// MegaMan: Battle and Chase
	0,	// Pokemon Stadium
	7,	// Savvy Manager
	10,	// Sonic Jam
	4,	// Sonic Mania
	5,	// Style Savvy
	8,	// Style Savvy: Trendsetters
	11,	// Style Savvy: Fashion Forward
	9,	// Super Mario Bros.
	3,	// Tetris Party
};

static int metalXpos = 0;
#ifdef NDS
extern bool displayChars;
static bool metalDelay = false;
#endif
static int currentCharacterRendered = 0;
extern bool musicPlayStarted;
extern bool clearTop;
extern bool renderTop;

PhotoStudio::PhotoStudio() {
	currentCharacterRendered = 0;
	GFX::resetCharStatus();
	getList();
}

void PhotoStudio::getList() {
	getMaxChars();
}

void PhotoStudio::getMaxChars() {
	if (subScreenMode == 3) {
		// Music
		switch (bgmPageOrder[bgm_highlightedGame]) {
			case 0:
				import_totalCharacters = 0;
				break;
			case 1:
				import_totalCharacters = 0;
				break;
			case 2:
				import_totalCharacters = 0;
				break;
			case 3:
				import_totalCharacters = 1;
				break;
			case 4:
				import_totalCharacters = 5;
				break;
			case 5:
				import_totalCharacters = 10;
				break;
			case 6:
				import_totalCharacters = 0;
				break;
			case 7:
				import_totalCharacters = 0;
				break;
			case 8:
				import_totalCharacters = 17;
				break;
			case 9:
				import_totalCharacters = 0;
				break;
			case 10:
				import_totalCharacters = 2;
				break;
			case 11:
				import_totalCharacters = 10;
				break;
		}
	} else if (subScreenMode == 1) {
		// Locations
		switch (bgPageOrder[photo_highlightedGame]) {
			case 0:
				import_totalCharacters = 2;
				break;
			case 1:
				import_totalCharacters = 27;
				break;
			case 2:
				import_totalCharacters = 9;
				break;
			case 3:
				import_totalCharacters = 26;
				break;
			case 4:
				import_totalCharacters = 4;
				break;
			case 5:
				import_totalCharacters = 0;
				break;
			case 6:
				import_totalCharacters = 1;
				break;
		}
	} else {
		// Characters
		const int highlightedGame = char_highlightedGame[currentCharNum];
		switch (charPageOrder[highlightedGame]) {
			case 0:
				import_totalCharacters = 0x7;
				break;
			case 1:
				import_totalCharacters = 0x12;
				break;
			case 2:
				import_totalCharacters = 0x11;
				break;
			case 3:
				import_totalCharacters = 0xD;
				break;
			case 5:
				import_totalCharacters = 6;
				break;
			case 11:
			case 14:
			case 15:
			case 16:
			case 17:
				import_totalCharacters = 0;
				break;
			case 6:
			case 7:
			case 8:
			case 10:
			case 12:
			case 13:
				import_totalCharacters = 1;
				break;
			case 4:
			case 9:
				import_totalCharacters = 4;
				break;
			case 0xFF:
				import_totalCharacters = numberOfExportedCharacters-1;
				break;
		}
	}
}

const char* PhotoStudio::seasonName(void) const {
	switch (seasonNo[currentCharNum]) {
		case 0:
			return "Spring";
		case 1:
			return "Summer";
		case 2:
			return "Fall";
		case 3:
			return "Winter";
		case 4:
		switch (charPageOrder[char_highlightedGame[currentCharNum]]) {
			case 3:
				return ss4SpecialNames[importCharacterList_cursorPosition[currentCharNum]];
		}
	}
	
	return "";
}

const char* PhotoStudio::import_characterName(void) const {
	switch (charPageOrder[char_highlightedGame[currentCharNum]]) {
		case 0:
			return import_ss1CharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 1:
			return import_ss2CharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 2:
			return import_ss3CharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 3:
			return import_ss4CharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 4:
			return rocketCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 5:
			return smCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 6:
			return sthCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 7:
			return jfgCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 8:
			return conkerCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 9:
			return banjokCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 10:
			return pacCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 11:
			return swapCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 12:
			return metroidCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 13:
			return sc5CharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 14:
			return vvvvvvCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 15:
			return kirbyCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 16:
			return nesCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
		case 17:
			return nightsCharacterNames[importCharacterList_cursorPosition[currentCharNum]];
	}
	return "null";
}

const char* PhotoStudio::import_characterFileName(void) const {
	switch (charPageOrder[char_highlightedGame[currentCharNum]]) {
		case 2:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return ss3CharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return ss3CharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return ss3CharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return ss3CharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 3:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return ss4CharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return ss4CharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return ss4CharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return ss4CharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
			case 4:
				return ss4CharacterFileNamesSpecial[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 4:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return rocketCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return rocketCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return rocketCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return rocketCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 5:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return smCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return smCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return smCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return smCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 6:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return sthCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return sthCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return sthCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return sthCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 7:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return jfgCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return jfgCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return jfgCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return jfgCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 8:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return conkerCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return conkerCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return conkerCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return conkerCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 9:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return banjokCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return banjokCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return banjokCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return banjokCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 10:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return pacCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return pacCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return pacCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return pacCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 11:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return swapCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return swapCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return swapCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return swapCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 12:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return metroidCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return metroidCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return metroidCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return metroidCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 13:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return sc5CharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return sc5CharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return sc5CharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return sc5CharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 14:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return vvvvvvCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return vvvvvvCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return vvvvvvCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return vvvvvvCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 15:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return kirbyCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return kirbyCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return kirbyCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return kirbyCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 16:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return nesCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return nesCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return nesCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return nesCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
		case 17:
		switch (seasonNo[currentCharNum]) {
			case 0:
				return nightsCharacterFileNamesSpring[importCharacterList_cursorPosition[currentCharNum]];
			case 1:
				return nightsCharacterFileNamesSummer[importCharacterList_cursorPosition[currentCharNum]];
			case 2:
				return nightsCharacterFileNamesFall[importCharacterList_cursorPosition[currentCharNum]];
			case 3:
				return nightsCharacterFileNamesWinter[importCharacterList_cursorPosition[currentCharNum]];
		}
	}
	return "null";
}

const char* PhotoStudio::import_SS2CharacterNames(int i) const {
	switch (sysRegion) {
		default:
			return import_ss2CharacterNames[i];
		case CFG_REGION_EUR:
		case CFG_REGION_AUS:
			return import_nsbCharacterNames[i];
	}
}

const char* PhotoStudio::NESCharacterNames(int i) const {
	switch (sysRegion) {
		default:
			return nesCharacterNames[i];
		case CFG_REGION_JPN:
		case CFG_REGION_CHN:
		case CFG_REGION_KOR:
			return famiCharacterNames[i];
	}
}

const char* PhotoStudio::superPhotoTitle(void) const {
	switch (sysRegion) {
		default:
			return "Super Photo Studio";
		case CFG_REGION_JPN:
			return "Girls Mode: Photo Shoot X-over!";
	}
}

const char* PhotoStudio::mmBattleAndChaseTitle(void) const {
	switch (sysRegion) {
		default:
			return "MegaMan: Battle and Chase";
		case CFG_REGION_JPN:
			return "Rockman: Battle and Chase";
	}
}

const char* PhotoStudio::pkmnStadiumTitle(void) const {
	switch (sysRegion) {
		default:
			return "Pokemon Stadium";
		case CFG_REGION_JPN:
		case CFG_REGION_KOR:
			return "Pocket Monsters Stadium";
	}
}

const char* PhotoStudio::ss1Title(void) const {
	switch (sysRegion) {
		default:
			return "Style Savvy";
		case CFG_REGION_EUR:
		case CFG_REGION_AUS:
			return "Style Boutique";
		case CFG_REGION_JPN:
			return "Wagamama Fashion: Girls Mode";
		case CFG_REGION_KOR:
			return "Namanui Collection: Girls Style";
	}
}

const char* PhotoStudio::ss2Title(void) const {
	switch (sysRegion) {
		default:
			return "Style Savvy: Trendsetters";
		case CFG_REGION_EUR:
		case CFG_REGION_AUS:
			return "New Style Boutique";
		case CFG_REGION_JPN:
			return "Girls Mode: Yokubari Sengen";
		case CFG_REGION_KOR:
			return "Girls Style: Paesyeon Lideo Seon-eon!";
	}
}

const char* PhotoStudio::ss3Title(void) const {
	switch (sysRegion) {
		default:
			return "Style Savvy: Fashion Forward";
		case CFG_REGION_EUR:
		case CFG_REGION_AUS:
			return "New Style Boutique 2: Fashion Forward";
		case CFG_REGION_JPN:
			return "Girls Mode 3: Kirakira * Code";
		case CFG_REGION_KOR:
			return "Girls Style: Kirakira * Code";
	}
}

const char* PhotoStudio::ss4Title(void) const {
	switch (sysRegion) {
		default:
			return "Style Savvy: Styling Star";
		case CFG_REGION_EUR:
		case CFG_REGION_AUS:
			return "New Style Boutique 3: Styling Star";
		case CFG_REGION_JPN:
			return "Girls Mode 4: Star Stylist";
		case CFG_REGION_KOR:
			return "Girls Style: Star Stylist";
	}
}

const char* PhotoStudio::nesTitle(void) const {
	switch (sysRegion) {
		default:
			return "Nintendo Entertainment System";
		case CFG_REGION_JPN:
		case CFG_REGION_CHN:
		case CFG_REGION_KOR:
			return "Family Computer";
	}
}

const char* PhotoStudio::bgGameTitle(void) const {
	switch (bgPageOrder[photo_highlightedGame]) {
		case 0:
			return ss1Title();
		case 1:
			return ss2Title();
		case 2:
			return ss3Title();
		case 3:
			return ss4Title();
		case 4:
			return "Logos";
		case 5:
			return "Perfect Dark";
		case 6:
			return "Super Mario series";
	}
	return "???";
}

const char* PhotoStudio::charGameTitle(void) const {
	switch (charPageOrder[char_highlightedGame[currentCharNum]]) {
		case 0:
			return ss1Title();
		case 1:
			return ss2Title();
		case 2:
			return ss3Title();
		case 3:
			return ss4Title();
		case 4:
			return superPhotoTitle();
		case 5:
			return "Super Mario series";
		case 6:
			return "Sonic the Hedgehog series";
		case 7:
			return "Jet Force Gemini";
		case 8:
			return "Conker series";
		case 9:
			return "Banjo-Kazooie series";
		case 10:
			return "Pac-Man series";
		case 11:
			return "Swapnote/Swapdoodle";
		case 12:
			return "Metroid series";
		case 13:
			return "Space Channel 5";
		case 14:
			return "VVVVVV";
		case 15:
			return "Kirby series";
		case 16:
			return nesTitle();
		case 17:
			return "NiGHTS series";
		case 0xFF:
			return "Custom";
	}
	return "???";
}

bool PhotoStudio::charGender(int i) const {
	switch (charPageOrder[char_highlightedGame[currentCharNum]]) {
		case 0:
			return import_ss1CharacterGenders[i];
		case 1:
			return import_ss2CharacterGenders[i];
		case 2:
			return import_ss3CharacterGenders[i];
		case 3:
			return import_ss4CharacterGenders[i];
		case 4:
			return rocketCharacterGenders[i];
		case 5:
			return smCharacterGenders[i];
		case 6:
			return sthCharacterGenders[i];
		case 7:
			return jfgCharacterGenders[i];
		case 8:
			return conkerCharacterGenders[i];
		case 9:
			return banjokCharacterGenders[i];
		case 10:
			return pacCharacterGenders[i];
		case 11:
			return swapCharacterGenders[i];
		case 12:
			return metroidCharacterGenders[i];
		case 13:
			return sc5CharacterGenders[i];
		case 14:
			return vvvvvvCharacterGenders[i];
		case 15:
			return kirbyCharacterGenders[i];
		case 16:
			return nesCharacterGenders[i];
		case 17:
			return nightsCharacterGenders[i];
		case 0xFF:
			return getExportedCharacterGender(i);
	}
	return true;
}

int PhotoStudio::charPose(int i) const {
	switch (charPageOrder[char_highlightedGame[currentCharNum]]) {
		case 0:
			return import_ss1CharacterPoses[i];
		/* case 1:
			return import_ss2CharacterGenders[i];
		case 2:
			return import_ss3CharacterGenders[i];
		case 3:
			return import_ss4CharacterGenders[i];
		case 4:
			return rocketCharacterGenders[i];
		case 5:
			return smCharacterGenders[i];
		case 6:
			return sthCharacterGenders[i];
		case 7:
			return jfgCharacterGenders[i];
		case 8:
			return conkerCharacterGenders[i];
		case 9:
			return banjokCharacterGenders[i];
		case 10:
			return pacCharacterGenders[i];
		case 11:
			return swapCharacterGenders[i];
		case 12:
			return metroidCharacterGenders[i];
		case 13:
			return sc5CharacterGenders[i];
		case 14:
			return vvvvvvCharacterGenders[i];
		case 15:
			return kirbyCharacterGenders[i];
		case 16:
			return nesCharacterGenders[i];
		case 17:
			return nightsCharacterGenders[i];
		case 0xFF:
			return getExportedCharacterGender(i); */
	}
	return 0;
}

const char* PhotoStudio::bgmGameTitle(void) const {
	switch (bgmPageOrder[bgm_highlightedGame]) {
		case 0:
			return pkmnStadiumTitle();
		case 1:
			return mmBattleAndChaseTitle();
		case 2:
			return "Mario Golf: Advance Tour";
		case 3:
			return "Tetris Party";
		case 4:
			return "Sonic Mania";
		case 5:
			return ss1Title();
		case 6:
			return "Jim Power";
		case 7:
			return "Savvy Manager";
		case 8:
			return ss2Title();
		case 9:
			return "Super Mario Bros.";
		case 10:
			return "Sonic Jam";
		case 11:
			return ss3Title();
	}
	return "???";
}

const char* PhotoStudio::bgName(int i) const {
	switch (bgPageOrder[photo_highlightedGame]) {
		case 0:
			return import_ss1BgNames[i];
		case 1:
			return import_ss2BgNames[i];
		case 2:
			return import_ss3BgNames[i];
		case 3:
			return import_ss4BgNames[i];
		case 4:
			return logoBgNames[i];
		case 5:
			return pdarkBgNames[i];
		case 6:
			return smBgNames[i];
	}
	return "???";
}

const char* PhotoStudio::charName(int i) const {
	switch (charPageOrder[char_highlightedGame[currentCharNum]]) {
		case 0:
			return import_ss1CharacterNames[i];
		case 1:
			return import_SS2CharacterNames(i);
		case 2:
			return import_ss3CharacterNames[i];
		case 3:
			return import_ss4CharacterNames[i];
		case 4:
			return rocketCharacterNames[i];
		case 5:
			return smCharacterNames[i];
		case 6:
			return sthCharacterNames[i];
		case 7:
			return jfgCharacterNames[i];
		case 8:
			return conkerCharacterNames[i];
		case 9:
			return banjokCharacterNames[i];
		case 10:
			return pacCharacterNames[i];
		case 11:
			return swapCharacterNames[i];
		case 12:
			return metroidCharacterNames[i];
		case 13:
			return sc5CharacterNames[i];
		case 14:
			return vvvvvvCharacterNames[i];
		case 15:
			return kirbyCharacterNames[i];
		case 16:
			return NESCharacterNames(i);
		case 17:
			return nightsCharacterNames[i];
		case 0xFF:
			return getExportedCharacterName(i);
	}
	return "???";
}

const char* PhotoStudio::bgmName(int i) const {
	switch (bgmPageOrder[bgm_highlightedGame]) {
		case 0:
			return pkmnStadiumBgmNames[i];
		case 1:
			return mmBattleAndChaseBgmNames[i];
		case 2:
			return marioGolfAdvanceTourBgmNames[i];
		case 3:
			return tetrisPartyBgmNames[i];
		case 4:
			return sonicManiaBgmNames[i];
		case 5:
			return ss1BgmNames[i];
		case 6:
			return jimPowerBgmNames[i];
		case 7:
			return savvyMgrBgmNames[i];
		case 8:
			return ss2BgmNames[i];
		case 9:
			return superMarioBrosBgmNames[i];
		case 10:
			return sonicJamBgmNames[i];
		case 11:
			return ss3BgmNames[i];
	}
	return "???";
}

int PhotoStudio::getBgNum(void) const {
	switch (bgPageOrder[photo_highlightedGame]) {
		case 0:
			return import_ss1BgNums[bgList_cursorPosition];
		case 1:
			return import_ss2BgNums[bgList_cursorPosition];
		case 2:
			return import_ss3BgNums[bgList_cursorPosition];
		case 3:
			return import_ss4BgNums[bgList_cursorPosition];
		case 4:
			return logoBgNums[bgList_cursorPosition];
		case 5:
			return pdarkBgNums[bgList_cursorPosition];
		case 6:
			return smBgNums[bgList_cursorPosition];
	}
	return 0;
}

int PhotoStudio::getBgmNum(void) const {
	switch (bgmPageOrder[bgm_highlightedGame]) {
		case 0:
			return pkmnStadiumBgmNums[bgmList_cursorPosition];
		case 1:
			return mmBattleAndChaseBgmNums[bgmList_cursorPosition];
		case 2:
			return marioGolfAdvanceTourBgmNums[bgmList_cursorPosition];
		case 3:
			return tetrisPartyBgmNums[bgmList_cursorPosition];
		case 4:
			return sonicManiaBgmNums[bgmList_cursorPosition];
		case 5:
			return ss1BgmNums[bgmList_cursorPosition];
		case 6:
			return jimPowerBgmNums[bgmList_cursorPosition];
		case 7:
			return savvyMgrBgmNums[bgmList_cursorPosition];
		case 8:
			return ss2BgmNums[bgmList_cursorPosition];
		case 9:
			return superMarioBrosBgmNums[bgmList_cursorPosition];
		case 10:
			return sonicJamBgmNums[bgmList_cursorPosition];
		case 11:
			return ss3BgmNums[bgmList_cursorPosition];
	}
	return 0;
}

void PhotoStudio::drawMsg(void) const {
	#ifdef __3DS__
	GFX::DrawSprite(sprites_msg_idx, 0, 8, 1, 1);
	GFX::DrawSprite(sprites_msg_idx, 160, 8, -1, 1);
	GFX::DrawSprite(messageNo == 4 ? sprites_icon_question_idx : sprites_icon_msg_idx, 132, -2);

	Gui::DrawStringCentered(0, 84, 0.60, BLACK, "Please remove latest");
	Gui::DrawStringCentered(0, 104, 0.60, BLACK, "character(s) first.");

	GFX::DrawSprite(sprites_button_msg_shadow_idx, 114, 197);
	GFX::DrawSprite(sprites_button_msg_idx, 115, 188);
	Gui::DrawString(134, 196, 0.70, MSG_BUTTONTEXT, " OK!");
	#endif
}

void PhotoStudio::loadChrImage(void) {
	previewCharacter = false;
	#ifdef NDS
	ditherlaceOnVBlank = true;
	const char* nullPath = "nitro:/null.png";
	#else
	gspWaitForVBlank();
	const char* nullPath = "romfs:/null.t3x";
	#endif
	if (charPageOrder[char_highlightedGame[currentCharNum]] == 0xFF) {
		#ifdef NDS
		sprintf(chrFilePath, "/_nds/SuperPhotoStudio/characters/%s.png", getExportedCharacterName(importCharacterList_cursorPosition[currentCharNum]));
		#else
		sprintf(chrFilePath, "sdmc:/3ds/SuperPhotoStudio/characters/%s.t3x", getExportedCharacterName(importCharacterList_cursorPosition[currentCharNum]));
		#endif
		previewCharacterFound[currentCharNum] = GFX::loadCharSprite(currentCharNum, nullPath, chrFilePath, nullPath);
	} else if (charPageOrder[char_highlightedGame[currentCharNum]] >= 2) {
		/*if (numberOfExportedCharacters > 0) {
			sprintf(chrFilePath, "sdmc:/3ds/SavvyManager/SS%i/characters/previews/%s.t3x", 4, getExportedCharacterName(importCharacterList_cursorPosition[currentCharNum]));	// All Seasons
		} else {
			sprintf(chrFilePath, "romfs:/gfx/null.t3x");	// All Seasons
		}*/
		#ifdef NDS
		sprintf(chrFilePath, "nitro:/graphics/char/%s.png", import_characterFileName());
		#else
		sprintf(chrFilePath, "romfs:/gfx/%s.t3x", import_characterFileName());
		#endif
		previewCharacterFound[currentCharNum] = GFX::loadCharSprite(currentCharNum, nullPath, chrFilePath, chrFilePath);
	} else {
		#ifdef NDS
		sprintf(chrFilePathPose, "nitro:/graphics/char/ss%i_%s0/%i.png", 4, import_characterName(), characterPose[currentCharNum]+1);
		sprintf(chrFilePath, "nitro:/graphics/char/ss%i_%s.png", 4, import_characterName());				// All Seasons
		sprintf(chrFilePath2, "nitro:/graphics/char/ss%i_%s%i.png", 4, import_characterName(), seasonNo[currentCharNum]);	// One Season
		#else
		sprintf(chrFilePathPose, "romfs:/gfx/ss%i_%s0_pose%i.t3x", 4, import_characterName(), characterPose[currentCharNum]+1);
		sprintf(chrFilePath, "romfs:/gfx/ss%i_%s.t3x", 4, import_characterName());				// All Seasons
		sprintf(chrFilePath2, "romfs:/gfx/ss%i_%s%i.t3x", 4, import_characterName(), seasonNo[currentCharNum]);	// One Season
		#endif
		previewCharacterFound[currentCharNum] = GFX::loadCharSprite(currentCharNum, chrFilePathPose, chrFilePath, chrFilePath2);
	}
	#ifdef NDS
	ditherlaceOnVBlank = false;
	GFX::loadCharSpriteMem(zoomIn, &characterFlipH[0]);
	#else
	if (previewCharacterFound[0] && !characterPicked[1]) {
		GFX::loadCharSpriteMem(0);
	}
	#endif
	previewCharacter = true;
}


void PhotoStudio::Draw(void) const {
	if (!musicLoaded) {
		loadMusic(savedMusicId);
		musicLoaded = true;
	}

	#ifdef NDS	// Bottom screen only
	extern void updateTitleScreen(const int metalXposBase);
	updateTitleScreen(metalXpos);
	GFX::animateBgSprite(zoomIn, &characterFlipH[0]);

	extern void Play_Music();
	Play_Music();

	if (redrawText) {
		clearText(false);
		int cursorPosition = (subScreenMode==1 ? bgList_cursorPosition : importCharacterList_cursorPosition[currentCharNum]);
		if (subScreenMode == 3) {
			cursorPosition = bgmList_cursorPosition;
		}
		sprintf((char*)chrCounter, "%d/%d", cursorPosition+1, import_totalCharacters+1);
	}

	if (showScrollingBg) {
	  if (!metalDelay) {
		metalXpos++;
		if (metalXpos > 96) {
			metalXpos = 0;
		}
	  }
		metalDelay = !metalDelay;
	}

	cursorX = 200;
	if (subScreenMode == 4) {
		cursorY = 52+(40*charSettings_cursorPositionOnScreen);
		if (redrawText) {
			printSmall(false, 0, 6, "Character Settings", Alignment::center);
		}

	  if (!displayNothing) {
		int i2 = 40;
		int i3 = 0;
		// Reset item button Y positions
		for (int i = 0; i < 3; i++) {
			for (int x = 0; x < 4; x++) {
				oamSub.oamMemory[(2+i)+(x*3)].y = 192;
			}
			oamSub.oamMemory[14+i].y = 192;
			oamSub.oamMemory[17+i].y = 192;
		}
		//for (int i = 0; i < 2; i++) {
			for (int x = 0; x < 4; x++) {
				oamSub.oamMemory[(2+i3)+(x*3)].y = i2-16;
			}
			sprintf((char*)chrPoseCounter, "Pose < %d/%d >", characterPose[currentCharNum]+1, charPose(importCharacterList_cursorPosition[currentCharNum]));
			if (redrawText) printSmall(false, 26, i2, chrPoseCounter);
			//i2 += 40;
			//i3++;
		//}
	  }
	} else if (subScreenMode == 3) {
		cursorY = 52+(40*bgmList_cursorPositionOnScreen);
		if (redrawText) {
			printSmall(false, 0, 6, bgmGameTitle(), Alignment::center);
			printSmall(false, 6, 6, "<");
			printSmall(false, 242, 6, ">");
			printSmall(false, 56, 152, chrCounter);
		}

	  if (!displayNothing) {
		int i2 = 40;
		int i3 = 0;
		// Reset item button Y positions
		for (int i = 0; i < 3; i++) {
			for (int x = 0; x < 4; x++) {
				oamSub.oamMemory[(2+i)+(x*3)].y = 192;
			}
		}
		for (int i = bgmShownFirst; i < bgmShownFirst+3; i++) {
			if (i >= import_totalCharacters+1) break;
			for (int x = 0; x < 4; x++) {
				oamSub.oamMemory[(2+i3)+(x*3)].y = i2-16;
			}
			if (redrawText) printSmall(false, 26, i2, bgmName(i));
			i2 += 40;
			i3++;
		}
	  }
	} else if (subScreenMode == 2) {
		cursorY = 52+(40*importCharacterList_cursorPositionOnScreen[currentCharNum]);
		if (redrawText) {
			printSmall(false, 0, 6, charGameTitle(), Alignment::center);
			printSmall(false, 6, 6, "<");
			printSmall(false, 242, 6, ">");

			if (currentCharNum > 0) {
				printSmall(false, 24, 152, "X: Remove", Alignment::center);
			}
			printSmall(false, 56, 152, chrCounter);

			if (charPageOrder[char_highlightedGame[currentCharNum]] != 0xFF) {
				printLarge(false, -60, 166, "L", Alignment::center);
				printSmall(false, -26, 168, seasonName(), Alignment::center);
				printLarge(false, 10, 166, "R", Alignment::center);
			}

			printSmall(false, 158, 168, "SELECT: Flip H");
		}

	  if (!displayNothing) {
		int i2 = 40;
		int i3 = 0;
		// Reset gender icon and item button Y positions
		for (int i = 0; i < 3; i++) {
			for (int x = 0; x < 4; x++) {
				oamSub.oamMemory[(2+i)+(x*3)].y = 192;
			}
			oamSub.oamMemory[14+i].y = 192;
			oamSub.oamMemory[17+i].y = 192;
		}
		for (int i = import_characterShownFirst[currentCharNum]; i < import_characterShownFirst[currentCharNum]+3; i++) {
			if (i >= import_totalCharacters+1) break;
			for (int x = 0; x < 4; x++) {
				oamSub.oamMemory[(2+i3)+(x*3)].y = i2-16;
			}
			oamSub.oamMemory[(charGender(i) ? 17 : 14)+i3].y = i2-6;
			if (redrawText) printSmall(false, 54, i2, charName(i));
			i2 += 40;
			i3++;
		}
	  }
	} else if (subScreenMode == 1) {
		cursorY = 52+(40*bgList_cursorPositionOnScreen);
		if (redrawText) {
			printSmall(false, 0, 6, bgGameTitle(), Alignment::center);
			printSmall(false, 6, 6, "<");
			printSmall(false, 242, 6, ">");
			printSmall(false, 56, 152, chrCounter);
		}

	  if (!displayNothing) {
		int i2 = 40;
		int i3 = 0;
		// Reset item button Y positions
		for (int i = 0; i < 3; i++) {
			for (int x = 0; x < 4; x++) {
				oamSub.oamMemory[(2+i)+(x*3)].y = 192;
			}
		}
		for (int i = import_bgShownFirst; i < import_bgShownFirst+3; i++) {
			if (i >= import_totalCharacters+1) break;
			for (int x = 0; x < 4; x++) {
				oamSub.oamMemory[(2+i3)+(x*3)].y = i2-16;
			}
			if (redrawText) printSmall(false, 26, i2, bgName(i));
			i2 += 40;
			i3++;
		}
	  }
	} else {
		cursorY = 52+(40*characterChangeMenu_cursorPositionOnScreen);
		if (redrawText) {
			printSmall(false, 6, 6, "What do you want to do?");

			extern bool fatInited;
			if (fatInited) {
				printSmall(false, 162, 168, "Y: Take Photo");
			}
		}

		// Reset gender icon Y positions
		for (int i = 0; i < 3; i++) {
			oamSub.oamMemory[14+i].y = 192;
			oamSub.oamMemory[17+i].y = 192;
		}
		int i2 = 0;
		i2 += 40;
		for (int x = 0; x < 4; x++) {
			oamSub.oamMemory[2+(x*3)].y = i2-16;
		}
		if (redrawText) printSmall(false, 26, i2, "Change Location");
		i2 += 40;
		for (int x = 0; x < 4; x++) {
			oamSub.oamMemory[3+(x*3)].y = i2-16;
		}
		if (redrawText) {
		if (currentCharNum==4) {
			printSmall(false, 26, i2, characterPicked[4] ? "Change Character < 5 >" : "Add Character < 5 >");
		} else if (currentCharNum==3) {
			printSmall(false, 26, i2, characterPicked[3] ? "Change Character < 4 >" : "Add Character < 4 >");
		} else if (currentCharNum==2) {
			printSmall(false, 26, i2, characterPicked[2] ? "Change Character < 3 >" : "Add Character < 3 >");
		} else if (currentCharNum==1) {
			printSmall(false, 26, i2, characterPicked[1] ? "Change Character < 2 >" : "Add Character < 2 >");
		} else {
			printSmall(false, 26, i2, characterPicked[0] ? "Change Character < 1 >" : "Add Character < 1 >");
		}
		}
		//if (dsDebugRam || mepFound) {
			i2 += 40;
			for (int x = 0; x < 4; x++) {
				oamSub.oamMemory[4+(x*3)].y = i2-16;
			}
			if (redrawText) printSmall(false, 26, i2, "Change Music");
		/*} else {
			// Hide 3rd button
			for (int x = 0; x < 4; x++) {
				oamSub.oamMemory[4+(x*3)].y = 192;
			}
		}*/
	}

	oamSub.oamMemory[0].y = (subScreenMode != 0) ? 156 : 192;
	oamSetXY(&oamSub, 1, cursorX, showCursor ? cursorY : 192);

	if (redrawText) {
		extern void updateText(bool top);
		updateText(false);
		redrawText = false;
	}
	#else
	animateBg = bgCanAnimate;

	if (!musicPlayStarted && !musicPlayOnce) {
		extern void Play_Music();
		Play_Music();
		musicPlayStarted = true;
		musicPlayOnce = true;
	}

  if (renderTop) {
	Gui::ScreenDraw(Top);

	preview();

	if (currentCharacterRendered == 0) {
		if (displayStudioBg) {
			GFX::showBgSprite(zoomIn);
		} else if (showScrollingBg) {
			GFX::DrawSpriteLinear(sprites_titleMetal_idx, metalXpos, 0, 16, 16);
			metalXpos--;
			if (metalXpos < -8*16) {
				metalXpos = 0;
			}
			GFX::DrawSprite(sprites_title_idx, 0, 0, 0.5);
		} else {
			Gui::Draw_Rect(0, 0, 400, 240, WHITE);
		}
		if (previewCharacter) {
			if (previewCharacterFound[0]) {
				if (characterPicked[1]) {
					GFX::loadCharSpriteMem(0);
				}
				GFX::showCharSprite(0, characterFlipH[0], zoomIn, charFadeAlpha, displayStudioBg);
			} else {
				Gui::DrawStringCentered(0, 104, 0.65, WHITE, (/*charPageOrder[char_highlightedGame[currentCharNum]]==4 ? "Preview not found." :*/ "Preview unavailable."));
			}
		}
	} else {
		if (characterPicked[currentCharacterRendered] && previewCharacterFound[currentCharacterRendered]) {
			GFX::loadCharSpriteMem(currentCharacterRendered);
			GFX::showCharSprite(currentCharacterRendered, characterFlipH[currentCharacterRendered], zoomIn, charFadeAlpha, displayStudioBg);
		}
	}

	if (renderTop && subScreenMode!=1 && previewCharacter && previewCharacterFound[0] && previewCharacterFound[1]) {
		currentCharacterRendered++;
		if (currentCharacterRendered > charsShown) {
			currentCharacterRendered = 0;
			renderTop = false;
		}
		clearTop = (currentCharacterRendered == 0);
	}

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
  }

	if (cinemaWide) {
		Gui::Draw_Rect(0, 0, 400, 36, C2D_Color32(0, 0, 0, 255));
		Gui::Draw_Rect(0, 204, 400, 36, C2D_Color32(0, 0, 0, 255));
	}

	if (shiftBySubPixel) {
		Gui::Draw_Rect(0, 239, 400, 1, C2D_Color32(0, 0, 0, 255));
		return;
	}
	Gui::ScreenDraw(Bottom);
	int cursorPosition = (subScreenMode==1 ? bgList_cursorPosition : importCharacterList_cursorPosition[currentCharNum]);
	if (subScreenMode == 3) {
		cursorPosition = bgmList_cursorPosition;
	}
	sprintf((char*)chrCounter, "%d/%d", cursorPosition+1, import_totalCharacters+1);
	GFX::DrawSprite(sprites_photo_bg_idx, 0, 0);

	cursorX = 248;
	if (subScreenMode == 10) {
		SettingsDraw();
	} else if (subScreenMode == 4) {
		cursorY = 64+(48*charSettings_cursorPositionOnScreen);

		Gui::DrawStringCentered(0, 8, 0.50, WHITE, "Character Settings");

	  if (!displayNothing) {
		int i2 = 48;
		//for (int i = 0; i < 2; i++) {
			GFX::DrawSprite(sprites_item_button_idx, 18, i2-20);
			sprintf((char*)chrPoseCounter, "Pose < %d/%d >", characterPose[currentCharNum]+1, charPose(importCharacterList_cursorPosition[currentCharNum]));
			Gui::DrawString(32, i2, 0.65, WHITE, chrPoseCounter);
			//i2 += 48;
		//}
	  }
	} else if (subScreenMode == 3) {
		cursorY = 64+(48*bgmList_cursorPositionOnScreen);

		Gui::DrawStringCentered(0, 8, 0.50, WHITE, bgmGameTitle());
		Gui::DrawString(8, 8, 0.50, WHITE, "<");
		Gui::DrawString(304, 8, 0.50, WHITE, ">");
		Gui::DrawString(64, 184, 0.55, WHITE, chrCounter);

	  if (!displayNothing) {
		int i2 = 48;
		for (int i = bgmShownFirst; i < bgmShownFirst+3; i++) {
			if (i >= import_totalCharacters+1) break;
			GFX::DrawSprite(sprites_item_button_idx, 18, i2-20);
			Gui::DrawString(32, i2, 0.65, WHITE, bgmName(i));
			i2 += 48;
		}
	  }
	} else if (subScreenMode == 2) {
		cursorY = 64+(48*importCharacterList_cursorPositionOnScreen[currentCharNum]);

		Gui::DrawStringCentered(0, 8, 0.50, WHITE, charGameTitle());
		Gui::DrawString(8, 8, 0.50, WHITE, "<");
		Gui::DrawString(304, 8, 0.50, WHITE, ">");

		if (currentCharNum > 0) {
			Gui::DrawStringCentered(12, 184, 0.55, WHITE, ": Remove");
		}
		Gui::DrawString(64, 184, 0.55, WHITE, chrCounter);

		if (charPageOrder[char_highlightedGame[currentCharNum]] != 0xFF) {
			// Selected season
			Gui::DrawString(120-36, 208, 0.65, WHITE, "L");
			Gui::DrawStringCentered(-36, 210, 0.50, WHITE, seasonName());
			Gui::DrawString(192-36, 208, 0.65, WHITE, "R");
		}

		Gui::DrawString(184, 208, 0.65, WHITE, "SELECT: Flip H");

	  if (!displayNothing) {
		int i2 = 48;
		for (int i = import_characterShownFirst[currentCharNum]; i < import_characterShownFirst[currentCharNum]+3; i++) {
			if (i >= import_totalCharacters+1) break;
			GFX::DrawSprite(sprites_item_button_idx, 18, i2-20);
			GFX::DrawSprite((charGender(i) ? sprites_icon_male_idx : sprites_icon_female_idx), 14, i2-8);
			Gui::DrawString(66, i2, 0.65, WHITE, charName(i));
			i2 += 48;
		}
	  }
	} else if (subScreenMode == 1) {
		cursorY = 64+(48*bgList_cursorPositionOnScreen);

		Gui::DrawStringCentered(0, 8, 0.50, WHITE, bgGameTitle());
		Gui::DrawString(8, 8, 0.50, WHITE, "<");
		Gui::DrawString(304, 8, 0.50, WHITE, ">");
		Gui::DrawString(64, 184, 0.55, WHITE, chrCounter);

		/*if (photo_highlightedGame != 4) {
			// Selected season
			Gui::DrawString(120, 208, 0.65, BLACK, "L");
			Gui::DrawStringCentered(0, 210, 0.50, BLACK, seasonName());
			Gui::DrawString(192, 208, 0.65, BLACK, "R");
		}*/

	  if (!displayNothing) {
		int i2 = 48;
		for (int i = import_bgShownFirst; i < import_bgShownFirst+3; i++) {
			if (i >= import_totalCharacters+1) break;
			GFX::DrawSprite(sprites_item_button_idx, 18, i2-20);
			Gui::DrawString(32, i2, 0.65, WHITE, bgName(i));
			i2 += 48;
		}
	  }
	} else {
		cursorY = 64+(48*characterChangeMenu_cursorPositionOnScreen);

		Gui::DrawString(8, 8, 0.50, WHITE, "What do you want to do?");

		Gui::DrawString(192, 206, 0.65, WHITE, ": Take Photo");

		int i2 = 0;
		i2 += 48;
		GFX::DrawSprite(sprites_item_button_idx, 18, i2-20);
		Gui::DrawString(32, i2, 0.65, WHITE, "Change Location");
		i2 += 48;
		GFX::DrawSprite(sprites_item_button_idx, 18, i2-20);
		if (currentCharNum==4) {
			Gui::DrawString(32, i2, 0.65, WHITE, characterPicked[4] ? "Change Character < 5 >" : "Add Character < 5 >");
		} else if (currentCharNum==3) {
			Gui::DrawString(32, i2, 0.65, WHITE, characterPicked[3] ? "Change Character < 4 >" : "Add Character < 4 >");
		} else if (currentCharNum==2) {
			Gui::DrawString(32, i2, 0.65, WHITE, characterPicked[2] ? "Change Character < 3 >" : "Add Character < 3 >");
		} else if (currentCharNum==1) {
			Gui::DrawString(32, i2, 0.65, WHITE, characterPicked[1] ? "Change Character < 2 >" : "Add Character < 2 >");
		} else {
			Gui::DrawString(32, i2, 0.65, WHITE, characterPicked[0] ? "Change Character < 1 >" : "Add Character < 1 >");
		}
		i2 += 48;
		GFX::DrawSprite(sprites_item_button_idx, 18, i2-20);
		Gui::DrawString(32, i2-8, 0.65, WHITE, "Change Music");
		Gui::DrawString(32, i2+12, 0.50, WHITE, "Relaunch to take effect.");
	}

	if (subScreenMode != 0) {
		GFX::DrawSprite(sprites_button_shadow_idx, 5, 199);
		GFX::DrawSprite(sprites_button_red_idx, 5, 195);
		GFX::DrawSprite(sprites_arrow_back_idx, 19, 195);
		GFX::DrawSprite(sprites_button_b_idx, 44, 218);
	}

	GFX::drawCursor(cursorX, cursorY);

	if (showMessage) {
		drawMsg();
	}

	if (fadealpha > 0) Gui::Draw_Rect(0, 0, 400, 240, C2D_Color32(fadecolor, fadecolor, fadecolor, fadealpha)); // Fade in/out effect
	#endif
}

void PhotoStudio::preview() const {
	if (characterPicked[1]) {
		charFadeAlpha = 255;
	} else if (previewCharacter) {
		switch (iFps) {
			default:
				charFadeAlpha += 20;
				break;
			case 30:
				charFadeAlpha += 40;
				break;
			case 24:
				charFadeAlpha += 55;
				break;
		}
		if (charFadeAlpha > 255) charFadeAlpha = 255;
	} else {
		charFadeAlpha = 0;
	}
}



void PhotoStudio::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	#ifdef NDS
	if (subScreenMode==0 && !characterPicked[3])
	#else
	if ((subScreenMode==0 || subScreenMode==2) && (!characterPicked[1] || (characterPicked[1] && !renderTop)) && !characterPicked[3])
	#endif
	{
		int zoomLimit = characterPicked[1] ? 1 : 2;
		if (hDown & KEY_CPAD_UP) {
			zoomIn++;
			if (zoomIn > zoomLimit) {
				zoomIn = zoomLimit;
			} else {
				#ifdef NDS
				GFX::loadCharSpriteMem(zoomIn, &characterFlipH[0]);
				#endif
				renderTop = true;
			}
		}
		if (hDown & KEY_CPAD_DOWN) {
			zoomIn--;
			if (zoomIn < 0) {
				zoomIn = 0;
			} else {
				#ifdef NDS
				GFX::loadCharSpriteMem(zoomIn, &characterFlipH[0]);
				#endif
				renderTop = true;
			}
		}
	}

	if (showMessage) {
		if ((hDown & KEY_A) || ((hDown & KEY_TOUCH) && touch.px >= 115 && touch.px < 115+90 && touch.py >= 188 && touch.py < 188+47)) {
			sndSelect();
			showMessage = false;
		}
	#ifdef __3DS__
	} else if (subScreenMode == 10) {
		SettingsLogic(hDown, hHeld, touch);
	#endif
	} else if (subScreenMode == 4) {
		if (hDown & KEY_A) {
			sndSelect();
			subScreenMode = 0;
			#ifdef NDS
			redrawText = true;
			Gui::DrawScreen();
			#endif
		}

		if (hDown & KEY_SELECT) {
			sndSelect();
			characterFlipH[currentCharNum] = !characterFlipH[currentCharNum];
			#ifdef NDS
			GFX::loadCharSpriteMem(zoomIn, &characterFlipH[0]);
			#endif
			renderTop = true;
		}

		if (charPose(importCharacterList_cursorPosition[currentCharNum]) > 0) {
			if (hDown & KEY_DLEFT) {
				sndHighlight();
				characterPose[currentCharNum]--;
				if (characterPose[currentCharNum] < 0) characterPose[currentCharNum] = charPose(importCharacterList_cursorPosition[currentCharNum])-1;
				getMaxChars();
				renderTop = true;
			}

			if (hDown & KEY_DRIGHT) {
				sndHighlight();
				characterPose[currentCharNum]++;
				if (characterPose[currentCharNum] > charPose(importCharacterList_cursorPosition[currentCharNum])-1) characterPose[currentCharNum] = 0;
				getMaxChars();
				renderTop = true;
			}

			if ((hDown & KEY_DLEFT) || (hDown & KEY_DRIGHT)) {
				if (charPageOrder[char_highlightedGame[currentCharNum]] == 0xFF) {
					previewCharacter = false;
					if (!exportedCharListGotten) {
						displayNothing = true;
						gspWaitForVBlank();
						getExportedCharacterContents();
						exportedCharListGotten = true;
						displayNothing = false;
					}
				}
				getMaxChars();
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
				renderTop = true;
				loadChrImage();
			}
		}

		if (charPageOrder[char_highlightedGame[currentCharNum]] != 0xFF) {
			if ((hDown & KEY_L) || (hDown & KEY_ZL)) {
				sndHighlight();
				seasonNo[currentCharNum]--;
				if (seasonNo[currentCharNum] < 0) seasonNo[currentCharNum] = 3;
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
				loadChrImage();
				renderTop = true;
			}

			if ((hDown & KEY_R) || (hDown & KEY_ZR)) {
				sndHighlight();
				seasonNo[currentCharNum]++;
				if (seasonNo[currentCharNum] > 3) seasonNo[currentCharNum] = 0;
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
				loadChrImage();
				renderTop = true;
			}

			if (hDown & KEY_Y) {
				int seasonNoBak = seasonNo[currentCharNum];
				seasonNo[currentCharNum] = 4;	// Special outfit
				if (strcmp(seasonName(), "") != 0) {
					sndHighlight();
					#ifdef NDS
					redrawText = true;
					Gui::DrawScreen();
					#endif
					loadChrImage();
					renderTop = true;
				} else {
					seasonNo[currentCharNum] = seasonNoBak;
				}
			}
		}

		if ((hDown & KEY_B) || ((hDown & KEY_TOUCH) && touchingBackButton())) {
			sndBack();
			subScreenMode = 2;
			#ifdef NDS
			redrawText = true;
			Gui::DrawScreen();
			#endif
		}
	} else if (subScreenMode == 3) {
		if (showCursor) {
			if (hDown & KEY_DUP) {
				sndHighlight();
				bgmList_cursorPosition--;
				bgmList_cursorPositionOnScreen--;
				if (bgmList_cursorPosition < 0) {
					bgmList_cursorPosition = 0;
					bgmShownFirst = 0;
				} else if (bgmList_cursorPosition < bgmShownFirst) {
					bgmShownFirst--;
				}
				if (bgmList_cursorPositionOnScreen < 0) {
					bgmList_cursorPositionOnScreen = 0;
				}
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
			}

			if (hDown & KEY_DDOWN) {
				sndHighlight();
				bgmList_cursorPosition++;
				bgmList_cursorPositionOnScreen++;
				if (bgmList_cursorPosition > import_totalCharacters) {
					bgmList_cursorPosition = import_totalCharacters;
					bgmShownFirst = import_totalCharacters-2;
					if (bgmShownFirst < 0) bgmShownFirst = 0;
					if (bgmList_cursorPositionOnScreen > import_totalCharacters) {
						bgmList_cursorPositionOnScreen = import_totalCharacters;
					}
				} else if (bgmList_cursorPosition > bgmShownFirst+2) {
					bgmShownFirst++;
				}
				if (bgmList_cursorPositionOnScreen > 2) {
					bgmList_cursorPositionOnScreen = 2;
				}
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
			}
		}

		if (hDown & KEY_A) {
			sndSelect();
			subScreenMode = 0;
			savedMusicId = getBgmNum();
			#ifdef NDS
			redrawText = true;
			Gui::DrawScreen();
			ditherlaceOnVBlank = true;
			extern void Stop_Music(void);
			Stop_Music();
			loadMusic(savedMusicId);
			extern void saveSettings(void);
			saveSettings();
			#endif
			#ifdef NDS
			ditherlaceOnVBlank = false;
			#else
			musicPlayStarted = true;
			#endif
		}

		if (hDown & KEY_DLEFT) {
			sndHighlight();
			bgm_highlightedGame--;
			if (bgm_highlightedGame < 0) bgm_highlightedGame = (int)sizeof(bgmPageOrder)-1;
			getMaxChars();
		}

		if (hDown & KEY_DRIGHT) {
			sndHighlight();
			bgm_highlightedGame++;
			if (bgm_highlightedGame > (int)sizeof(bgmPageOrder)-1) bgm_highlightedGame = 0;
			getMaxChars();
		}

		if ((hDown & KEY_DLEFT) || (hDown & KEY_DRIGHT)) {
			bgmList_cursorPosition = 0;
			bgmList_cursorPositionOnScreen = 0;
			bgmShownFirst = 0;
			getMaxChars();
			#ifdef NDS
			redrawText = true;
			Gui::DrawScreen();
			#endif
		}

		if ((hDown & KEY_B) || ((hDown & KEY_TOUCH) && touchingBackButton())) {
			sndBack();
			subScreenMode = 0;
			#ifdef NDS
			redrawText = true;
			Gui::DrawScreen();
			#endif
		}

	} else if (subScreenMode == 2) {
		if (showCursor) {
			if (hDown & KEY_DUP) {
				sndHighlight();
				importCharacterList_cursorPosition[currentCharNum]--;
				importCharacterList_cursorPositionOnScreen[currentCharNum]--;
				if (importCharacterList_cursorPosition[currentCharNum] < 0) {
					importCharacterList_cursorPosition[currentCharNum] = 0;
					import_characterShownFirst[currentCharNum] = 0;
				} else if (importCharacterList_cursorPosition[currentCharNum] < import_characterShownFirst[currentCharNum]) {
					import_characterShownFirst[currentCharNum]--;
				}
				if (importCharacterList_cursorPositionOnScreen[currentCharNum] < 0) {
					importCharacterList_cursorPositionOnScreen[currentCharNum] = 0;
				}
				if (seasonNo[currentCharNum] == 4 && strcmp(seasonName(), "") == 0) {
					seasonNo[currentCharNum] = 0;
				}
				characterPose[currentCharNum] = 0;
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
				renderTop = true;
				loadChrImage();
			}

			if (hDown & KEY_DDOWN) {
				sndHighlight();
				importCharacterList_cursorPosition[currentCharNum]++;
				importCharacterList_cursorPositionOnScreen[currentCharNum]++;
				if (importCharacterList_cursorPosition[currentCharNum] > import_totalCharacters) {
					importCharacterList_cursorPosition[currentCharNum] = import_totalCharacters;
					import_characterShownFirst[currentCharNum] = import_totalCharacters-2;
					if (import_characterShownFirst[currentCharNum] < 0) import_characterShownFirst[currentCharNum] = 0;
					if (importCharacterList_cursorPositionOnScreen[currentCharNum] > import_totalCharacters) {
						importCharacterList_cursorPositionOnScreen[currentCharNum] = import_totalCharacters;
					}
				} else if (importCharacterList_cursorPosition[currentCharNum] > import_characterShownFirst[currentCharNum]+2) {
					import_characterShownFirst[currentCharNum]++;
				}
				if (importCharacterList_cursorPositionOnScreen[currentCharNum] > 2) {
					importCharacterList_cursorPositionOnScreen[currentCharNum] = 2;
				}
				if (seasonNo[currentCharNum] == 4 && strcmp(seasonName(), "") == 0) {
					seasonNo[currentCharNum] = 0;
				}
				characterPose[currentCharNum] = 0;
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
				renderTop = true;
				loadChrImage();
			}
		}

		if (hDown & KEY_A) {
			sndSelect();
			subScreenMode = 4;
			#ifdef NDS
			redrawText = true;
			Gui::DrawScreen();
			#endif
		}

		if ((hDown & KEY_X) && (currentCharNum > 0) && characterPicked[currentCharNum]) {
			if ((currentCharNum == 4) || !characterPicked[currentCharNum+1]) {
				sndSelect();
				characterPicked[currentCharNum] = false;
				previewCharacterFound[currentCharNum] = false;
				GFX::resetCharStatus(currentCharNum);
				currentCharNum--;
				charsShown--;
				subScreenMode = 0;
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				GFX::loadCharSpriteMem(zoomIn, &characterFlipH[0]);
				#endif
				renderTop = true;
				#ifndef NDS
				loadChrImage();
				#endif
			} else {
				sndBack();
				showMessage = true;
			}
		}

		if (hDown & KEY_SELECT) {
			sndSelect();
			characterFlipH[currentCharNum] = !characterFlipH[currentCharNum];
			#ifdef NDS
			GFX::loadCharSpriteMem(zoomIn, &characterFlipH[0]);
			#endif
			renderTop = true;
		}

		if (hDown & KEY_DLEFT) {
			sndHighlight();
			char_highlightedGame[currentCharNum]--;
			if (char_highlightedGame[currentCharNum] < 0) char_highlightedGame[currentCharNum] = (int)sizeof(charPageOrder)-1;
			getMaxChars();
			renderTop = true;
		}

		if (hDown & KEY_DRIGHT) {
			sndHighlight();
			char_highlightedGame[currentCharNum]++;
			if (char_highlightedGame[currentCharNum] > (int)sizeof(charPageOrder)-1) char_highlightedGame[currentCharNum] = 0;
			getMaxChars();
			renderTop = true;
		}

		if ((hDown & KEY_DLEFT) || (hDown & KEY_DRIGHT)) {
			importCharacterList_cursorPosition[currentCharNum] = 0;
			importCharacterList_cursorPositionOnScreen[currentCharNum] = 0;
			import_characterShownFirst[currentCharNum] = 0;
			if (charPageOrder[char_highlightedGame[currentCharNum]] == 0xFF) {
				previewCharacter = false;
				if (!exportedCharListGotten) {
					displayNothing = true;
					gspWaitForVBlank();
					getExportedCharacterContents();
					exportedCharListGotten = true;
					displayNothing = false;
				}
				if (numberOfExportedCharacters == 0) {
					if (hDown & KEY_DLEFT) {
						char_highlightedGame[currentCharNum]--;
						if (char_highlightedGame[currentCharNum] < 0) char_highlightedGame[currentCharNum] = (int)sizeof(charPageOrder)-1;
					}
					if (hDown & KEY_DRIGHT) {
						char_highlightedGame[currentCharNum]++;
						if (char_highlightedGame[currentCharNum] > (int)sizeof(charPageOrder)-1) char_highlightedGame[currentCharNum] = 0;
					}
				}
			}
			getMaxChars();
			characterPose[currentCharNum] = 0;
			#ifdef NDS
			redrawText = true;
			Gui::DrawScreen();
			#endif
			renderTop = true;
			loadChrImage();
		}

		if (charPageOrder[char_highlightedGame[currentCharNum]] != 0xFF) {
			if ((hDown & KEY_L) || (hDown & KEY_ZL)) {
				sndHighlight();
				seasonNo[currentCharNum]--;
				if (seasonNo[currentCharNum] < 0) seasonNo[currentCharNum] = 3;
				characterPose[currentCharNum] = 0;
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
				loadChrImage();
				renderTop = true;
			}

			if ((hDown & KEY_R) || (hDown & KEY_ZR)) {
				sndHighlight();
				seasonNo[currentCharNum]++;
				if (seasonNo[currentCharNum] > 3) seasonNo[currentCharNum] = 0;
				characterPose[currentCharNum] = 0;
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
				loadChrImage();
				renderTop = true;
			}

			if (hDown & KEY_Y) {
				int seasonNoBak = seasonNo[currentCharNum];
				seasonNo[currentCharNum] = 4;	// Special outfit
				if (strcmp(seasonName(), "") != 0) {
					sndHighlight();
					#ifdef NDS
					redrawText = true;
					Gui::DrawScreen();
					#endif
					loadChrImage();
					renderTop = true;
				} else {
					seasonNo[currentCharNum] = seasonNoBak;
				}
			}
		}

		if ((hDown & KEY_B) || ((hDown & KEY_TOUCH) && touchingBackButton())) {
			sndBack();
			subScreenMode = 0;
			#ifdef NDS
			redrawText = true;
			Gui::DrawScreen();
			#endif
		}

	} else if (subScreenMode == 1) {
		if (showCursor) {
			if (hDown & KEY_DUP) {
				sndHighlight();
				bgList_cursorPosition--;
				bgList_cursorPositionOnScreen--;
				if (bgList_cursorPosition < 0) {
					bgList_cursorPosition = 0;
					import_bgShownFirst = 0;
				} else if (bgList_cursorPosition < import_bgShownFirst) {
					import_bgShownFirst--;
				}
				if (bgList_cursorPositionOnScreen < 0) {
					bgList_cursorPositionOnScreen = 0;
				}
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
				studioBg = getBgNum();
				displayStudioBg = false;
				gspWaitForVBlank();
				GFX::reloadBgSprite();
				displayStudioBg = true;
			}

			if (hDown & KEY_DDOWN) {
				sndHighlight();
				bgList_cursorPosition++;
				bgList_cursorPositionOnScreen++;
				if (bgList_cursorPosition > import_totalCharacters) {
					bgList_cursorPosition = import_totalCharacters;
					import_bgShownFirst = import_totalCharacters-2;
					if (import_bgShownFirst < 0) import_bgShownFirst = 0;
					if (bgList_cursorPositionOnScreen > import_totalCharacters) {
						bgList_cursorPositionOnScreen = import_totalCharacters;
					}
				} else if (bgList_cursorPosition > import_bgShownFirst+2) {
					import_bgShownFirst++;
				}
				if (bgList_cursorPositionOnScreen > 2) {
					bgList_cursorPositionOnScreen = 2;
				}
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
				studioBg = getBgNum();
				displayStudioBg = false;
				gspWaitForVBlank();
				GFX::reloadBgSprite();
				displayStudioBg = true;
			}
		}

		if (hDown & KEY_A) {
			sndSelect();
			subScreenMode = 0;
			previewCharacter = characterPicked[currentCharNum];
			if (characterPicked[3] && zoomIn < 1) {
				zoomIn = 1;
			}
			#ifdef NDS
			displayChars = true;
			redrawText = true;
			Gui::DrawScreen();
			if (characterPicked[0]) GFX::loadCharSpriteMem(zoomIn, &characterFlipH[0]);
			#endif
		}

		if (hDown & KEY_DLEFT) {
			sndHighlight();
			photo_highlightedGame--;
			if (photo_highlightedGame < 0) photo_highlightedGame = (int)sizeof(bgPageOrder)-1;
			getMaxChars();
		}

		if (hDown & KEY_DRIGHT) {
			sndHighlight();
			photo_highlightedGame++;
			if (photo_highlightedGame > (int)sizeof(bgPageOrder)-1) photo_highlightedGame = 0;
			getMaxChars();
		}

		if ((hDown & KEY_DLEFT) || (hDown & KEY_DRIGHT)) {
			bgList_cursorPosition = 0;
			bgList_cursorPositionOnScreen = 0;
			import_bgShownFirst = 0;
			getMaxChars();
			#ifdef NDS
			redrawText = true;
			Gui::DrawScreen();
			#endif
			studioBg = getBgNum();
			displayStudioBg = false;
			gspWaitForVBlank();
			GFX::reloadBgSprite();
			displayStudioBg = true;
		}

		if ((hDown & KEY_B) || ((hDown & KEY_TOUCH) && touchingBackButton())) {
			sndBack();
			subScreenMode = 0;
			previewCharacter = characterPicked[currentCharNum];
			if (characterPicked[3] && zoomIn < 1) {
				zoomIn = 1;
			}
			#ifdef NDS
			displayChars = true;
			redrawText = true;
			Gui::DrawScreen();
			if (characterPicked[0]) GFX::loadCharSpriteMem(zoomIn, &characterFlipH[0]);
			#endif
		}

	} else {
		if (showCursor) {
			if (hDown & KEY_DUP) {
				sndHighlight();
				characterChangeMenu_cursorPosition--;
				characterChangeMenu_cursorPositionOnScreen--;
				if (characterChangeMenu_cursorPosition < 0) {
					characterChangeMenu_cursorPosition = 0;
				}
				if (characterChangeMenu_cursorPositionOnScreen < 0) {
					characterChangeMenu_cursorPositionOnScreen = 0;
				}
			}

			if (hDown & KEY_DDOWN) {
				sndHighlight();
				characterChangeMenu_cursorPosition++;
				characterChangeMenu_cursorPositionOnScreen++;
				/*#ifdef NDS
				int limit = (dsDebugRam || mepFound) ? 2 : 1;
				if (characterChangeMenu_cursorPosition > limit) {
					characterChangeMenu_cursorPosition = limit;
				}
				if (characterChangeMenu_cursorPositionOnScreen > limit) {
					characterChangeMenu_cursorPositionOnScreen = limit;
				}
				#else*/
				if (characterChangeMenu_cursorPosition > 2) {
					characterChangeMenu_cursorPosition = 2;
				}
				if (characterChangeMenu_cursorPositionOnScreen > 2) {
					characterChangeMenu_cursorPositionOnScreen = 2;
				}
				//#endif
			}

			if (characterChangeMenu_cursorPosition == 1) {
				if ((hDown & KEY_DLEFT) && characterPicked[0]) {
					sndHighlight();
					currentCharNum--;
					if (currentCharNum < 0) {
						currentCharNum = 0;
					}
					#ifdef NDS
					else {
						redrawText = true;
					}
					#endif
				}
				if ((hDown & KEY_DRIGHT) && characterPicked[0]) {
					sndHighlight();
					currentCharNum++;
					if (currentCharNum > charsShown+1) {
						currentCharNum = charsShown+1;
					}
					if (currentCharNum > characterLimit) {
						currentCharNum = characterLimit;
					}
					#ifdef NDS
					else {
						redrawText = true;
					}
					#endif
				}
			}
		}

		if (hDown & KEY_A) {
			if (characterChangeMenu_cursorPosition == 0) {
				sndSelect();
				subScreenMode = 1;
				getMaxChars();
				previewCharacter = false;
				#ifdef NDS
				displayChars = false;
				redrawText = true;
				Gui::DrawScreen();
				#endif
				int bgNum = getBgNum();
				//if (studioBg != bgNum) {
					showScrollingBg = false;
					displayStudioBg = false;
					gspWaitForVBlank();
					studioBg = bgNum;
					GFX::reloadBgSprite();
					displayStudioBg = true;
				//}
				zoomIn = 0;
				renderTop = true;
			} else if (characterChangeMenu_cursorPosition == 1) {
				sndSelect();
				displayNothing = true;
				subScreenMode = 2;
				/*if ((subScreenMode == 2) && (charPageOrder[char_highlightedGame[currentCharNum]] == 4) && !exportedCharListGotten) {
					gspWaitForVBlank();
					getExportedCharacterContents();
					exportedCharListGotten = true;
				}*/
				getMaxChars();
				displayNothing = false;
				bool doCharLoad = !characterPicked[currentCharNum];
				if (!characterPicked[currentCharNum] && currentCharNum != 0) {
					charsShown++;
				}
				characterPicked[currentCharNum] = true;
				if (characterPicked[0] && characterPicked[1] && zoomIn > 1) {
					zoomIn = 1;
				}
				if (characterPicked[3] && zoomIn < 1) {
					zoomIn = 1;
				}
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
				if (doCharLoad) {
					renderTop = true;
					loadChrImage();
				}
			} else if (characterChangeMenu_cursorPosition == 2) {
				sndSelect();
				subScreenMode = 3;
				getMaxChars();
				#ifdef NDS
				redrawText = true;
				Gui::DrawScreen();
				#endif
			}
		}

		if (hDown & KEY_Y) {
			extern bool doScreenshot;
			doScreenshot = true;
		}
	
		#ifdef __3DS__
		if (hDown & KEY_SELECT) {
			sndSelect();
			subScreenMode = 10;
		}
		#endif
	}
}