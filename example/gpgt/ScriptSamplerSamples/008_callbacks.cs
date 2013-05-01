// 
// Callbacks
//

function mySO::onAdd( %Obj )
{   
   echo("mySO::onAdd( ", %Obj , " )" );
   
   echo("\n\n");
}


function mySO::onRemove( %Obj )
{   
   echo("mySO::onRemove( ", %Obj , " )" );
   
   echo("\n\n");
}



%so = new ScriptObject( mySO );


echo("The ID of mySo == ", %so , "\n\n" );

%so.delete();



