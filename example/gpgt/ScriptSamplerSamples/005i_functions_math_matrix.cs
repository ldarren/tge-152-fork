// 
// Functions (Pre-defined)
// 
// Math Functions --> Matrix
//

%matA = MatrixCreateFromEuler( "60 90 30" );

%matB = MatrixCreateFromEuler( "0 0 90" );

%matC = MatrixMultiply( %matA , %matB );


echo(" Matrix A: ", %matA , "\n\n" );

echo(" Matrix B: ", %matB , "\n\n" );

echo(" Matrix C: ", %matC , "\n\n" );




