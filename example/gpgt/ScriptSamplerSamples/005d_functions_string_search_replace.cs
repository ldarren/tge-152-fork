// 
// Functions (Pre-defined)
// 
// String Functions --> Search & Replace
//

%testString = "TGE is cool. TGE is fun.  TGE Rocks.  Use TGE to make a game!";

echo( %testString, "\n" );

// Get string length
%len = strlen(%testString);

echo( "\c3This string is ", %len , " characters long.", "\n" );

// Count instances of TGE
%lastTGE = -1;

while( %foundAt >= 0 ) 
{
    %foundAt = strpos( %testString , "TGE" , %lastTGE + 1 );

    if ( %foundAt > -1 ) 
    {
        %lastTGE = %foundAt;
        %count++;			
    } 		
	
}

echo( "\c3It contains ", %count, " instances of the substring TGE.", "\n" );

// Replace all instances of TGE
echo( "\c3Replacing all instances of TGE...", "\n" );

%testString2 = strReplace( %testString , "TGE", "Torque Game Engine" );

echo( %testString2, "\n" );

// Only replace last instance of TGE
echo( "\c3Replacing last instance of TGE...", "\n" );
	
%testString3 = getSubStr( %testString , 0 , %lastTGE ) @
               "the Torque Game Engine" @
               getSubStr( %testString , %lastTGE + 3 , %len );

echo( %testString3, "\n" );

// Modify and print the last sentence.
echo( "\c3Modifying and printing last sentence only...", "\n" );

%testString4 = strchr( %testString , "U" ) ;

%testString4 = strReplace( %testString4 , "a game" , 
                           "\cp\c3games\co that Rock" );

echo( %testString4, "\n" );




