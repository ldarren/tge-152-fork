Readme for Content Packs
August 13th, 2004
(C) 2004 Timothy Aste
http://www.gremlinstudios.com

Distributed by Garage Games
http://www.garagegames.com

Please read the licensing agreement and special notes before using this content, thanks.
Always back up your work!

-------------------------------------

Table of Contents

X.1 - Version History
X.2 - Quick Setup Guide
1 - Introduction
2 - About the packs
3 - Special Notes**
4 - Help! It doesn't work!
5 - Credits & Special Thanks

-------------------------------------

x.1
-Added collision meshes to rock objects in Environment Pack
-Added collision meshes to deadtree objects in Environment Pack
-Updated Documenation
-Included .MAX 5.0 files for all DTS shapes
-Fixed a typo in Rpgville1.mis

x.2
-Quick Setup Guide
Just want to pop this stuff in Torque and play? Well this is what you have to do:
	
	I am using Starter.FPS as the demo I'm working with, from Torque 1.3

	1- When you open the Content pack, you will see a subdirectory called "textures"

	2- Copy the entire contents of "textures" into "../yourtorque/starter.fps/data/interiors/"
	   Keep in mind it's important that they stay in the high level directory "interiors"

	3- Create a new subdirectory inside "interiors" for the content packs contents named after 
	   the content pack, for example "buildings" or "castles" or "misc." depending on what your 
	   copying and what your preferences for labeling things are.

	4- Go back to the content pack directory and locate the subdirectory labeled "compiled" or 
	   "precompiled".

	5- Copy the entire contents of the "compiled"/"precompiled" directory over to the new 
	   subdirectory you just made in "interiors". For example, if I were using the building pack, 
	   I would copy all the files from "Building_Pack/Compiled/" to 
	   "../yourtorque/Starter.FPS/interiors/buildings", and /buildings being the name of the 
	   direcotry you just made in step 3.

	6- Go back to step 1 & repeat this process for each content pack you have. When coping new 
	   textures, you may be asked to over-write some textures, this is ok because there is a little
	   overlap in some textures between packs.

	7- If your newer to Torque, **PLEASE** read the Special Notes & Help it Doesn't work sections
	   of this readme file. This will explain the following: Common problems & fixes, how to make 
	   the windmill blades animate, how to set up environment effects, how to piece together 
	   multiple piece buildings and where to locate prearranged multiple piece buildings that you 
	   can just copy & paste into your mission file, and other misc. stuff. PLEASE read this.


1 ------ Introduction: A note from the designer
-----------------------------------------------------

This content pack was designed as part of a whole interchanging system of packs that work extremely  
well with each other for the best results. To read more about the other packs available in this  
series, please check out http://www.garagegames.com

The content packs started as a personal challenge (and practice of course) in late June of 2004. I  
wanted to see if I could design a whole town, so I just sat down and began chipping away at it  
learning the ins and out's of Torque. After posting some of my playing around up on  
Garagegames.com I received an overwhelming response and demand for high quality assets similar to  
what I had been "playing" around with and I was quickly contacted by Josh Williams of GarageGames  
and we laid down a roadmap for a content pack plan which over the course of a month became content  
"packs". This is my first attempt at a professional product so please give me a slack, and feel free 
to email me criticisms or problems and I will try to respond as best and fast as I can. I would of given
 these away for free, but I think GG would of murdered me! :)

The assets in the content pack are made to be used in a variety of game genres from RPG, to FPS, to  
even RTS. Much, much time went into detailing each and every asset to work across as many mediums as  
possible while providing as much variety as possible without losing their connection to each other.  
I hope you enjoy these packs as much as I've enjoyed the journey of making them from their random  
beginning to their fantastic end. Please feel free to drop me a line at tim@gremlinstudios.com and  
show off how you are using them!

2 ------ About the Packs
-----------------------------------------------------

