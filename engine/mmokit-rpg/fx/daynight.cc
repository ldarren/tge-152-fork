
/* 
tgeDayNight - a fast Day and Night cycles for the Torque Game Engine in more ways than one

Joshua Ritter - joshua@actionrpg.com - www.actionrpg.com 

This version of the day and night stuff is released as a GNU patch against the GG CVS HEAD (12-05-02)
The changes to the core engine are contained in the patch

The interior rendering code CURRENTLY REQUIRES 3 TMU UNITS ON YOUR VIDEO CARD!  If you have 2 it will look
like poo.,, if you get to this before I do, please post the code... :)

For controlling speed of Day/Night see the sfactor variable in void tgeDNSetTime(float hour,float minute)

*/


#include "math/mMath.h"
#include "core/color.h"
#include "dgl/dgl.h"
#include "core/tVector.h"
#include "terrain/sun.h"
#include "terrain/sky.h"
#include "lightingSystem/sgLightManager.h"
#include "sceneGraph/sceneGraph.h"
#include "game/gameConnection.h"

#include "game/fx/fxSunLight.h"

#include "math/mathUtils.h"

#include "math/mRandom.h"

struct tgeDNColorTarget
{
	F32			time;  //hour*60+minute
	Point3F		color; //normalized 0 = 1.0 ... 
	F32         bandMod;  //6 is max
	Point3F     bandColor;
};

Vector<tgeDNColorTarget> tgeDNColorTargets;
Point3F tgeDNMoonPos;


F32		tgeDNHourofDay=12.0f;
F32		tgeDNMinuteofDay=0.0f;

Point3F tgeDNCurrentColor;
Point3F tgeDNSunVector;

F32		tgeDNBandMod=1.0f;
Point3F tgeDNCurrentBandColor;


//temp hack
bool tgeDayNightInit=true;

double tgeDNLastTick=-1.0;

Sun* tgeDNSun =NULL;
Sun* tgeDNServerSun =NULL;

float tgeDNDayPhase=0.0f;

F32 tgeDNCloudCover=0.f;

//36
char* tgeDNLightningString = "mzpizrztmkmgzmtmsmvmzitixizgjhkumkgz";
float tgeDNLightningValues[36];
float tgeDNLightningScalar = 0.f;
float tgeDNLightningTimer = 0.f;




static MRandomLCG RandomGen;
  



//TGE's rendering code is sprinkled all over the place... bad for doing global operations such as 
//day and night cycles... will keep this as small as possible... but there are special cases



//!!!MAKE SURE TARGETS ARE ADDED TIME OF DAY ORDERED!!!  
//This function get's needlessly complex otherwise
//hour is in military time ... first add should be 0.0 last 24.0 ,,, colors should
//match at these times
void tgeDNAddColorTarget(float hour, float minute, Point3F& color, float bandMod, Point3F& bandColor)
{

	mClampF(hour,0.0f,24.0f);
	mClampF(minute,0.0f,60.0f);
	if (hour==24.0f)
		minute=0.0f;


	tgeDNColorTarget  newTarget;
	float time=hour*60+minute;

	newTarget.time=time;
	newTarget.color=color;
	newTarget.bandMod=bandMod;
	newTarget.bandColor=bandColor;

	tgeDNColorTargets.push_back(newTarget);
}

void tgeDNGLEnableLighting(float emissiveScale)
{

    GLfloat matProp[] = {1.0f,1.0f,1.0f,1.0};
    GLfloat zeroColor[] = {0,0,0,1.0};
    GLfloat tgecolor[4];


	tgecolor[0]=mClampF(tgeDNCurrentColor.x*emissiveScale,0.0f,1.0f);
	tgecolor[1]=mClampF(tgeDNCurrentColor.y*emissiveScale,0.0f,1.0f);
	tgecolor[2]=mClampF(tgeDNCurrentColor.z*emissiveScale,0.0f,1.0f);
	tgecolor[3]=1.0f;

			   
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   

   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,matProp);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,matProp);
	glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,tgecolor);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,zeroColor);

	glEnable(GL_LIGHTING);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, zeroColor);

	glNormal3f(0.0,0.0,1.0);

}

