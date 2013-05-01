// 
// Scope and Namespaces (Without them)
//

function Square1::printArea( %obj ) 
{
    echo( "The area of this square is: " , 
    %obj.width * %obj.height , "\n\n" );
}


function Square2::printArea( %obj ) 
{
    echo( "The area of this square is: " , 
    %obj.width * %obj.height , "\n\n" );
}


%myFirstSquare = new ScriptObject( Square1 ) 
{
    width  = 10.0;
    height = 5.0;
};

%mySecondSquare = new ScriptObject( Square2 ) 
{
    width  = 20.0;
    height = 5.0;
};


%myFirstSquare.printArea( );

%mySecondSquare.printArea( );


%myFirstSquare.delete();
%mySecondSquare.delete();



