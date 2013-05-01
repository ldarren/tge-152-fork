//-----------------------------------------------------------------------------
// Torque Game Engine
// 
// Written by Melvyn May, Started on 9th September 2002.
//
// "My code is written for the Torque community, so do your worst with it,
//	just don't rip-it-off and call it your own without even thanking me".
//
//	- Melv.
//
//-----------------------------------------------------------------------------

//$foo = new fxRenderObject() { position = LocalClientConnection.player.getPosition(); Texture = "starter.fps/data/water/water.png"; };

#include "dgl/dgl.h"
#include "console/consoleTypes.h"
#include "core/bitStream.h"
#include "math/mathIO.h"
#include "game/gameConnection.h"
#include "console/simBase.h"
#include "sceneGraph/sceneGraph.h"
#include "sceneGraph/sgUtil.h"
#include "fxRenderObject.h"

//------------------------------------------------------------------------------
//
//	Okay, here's the wordy-wordy explanation of what's going on here.
//
//	The object is inheriting from the core SceneObject.  It is essential that
//	you inherit from at least this object to be able to add your object into
//	the SceneGraph.
//
//	DECLARE_CONOBJECT(fxRenderObject);
//	IMPLEMENT_CO_NETOBJECT_V1(fxRenderObject);
//
//	The lines above are vitally important when creating these network objects.
//	They register the object with the engine and you can't proceed without them.
//	The DECLARE goes in your class definition and the IMPLEMENT goes into your
//	implementation.  Anyone who has used MFC should recognise this procedure.
//
//
//	*** CONSTRUCTOR ***
//
//	In the constructor you will notice that I set the mTypeMask.  This is used
//	as a standard way of identifing object 'types'.  It's called a mask because
//	you can merge multiple types.  One of the most common uses for this is
//	the collision detection system.  The collision detection system allows you
//	to include certain types when searching.  It's these flags here that are
//	used.  You can create your own but be careful as there are only so many
//	available.  Normally you wouldn't do this as you should find one that fits
//	into what you are doing but the option is always there.
//
//	I also set the net flags.  These are a bunch of flags that signify some
//	important aspects of the object.  You should really look at the networking
//	documentation on the GG site for a basic explanation of ghosts and scoping
//	as it's a bit much to explain here.  The default settings should suffice
//	unless you are doing something weird (like my fxShapeReplicator)!
//
//	I'm also resetting the mLastRenderTime variable as I used this as a
//	timestamp.  Subtracting the current time from the last obviously gives
//	me the elapsed time and it's this value that can be used to create
//	frame-independant animation.
//
//	I also reset most other stuff here to their defaults.  It is vitally
//	important to note that even on a single machine there are two objects
//	created, one on the server and one on the client.  The client could
//	potentially be on another machine but Torque handles that for you.
//
//	The constructor will be called for both the server and client objects and
//	so this is a great place to initialise stuff.  Try not to allocate memory
//	in the constructor though, this goes for OOP everywhere though!
//
//
//	*** DESTRUCTOR ***
//
//	My destructor is empty because I don't need to do anything for shutdown.
//	If I had textures allocated then the variable holding that texture
//	reference will go out of scope and the resource manager will remove the
//	reference and possibly destroy the texture (if no other objects have
//	a reference to the same texture object).
//
//
//	*** InitPersistFields ***
//	
//	It's here that we add our custom fields.  Note that this gets called
//	when the engine is starting up and *not* when the object is created.
//	What you are essentially doing here is registering with the engine, the
//	fields that objects of this 'class' will have.  Don't put any
//	object-instance specific code in here otherwise you will be disappointed!
//
//	Use the addField function to define your fields but be careful not to
//	use duplicate fieldnames or ones with spaces in them.  You can happily
//	use underscores though.
//
//	If you've merged with the [HEAD] then you can used my new field-grouping
//	which enables you to group blocks of fields that are related.  They look
//	much nicer in the editor when they are grouped and most things become
//	easier to find.  You can use addGroup("MyGroup1")/endGroup("MyGroup1")
//	entries around you field-blocks.  I have left an example in the code for
//	you to look at (commented-out).
//
//
//	*** OnAdd ***
//
//	After the object is built (either from you creating one in the editor or
//	upon mission start-up) this function is called by the engine to allow the
//	object to add itself into the scene and setup scene specific attributes.
//
//	First-up always try to call parent functions if they are there.
//
//	Next I setup the object box.  This is an object-space box that defines the
//	region which the object occupies.  It is important that you get this correct
//	as the SceneTraversal routines uses this box to see if the object is in the
//	view.  Setting it too small results in the object not being rendered when
//	in-fact it should be.  Too big and the objects render function will get
//	called when it's not on the screen which is just wasted processing.  If the
//	object dynamically changes shape during gameplay then you can either create
//	an object-box at start-up that completely encapsulates its' volume or you
//	can dynamically change the object-box.  Remember though, that if you must
//	reset the world box and set the render transform after doing so and that
//	you must do this client-side as well.
//
//	After you've set the object-space box you *must always* reset the world-box.
//	What this does is calculate, using the current objects position, the
//	world-space coordinates of this box.  It also calculates the world-sphere
//	which is a sphere that encapsulates the specified box.
//	Also, you must set the render transform.  Failure to do this can result in
//	weird and hard to track results in animations and editor manipulation.
//
//	Finally we add the object to the scene with the simple call "addToScene()".
//
//	Returning true here confirms that everything was okay.  If you return false
//	then the server will assume that there has been a problem and will
//	disconnect the client informing them that this resource failed to load and
//	that you may have incorrect resources or wrong engine version, so watch out!
//
//
//	*** onRemove ***
//
//	This one's dead easy.  "removeFromScene()" does exactly that.  Again, call
//	the parent functions where needed.
//
//
//	*** inspectPostApply ***
//
//	This is an interesting function as it's called (Server-Side only) when the
//	user hits the "Apply" button when editing the object.  We respond by doing
//	the nice thing and calling the parent and then signalling which bits of data
//	need to get transferred to the client.
//
//	We do this using "setMaskBits(fxRenderObjectMask)".  Doing this will result
//	in the "packUpdate(...)" (see below) function being called.  Essentially,
//	the mask you specify here is your customised mask you created in the header
//	of the object.
//
//	Quite often you will only have one and it will trigger all the object data
//	being sent.  This can be wasteful and if your object does this alot during
//	gameplay then you would want to send only the data that has changed or at
//	least only relevant groups of data that has changed.  I explain these masks
//	more in the "packUpdate" section below.
//
//
//	*** onEditorEnable / onEditorDisable ***
//	
//	I left these functions in as they can be extremely useful when you want your
//	object to act differently when you know the editor is on.  For instance, I
//	used this in my fxShapeReplicator to signal that the editor was on.  When it
//	was I would additionally draw a sweeping arc defining the replication area.
//
//
//	*** prepRenderImage ***
//
//	Let me just say before I start an explanation here that this function should
//	work without modification for most objects you will want to create and so if
//	I loose you then just skip it. ;)
//
//	Let me open-up the inner workings of the engine a little...
//
//	After an object has been created client-side, it's "OnAdd" function is called
//	(see above) and it adds itself to the scene.  But what does this mean?
//
//	Well the engine has lots of structures (believe me lots) one of which is a
//	container.  This, as it's name implies, can contain objects.  You've heard
//	me throw the term SceneGraph around but what does it do?  Well, in Torque, all
//	client-side objects are in a Container named "gClientContainer".  Similarly,
//	all server-side objects are in a Container named "gServerContainer" but let's
//	focus on the client-side here as it's ultimately the client-side rendering we
//	are interested in for this discussion.
//
//	A container isn't just a big ol' list of objects, it's more than that.  You may
//	have seen various error messages like "Object is not out of the bins" etc?
//
//	The Container splits up the world into cubes called bins.  When an object is
//	added to the scene, it's bounding-box is used to find which of these bins it
//	belongs in.  Basically it identifies a box in the world.  If the object is too
//	big then it gets put into a linear list called the overflow bin (where naughty
//	objects go).
//
//	When you do a CastRay using the engine it basically checks which of these
//	bins it intersects.  It then calls all the objects in turn that reside in these
//	bins.  The objects can then do their own, perhaps more detailed, collision check
//	that may be polygon perfect or just the bounding-box, it's entirely upto the
//	object itself.
//
//	Where is this going you say?  Well, this system is also used to determine what
//	objects are in the view.  The viewing frustum defines a region that can be
//	checked to see which bins it intersects (plus the naughty objects in the overflow
//	bin).  Using this, these objects "in-scope" have their "prepRenderImage" called
//	to allow them to do exactly that, prepare a render image.
//
//	Phew!  Well what's a render image you say?  Boy am I glad you asked that!
//
//	A "SceneRenderImage" is simply a structure that defines the rendering attributes
//	of the object that control the way the SceneTraversal routines handle it.
//	Important factors such as is the object translucent (in which case it needs
//	sorted back->front) can be specified here.
//
//	In my example, you will see me dynamically allocating a "SceneRenderImage",
//	populating relevant fields and then inserting it into the SceneState.  You don't
//	need to destroy it, the SceneGraph will do that after the frame is complete.
//
//	There are lots of fields that I just haven't got the energy to go into here but
//	two important ones I will.  The "isTranslucent" field tells the SceneGraph that
//	at least one portion of your object has transparency.  If you didn't already
//	know it, rendering transparent objects requires that objects be rendered from
//	back to front (far to near) order to look correct.  I won't go into why this is
//	so because then I'm getting into imaging and then I'll really bore you. ;)
//
//	The second important field is "sortType" which allows you to specify the way
//	the object is handled by the scene.  You can use the following ...
//
//		Non-translucent objects ...
//		
//      Sky			- Specialised for Sky object only.
//      Terrain		- Specialised for Terrain object only.
//      Normal		- Standard one to use for non-translucent objects.
//
//		Translucent objects only ...
//		
//      Point		- Object is a point defined by state->poly[0]
//      Plane		- Object is a plane defined by state->poly[0-3]
//      EndSort		- Object should appear after everything else has rendered
//      BeginSort	- Object should appear before everything else has rendered
//	
//	The "EndSort" is useful for rendering lens flares which need to be rendered
//	after everything else has been rendered.
//
//	The "BeginSort" is handy for rendering stuff behind the terrain but after
//	the Sky like my fxSunLight object which renders a remote object and a local
//	object.
//
//	This brings me to an interesting point.  You can insert more than one
//	SceneRenderImage into the SceneState during the call to "prepRenderImage"
//	which results in the "renderObject" being called more than once at the
//	point defined by the "SceneRenderImage"s attributes.  I do this with the
//	fxSunLight but be sure you know what you are doing with this as you can
//	significantly reduce your framerate by causing the same objects to be
//	rendered multiple times.
//
//	Why this complexity?  Well, it's a very powerful method to use because you
//	do all sorts of wizardry by traversing scene portals, clipping the frustum
//	and iterating the new frustum upto a specified depth to give you the
//	ability to create mirrors, water reflections and other portal effects.
//
//
//	*** renderObject ***
//
//	Alrighty then!  The SceneTraversal is stuffed with "SceneRenderImage"s, it
//	then does all the sorting it needs and then proceeds to actually act on them.
//
//	Because each "SceneRenderImage" has a reference to the object (notice the line
//	that adds "state->obj = this", it then calls the "renderObject" for each object.
//
//	Now we are nearly ready to render.  When you enter the "renderObjec" function
//	we are in what is known as the Canonical state.  It's just another way of
//	saying a standard state or a consistent state that is guaranteed everytime
//	we get to this position.  This is handy for many reason but mostly because
//	you reduce the possibilty of other objects' state settings interacting with
//	your functions.  Imagine having to reset every possible state setting that could
//	possibly affect you, nightmare!
//
//	It's always good to check for this state with an AssertFatal at the beginning
//	and end of this function.
//
//	First-up, we get the current timestamp.  We use "getVirtualMilliseconds" here
//	that gives us the virtual game timeslice timestamp and we calculate the
//	elapsed time since our last render.  This is useful because we can use it by
//	multiplying it with our changes to give us a frame-rate independant animation.
//	This is also handy as you can normalise your object attributes in units/second.
//
//	We then check the texture handle in the example, we can't render without it.
//	This is specific to this example and can be ignored for you own object if you wish.
//	Note though, that as soon as you have changed the rendering state in any way
//	then you should restore the canonical state before returning otherwise you
//	will get a fatal assertion further on down the line!
//
//	I'm not going to explain the rendering pipeline here because there are many
//	books which do the same.  Drop me an email and I'll recommend one, I think
//	I've personally got them all!!
//
//	We *must* save the PROJECTION and MODELVIEW matrices plus the VIEWPORT so
//	that we can restore them to our nice and friendly canonical state before exit.
//
//	With everything saved, we multiply the MODELVIEW by our objects transform
//	to effectively move our origin to the objects' position.  All our graphical
//	commands are then in object-space (or at-least relative to the objects origin).
//
//	I won't explain the rest of the function because it's just OpenGL (possibly
//	translated into DirectX) and I'm not here to teach you that!
//
//	Before we exit we restore canonical state which is really important.  As I
//	stated above, I do an assertion to double-check this.  You can never be to
//	cautious with this and it goes away in the RELEASE build anyway.
//
//
//	*** packUpdate ***
//
//	Right, we are back to the networking.  Remember above (inspectPostApply) when
//	we made a call to "setMaskBits(...)"?
//
//	After we did that, this function gets called.  Remember that we are now
//	server-side.  This function effectively queues-up a bit-stream to be sent
//	to the client (irrelevant of whether the client is on the same PC or half-way
//	around the world).
//
//	First-up we call the parent so that it can get any data it owns sent-out.
//
//	We then do something a little clever.  When the function was called, it was
//	passed the mask that we sent using the "setMaskBits(...)" function.
//
//	When designing your objects you can split the data up into groups that
//	you can get sent using the appropriate mask(s).
//
//	For each group you *always* write a flag signalling whether the group has
//	data to send.  The client will check this and know whether the following
//	data is the rest of the group data or another group flag.
//
//	In the example I have only one group controlled by the mask,
//	"fxRenderObjectMask".  Because I do a boolean "AND" against the passed-in
//	mask I effectively send whether the group is being sent or not.  The
//	"writeFlag" function is also kind enough to return the result of this
//	masking so that you can use it as a condition in an if(cond) statement as
//	I do.
//
//	Within this block you can call a multitude of functions from the BitStream
//	class that allow you to write-out your data to the network.
//
//	The return value here is important as it becomes the new state mask used by
//	the networking code.  If you manage to send all your data then you should
//	return a zero.  Returning any other mask value results in the "packUpdate"
//	function being called again with the returned mask.  Typically you would
//	return the value from the "Parent::packUpdate" function" which which returns
//	zero.  If there are 'blocks' of data that you couldn't pack for any reason
//	and you want to try again next time then you should return something like
//	"mask & ~(state flags sent)".  To cut a long story short though, returning
//	zero indicates that you send all your data and the story ends there unless
//	you are doing something a little more technical!
//
//
//	*** unpackUpdate ***
//
//	If you've understood the above then you shouldn't have any problems here.
//
//	You should also know that we are now client-side.
//
//	Eventually, after the "packUpdate" function was called (for each client),
//	the data is sent out to respective clients.  It is important to note that
//	the "packUpdate" function is called once for each client connected and that
//	each client maintains it's own state mask.
//
//	First-up here is to do the friendly thing and call the parents "unpackUpdate"
//	routine to allow it to load it's data from the stream.
//
//	We then call "readFlag" which should correspond to the flag we wrote in the
//	"packUpdate" function.  If it's true then we know that the group data follows
//	and we proceed to read it.
//
//	It's using this "readFlag" then reading group data that we can control exactly
//	what data elements get sent/received.  You could do a mask per data item but that
//	would be overkill.  If you look deeper you find that the stream read/write
//	commands also let you save bandwidth by allowing you to specify bit-lengths
//	of data items but be sure you know what you are doing with these.
//
//	Another interesting note is that after the data has been loaded I load-up
//	any specified textures here.  Most of the standard engine objects *don't* do
//	this and only load them up in the "onAdd()" function (see above) which 
//	obviously only gets called once at startup.  This accounts for why you need
//	to restart the mission to see your changes.  I recently modified the "Sky"
//	object by adding the "loadDML()" call into the client-side "unpackUpdate"
//	function.
//
//	Also, note that you should never load texture resources on the server as the
//	server doesn't actually do any rendering and so it's pointless.
//
//	Also, because some functions are called server-side *and* client-side you need a
//	method of knowing which side you are on and doing an appropriate action.
//	You can do this with the calls "isClientObject()" and "isServerObject()".
//
//	******************
//
//	Well, I hope you found this informative and I'm sorry if I didn't go into
//	some areas in enough detail but I had to limit this somehow.
//
//	If you have any specific questions then drop me an email to...
//
//														melv.may@btinternet.com
//
//	All the best everyone,
//
//	- Melv.
//
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//
//	Put the function in /example/common/editor/ObjectBuilderGui.gui [around line 458] ...
//
//	function ObjectBuilderGui::buildfxRenderObject(%this)
//	{
//		%this.className = "fxRenderObject";
//		%this.process();
//	}
//
//------------------------------------------------------------------------------
//
//	Put this in /example/common/editor/EditorGui.cs in [function Creator::init( %this )]
//
//   %Environment_Item[ next free entry ] = "fxRenderObject";  <-- ADD THIS.
//
//------------------------------------------------------------------------------
extern bool gEditingMission;

