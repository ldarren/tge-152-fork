// 
// Variables - Local Scope
//

function testLocal( ) 
{
   echo( "\n\n %localVar == " , %localVar );
   
   %localVar = 50;
   
   echo( "\n\n %localVar == " , %localVar );
}


%localVar = 10;


echo( "\n\n %localVar == " , %localVar );


testLocal();


echo( "\n\n %localVar == " , %localVar );


