function PathManager::getNextWaypoint(%curPos, %goal, %didWP)
{
   // John Eckhardt's pathfinding algorithm.
   // First, find the first obstacle in the way.  If none, just head to destination
   // Second find the side of the object you hit first
   // Head to the corner closest to your straight-to-goal path
   // Now, we calculate a new path to see if we can go straight to our objective or not.
   // If not, we calculate another path through the object, and go to the
   //   Farthest corner along the edge we run into.
   // Once at that corner, we are guarenteed to have a straight shot to home!
   
   //the list of object types we will avoid
   %searchMasks = ($TypeMasks::InteriorObjectType | $TypeMasks::StaticShapeObjectType | $TypeMasks::VehicleObjectType      | $TypeMasks::ForceFieldObjectType | $TypeMasks::PhysicalZoneObjectType | $TypeMasks::StaticTSObjectType);
   //the first object we run into along our path
   %target = containerRayCast (%curPos, %goal, %searchMasks);  

  if (%target) {
      
      %unit = getWord(%target, 0);
      %xx = getWord(%target, 1);   
      %yy = getWord(%target, 2);   
      %zz = getWord(%target, 3);
      %nodePos = %xx SPC %yy;   

      //get the coords of the edges of the object we run into.
      %x1 = getword(%unit.getworldbox(), 0);
      %y1 = getword(%unit.getworldbox(), 1);
      %x2 = getword(%unit.getworldbox(), 3);
      %y2 = getword(%unit.getworldbox(), 4);

      //these are the corners of the object we run into.
      // A(x1, y1)  D(x2, y1)    0 1  
      // C(x1, y2)  B(x2, y2)         3 4
      %unitCornerA = (%x1 SPC %y1);   
      %unitCornerB = (%x2 SPC %y2);   
      %unitCornerC = (%x1 SPC %y2);   
      %unitCornerD = (%x2 SPC %y1);   

      //find side that we are closest to.
      %minDist = mAbs(%xx - %x1);   //the distance to the closest side
      %minCtr = 1;                  //remember which side we are closest to
      if (%minDist > mAbs(%xx - %x2)) {
         %minDist = mAbs(%xx - %x2);
         %minCtr = 2;
      } 
      if (%minDist > mAbs(%yy - %y1)) {
         %minDist = mAbs(%yy - %y1);
         %minCtr = 3;
      } 
      if (%minDist > mAbs(%yy - %y2)) {
         %minDist = mAbs(%yy - %y2);
         %minCtr = 4;
      }
      
      
      //now, we choose the corner that our path is closest to and move out!
      //we want to go to the closest corner on our first leg, but the farthest on our second leg
      switch (%minCtr) {
         case 1:                 //closest side       
            %x = %x1;
            if(VectorDist(%nodePos, %unitCornerA) < VectorDist(%nodePos, %unitCornerC)) {  
               %y = (%didWP == 0) ? %y1 - 1 : %y2 + 1;      //closest (or farthest) corner
            } else {
               %y = (%didWP == 0) ? %y2 + 1 : %y1 - 1;
            } 
         break;
         
         case 2:
            %x = %x2;
            if(VectorDist(%nodePos, %unitCornerB) < VectorDist(%nodePos, %unitCornerD)) {  
               %y = (%didWP == 0) ? %y2 + 1 : %y1 - 1;
            } else {
               %y = (%didWP == 0) ? %y1 - 1 : %y2 + 1;
            }        
         break;
         
         case 3:
            %y = %y1;          
            if(VectorDist(%nodePos, %unitCornerA) < VectorDist(%nodePos, %unitCornerD)) {  
               %x = (%didWP == 0) ? %x1 - 1 : %x2 + 1;
            } else {
               %x = (%didWP == 0) ? %x2 + 1 : %x1 - 1;
            }
         break;
         
         case 4:         
            %y = %y2; 
            if(VectorDist(%nodePos, %unitCornerB) < VectorDist(%nodePos, %unitCornerC)) {  
               %x = (%didWP == 0) ? %x2 + 1 : %x1 - 1;
            } else {
               %x = (%didWP == 0) ? %x1 - 1 : %x2 + 1;
            }
         break;
            
         default:
            echo ("Big error in pathManager.cs  %minCtr should be 1-4!!!");     //should NEVER display this.
         
      }
      
      %nextWP = (%x SPC %y SPC 250);
      return %nextWP;   
   
   } else {
      return %goal;
   }

   
}