Buildings Pack
--------------
This content pack includes 27 high quality buildings of all shapes and sizes.
Every building comes portaled and with 3-6 levels of detail to optimize frame rates on all systems.  
No expense was spared in decorating the interiors or exteriors. There are enough buildings here to  
put together a whole small town, enjoy! 

- 2 Bars
- 2 Large Houses
- 5 Medium Houses
- 5 Small Houses
- 3 Medium Shops
- 3 Small Shops
- 3 Taverns
- 1 Church
---1 Altar
---1 Pew
- 1 Windmill
---1 Animated Windmill Fan

Castle Pack
-----------
This content pack includes 69 high quality modular infrastructure units of all shapes and sizes to  
build larger complex systems. Every object comes with 3-6 levels of detail to optimize frame rates on  
all systems. Every prop here was design to be used as a modular unit to piece together to create  
large complex structures of any shape and size to fit what you need!

- 1 Four-Piece Castle Set
- 3 Road pieces
- 2 Stone Walls
- 2 Metal Walls
- 4 Palisade Walls
- 2 Two story Corners
- 2 Two Story City Walls
- 1 One Story Corner
- 3 One Story City Walls
- 2 Gates
- 7 Piece Aqua duct System
- 4 Foot Bridges
- 1 Rope Bridge
- 1 Stone Bridge
- 2 Large Signs
- 4 Medium Signs
- 3 Small Signs
- 3 Piece Skywalk System
- 9 Staircases
- 3 Watch Towers
- 3 Wells
- 4 Columns


Outside Accessories Pack
------------------------
This content pack includes 57 high quality props of all shapes and sizes. Every object comes with  
3-6 levels of detail to optimize frame rates on all systems. Use the props in-game to decorate your  
town with flair or take the source .maps and hardcode them as prefabs into any project to give it  
that special touch!

- 4 Banners
- 1 Large Town bell
- 3 Carts
- 2 Clotheslines
- 2 Market Stands
- 1 Construction Site Kit
---3 Bases
---2 Planks
---2 Scaffolding Pieces
---3 Poles
- 6 Fences
- 1 Firepit
- 1 Gallows
- 1 Hitching Post
- 3 Outdoor Lanterns
- 1 Outdoor Stove
- 3 Shacks
- 1 Smithy Kit
---1 Anvil
---1 Forge
- 1 Tent
- 11 Piece Tombstone Kit
- 1 Trough
- 1 Waterbased Kit
---1 Rowboat
---1 Oar
---1 Coiled Rope
---1 Large Dock


Furniture Pack ETA: November 2004
-------------
This content pack includes 36 high quality props of all shapes and sizes. Every object comes with  
3-6 levels of detail to optimize frame rates on all systems. Use the props to decorate lavish  
interiors in-game or take the source art and import them into your map files to hard code them in as 
prefabs! This list is not final and is likely to grow and change!

- 5 Benches
- 1 Bucket
- 3 Bottles
- 1 Wine Rack
- 3 Cupboards
- 2 Bookcases
- 1 Keg
- 3 Ladders
- 5 Painting
- 1 Tapestry
- 1 Plant Pot
- 7 Tables
- 1 Tailor Kit
---1 Loom
---1 Spinning Wheel
1 - Training Bag


Environment Pack
----------------
This pack contains three distinct new environmental settings that provide just the kick-start you  
need to a great looking demo or game. Optimized for Torque, this contains great new environmental  
settings, terrains, skies, and environmental objects.


- 3 New Precipitation Environments (Sandstorm, Snowstorm & Improved Heavy Rain)
- 3 Terrain Settings (Snow land, Wasteland & Grassland)
- 2 New Skyboxes (Apocalypse & Stormy Sky)
- 3 New Grass Files
- 5 Bonus Detail Textures
- 3 Storm Ambiences
- 3 Dead Trees
- 5 Rocks
- 4 Trees

Free Stuff
----------
Tons of content was created for the use of the content packs, but not all of it got used! We had to  
cut some things and had some excess textures but instead of wasting them we are providing them free  
of charge back to the community royalty-free to be put to good use in a good home. Be sure to get  
this free download from http://www.garagegames.com

