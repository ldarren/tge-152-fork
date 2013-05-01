Blue Guy by Joe Maruschak

This model was made by Joe and released to the community.  It uses the standard DSQ animation sequences.  It has been included here for use in your own projects when you need a lightweight model for testing purposes.  

Thanks Joe!  The original thread on this can be found at: http://www.garagegames.com/index.php?sec=mg&mod=resource&page=view&qid=6056

// Note: This file is in the position (hierarchy) that it is because:
// 
You may find that placing a TSShapeConstructor(PlayerDts) too deeply in your hiearchy will cause the game to crash/fail when transmitting datablocks.  The reason for this is that the total size of this datablock will exceed 1024 bytes which is the default limit for a single packet.  

 Solutions:
 1. Move lower in hierarchy
 2. Increase packet size.
 3. Modify packet code to split this into two or more packets.

I've chosen solution 1.

// Note2: I made a minor modification to Joe's original model (an no I won't explain how it was
// accomplished), enabling multi-skinning by use of the %obj.setSkinName( texturePath ) method.
//
// To use alternate skins, simply create a texture file named: 
//
//    'tag'.tmskin.png  (where 'tag' can be anything)
//
// Then, create an instance of the Blue Guy and change his skin like this: 
//
//    %myBlueGuy.setSkinName( path + 'tag' );
//