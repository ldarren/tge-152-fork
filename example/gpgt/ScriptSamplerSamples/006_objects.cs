// 
// Objects
//

%so = new ScriptObject( mySO );

echo("The ID of the object   == ", %so.getID() , "\n\n" );

echo("The NAME of the object == ", %so.getName() , "\n\n" );



%so.myFavoriteAge = 25;

%so.myFavoriteColor = "Green";


echo("Field myFavoriteAge == ", %so.myFavoriteAge , "\n\n" );

echo("Field myFavoriteColor == ", %so.myFavoriteColor , "\n\n" );


echo("Is mySo an Object? == ", isObject( mySo ) , "\n\n" );

%so.delete();

echo("Is mySo still an Object? == ", isObject( mySo ) , "\n\n" );



