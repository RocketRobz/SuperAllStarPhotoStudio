#include <nds/dma.h>
#include "productIdent.hpp"
#include "lodepng.h"
#include "myDSiMode.h"

extern char verText[32];

void ProductIdent::Draw(void) const {
	fadecolor = 0;	// Always use black color for fading effects

	/*Gui::DrawString(26, 86, 0.45, LIGHTRED, "Nintendo DS Product Identification");
	Gui::Draw_Rect(24, 100, 400-28, 1, LIGHTRED);
	Gui::DrawString(40, 100, 0.45, LIGHTRED, "Title:");
	Gui::DrawString(192, 100, 0.45, LIGHTRED, "SUPERPHOTO");
	Gui::DrawString(40, 110, 0.45, LIGHTRED, "Product Code:");
	Gui::DrawString(192, 110, 0.45, LIGHTRED, "TWL-HPHA");
	Gui::DrawString(40, 130, 0.45, LIGHTRED, "Variant:");
	Gui::DrawString(192, 130, 0.45, LIGHTRED, verText);
	Gui::DrawString(40, 140, 0.45, LIGHTRED, "Developer:");
	Gui::DrawString(192, 140, 0.45, LIGHTRED, "RocketRobz");
	Gui::Draw_Rect(38, 155, 400-42, 1, LIGHTRED);
	Gui::DrawString(40, 198, 0.50, LIGHTRED, "2020 RocketRobz");
	Gui::Draw_Rect(38, 214, 400-42, 1, LIGHTRED);
	Gui::DrawString(40, 214, 0.45, LIGHTRED, "robz designs on the future <<<");*/
}


void ProductIdent::Logic(u32 hDown, u32 hHeld, touchPosition touch) {
	if (!graphicLoaded) {
		u16* bgLoc = new u16[256*192];
		extern int bg2Main;
		extern int bg3Main;
		extern void applyColorLutBitmap(u16 *palette, int size);

		std::vector<unsigned char> image;
		unsigned width, height;
		lodepng::decode(image, width, height, dsiFeatures() ? "nitro:/graphics/logos/productIdentI.png" : "nitro:/graphics/logos/productIdent.png");
		for(unsigned i=0;i<image.size()/4;i++) {
			bgLoc[i] = image[i*4]>>3 | (image[(i*4)+1]>>3)<<5 | (image[(i*4)+2]>>3)<<10 | BIT(15);
		}
		dmaCopyHalfWordsAsynch(0, bgLoc, bgGetGfxPtr(bg2Main), 0x18000);
		if (dsiFeatures()) {
			dmaCopyHalfWordsAsynch(1, bgLoc, bgGetGfxPtr(bg3Main), 0x18000);
		} else {
			while (dmaBusy(0));
			delete[] bgLoc;
		}
		applyColorLutBitmap(bgGetGfxPtr(bg2Main), 0x18000/sizeof(u16));
		if (dsiFeatures()) {
			applyColorLutBitmap(bgGetGfxPtr(bg3Main), 0x18000/sizeof(u16));
		}
		graphicLoaded = true;
	}
}