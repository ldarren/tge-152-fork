// ============================================================
// Project            :  tank3d
// File               :  .\server\particles.cs
// Copyright          :  
// Author             :  Mclelun
// Created on         :  Monday, May 21, 2007 10:52 AM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================


//-----------------------------------------------------------------------------

datablock ParticleData(ChimneySmoke)
{
   textureName          = "~/data/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.2;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 3000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.6 0.6 0.6 0.1";
   colors[1]     = "0.6 0.6 0.6 0.1";
   colors[2]     = "0.6 0.6 0.6 0.0";

   sizes[0]      = 0.5;
   sizes[1]      = 0.75;
   sizes[2]      = 1.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(ChimneySmokeEmitter)
{
   ejectionPeriodMS = 20;
   periodVarianceMS = 5;

   ejectionVelocity = 0.25;
   velocityVariance = 0.10;

   thetaMin         = 0.0;
   thetaMax         = 90.0;  

   particles = ChimneySmoke;
};

datablock ParticleEmitterNodeData(ChimneySmokeEmitterNode)
{
   timeMultiple = 1;
};

datablock ParticleData(CottageSmoke)
{
   textureName          = "~/data/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = 0.02;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 3000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.0 0.0 0.0 0.0";
   colors[1]     = "0.2 0.2 0.2 0.1";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 0.5;
   sizes[1]      = 0.75;
   sizes[2]      = 1.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

//-----------------------------------------------------------------------------

datablock ParticleData(ChimneyFire1)
{
   textureName          = "~/data/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.3;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 500;
   lifetimeVarianceMS   = 250;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.8 0.6 0.0 0.1";
   colors[1]     = "0.8 0.6 0.0 0.1";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 1.0;
   sizes[1]      = 1.0;
   sizes[2]      = 5.0;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleData(ChimneyFire2)
{
   textureName          = "~/data/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.5;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 800;
   lifetimeVarianceMS   = 150;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.6 0.6 0.0 0.1";
   colors[1]     = "0.6 0.6 0.0 0.1";
   colors[2]     = "0.0 0.0 0.0 0.0";

   sizes[0]      = 0.5;
   sizes[1]      = 0.5;
   sizes[2]      = 0.5;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(ChimneyFireEmitter)
{
   ejectionPeriodMS = 15;
   periodVarianceMS = 5;

   ejectionVelocity = 0.25;
   velocityVariance = 0.10;

   thetaMin         = 0.0;
   thetaMax         = 90.0;  

   particles = "ChimneyFire1" TAB "ChimneyFire2";
};

datablock ParticleEmitterNodeData(ChimneyFireEmitterNode)
{
   timeMultiple = 1;
};


// ----------------------------------------

datablock ParticleData(FliesParticle)
{
   textureName          = "~/data/particles/ember";
   dragCoefficient      = 0.0;
   windCoefficient      = 5.0;
   gravityCoefficient   = 0.0;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 8000;
   lifetimeVarianceMS   = 0;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;

   colors[0]     = "0 0 0 0 ";
   colors[1]     = "1 0 0 1";
   colors[2]     = "1 1 0 1";
   colors[3]     = "0 0 0 0";
   
   sizes[0]      = 0.0;
   sizes[1]      = 0.15;
   sizes[2]      = 0.2;
   sizes[3]      = 0.0;
   
   times[0]      = 0.0;
   times[1]      = 0.1;
   times[2]      = 0.5;
   times[3]      = 1.0;
   
};

datablock ParticleEmitterData(FliesEmitter)
{
   ejectionPeriodMS = 300;
   periodVarianceMS = 0;

   ejectionVelocity = 3;
   velocityVariance = 1.00;
   ejectionOffset = 1.0;

   thetaMin         = 75.0;
   thetaMax         = 90.0;
   
   phiReferenceVel  = 360.00;
   phiVariance      = 360.00;

   particles = "FliesParticle";
};

datablock ParticleEmitterNodeData(FliesNode)
{
   timeMultiple = 1;
};

// ----------------------------------------

datablock ParticleData(EmberParticle)
{
   textureName          = "~/data/particles/ember";
   dragCoefficient      = 0.0;
   windCoefficient      = 0.0;
   gravityCoefficient   = -0.05;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 5000;
   lifetimeVarianceMS   = 0;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;

   colors[0]     = "1.000000 0.800000 0.000000 0.800000";
   colors[1]     = "1.000000 0.700000 0.000000 0.800000";
   colors[2]     = "1.000000 0.000000 0.000000 0.200000";

   sizes[0]      = 0.05;
   sizes[1]      = 0.1;
   sizes[2]      = 0.05;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1.0;
};

datablock ParticleEmitterData(EmberEmitter)
{
   ejectionPeriodMS = 100;
   periodVarianceMS = 0;

   ejectionVelocity = 0.75;
   velocityVariance = 0.00;
   ejectionOffset = 2.0;

   thetaMin         = 1.0;
   thetaMax         = 100.0;  

   particles = "EmberParticle";
};

datablock ParticleEmitterNodeData(EmberNode)
{
   timeMultiple = 1;
};

// ----------------------------------------

datablock ParticleData(CampFireParticle)
{
   textureName          = "~/data/particles/smoke";
   dragCoefficient      = 0.0;
   windCoefficient      = 0.0;
   gravityCoefficient   = -0.05;   // rises slowly
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 5000;
   lifetimeVarianceMS   = 1000;
   useInvAlpha = false;
   spinRandomMin = -90.0;
   spinRandomMax = 90.0;
   spinSpeed = 1.0;

   colors[0]     = "0.2 0.2 0.0 0.2";
   colors[1]     = "0.6 0.2 0.0 0.2";
   colors[2]     = "0.4 0.0 0.0 0.1";
   colors[3]     = "0.1 0.04 0.0 0.3";

   sizes[0]      = 0.5;
   sizes[1]      = 4.0;
   sizes[2]      = 5.0;
   sizes[3]      = 6.0;

   times[0]      = 0.0;
   times[1]      = 0.1;
   times[2]      = 0.2;
   times[3]      = 0.3;
};

datablock ParticleEmitterData(CampFireEmitter)
{
   ejectionPeriodMS = 50;
   periodVarianceMS = 0;

   ejectionVelocity = 0.55;
   velocityVariance = 0.00;
   ejectionOffset = 1.0;
   

   thetaMin         = 1.0;
   thetaMax         = 100.0;  

   particles = "CampFireParticle";
};

datablock ParticleEmitterNodeData(CampFireNode)
{
   timeMultiple = 1;
};

//-----------------------------------------------------------------------------

datablock ParticleData(FlameSmoke)
{
   textureName          = "~/data/particles/smoke";
   dragCoefficient     = 0.0;
   gravityCoefficient   = -0.2;   // rises slowly
   windCoefficient      = 0;
   inheritedVelFactor   = 0.00;
   lifetimeMS           = 2000;
   lifetimeVarianceMS   = 250;
   useInvAlpha = false;
   spinRandomMin = -30.0;
   spinRandomMax = 30.0;

   colors[0]     = "0.6 0.6 0.6 0.1";
   colors[1]     = "0.6 0.6 0.6 0.1";
   colors[2]     = "0.6 0.6 0.6 0.0";

   sizes[0]      = 0.25;
   sizes[1]      = 0.5;
   sizes[2]      = 1.0;

   times[0]      = 0.0;
   times[1]      = 0.5;
   times[2]      = 1;
};

datablock ParticleEmitterData(FlameSmokeEmitter)
{
   ejectionPeriodMS = 40;
   periodVarianceMS = 5;

   ejectionVelocity = 0.25;
   velocityVariance = 0.10;

   thetaMin         = 0.0;
   thetaMax         = 90.0; 

   particles = FlameSmoke;
};

datablock ParticleEmitterNodeData(FlameSmokeEmitterNode)
{
   timeMultiple = 1;
};


