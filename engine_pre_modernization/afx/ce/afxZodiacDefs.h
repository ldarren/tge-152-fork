
#ifndef _AFX_ZODIAC_DEFS_H_
#define _AFX_ZODIAC_DEFS_H_

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//
// Arcane-FX
// Copyright (C) Faust Logic, Inc.
//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxZodiacDefs
{
public:
  enum 
  {
     MAX_ZODIACS = 256,
     N_ZODIAC_FIELD_INTS = (MAX_ZODIACS - 1) / 32 + 1,
  };

  enum 
  {
    BLEND_NORMAL          = 0x0,
    BLEND_ADDITIVE        = 0x1,
    BLEND_SUBTRACTIVE     = 0x2,
    BLEND_RESERVED        = 0x3,
    BLEND_MASK            = 0x3,
    //
    SHOW_ON_TERRAIN       = BIT(2),
    SHOW_ON_INTERIORS     = BIT(3),
    SHOW_ON_WATER         = BIT(4),
    SHOW_ON_MODELS        = BIT(5),
    RESPECT_ORIENTATION   = BIT(6),
    //
    INTERIOR_VERT_IGNORE  = BIT(7),
    INTERIOR_HORIZ_ONLY   = BIT(8),
    INTERIOR_BACK_IGNORE  = BIT(9),
    INTERIOR_FILTERS = INTERIOR_VERT_IGNORE | INTERIOR_HORIZ_ONLY | INTERIOR_BACK_IGNORE
  };
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

class afxZodiacBitmask : public afxZodiacDefs
{
  U32 mBits[N_ZODIAC_FIELD_INTS];
  
public:
  afxZodiacBitmask() { clear(); }
  afxZodiacBitmask(const afxZodiacBitmask& field) { *this = field; }
  
  bool test(U32 index)
  {
    U32 word = index / 32;
    U32 bit = index % 32;
    return mBits[word] & (1 << bit);
  }
  
  void set(U32 index)
  {
    U32 word = index / 32;
    U32 bit = index % 32;
    mBits[word] |= 1 << bit;
  }
  
  void unset(U32 index)
  {
    U32 word = index / 32;
    U32 bit = index % 32;
    mBits[word] &= ~(1 << bit);
  }
  
  S32 findLastSetBit(U32 startbit=(MAX_ZODIACS-1))
  {
    U32 word = startbit / 32;
    U32 bit = startbit % 32;

    if (mBits[word] != 0)
    { 
      U32 mask = mBits[word] << (31-bit);
      for (U32 j = bit; j >= 0; j--)
      {
        if (mask & 0x80000000)
          return word*32 + j;
        mask <<= 1;
      }
    }
    
    for (U32 k = word-1; k >= 0; k--)
    {
      if (mBits[k] != 0)
      { 
        U32 mask = mBits[k];
        for (U32 j = 31; j >= 0; j--)
        {
          if (mask & 0x80000000)
            return k*32 + j;
          mask <<= 1;
        }
      }
    }

    return -1;
  }
  
  void clear()
  {
    for (U32 k = 0; k < N_ZODIAC_FIELD_INTS; k++)
      mBits[k] = 0x00000000;
  }
  
  bool isEmpty()
  {
    for (U32 k = 0; k < N_ZODIAC_FIELD_INTS; k++)
      if (mBits[k] != 0)
        return false;
      return true;
  }
  
  afxZodiacBitmask& operator=(const afxZodiacBitmask& field)
  {
    for (U32 k = 0; k < N_ZODIAC_FIELD_INTS; k++)
      mBits[k] = field.mBits[k];
    return *this;
  }
  
  operator bool() { return !isEmpty(); }
};

//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~//~~~~~~~~~~~~~~~~~~~~~//

#endif // _AFX_ZODIAC_DEFS_H_
