// 
// Methods (Object functions)
//

function mySO::test_func( %Obj , %var )
{
   
   echo("mySO::test_func( ", %Obj , " , " , %var , " )" );
   
   echo("\n\n");
}


%so = new ScriptObject( mySO );


echo("The ID of mySo == ", %so , "\n\n" );


echo("A ==>");
%so.test_func( 10 );


echo("B ==>");
mySO.test_func( 10 );


echo("C ==>");
mySO::test_func( mySO , 10 );


echo("D ==>");
"mySo".test_func( 10 );


echo("F ==>");
mySO::test_func( mySO.getID() , 10 );


%so.delete();


