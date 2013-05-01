//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
//-----------------------------------------------------------------------------
//					BASIC 2D Debris
//-----------------------------------------------------------------------------
datablock DebrisData(TwoDeeDebris) {
	render2D			= true;
	
	//GPGT - Note: Engine does not expand texture path on its own, we must do this.
	texture				= expandFilename("~/data/GPGTBase/shapes/Debris/2DDebrisTexture.png");
	//shapeFile			= "~/data/GPGTBase/shapes/Debris/3DDebrisShape.dts";

	lifetime			= 3.0;
	lifetimeVariance	= 0.5;

	elasticity = 0.6;
	friction = 0.5;
	numBounces = 5;
	bounceVariance = 3;

	useRadiusMass = true;
	baseRadius = 0.3;

	gravModifier = 0.5;

	terminalVelocity = 10;

	velocity = 5;

	velocityVariance = 0.5;

};

//-----------------------------------------------------------------------------
//					BASIC 3D Debris
//-----------------------------------------------------------------------------
datablock DebrisData(BasicThreeDeeDebris) {
	shapeFile			= "~/data/GPGTBase/shapes/Debris/3DDebrisShape.dts";
	lifetime			= 3.0;
	lifetimeVariance	= 0.5;

	elasticity			= 0.6;
	friction			= 0.5;
	numBounces			= 5;
	bounceVariance		= 3;

	useRadiusMass		= true;
	baseRadius			= 0.3;

	gravModifier		= 0.5;

	terminalVelocity	= 10;

	velocity			= 5;

	velocityVariance	= 0.5;

	fade = false;
	
};


//-----------------------------------------------------------------------------
//					Fading Debris
//-----------------------------------------------------------------------------
datablock DebrisData(Fading3DDebris : BasicThreeDeeDebris) {
	fade = true;
};


//-----------------------------------------------------------------------------
//					Fast Fire (rapid dispersal) Debris
//-----------------------------------------------------------------------------
datablock DebrisData(FastFireDebris : BasicThreeDeeDebris) {
	useRadiusMass		= true;
	baseRadius			= 1.5;

	gravModifier		= 1.5;

	terminalVelocity	= 20;

	velocity			= 15;

	velocityVariance	= 7.5;
};

//-----------------------------------------------------------------------------
//					Bouncy Debris
//-----------------------------------------------------------------------------
datablock DebrisData(BouncyDebris : BasicThreeDeeDebris) {
	elasticity			= 1.0;
	friction			= 0.0;
	gravModifier		= 1.5;
	terminalVelocity	= 20;

	numBounces			= 20;
	bounceVariance		= 0;
};

//-----------------------------------------------------------------------------
//					Sliding Debris
//-----------------------------------------------------------------------------
datablock DebrisData(SlidingDebris : BasicThreeDeeDebris) {

 shapeFile			= "~/data/GPGTBase/shapes/Debris/3DDebrisShapeBig.dts";

	numBounces			= 3;
	bounceVariance		= 1;
	friction			= 0.0;
	lifetime			= 8.0;
	lifetimeVariance	= 0.5;

	fade = true;
	};

//-----------------------------------------------------------------------------
//					Replace Debris (with Static)
//-----------------------------------------------------------------------------
datablock DebrisData(ReplaceWithStaticDebris : BasicThreeDeeDebris) {
	lifetime			= 15.0;
	lifetimeVariance	= 5.0;

	fade = true;


	snapOnMaxBounce = false;

	staticOnMaxBounce = true;
};


//-----------------------------------------------------------------------------
//					Flaming Debris
//-----------------------------------------------------------------------------
datablock ParticleData(PDDebrisFire : baseFirePD0 )
{
	dragCoeffiecient		= 100.0;
	gravityCoefficient		= 0;
	inheritedVelFactor		= 0.25;
	constantAcceleration	= 0.1;
	lifetimeMS				= 1500;
	lifetimeVarianceMS		= 300;
	useInvAlpha				= false;
	spinRandomMin			= -80.0;
	spinRandomMax			= 80.0;

	colors[0]				= "0.8 0.4 0 0.8";
	colors[1]				= "0.2 0.0 0 0.8";
	colors[2]				= "0.0 0.0 0.0 0.0";

	sizes[0]				= 0.2;
	sizes[1]				= 0.6;
	sizes[2]				= 0.1;

	times[0]				= 0.0;
	times[1]				= 0.5;
	times[2]				= 1.0;
};

datablock ParticleEmitterData(PEDDebrisFire)
{
	ejectionPeriodMS		= 10;
	periodVarianceMS		= 0;
	ejectionVelocity		= 0.8;
	velocityVariance		= 0.5;
	thetaMin				= 0.0;
	thetaMax				= 180.0;
	lifetimeMS				= 2500;
	particles				= "PDDebrisFire";
};



datablock DebrisData(ParticleTrailingDebris : BasicThreeDeeDebris) {
	velocity			= 25;
	velocityVariance	= 5;

	emitters[0] = "PEDDebrisFire";
};





//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//					HELPER EXPLOSION DATABLOCKS
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

datablock ExplosionData(BaseDebrisExplosion)
{
	debris					= BasicThreeDeeDebris;

	lifeTimeMS				= 100;
	
	debrisThetaMin			= 0;
	debrisThetaMax			= 10;
	debrisPhiMin			= 0;
	debrisPhiMax			= 360;
	debrisNum				= 10;
	debrisNumVariance		= 2;
	debrisVelocity			= 15;
	debrisVelocityVariance	= 0.0;
};


datablock ExplosionData(Basis2DDebrisExplosion : BaseDebrisExplosion) 
{
	debris = TwoDeeDebris;
};


datablock ExplosionData(Fading3DDebrisExplosion : BaseDebrisExplosion)
{
	debris = Fading3DDebris;
};


datablock ExplosionData(FastFireDebrisExplosion : BaseDebrisExplosion)
{
	debris = FastFireDebris;
};

datablock ExplosionData(BouncyDebrisExplosion : BaseDebrisExplosion)
{
	debris = BouncyDebris;
};

datablock ExplosionData(SlidingDebrisExplosion : BaseDebrisExplosion)
{
	debris = SlidingDebris;
};

datablock ExplosionData(ReplaceWithStaticDebrisExplosion : BaseDebrisExplosion)
{
	debris = ReplaceWithStaticDebris;
};

datablock ExplosionData(ParticleTrailingDebrisExplosion : BaseDebrisExplosion)
{
	debris = ParticleTrailingDebris;
};