- 61 Bonus Textures
- 1 Tavern Ambience
- 3 Dynamic Music Cut Songs

3 ------ Special notes
-----------------------------------------------------
Please make backups before overwriting or changing any files.


1. Windmill Fan Blades
========================
In the building pack, the Windmill's Fan blades are animated to spin and are a .DTS object (with a   
.3DS source for non Torque users). If you are going to be using these fan blades in Torque, you will  
need to incorporate the script filed used to animate the object. The script file is located with the  
fan source and is called "fan.cs". Copy this file into your "./torque/demo/server/scripts/..."  
directory. Next, you will need to call this file from "game.cs" located in the same directory. Open  
up "game.cs" and add the line: " exec("./fan.cs");  " to the existing exec commands found at the  
start of the file. Save and now your fan should spin when added to the world! If you run into  
problems, please be sure to check your syntax is right (no extra characters anywhere) and that your  
paths are pointing in the right direction first.

2. Multiple Piece Buildings
========================
For Torque users, due to the outdated limitations on the current map2dif build, some objects that  
are highly detailed or particularly large had to be built and split up into separate pieces in order  
for them to work crash free or compile. These objects are labeled as such and must be easily  
rearranged within Torque's Mission Editor in order to be a complete unit. Some objects were also  
split up to allow flexibility in arrange the design. To see how a multiple piece object is put  
together, please open up the respective demo mission included with each pack. In the castle pack,  
many infrastructure objects are units that are made to be put together to create complex systems.  
For example, the city wall set has multiple walls and jointing corners so you can create a city wall  
 to conform to any shape you need all right within Torque itself with little performance hit! Again,  
please check the demo missions to see how these systems are used.

3. Zones inside Zones kill your framerate! ** Important
========================
In the Torque Engine, having interiors inside interiors can cause problems when done in mass amounts  
because of zoning problems and anomalies that can occur. The engine sometimes tries to create a  
unique zone for each DIF objects and this means that by placing the DIF based example furniture  
inside a building within Torque itself could lead to being a real big performance killer. The  
optimal way to utilize the furniture props is to take them source files and copy the source art  
directly into whatever building you wish to have it within and compile it that way with furniture  
included. If that's confusing, what that basically means is using DIF Furniture objects inside DIF  
buildings in large amounts is very bad for performance. The DIF examples were included solely for  
the demo and are not meant to be placed within DIF buildings!

4. Environment Pack Scripting
========================
If your using Torque and wish to take advantage of the Environment pack, you will need to incorporate  
the new enviroment.cs file which includes the settings for the precipitation effects. You will need  
to be using a build of Torque that included the New Precipitation code implemented in June, 2004. Be  
sure to backup any and all files before changing or overwriting the current files. To add these  
settings, copy enviroment.cs into "./torque/server/scripts/...". For advanced users, you can simply  
append the current enviroment.cs with the data located in the Environment Packs enviroment.cs. Also,  
be sure that game.cs is making a call to enviroment.cs (see Special Note #1 for info on how to do  
this).

5. Locating pre-built DIF & DTS files in your new pack
========================
Pre-built versions of every asset that requires some sort of compiling or exporting are located for  
your convenience in the "./precompiled/" directory located within every content pack. 

6. MAP files
========================
After much deliberation, arguing, cursing, and sleepless nights the decision was made to restrict  
distribution of the brushed based Geometry to the universal .MAP format due to it's simple internal  
format and compatibility with the most platforms. The idea to distribute Quark & Hammer based  
formats were on the table because of their advanced ability, but it increased the distribution size  
and there were some issues concerning legalities and licensing. All 1000+ map files have been tested  
to be able to import into Hammer 3.5, Quark 6.4 Alpha, Q3Radiant, and Conitec Game Studio A6 WED.  
There is currently an issue with Game Studio WED being able to import the .map files because of a  
.map formatting inconsistency, but this can be easily worked around but opening the .map file within  
Hammer, Quark, or Radiant and re-exporting/saving the .map. 

