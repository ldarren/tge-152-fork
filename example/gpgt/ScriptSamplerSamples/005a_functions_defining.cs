// 
// Functions
//

function test_func( %var )
{
   
   echo("test_func( ", %var , " )" );   
   
}

test_func( 10 );


echo("\n\n");



function recursive_func( %var )
{  
   
   echo("ENTER:: recursive_func( ", %var , " )" );
   
   if( %var <=  0 ) return;
   
    
   recursive_func( %var-- );
   

   echo(" EXIT:: recursive_func( ", %var , " )" );

}

recursive_func( 10 );