void tgeDNGLDisableLighting()
{
	GLfloat zeroColor[] = {0,0,0,0};
    glDisable(GL_LIGHTING);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,zeroColor);
}

//if hour and minute are not specified we use the systems
void tgeDNGetColor(Point3F& color,float hour=-1.0f,float minute=-1.0f)
{
	if (hour==-1.0f)
		hour=tgeDNHourofDay;
	if (minute==-1.0f)
		minute=tgeDNMinuteofDay;

	mClampF(hour,0.0f,24.0f);
	mClampF(minute,0.0f,60.0f);
	if (hour==24.0f)
		minute=0.0f;


	tgeDNColorTarget* ct=NULL;

	float time=hour*60.0f+minute;
	float phase=-1.0f;
	float div;


	if (!tgeDNColorTargets.size())
	{
		color.set(1.0f,1.0f,1.0f);
		return;
	}

	if (tgeDNColorTargets.size()==1)
	{
		ct=&tgeDNColorTargets[0];
		color.set(ct->color.x,ct->color.y,ct->color.z);
		return;
	}

	//simple check

	if (tgeDNColorTargets[0].time!=0.0f)
	{
		AssertFatal(0,"tgeDNGetColor() - First Time Must Be 0:00")
		color.set(1.0f,1.0f,1.0f);
		tgeDNBandMod=1.0f;
		tgeDNCurrentBandColor=color;

		return;
	}
	if (tgeDNColorTargets[tgeDNColorTargets.size()-1].time!=(24.0f*60.0f))
	{
		AssertFatal(0,"tgeDNGetColor() - Last Time Must Be 24:00")
		color.set(1.0f,1.0f,1.0f);
		tgeDNBandMod=1.0f;
		tgeDNCurrentBandColor=color;

		return;
	}


	//we need to find the phase and interp... also loop back around
	U32 count=0;
	for (;count<tgeDNColorTargets.size()-1;count++)
	{
		tgeDNColorTarget* one=&tgeDNColorTargets[count];
		tgeDNColorTarget* two=&tgeDNColorTargets[count+1];

		if (time>=one->time && time<=two->time)
		{
			div=two->time-one->time;
			//catch bad input divide by zero
			if (fabs(div)<.01)
				div=.01;
			
			phase=(time-one->time)/div;

			color.interpolate(one->color,two->color,phase);

			tgeDNCurrentBandColor.interpolate(one->bandColor,two->bandColor,phase);

			tgeDNBandMod = one->bandMod * (1.0f - phase) + two->bandMod * phase;

			//we need to scale up band calc for sky... they are set for a visible
			//distance of 500... hopefully linear works 
			Sky* s=(Sky*)Sim::findObject("Sky");
			float bscale=1.0;
			if (s)
			{
				bscale=s->getVisibleDistance()/500.0f;
			}

			tgeDNBandMod*=bscale;


			return;

		}
	}

	AssertFatal(0,"This isn't supposed to happen");

}