7. Texture Size ** Important
========================
In the packs, I have all the default texture sizes at 512x512 at their MAXIMUM texture size. Depending
 on your system you will more than likely have to scale textures down that are being used depending on
 how much video ram you have available and what your target system is, please consider this! Without 
resizing these textures, you may have horribly slow frame-rates when you really start to get many many
 textures on the screen. I included all textures at their maximum recommended size so you as the 
end-user have this option on how you want to resize what textures and where so you can get the best 
results possible custom to your product or project.

8. Inside the buildings!
========================
Every building as a equally detailed interior, albeit a bit empty to allow for maximum furniture 
arrangement of your choice. Also, most interiors were lit on a custom lighting setup in a very specific 
(and dark) way, you will more than likely have to adjust the omni_lights inside the .map files to your 
custom setup depending on what mood you are trying to set, otherwise you may get very dark results!

9. Environment Pack Mission Files
================================
It's recommended you look and use the precipitation objects in the example mission files directory 
located in the environment pack base directory. Many of the weather effects shown in the demos and 
screenshots were created using these exact overlaying precipitation effects!

10. Castle Pack Mission File
================================
It's recommended you also look at the mission file included with the castle pack to see how the 4 
piece castle is set up according to it's design. 



4 ------ Help! It doesn't work!
-----------------------------------------------------

Q: I was able to load the Interiors into Torque, but they are all black? WTF is up with that?
A: You will have to relight the scene before you see textures on the exteriors of DIF files. To do 
this in stock Torque, open up the Mission Editor (Default F11) and press ALT+L. You can also save and
 then reload the mission if this feature is unavailable to you.\

===========

Q: Everything seems so dark/light, how can I fix this?
A: For the exteriors of buildings you will have to edit the Suns Values. You can do this by opening 
up the Sun in the Torque mission editor and goin from there. Color is the suns brightness value while 
ambient is the sun shadows and other ambient features. It's a good idea to keep a nice contracts
 between these too with your Sun Value at a medium level and the Shadows at a dark level. Note that
 putting these two values exactly the same will wash out the textures very badly.

As far as upping the interior lighting inside the interiors, you will have to unfortunately recompile
 the DIF itself using map2dif & your favorite torque mapping tool. Open the map itself (this will 
always be mapname_0.map, the _0 is the highest level of detail and the only level with an inside), 
then look for entitity's called omni_lights and adjust their values as you see fit.

Please keep in mind this content pack was designed on the Synapse Gaming Lighting Pack* and the way 
the lights look and work on various platforms and versions of Torque is liable to be very different. 
Thanks for understanding and please email me** if you want any advice or help on adjusting and 
recompiling maps.

*http://www.synapsegaming.com
**tima@garagegames.com

===========

Q: Help, when I try to load an interior or mission with an interior in it, torque quits to the main 
menu with a "resource not found" type error.
A: This error is more than likely from not being able to find the textures when trying to add the DIF
 files to the mision. Be sure to put your textures in a place where Torque can find them, the best 
place would be:

.../YourTorqueGame/data/interiors/

And be sure to have your DIF files *IN* that same directory, or within a sub-directory branching out 
of that directory such as:

../YourTorqueGame/data/interiors/walls/

===========

Q: When I add foliage, I get white blocks?
A: This is a similiar error as from the last question, be sure that all your paths are pointing to 
the correct files, as this error is caused from not finding a texture that is required.

===========

Q: The precipitation effects do not look like those in the movies, what am I doing wrong?
A: The precipitation effects done in the screenshots, demos, and movies were done by using a 
combination of both predefined effects in environment.cs USED IN CONJUNCTION WITH the Sky/Wind
 Settings & Mission Precipitation Datablock Settings. To copy these effects, open up the example
 mission files included with the environment pack and copy the datablocks specified with the 
precipitation effects & skybox over to your own mission file. Also be sure to link the environment.cs 
file in your TOrqueGame/Server/Scripts/ directory if you haven't already, more info on how to do this 
is located above in the readme.

