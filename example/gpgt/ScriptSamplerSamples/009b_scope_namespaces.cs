// 
// Scope and Namespaces (A Solution?)
//

function ScriptObject::printArea( %obj ) 
{
   echo( "The area of this square is: " , 
   %obj.width * %obj.height , "\n\n" );
}

new ScriptObject( Square0 ) 
{
   width  = 10.0;
   height = 5.0;
};

new ScriptObject( Circle0 ) 
{
   radius = 25;
};


Square0.printArea( );
Circle0.printArea( );


Square0.delete();
Circle0.delete();