void tgeDNInit()
{
	Point3F c;
	Point3F bc;

   int i;
   for (i=0;i<36;i++)
      tgeDNLightningValues[i]=(float)(tgeDNLightningString[i] - 'a') / (float)('m' - 'a');


   RandomGen.setSeed(Platform::getVirtualMilliseconds()*10000);
	
	//MIDNIGHT
	c.set(.15,.15,.2);
	tgeDNAddColorTarget(0.0f, 0.0f, c,1.0f,c);
	c.set(.25,.25,.35);
	tgeDNAddColorTarget(1.0f, 0.0f, c,1.0f,c);
	c.set(.25,.25,.4);
	tgeDNAddColorTarget(2.0f, 0.0f, c,1.0f,c);
	c.set(.2,.2,.35);
	tgeDNAddColorTarget(3.0f, 0.0f, c,1.0f,c);
	c.set(.25,.25,.4);
	tgeDNAddColorTarget(4.0f, 0.0f, c,1.0f,c);
	c.set(.25,.25,.3);
	tgeDNAddColorTarget(5.0f, 0.0f, c,1.0f,c);


	//DAWN
	c.set(.4,.4,.35);
	bc.set(.9,.5,.4);
	tgeDNAddColorTarget(6.0f, 0.0f, c,.75f,bc);
	
	
	c.set(.45,.45,.4);
	bc.set(.9,.7,.4);
	tgeDNAddColorTarget(7.0f, 0.0f, c,1.0f,bc);
	c.set(.55,.55,.35);
	bc.set(.95,.8,.5);
	tgeDNAddColorTarget(8.0f, 0.0f, c,1.5f,bc);
	c.set(.6,.55,.35);
	bc.set(.95,.95,.6);
	tgeDNAddColorTarget(9.0f, 0.0f, c,2.5f,bc);
	

	//DAY
	c.set(.7,.65,.55);
	tgeDNAddColorTarget(10.0f, 0.0f, c,2.5f,c);
	c.set(.8,.8,.8);
	tgeDNAddColorTarget(11.0f, 0.0f, c,1.5f,c);
	c.set(.9,.9,.9);
	tgeDNAddColorTarget(12.0f, 0.0f, c,1.0f,c);
	c.set(1.0,1.0,1.0);
	tgeDNAddColorTarget(13.0f, 0.0f, c,1.0f,c);
	c.set(1.0,1.0,.9);
	tgeDNAddColorTarget(14.0f, 0.0f, c,1.0f,c);
	c.set(1.0,1.0,.9);
	tgeDNAddColorTarget(15.0f, 0.0f, c,1.0f,c);
	c.set(.9,.85,.85);
	tgeDNAddColorTarget(16.0f, 0.0f, c,1.0f,c);
	c.set(.8,.75,.75);
	tgeDNAddColorTarget(17.0f, 0.0f, c,1.0f,c);

	//DUSK
	c.set(.8,.75,.75);
	bc.set(.8,.6,.3);
	tgeDNAddColorTarget(18.0f, 0.0f, c,3.0f,bc);
	c.set(.75,.64,.5);
	bc.set(.75,.5,.4);
	tgeDNAddColorTarget(18.5f, 0.0f, c,2.75f,bc);
	c.set(.65,.55,.35);
	bc.set(.65,.3,.3);
	tgeDNAddColorTarget(19.0f, 0.0f, c,2.5f,bc);

	//NIGHT
	c.set(.5,.5,.5);
	bc.set(.7,.4,.2);
	tgeDNAddColorTarget(20.0f, 0.0f, c,1.25f,bc);
	c.set(.4,.4,.4);
	bc.set(.8,.3,.2);
	tgeDNAddColorTarget(21.0f, 0.0f, c,1.00f,bc);
	c.set(.35,.35,.3);
	tgeDNAddColorTarget(22.0f, 0.0f, c,1.0f,c);
	c.set(.2,.2,.35);
	tgeDNAddColorTarget(23.0f, 0.0f, c,1.0f,c);
	c.set(.15,.15,.2);
	tgeDNAddColorTarget(24.0f, 0.0f, c,1.0f,c);

}

void tgeDNTriggerLightning()
{
// lightning -> "mzpizrztmkmgzmtmsmvmzitixizgjhkumkgz");
   

   if (!RandomGen.randI(0,10))
   {
      tgeDNLightningTimer = RandomGen.randF(1,4);
      tgeDNLightningScalar = RandomGen.randF(.1f,1.f);
   }
   else
   {
      tgeDNLightningTimer = RandomGen.randF(1,2);
      tgeDNLightningScalar = RandomGen.randF(.1f,.5f);
   }



}

