// 
// Functions (Pre-defined)
// 
// String Functions --> Comparison
//

echo("\c3Lexicographic comparisons are not the same as arithmetic comparisons...");

echo("100 - 10 == 90, but strcmp( \"100\" , \"10\" ) == " ,  
     strcmp( "100" , "10" ) );

echo("\n", "\c3Don\'t forget about case-senstivity...");

echo("strcmp( \"ABC\" , \"abc\" )  == " , strcmp( "ABC" , "abc" ) , ", but " );

echo("stricmp( \"ABC\" , \"abc\" ) == " , stricmp( "ABC" , "abc" ) );