//------------------------------------------------------------------------------

IMPLEMENT_CO_NETOBJECT_V1(fxRenderObject);


//------------------------------------------------------------------------------
// Class: fxRenderObject
//------------------------------------------------------------------------------

fxRenderObject::fxRenderObject()
{
	// Setup NetObject.
	mTypeMask |= StaticObjectType | StaticTSObjectType | StaticRenderedObjectType;
	mNetFlags.set(Ghostable);

	// Reset Last Render Time.
	mLastRenderTime = 0;

	// Texture Handle.
	mTextureHandle = NULL;
	// Flare Texture Name.
	mTextureName = StringTable->insert("");

	// Reset Quad Size.
	mQuadSize = 5.0f;
	// Reset Quad Rotate Speed.
	mQuadRotateSpeed = 90.0f;

	// Reset Current Angle.
	mCurrentAngle = 0.0f;

   mObjColor = ColorI(255, 0, 0);
}

//------------------------------------------------------------------------------

fxRenderObject::~fxRenderObject()
{
}

//------------------------------------------------------------------------------

void fxRenderObject::initPersistFields()
{
	// Initialise parents' persistent fields.
	Parent::initPersistFields();

	// Add out own persistent fields.
	//addGroup( "MyFirstGroup1" );
    addField( "QuadSize",		TypeF32,		Offset( mQuadSize,			fxRenderObject ) );
    addField( "QuadRotateSpeed",TypeF32,		Offset( mQuadRotateSpeed,	fxRenderObject ) );
    addField( "Texture",		TypeFilename,	Offset( mTextureName,		fxRenderObject ) );
    addField( "ObjColor",		   TypeColorI,  Offset( mObjColor,          fxRenderObject ) );
	//enddGroup( "MyFirstGroup1" );
}

