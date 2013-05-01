// 
// Scope and Namespaces (With them)
//

function Square::printArea( %obj ) 
{
   echo( "The area of this square is: " , 
   %obj.width * %obj.height , "\n\n" );
}

function Circle::printArea( %obj ) 
{
   echo( "The area of this circle is: " , 
   %obj.radius * %obj.radius * 3.1415962 ,
   "\n\n"  );
}

new ScriptObject( Square0 ) 
{
   width  = 10.0;
   height = 5.0;
   class  = Square;
};

new ScriptObject( Square1 ) 
{
   width       = 20.0;
   height      = 5.0;
   superClass  = Square;    
};

new ScriptObject( Circle0 ) 
{
   radius = 10;
   class  = Circle;
};


Square0.printArea( );
Square1.printArea( );
Circle0.printArea( );


Square0.delete();
Square1.delete();
Circle0.delete();




