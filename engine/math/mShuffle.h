// Illustrates	: Shuffle algorithm, srand, rand.
// Improvements	: Use classes for Card and Deck.
// Author		: Fred Swartz 2003-08-24, shuffle correction 2007-01-18
//                Placed in the public domain.
//				: Darren Liew 2007-11-04, TGE integration

#ifndef _MATH_SHUFFLE_H_
#define _MATH_SHUFFLE_H_

#ifndef _PLATFORM_H_
#  include "platform/platform.h"
#endif
#include "console/simBase.h"
#include "console/consoleTypes.h"

class ShuffledDeck : public SimObject
{
   typedef SimObject Parent;
   StringTableEntry mClassName;
   StringTableEntry mSuperClassName;

public:
	ShuffledDeck();
	bool onAdd();
	void onRemove();

	DECLARE_CONOBJECT(ShuffledDeck);

	static void initPersistFields();

	void shuffle(S32 s = 0);
	U16 getSize();
	U16 getRemaining() const;

	S32 next();
	S32 previous();

protected:
	static const U16 MaxDeckSize = 0x7FFF;

	S32 start;
	S32 end;
	S8 interval;

	S32 newStart;
	S32 newEnd;
	S8 newInterval;
	
	U16 size;
	S32 seed;

	S32 current;
	S32* cards;

	U16 createBuffer();
	void destroyBuffer();
};

#endif // _MATH_SHUFFLE_H_