===========

Q: Certain large buildings are in multiple pieces, how the heck do I get them to go together?
A: Due to constraints on MAP2DIF (the interior exporting tool) some buildings had to be split up 
into multiple pieces to export right. A big example of this would be the Castle in the Castle pack. 
These pieces needs to be rearranged inside of the Torque mission editor in order to look right. For 
the castle pack, a demo mission was included under CastlePack/Mission/ which includes a "pre-arranged"
 castle for you, so all you have to do is open the mission file in a text editor & copy the Interior
 Instance data blocks over to your own mission file and then open the mission file and move all the 
pieces (as a whole) to where you want the castle. Hopefully this will save you some time.

===========

Q: I have the combo pack and I put everything in one lump directory but now I can't find some find 
certain files, were they not included?
A: Chances are you happed to overwrite some files by accident. As is shown in the 
YourContentPack/Geometry/ directory the files themselves are split down into subdirectories and 
inside some of these subdirectories some files will be labeled as med1, med2 in shops and med1, med2 
in houses, etc. etc. The best way to avoid this is to simply keep the directory structure and not lump 
everything in the same directory. Please, always keep in mind that the textures being used have to be 
in the same directory or a directory higher, so it's always the safest bet to keep them in 
/data/interiors/ if your working out of that directory.

===========

Q: The grass looked fuller and thicker in the screenshots, why?
A: The screenshots were taken on a very high end system and the foliage count was upped to 14,000 on 
grass1 in rpgville1.mis. Feel free to experiment to find something that works for you.

===========

Q: Can I get a copy of the demo mission used in the videos?
A: Yes, it is now freely available at http://www.gremlinstudios.com/promo/demomission.zip
Please keep in mind this will be useless without owning all four content packs as it uses all four 
in combination.

===========

Q: I have a lot of buildings in now and it runs very sluggish!
A: There are two causes usually for this straight out of the box, 1) You need to resize your textures to
be much smaller as they are eating up VRAM very fast, please read the section on Resizing textures above 
in the special note sections. 2) The LOD's intial pop is too high for what your trying to do, you will have 
to recompile the buildings your using to have a faster initial LOD pop than it's default. Having 4 levels of
detail is great but it won't do you any good if you have 30 buildings onscreen and they don't skip down to 
the next level of detail at 1000 meters! :) 
Adjust LOD is an advanced Map Modelling Technique and in the interest of time I recommend browsing the Torque
Website resources section & Forums for how to do this. I am also available for email support by using the 
content pack product page support feature or emailing me at tim@garagegames.com and I will try to help as best
I can as fast as I can. Thanks.


5 ------ Credits & Special Thanks
-----------------------------------------------------

About Tim Aste:

Timothy Aste - (Artist, GarageGames.com, Inc.)
Tim Aste is a former freelance Graphic Designer educated at the Ringling School of Art and Design. 
Tim attracted the attention of GarageGames, Inc. with his 3-D and Level Design work and since has
 moved out to Eugene to work on the Torque Shader Engine and beyond. He recently was a speaker at 
Indie Games Con '04 and presented an updated Torque Shader Engine Demo which he recently designed.


Developed By:
--------------------------

Timothy Aste
http://www.gremlinstudios.com


GarageGames liaison:
--------------------------

Josh Williams


Demo Technology Developed By:
--------------------------

Matt Fairfax
Brave Tree Productions


Windmill Animation By:
--------------------------

Craig Fortune
http://www.craigfortune.com


Special Thanks:
--------------------------

GarageGames
http://www.garagegames.com
--Ben Garney
--Jay Moore
--Josh Williams

Brave Tree Productions:
http://www.bravetree.com
--Matt Fairfax
--Joe Maruschak

Burnt Wasp:
http://www.burntwasp.com/
--Tom Bampton
--Craig Fortune
--Nauris Krauze

The Render Engine:
--Brett Fattori
http://www.renderengine.com

Synapse Gaming
--John Kabus
http://www.synapsegaming.com
