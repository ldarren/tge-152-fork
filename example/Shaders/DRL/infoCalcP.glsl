uniform sampler2D AverageIntTexture;
uniform sampler2D PrevInfoTexture;

uniform float GoalIntensity;
uniform vec2 ChangeRates;
uniform vec2 ScaleLimits;
uniform vec2 BiasLimits;
uniform float ChangeTolerance;
uniform float tge_DeltaFrameTime;

void main()
{
   vec4 intColor = texture2D(AverageIntTexture, gl_TexCoord[0].st, 9.0);
   float averageIntensity = dot(intColor.rgb, vec3(0.3, 0.59, 0.11));
   float scale, bias;
   vec4 prevInfo = texture2D(PrevInfoTexture, vec2(0.5, 0.5));
   bias = -prevInfo.g;
   scale = prevInfo.r * 2.0;
   scale += prevInfo.a * 2.0;
   
   float difference = averageIntensity - GoalIntensity;
   difference = abs(difference);
   
   if(averageIntensity < (GoalIntensity - ChangeTolerance))
   {
      bias += tge_DeltaFrameTime * ChangeRates.y * difference;
      scale += tge_DeltaFrameTime * ChangeRates.x * difference;
      bias = clamp(bias, BiasLimits.x, BiasLimits.y);
      scale = clamp(scale, ScaleLimits.x, ScaleLimits.y);
   }
   if(averageIntensity > (GoalIntensity + ChangeTolerance))
   {
      bias -= tge_DeltaFrameTime * ChangeRates.y * difference;
      scale -= tge_DeltaFrameTime * ChangeRates.x * difference;
      bias = clamp(bias, BiasLimits.x, BiasLimits.y);
      scale = clamp(scale, ScaleLimits.x, ScaleLimits.y);
   }
   
   float scaleA = max(0.0, scale - 2.0);
   float scaleR = min(scale, 2.0);
   
   gl_FragColor = vec4( scaleR / 2.0, -bias, averageIntensity, scaleA / 2.0);
}