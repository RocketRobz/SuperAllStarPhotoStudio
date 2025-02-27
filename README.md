<p align="center">
 <img src="https://github.com/RocketRobz/SuperAllStarPhotoStudio/blob/master/resources/title.png"><br>
	<a href="https://gbatemp.net/threads/dsi-3ds-super-photo-studio-take-pictures-of-your-favorite-all-star-characters.573276/" style="padding-left: 5px;">
		<img src="https://img.shields.io/badge/GBATemp-thread-blue.svg" height="20">
	</a>
</p>

What happens when you take the concept of *Super Smash Bros.*, and turn it from a fighting game, into a photo shoot party?     
The result, a *Super All-Star Photo Studio*! Take pictures of your favorite all-star characters, in any location possible!

*Super All-Star Photo Studio* has it all: An all-star video game cast, well-known outdoor and indoor locations, stylish girls, the coolest of TWL himself, and fan-favorites like Mario & Luigi, Sonic & Tails, Banjo & Kazooie, and even Waluigi!

# What is this?

*Super All-Star Photo Studio* is basically an upgraded version of the *Style Savvy* series's photo studio feature, with added characters and locations outside of said series. You can also change the character's expressions and poses, though *Super All-Star Photo Studio* does not support changing expressions.
- As of currently, only Grace (from *Style Savvy*) has changeable poses.

# Features

- High-resolution *Horizon HD* (800px wide mode), exclusive to 3DS, New 3DS, and New 2DS consoles!
- Pre-rendered characters and backgrounds, as well as music, from     
  - Super Mario     
  - Sonic the Hedgehog     
  - Pac-Man     
  - Banjo-Kazooie        
  - Style Savvy     
  and more!
- Around/Nearly 100 characters and 50 locations.
- The first homebrew appearance of Robz' fashionable twin, Rabz!

# Adding custom characters

- **DS(i):** `.png` files go in `sdmc:/_nds/SuperPhotoStudio/characters/` ([Example here](https://github.com/RocketRobz/SuperAllStarPhotoStudio/blob/master/nds/nitrofiles/graphics/char/ss4_Robz0.png))
- **3DS:** `.t3x` files go in `sdmc:/3ds/SuperPhotoStudio/characters/` (Made from three `.png` files, examples here: [zoom0](https://github.com/RocketRobz/SuperAllStarPhotoStudio/blob/master/3ds/assets/gfx_chars/characters/Rocket%20Photo%20Shoot/Spring/zoom0/Robz.png), [zoom1](https://github.com/RocketRobz/SuperAllStarPhotoStudio/blob/master/3ds/assets/gfx_chars/characters/Rocket%20Photo%20Shoot/Spring/zoom1/Robz.png), [zoom2](https://github.com/RocketRobz/SuperAllStarPhotoStudio/blob/master/3ds/assets/gfx_chars/characters/Rocket%20Photo%20Shoot/Spring/zoom2/Robz.png))

Info about the character(s) go in the `characters` folder, and must be named something like `NameOfChar.ini` for each character.

### `NameOfChar.ini` example

Male (Blue tie icon):

```
[Character]
Gender = Male
```

Female (Pink bow icon):

```
[Character]
Gender = Female
```

# Controls
- A: Select highlighted option
- D-Pad Left/Right: Select character number (highlight character option in menu first)
- X/B: Zoom in/out (DS(i) version)
- Circle Pad Up/Down: Zoom in/out (3DS version)
- Y: Take photo (saved in `sdmc:/(yourconsole)/SuperPhotoStudio/photos/`)

# Characters, Locations, and Music by
- Namco
- Nicalis
- NightScript*
- Nintendo
- Rare
- Rocket Robz*
- SEGA
- StackZ*
- Syn Sophia

(*Homebrew dev)

# Available for
- Nintendo DS
- Nintendo DS⁽ⁱ⁾
- Nintendo 3DS/2DS

# Credits
- devkitPro: libnds, libctru, citro2d/3d, nds-hb-menu's file/folder browsing code
- [Universal-Team](https://github.com/Universal-Team): Universal Core
- [SuperSaiyajinStackZ](https://github.com/SuperSaiyajinStackZ): StackZ and Stackie avatars
     - StackZ avatar made by Rocket Robz
- [NightScript](https://github.com/NightScript370): NightScript avatar (made by Rocket Robz)
- [Church of Kondo](https://www.youtube.com/@ChurchofKondoh): Some included remastered music
- [Pk11](https://github.com/Epicpkmn11): DSi version's font rendering code
- [chyyran](https://github.com/chyyran): DS(i) version's alpha blending and sound streaming code
- [joel16](https://github.com/joel16): Screenshot code from [3DShell](https://github.com/joel16/3DShell)
- Freepik: [Camera icon](https://www.flaticon.com/free-icon/camera_2965705?term=camera&page=1&position=12)