//call this per tick
void tgeDNSetTime(float hour,float minute)
{
	if (tgeDayNightInit)
	{
		tgeDayNightInit=false;
		tgeDNInit();

	}


	if (tgeDNLastTick<0.0)
		tgeDNLastTick=((double)(Platform::getVirtualMilliseconds()) * .001);

   //tgeDNHourofDay = 12;
	double delta= ((double)(Platform::getVirtualMilliseconds()) * .001) - tgeDNLastTick;

	tgeDNLastTick=((double)(Platform::getVirtualMilliseconds()) * .001);

	//sfactor controls the length of time for every game hour ... at 1.0 it is 2.5 minutes per game hour

   //1 game hour = 2.5 real minutes
   //1 game day =  1 real hour

	tgeDNMinuteofDay+=(delta*24.f)/60.f;

	if (tgeDNMinuteofDay>=60.0)
	{
		
		//dPrintf("The time is %f\n",tgeDNHourofDay);
		
		tgeDNHourofDay+=1.0;
		if (tgeDNHourofDay>=24.0)
			tgeDNHourofDay=0.0;

		//XXX this could still be over 60 if lagged or something?
		tgeDNMinuteofDay=tgeDNMinuteofDay-60.0;

	}


	mClampF(hour,0.0f,24.0f);
	mClampF(minute,0.0f,60.0f);
	if (hour==24.0f)
		minute=0.0f;

	//tgeDNHourofDay=hour;
	//tgeDNMinuteofDay=minute;
	
	

	tgeDNGetColor(tgeDNCurrentColor,tgeDNHourofDay,tgeDNMinuteofDay);

   tgeDNCurrentColor.x = mClampF(tgeDNCurrentColor.x,.4f,1.0f);
   tgeDNCurrentColor.y = mClampF(tgeDNCurrentColor.y,.4f,1.0f);
   tgeDNCurrentColor.z = mClampF(tgeDNCurrentColor.z,.4f,1.0f);


   F32 cover = 1.0f - (tgeDNCloudCover*.10f);
   tgeDNCurrentColor*=cover;   
   tgeDNCurrentBandColor*=cover;

   if (tgeDNLightningTimer> 0.f)
   {   
      tgeDNLightningTimer -= delta;
      S32 ofs = Platform::getVirtualMilliseconds()*.01f;
      F32 lightning = tgeDNLightningValues[ofs%36]*tgeDNLightningScalar;

      tgeDNCurrentColor[0]+=lightning;
      tgeDNCurrentColor[1]+=lightning;
      tgeDNCurrentColor[2]+=lightning;

      tgeDNCurrentBandColor[0]+=lightning;
      tgeDNCurrentBandColor[1]+=lightning;
      tgeDNCurrentBandColor[2]+=lightning;
   
      tgeDNCurrentColor[0] = mClampF(tgeDNCurrentColor[0],0,1);
      tgeDNCurrentColor[1] = mClampF(tgeDNCurrentColor[0],0,1);
      tgeDNCurrentColor[2] = mClampF(tgeDNCurrentColor[0],0,1);
      
      tgeDNCurrentBandColor[0] = mClampF(tgeDNCurrentBandColor[0],0,1);
      tgeDNCurrentBandColor[1] = mClampF(tgeDNCurrentBandColor[0],0,1);
      tgeDNCurrentBandColor[2] = mClampF(tgeDNCurrentBandColor[0],0,1);
   }



	tgeDNDayPhase=(tgeDNHourofDay*60.0f+tgeDNMinuteofDay)/(24.0f*60.0f);

//	float phase=1.0f;
//	if (tgeDNHourofDay>=6.0f && tgeDNHourofDay<=18.5f )
//		phase = ((tgeDNHourofDay*60+tgeDNMinuteofDay)-(6.0f*60.0f))/((18.5f*60.0f)-(6.0f*60.0f));



	tgeDNSunVector.set(0.57f,0.57f,-0.57f); //default

   fxSunLight *tgeDNSunFlare = dynamic_cast<fxSunLight*>(Sim::findObject("sunflare"));
   
   
	if (tgeDNSunFlare)
	{
		//6am Sunrise 6pm sunset -90 - 90
		if (tgeDNHourofDay<4.0f || tgeDNHourofDay>20.5f )
		{
			//disable
			
			tgeDNSunFlare->mEnable=false;
		}
		else
		{

			tgeDNSunFlare->mEnable=true;
			
		}

		tgeDNSunFlare->mAnimationElevation=false;
		//fudge the sun so it set's a bit later // -90.0 is setting at 6pm
		tgeDNSunFlare->mSunElevation=-110.0+(tgeDNDayPhase*360.0f);
			
		tgeDNSunFlare->mAnimationAzimuth=false;
		tgeDNSunFlare->mSunAzimuth=90.f;
		tgeDNSunFlare->mMinAzimuth=-360;
		tgeDNSunFlare->mMaxAzimuth=360;
	//	tgeDNSunFlare->mAzimuthTime=1875.0f/sfactor;

		float shadphase=tgeDNDayPhase;

		if (shadphase>.75f)
			shadphase=.75f - (shadphase-.75f);

		if (shadphase<.25)
			shadphase=.5f - shadphase; 
			


		
		if (shadphase>=.25f && shadphase<=.35f)
		{
			shadphase=.35f;
		}

		if (shadphase>=.65f && shadphase<=.75f)
		{
			shadphase=.65f;
		}

		
		float azi=mDegToRad(shadphase*80.0f-90.f);
		//this is adjusted so shadow doesn't get too long... NOT SAME CALC AS SUN FLARE
		float ele=mDegToRad(-90.0+(shadphase*360.0f));

		MathUtils::getVectorFromAngles(tgeDNSunVector,azi,ele);
		//tgeDNSunVector.neg();
      tgeDNSunVector[2]=-tgeDNSunVector[2];




	}

	if (tgeDNSunFlare)
	{
      
		//6am Sunrise 6pm sunset -90 - 90
      /*
		if (tgeDNHourofDay>4.0f || tgeDNHourofDay>20.5f )
		{
			//disable
			
			tgeDNTheMoon->mEnable=false;
		}
		else
		{

			tgeDNTheMoon->mEnable=true;
			
		}
      */


		
		
		F32  mSunElevation=70+(tgeDNDayPhase*360.0f);
      F32  mMinElevation=-360;
      F32 mMaxElevation=360;
			
		
		F32 mSunAzimuth=-90.f;//(tgeDNDayPhase*80.0f)+90;
		F32 mMinAzimuth=-360;
		F32 mMaxAzimuth=360;
//		F32 mAzimuthTime=100.0f/sfactor;

	   // Calculate Sun Vector.
	   Point3F SunlightOffset;

	   // Fetch Sun Azimuth/Elevation (animated if using it).
	   F32 mTempAzimuth = mSunAzimuth;
	   F32 mTempElevation = mSunElevation;


	   // Create clamped Yaw/Patch and calculate sun direction.
	   F32		Yaw = mDegToRad(mTempAzimuth+90);
	   F32		Pitch = mDegToRad(mTempElevation);
	   MathUtils::getVectorFromAngles(SunlightOffset, Yaw, Pitch);

	   // Get Control Camera Eye Position.
	   Point3F eyePos;
	   MatrixF eye;
	   GameConnection* conn = GameConnection::getConnectionToServer();
      if (conn)
      {
	      conn->getControlCameraTransform(0, &eye);
	      eye.getColumn(3, &eyePos);

	      // Calculate new Sun Position.
	      F32 Radius = gClientSceneGraph->getVisibleDistance() * 0.999f;
	      tgeDNMoonPos =  (Radius * SunlightOffset);

      }

   }
	

	

	if (tgeDNSun)
	{
		
#ifdef DARREN_MMO		
		
		tgeDNSun->mLight.mDirection=tgeDNSunVector;


	  
		tgeDNSun->mLight.mColor.red=tgeDNCurrentColor.x;
		tgeDNSun->mLight.mColor.green=tgeDNCurrentColor.y;
		tgeDNSun->mLight.mColor.blue=tgeDNCurrentColor.z;

		//too low of ambient makes environment maps on shapes ugly
		//check that lighting is affecting these...
		tgeDNSun->mLight.mAmbient.red=tgeDNCurrentColor.x*.375f;
		tgeDNSun->mLight.mAmbient.green=tgeDNCurrentColor.y*.375f;
		tgeDNSun->mLight.mAmbient.blue=tgeDNCurrentColor.z*.375f;
#endif //DARREN_MMO
	}	


}


ConsoleFunction( TGEDayNightSyncTime, void, 3, 3, "TGEDayNightSyncTime()" )
{
   argc;

   dPrintf("was-> %f:%f\n",tgeDNHourofDay,tgeDNMinuteofDay);

   tgeDNHourofDay=dAtof(argv[1]);
   tgeDNMinuteofDay=dAtof(argv[2]);

   dPrintf("now -> %f:%f\n",tgeDNHourofDay,tgeDNMinuteofDay);
   tgeDNLastTick=((double)(Platform::getVirtualMilliseconds()) * .001);
}


