// 
// Functions (Pre-defined)
// 
// Math Functions --> Random
//

%seed = getRandomSeed();

for ( %count = 0 ; %count < 100 ; %count++ ) 
{
    %x[%count] = getRandom( %count );
}

setRandomSeed( %seed );

for ( %count = 0 ; %count < 100 ; %count++ ) 
{
    %y[%count] = getRandom( %count );
}

%mismatches = 0;

for ( %count = 0 ; %count < 100 ; %count++ ) 
{
    if(  %x[%count] != %y[%count] )
    {
        error( "Failed to reproduce same sequence of random numbers!" );

        error("Seed:" SPC %seed );

        error("Cout:" SPC %count );

        error(%x[%count] SPC "!=" SPC  %y[%count] );

	%mismatches++;
    }
}

echo("There were ", %mismatches, " mismatches.");


