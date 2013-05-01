$StartXYValue="0 0";
$EndXYValue="0 0";


//This function generates a 64x48 grid
function creategrid(){
   for(%y=0; %y<48; %y++){
      for(%x=0; %x<64; %x++){
         %test =new GuiBitMapCtrl() { 
            profile = "GuiDefaultProfile";
            position=%x*10 SPC %y*10;
            Extent = "10 10";
            };
            
            $PathArray[%x,%y] = %test;
            //%test.setbitmap("pathfind/client/ui/gray.png");
            PathFindHolderGUI.add(%test);
            
      }
   }
   PathFindGUI.add(%mouse);
   randomizegrid();
}


function randomizeGrid(){  //this jumbles up the grid
   %o=0;
   
   for(%y=0; %y<48; %y++){
      for(%x=0; %x<64; %x++){
         %m=getRandom(3);
         if (%m==0) {
            SetXYArray(%x,%y,1);
          }
          else{
            SetXYArray(%x,%y,0);
          }
         %o++;
         
      }


   }
}

function SetXYArray(%x,%y,%value){ // used to set the walkable status

   SetXYValue(%x,%y,%value);
   
   //Stuf for changing the bitmap
   if (!%value)
      $PathArray[%x,%y].setbitmap("pathfind/client/ui/white.png");
   else
       $PathArray[%x,%y].setbitmap("pathfind/client/ui/gray.png");

}



function ClickMe::onRightMouseDown(){
   $MouseDown=1;
   %x = getword(Canvas.getCursorPos(),0);
   %y = getword(Canvas.getCursorPos(),1);
   %x=mfloor(%x/=10);
   %y=mfloor(%y/=10);

   if($pathfindmode){  //Pathfinding mode is where you set a start and end

      if (!GetXYValue(%x,%y)){  //determine if this square is valid to mark
         clearOldPath(); //remove any previous paths
      
         if (isObject($EndXY)){
            $EndXY.setbitmap("pathfind/client/ui/white.png");
         }
            $EndXY=$PathArray[%x,%y];
      }
            $EndXY.setbitmap("pathfind/client/ui/red.png");

      $EndXYValue=%x SPC %y;  //sets the ending XY location
      PrepPathFind();  //this starts up the pathfinding routines
      return;
   }


   SetXYArray(%x,%y,1);  //sets a square as not walkable

   
}

function ClickMe::onRightMouseDragged(){ //used for "painting"
   if($pathfindmode){ClickMe::onRightMouseDown(); return;}
   %x = getword(Canvas.getCursorPos(),0);
   %y = getword(Canvas.getCursorPos(),1);
   %x=mfloor(%x/=10);
   %y=mfloor(%y/=10);
    SetXYArray(%x,%y,1);
   
}


function ClickMe::onMouseDown(){
   
   %x = getword(Canvas.getCursorPos(),0);
   %y = getword(Canvas.getCursorPos(),1);
   %x=mfloor(%x/=10);
   %y=mfloor(%y/=10);



   if($pathfindmode){  //Pathfinding mode is where you set a start and end

      if (!GetXYValue(%x,%y)){  //determine if this square is valid to mark
         clearOldPath(); //remove any previous paths
      
         if (isObject($StartXY)){
            $StartXY.setbitmap("pathfind/client/ui/white.png");
         }
            $StartXY=$PathArray[%x,%y];
            
            
            $StartXYValue=%x SPC %y;
            PrepPathFind();
            $StartXY.setbitmap("pathfind/client/ui/green.png");

            
      }

      return;
   }


   SetXYArray(%x,%y,0);

   
}

function ClickMe::onMouseDragged(){
   if($pathfindmode){ClickMe::onMouseDown(); return;}
   
    %x = getword(Canvas.getCursorPos(),0);
    %y = getword(Canvas.getCursorPos(),1);
    %x=mfloor(%x/=10);
    %y=mfloor(%y/=10);
    SetXYArray(%x,%y,0);
     
   
}

function PathfindMode(){
   $pathfindmode =!$pathfindmode;
}


function PrepPathFind(){
   %startx=GetWord($StartXYValue,0);
   %starty=GetWord($StartXYValue,1);
   %endx=GetWord($EndXYValue,0);
   %endy=GetWord($EndXYValue,1);

   %time = GetSimTime();
   %validpath = StartPathFinding(%startx,%starty,%endx,%endy);
   %time = GetSimTime() - %time;
   error("TIME " SPC %time);
   if (%validpath==1){
      echo("Valid Path Found");
      %length = getPathLength();      
         
         %loop=0;
      	for (%a=0; %a<%length*2; %a+=2){
			   %xy=getPathStep(%a);
			   %newx= getword(%xy,0);
			   %newy= getword(%xy,1);

			   $FinalPathX[%loop]=%newx;
			   $FinalPathY[%loop]=%newy;
    			%loop++;

			
}
			drawpath(%length); // draws a path onscreen.

      
      
   }
      
   
}

//a diagnostic function to print to the screen the values of the map data
function printXY(){
   for(%y=0; %y<48; %y++){
      %xline="";
      for(%x=0; %x<64; %x++){
         %xline = %xline SPC GetXYValue(%x,%y);
      }
   }
}




function drawpath(%length){

   clearOldPath();


   for (%a=0; %a<%length-1; %a++){
   %x = $FinalPathX[%a];
   %y = $FinalPathY[%a];

       %path =new GuiBitMapCtrl() { 
            profile = "GuiDefaultProfile";
            position=%x*10 SPC %y*10;
            Extent = "10 10";
            };
            
            %path.setbitmap("pathfind/client/ui/path.png");
            PathScreen.add(%path);
            
   
   }
   
   
}



//cleans up any old paths
function clearOldPath(){
   while(PathScreen.getCount()){  
      PathScreen.getObject(0).delete();
    }
}





pathfindmode();