//------------------------------------------------------------------------------

bool fxRenderObject::onAdd()
{
	if(!Parent::onAdd()) return(false);

	// Calculate Quad Radius.
	F32 QuadHalfSize = mQuadSize / 2.0f;

	// Set initial bounding box.
	//
	// NOTE:-	Set this box to completely encapsulate your object.
	//			You must reset the world box and set the render transform
	//			after changing this.
	mObjBox.min.set( -QuadHalfSize, -0.5f, -QuadHalfSize );
	mObjBox.max.set(  QuadHalfSize, +0.5f,  QuadHalfSize );
	// Reset the World Box.
	resetWorldBox();
	// Set the Render Transform.
	setRenderTransform(mObjToWorld);

	// Add to Scene.
	addToScene();

	// Return OK.
	return(true);
}

//------------------------------------------------------------------------------

void fxRenderObject::onRemove()
{
	// Remove from Scene.
	removeFromScene();

	// Do Parent.
	Parent::onRemove();
}

//------------------------------------------------------------------------------

void fxRenderObject::inspectPostApply()
{
	// Set Parent.
	Parent::inspectPostApply();

	// Set fxPortal Mask.
	setMaskBits(fxRenderObjectMask);
}

//------------------------------------------------------------------------------

void fxRenderObject::onEditorEnable()
{
}

