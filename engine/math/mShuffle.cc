#include "mShuffle.h"

#ifndef _MRANDOM_H_
#include "math/mRandom.h"
#endif
#include "console/console.h"

#include <math.h>

IMPLEMENT_CONOBJECT(ShuffledDeck);

//ShuffledDeck::MaxDeckSize = 0x7FFF;

void ShuffledDeck::initPersistFields()
{
   addGroup("Classes", "Script objects have the ability to inherit and have class information.");
   addField("class", TypeString, Offset(mClassName, ShuffledDeck), "Class of object.");
   addField("superClass", TypeString, Offset(mSuperClassName, ShuffledDeck), "Superclass of object.");
   endGroup("Classes");

   addGroup("Deck", "Deck related information");
   addField("start",	TypeS32,	Offset(newStart, ShuffledDeck), "smallest card id");
   addField("end",		TypeS32,	Offset(newEnd, ShuffledDeck), "biggest card id");
   addField("interval",	TypeS8,		Offset(newInterval, ShuffledDeck), "card id interval, never less than 1");
   endGroup("Deck");
}

ShuffledDeck::ShuffledDeck()
:	start(0),
	end(0),
	interval(1),
	newStart(0),
	newEnd(0),
	newInterval(1),
	size(0),
	seed(0),
	cards(0)
{
	mClassName = "";
	mSuperClassName = "";
}

bool ShuffledDeck::onAdd()
{
   if (!Parent::onAdd())
      return false;

   // superClassName -> ScriptObject
   StringTableEntry parent = StringTable->insert("ShuffledDeck");
   if(mSuperClassName[0])
   {
      if(Con::linkNamespaces(parent, mSuperClassName))
         parent = mSuperClassName;
   }

   // className -> superClassName
   if (mClassName[0])
   {
      if(Con::linkNamespaces(parent, mClassName))
         parent = mClassName;
   }

   // objectName -> className
   StringTableEntry objectName = getName();
   if (objectName && objectName[0])
   {
      if(Con::linkNamespaces(parent, objectName))
         parent = objectName;
   }

   // Store our namespace
   mNameSpace = Con::lookupNamespace(parent);

   shuffle();

   // Call onAdd in script!
   Con::executef(this, 2, "onAdd", Con::getIntArg(getId()));
   return true;
}
	
void ShuffledDeck::onRemove()
{
   // We call this on this objects namespace so we unlink them after. - jdd
   //
   // Call onRemove in script!
   Con::executef(this, 2, "onRemove", Con::getIntArg(getId()));

   // Restore NameSpace's
   StringTableEntry child = getName();
   if( child && child[0] )
   {
      if(mClassName && mClassName[0])
      {
         if(Con::unlinkNamespaces(mClassName, child))
            child = mClassName;
      }

      if(mSuperClassName && mSuperClassName[0])
      {
         if(Con::unlinkNamespaces(mSuperClassName, child))
            child = mSuperClassName;
      }

      Con::unlinkNamespaces(getClassName(), child);
   }
   else
   {
      child = mClassName;
      if(child && child[0])
      {
         if(mSuperClassName && mSuperClassName[0])
         {
            if(Con::unlinkNamespaces(mSuperClassName, child))
               child = mSuperClassName;
         }

         Con::unlinkNamespaces(getClassName(), child);
      }
      else
      {
         if(mSuperClassName && mSuperClassName[0])
            Con::unlinkNamespaces(getClassName(), mSuperClassName);
      }
   }

   destroyBuffer();

   Parent::onRemove();
}

void ShuffledDeck::shuffle(S32 s) 
{
	current = 0;

	if ((start != newStart) || (end != newEnd) || (interval != newInterval))
	{
		size = createBuffer();
	}

	if (s)
	{
		seed = s;
		gRandGen.setSeed(seed);
	}
	else
	{
		seed = gRandGen.getSeed();
		gRandGen.setSeed(seed);
	}

	U32 i;
	U32 hs = size/2;
	S32 j;
	for (i=0, j=start; i<size; i++, j += interval) 
	{
        cards[i] = j;  // fill the array in order
    }

    //--- Shuffle elements by randomly exchanging each with one other.
    for (i=0; i<hs; i++) 
	{
        U32 r = gRandGen.randI(i, size-1); // Random remaining position.
        S32 temp = cards[i]; cards[i] = cards[r]; cards[r] = temp;
    }
}

S32 ShuffledDeck::next()
{
	if (current >= size)
	{
		shuffle(seed-2);
	}
	return cards[current++];
}

S32 ShuffledDeck::previous()
{
	if (current == 0)
	{
		shuffle(seed-2);
		current = size-1;
	}
	return cards[current--];
}

U16 ShuffledDeck::getSize()
{
	if (interval < 1) interval = 1;
	if (start > end)
	{
		S32 temp = start;
		start = end;
		end = temp;
	}
	U16 s = (U16)ceil((end - start + 1)/float(interval));
	//U16 s = U16(((end - start + 1)/float(interval))+0.5f);
	AssertFatal ((s > 0 && s < ShuffledDeck::MaxDeckSize), "shuffle your deck with proper size first!");
	return s;
}

U16 ShuffledDeck::getRemaining() const
{
	return size - current;
}

U16 ShuffledDeck::createBuffer()
{
	if (cards) destroyBuffer();

	start = newStart;
	end = newEnd;
	interval = newInterval;

	size = getSize();
	cards = (S32*)dRealMalloc(sizeof(S32)*size);

	return size;
}

void ShuffledDeck::destroyBuffer()
{
	if (cards) dRealFree(cards);
	cards = 0;
	size = 0;
}

ConsoleMethod(ShuffledDeck, reshuffle, void, 2, 3, "resuffle(S32 seed) - resuffle deck, seed is optional")
{
   argc; argv;
   object->shuffle( argc==3?dAtoi(argv[2]):0 );
}

ConsoleMethod(ShuffledDeck, next, S32, 2, 2, "next() - next cards number")
{
   argc; argv;
   return object->next();
}

ConsoleMethod(ShuffledDeck, previous, S32, 2, 2, "previous() - previous cards number")
{
   argc; argv;
   return object->previous();
}

ConsoleMethod(ShuffledDeck, getSize, S32, 2, 2, "getSize() - get number of cards in deck")
{
   argc; argv;
   return object->getSize();
}

ConsoleMethod(ShuffledDeck, getRemaining, S32, 2, 2, "getRemaining() - get cards in deck")
{
   argc; argv;
   return object->getRemaining();
}

