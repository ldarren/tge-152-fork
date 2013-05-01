#include "forest/forest.h"

void Forest::calculateAmbientOcclusion(const Point3F &pos, const Point3F &norm, F32 &col)
{
   // Take the position & normal and cast a ray - if we hit, col goes to 
   // zero.
   
   // You could make this lots more complex by doing things like ambient 
   // occlusion, or adding virtual elemenents like metaballs for the
   // leaves, or things of this nature.

   AssertISV(smClientForest, "Forest::calculateAmbientOcclusion - no client forest exists!");

   static RayInfo ri;
   if(smClientForest->castRay(pos, pos + norm * 1000.f, &ri))
   {
      col = 0;
   }
   else
   {
      col = 1;
   }
}