//------------------------------------------------------------------------------

void fxRenderObject::onEditorDisable()
{
}

//------------------------------------------------------------------------------

bool fxRenderObject::prepRenderImage(	SceneState* state, const U32 stateKey, const U32 startZone,
										const bool modifyBaseZoneState)
{
	// Return if last state.
	if (isLastState(state, stateKey)) return false;
	// Set Last State.
	setLastState(state, stateKey);

   // Is Object Rendered?
   if (state->isObjectRendered(this))
   {	   
		// Yes, so get a SceneRenderImage.
		SceneRenderImage* image = new SceneRenderImage;
		// Populate it.
		image->obj = this;
		image->isTranslucent = false;
		image->sortType = SceneRenderImage::Normal;
		
		// Insert it into the scene images.
		state->insertRenderImage(image);
   }

   return false;
}

//------------------------------------------------------------------------------

void fxRenderObject::renderObject(SceneState* state, SceneRenderImage*)
{
	// Check we are in Canonical State.
	AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on entry");

	// Calculate Elapsed Time and take new Timestamp.
	S32 Time = Platform::getVirtualMilliseconds();
	F32 ElapsedTime = (Time - mLastRenderTime) * 0.001f;
	mLastRenderTime = Time;

	// Return if we don't have a texture.
	if (!mTextureHandle) return;

	// Save state.
	RectI viewport;

	// Save Projection Matrix so we can restore Canonical state at exit.
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();

	// Save Viewport so we can restore Canonical state at exit.
	dglGetViewport(&viewport);

	// Setup the projection to the current frustum.
	//
	// NOTE:-	You should let the SceneGraph drive the frustum as it
	//			determines portal clipping etc.
	//			It also leaves us with the MODELVIEW current.
	//
	state->setupBaseProjection();

	// Save ModelView Matrix so we can restore Canonical state at exit.
	glPushMatrix();

	// Transform by the objects' transform e.g move it.
	dglMultMatrix(&getTransform());

	// Rotate by Rotate Speed.
	//
	// NOTE:-	We use the elapsed time as a coeficient,
	//			that way we get consistent rotational speed
	//			independant of frame-rate.
	//
	mCurrentAngle += mFmod(mQuadRotateSpeed * ElapsedTime, 360);

	// Rotate Quad by current roation.
	glRotatef(mCurrentAngle, 0,0,1);

	// Setup our rendering state (alpha blending).
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Enable Texturing.
	glEnable(GL_TEXTURE_2D);

	// Select the objects' texture.
	glBindTexture(GL_TEXTURE_2D, mTextureHandle.getGLName());

	// Set Colour/Alpha.
	glColor4f(1,1,1,1);

	// Calculate Quad Radius.
	F32 QuadHalfSize = mQuadSize / 2.0f;

	// Draw a Quad.
	//
	// NOTE:-	We draw in object space here and *not* world-space.
	//			Notice that Z is UP in this system and XY lie on the terrain plane.
	//
	glBegin(GL_QUADS);
		glTexCoord2f(0,0);
		glVertex3f(-QuadHalfSize,0,+QuadHalfSize);
		glTexCoord2f(1,0);
		glVertex3f(+QuadHalfSize,0,+QuadHalfSize);
		glTexCoord2f(1,1);
		glVertex3f(+QuadHalfSize,0,-QuadHalfSize);
		glTexCoord2f(0,1);
		glVertex3f(-QuadHalfSize,0,-QuadHalfSize);
	glEnd();

	// Restore our canonical rendering state.
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	// Restore our canonical matrix state.
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	// Restore our canonical viewport state.
	dglSetViewport(viewport);

	// Check we have restored Canonical State.
	AssertFatal(dglIsInCanonicalState(), "Error, GL not in canonical state on exit");
}

