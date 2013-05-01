//------------------------------------------------------
// Copyright 2000-2006, GarageGames.com, Inc.
// Written or Modified by Ed Maurina, Hall Of Worlds, LLC
//------------------------------------------------------
echo("\n\c3--------- Loading GPGT String Utilities ---------");
function rldstrings() {
exec("./egs_String.cs");
}

// Some utilties for modifying strings of words
//
// swapWords - Swaps two words is string
// randomizeWords - Randmomizes locations of words in string
//
function swapWords( %words, %first, %second )
{
   %numWords = getWordCount(%words);

   if( ! ( ( 0 == %numWords) ||
         (%first < 0) ||
         (%first >= %numWords)  ||
         (%second < 0)  ||
         (%second >= %numWords) ||
         ( %first == %second) ) )
   {
         %tmp = getWord( %words , %first );
         %words = setWord( %words , %first , getWord( %words , %second ) );
         %words = setWord( %words , %second , %tmp );

   }

   return %words;
}

function RandomizeWords( %words , %iterations )
{
   %numWords = getWordCount(%words);
   if( 0 == %numWords) return "";
   for( %count = 0; %count < %iterations ; %count++) {
      %first  = getRandom( 0 , %numWords - 1 );
      %second = getRandom( 0 , %numWords - 1 );
      %words = swapWords( %words, %first, %second );
   }
   return %words;
}

