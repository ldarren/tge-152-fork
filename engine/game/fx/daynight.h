


#ifndef _TGEDAYNIGHT_H_
#define _TGEDAYNIGHT_H_

/* 
tgeDayNight - A fast Day and Night cycles for the Torque Game Engine .. in more ways then one :)

Joshua Ritter - joshua@actionrpg.com - www.actionrpg.com 

This version of the day and night stuff is released as a patch against the GG CVS HEAD (12-05-02)
The changes to the core engine are contained in that patch

The interior rendering code CURRENTLY REQUIRES 3 TMU UNITS ON YOUR VIDEO CARD!  If you have 2 it will look
like poo.,, if you get to this before I do, please post the code... :)

For controlling speed of Day/Night see the sfactor variable in void tgeDNSetTime(float hour,float minute)

*/

void tgeDNSetTime(float hour,float minute=0.0f);
void tgeDNGLEnableLighting(float emissiveScale=1.0f);
void tgeDNGLDisableLighting();
void tgeDNTriggerLightning();

extern Point3F tgeDNCurrentColor;
extern 	F32 tgeDNHourofDay;
extern 	F32 tgeDNMinuteofDay;

//modifies skybox bands
extern 	F32 tgeDNBandMod;
extern Point3F tgeDNCurrentBandColor;

class Sun;
class fxSunLight;

extern Sun* tgeDNSun;
extern Sun* tgeDNServerSun;
extern Point3F tgeDNSunVector;

extern F32 tgeDNCloudCover;

extern Point3F tgeDNMoonPos;

#endif
