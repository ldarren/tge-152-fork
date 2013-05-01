// 
// Variables - Global Scope
//

function testGlobal( ) 
{
   echo( "\n\n $globalVar == " , $globalVar );
   
   $globalVar = 200;
   
   echo( "\n\n $globalVar == " , $globalVar );
}


$globalVar = 20;


echo( "\n\n $globalVar == " , $globalVar );


testGlobal();


echo( "\n\n $globalVar == " , $globalVar );


