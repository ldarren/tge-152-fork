
//-----------------------------------------------------------------------------
// Structures                                                                  
//-----------------------------------------------------------------------------
struct ConnectData
{
   float2 texCoord        : TEXCOORD0;
   float3 eyePos          : TEXCOORD1;
   float3 normal          : TEXCOORD2;
   float3 pos             : TEXCOORD3;
   float4 hpos2           : TEXCOORD4;
   float3 screenNorm      : TEXCOORD5;
   float3 lightVec        : TEXCOORD6;
};


struct Fragout
{
   float4 col : COLOR0;
};


//-----------------------------------------------------------------------------
// Main                                                                        
//-----------------------------------------------------------------------------
Fragout main( ConnectData IN,
              uniform sampler2D  refractMap  : register(S0),
              uniform sampler2D  baseTxrMap  : register(S1),
              uniform sampler2D  bumpMap     : register(S2),
              uniform float4     spec_clr    : register(C0),
              uniform float      spec_pow    : register(C1),
              uniform float      fadeAlpha   : register(C7)
)
{
   Fragout OUT;

   float4 base_txr = tex2D(baseTxrMap, IN.texCoord);
   float3 bumpNorm = tex2D(bumpMap, IN.texCoord*3.0)*1.92 - 1.0;
   
   float3 eyeVec = normalize(IN.eyePos - IN.pos);
   
   
   float3 refractVec = refract(float3(0.0, 0.0, 1.0), normalize(bumpNorm), 1.0);
   
   float2 tc;
   tc = float2((IN.hpos2.x + refractVec.x)/(IN.hpos2.w), (-IN.hpos2.y + refractVec.y)/(IN.hpos2.w));
   tc = saturate((tc + 1.0)*0.5);
   
   OUT.col = lerp(tex2D(refractMap, tc), base_txr, base_txr.a*0.7);
   //OUT.col = lerp(tex2D(refractMap, tc).rgb, base_txr.rgb, base_txr.a);
   //OUT.col.a = 1;

   
   float3 halfAng = normalize(eyeVec + IN.lightVec);
   float specular = saturate( dot(bumpNorm, halfAng) );
   specular = pow(specular, spec_pow);
   OUT.col.rgb += spec_clr.rgb * specular;
   OUT.col.a *= fadeAlpha;

   return OUT;
}
