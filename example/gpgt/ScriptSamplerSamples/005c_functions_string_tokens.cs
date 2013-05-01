// 
// Functions (Pre-defined)
// 
// String Functions --> Tokens 
//
// (Delimeter Separated String)
//

function printTokens( %tokenString ) 
{

    %tmpTokens = %tokenString;

    while( "" !$= %tmpTokens ) {

        %tmpTokens = nextToken( %tmpTokens , "myToken" , ";" );

        echo( %myToken );

    }
}

printTokens( "This;is;a;sample;string;of;tokens;." );