//------------------------------------------------------------------------------

U32 fxRenderObject::packUpdate(NetConnection * con, U32 mask, BitStream * stream)
{
	// Pack Parent.
	U32 retMask = Parent::packUpdate(con, mask, stream);

	// Write fxPortal Mask Flag.
	if (stream->writeFlag(mask & fxRenderObjectMask))
	{
		// Write Object Transform.
		stream->writeAffineTransform(mObjToWorld);
		// Write Texture Name.
		stream->writeString(mTextureName);
		// Write Quad Size.
		stream->write(mQuadSize);
		// Write Quad Rotate Speed.
		stream->write(mQuadRotateSpeed);
      // Write Object Color
      stream->write( mObjColor );
	}

	// Were done ...
	return(retMask);
}

//------------------------------------------------------------------------------

void fxRenderObject::unpackUpdate(NetConnection * con, BitStream * stream)
{
	// Unpack Parent.
	Parent::unpackUpdate(con, stream);

	// Read fxPortal Mask Flag.
	if(stream->readFlag())
	{
		MatrixF		ObjectMatrix;

		// Read Object Transform.
		stream->readAffineTransform(&ObjectMatrix);
		// Read Texture Name.
		mTextureName = StringTable->insert(stream->readSTString());
		// Read Quad Size.
		stream->read(&mQuadSize);
		// Read Quad Rotate Speed.
		stream->read(&mQuadRotateSpeed);
      // Read Object Color
      stream->read( &mObjColor );


		// Set Transform.
		setTransform(ObjectMatrix);

		// Reset our previous texture handle.
		mTextureHandle = NULL;    
		// Load the texture (if we've got one)
		if (*mTextureName) mTextureHandle = TextureHandle(mTextureName, BitmapTexture, true);

		// Calculate Quad Radius.
		F32 QuadHalfSize = mQuadSize / 2.0f;

		// Set bounding box.
		//
		// NOTE:-	Set this box to completely encapsulate your object.
		//			You must reset the world box and set the render transform
		//			after changing this.
		mObjBox.min.set( -QuadHalfSize, -0.5f, -QuadHalfSize );
		mObjBox.max.set(  QuadHalfSize, +0.5f,  QuadHalfSize );
		// Reset the World Box.
		resetWorldBox();
		// Set the Render Transform.
		setRenderTransform(mObjToWorld);
	}
}
