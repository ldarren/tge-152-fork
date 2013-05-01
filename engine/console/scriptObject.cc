//-----------------------------------------------------------------------------
// Torque Game Engine
// Copyright (C) GarageGames.com, Inc.
//-----------------------------------------------------------------------------

#include "platform/platform.h"
#include "console/simBase.h"
#include "console/consoleTypes.h"

//-----------------------------------------------------------------------------
// Script object placeholder
//-----------------------------------------------------------------------------

class ScriptObject : public SimObject
{
   typedef SimObject Parent;
   StringTableEntry mClassName;
   StringTableEntry mSuperClassName;
public:
   ScriptObject();
   bool onAdd();
   void onRemove();

   DECLARE_CONOBJECT(ScriptObject);

   static void initPersistFields();
};

IMPLEMENT_CONOBJECT(ScriptObject);

void ScriptObject::initPersistFields()
{
   addGroup("Classes", "Script objects have the ability to inherit and have class information.");
   addField("class", TypeString, Offset(mClassName, ScriptObject), "Class of object.");
   addField("superClass", TypeString, Offset(mSuperClassName, ScriptObject), "Superclass of object.");
   endGroup("Classes");
}

ScriptObject::ScriptObject()
{
   mClassName = "";
   mSuperClassName = "";
}

bool ScriptObject::onAdd()
{
   if (!Parent::onAdd())
      return false;

   // it's possible that all the namespace links can fail, if
   // multiple objects are named the same thing with different script
   // hierarchies.
   // linkNamespaces will now return false and echo an error message
   // rather than asserting.

   // superClassName -> ScriptObject
   StringTableEntry parent = StringTable->insert("ScriptObject");
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

   // Call onAdd in script!
   Con::executef(this, 2, "onAdd", Con::getIntArg(getId()));
   return true;
}

void ScriptObject::onRemove()
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

   Parent::onRemove();
}

//-----------------------------------------------------------------------------
// Script group placeholder
//-----------------------------------------------------------------------------

class ScriptGroup : public SimGroup
{
   typedef SimGroup Parent;
   StringTableEntry mClassName;
   StringTableEntry mSuperClassName;
public:
   ScriptGroup();
   bool onAdd();
   void onRemove();

   DECLARE_CONOBJECT(ScriptGroup);

   static void initPersistFields();
};

IMPLEMENT_CONOBJECT(ScriptGroup);

void ScriptGroup::initPersistFields()
{
   addGroup("Classes");
   addField("class", TypeString, Offset(mClassName, ScriptGroup));
   addField("superClass", TypeString, Offset(mSuperClassName, ScriptGroup));
   endGroup("Classes");
}

ScriptGroup::ScriptGroup()
{
   mClassName = "";
   mSuperClassName = "";
}

bool ScriptGroup::onAdd()
{
   if (!Parent::onAdd())
      return false;

   // superClassName -> ScriptGroup
   StringTableEntry parent = StringTable->insert("ScriptGroup");
   if(mSuperClassName[0])
   {
      if(Con::linkNamespaces(parent, mSuperClassName))
         parent = mSuperClassName;
   }

   // className -> superClassName
   if(mClassName[0])
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

   // Call onAdd in script!
   Con::executef(this, 2, "onAdd", Con::getIntArg(getId()));
   return true;
}

void ScriptGroup::onRemove()
{
   // Call onRemove in script!
   Con::executef(this, 2, "onRemove", Con::getIntArg(getId()));
   Parent::onRemove();
}

//-----------------------------------------------------------------------------
// Script Class placeholder
//-----------------------------------------------------------------------------
class ScriptClass : public SimObject
{
   typedef SimObject Parent;
   StringTableEntry mLibraryName;
   StringTableEntry mClassName;
public:
   ScriptClass();

   DECLARE_CONOBJECT(ScriptClass);

   bool onAdd();

   static void initPersistFields();
};

IMPLEMENT_CONOBJECT(ScriptClass);

void ScriptClass::initPersistFields()
{
   Parent::initPersistFields();

   addGroup("ClassLibrary", "Script objects have the ability to inherit and have class information.");
   addField("libraryName", TypeString, Offset(mLibraryName, ScriptClass), "Class Library This Belongs to.");
   addField("className", TypeString, Offset(mClassName, ScriptClass), "Script Class Namespace this object defines.");
   endGroup("ClassLibrary");

}

ScriptClass::ScriptClass()
{
   mLibraryName = "";
   mClassName = "";
}


bool ScriptClass::onAdd()
{
   SimGroup *scriptClassGroup = Sim::getScriptClassGroup();
   if( !scriptClassGroup )
   {
      Con::errorf("ScriptClass::onAdd - No ScriptClassGroup found!");
      return false;
   }

   if (!Parent::onAdd())
      return false;

   // No library goes in root.
   if( !mLibraryName || mLibraryName == "" )
   {
      scriptClassGroup->addObject( this );
      return true;
   }

   SimGroup *libraryGroup = dynamic_cast<SimGroup*>( scriptClassGroup->findObjectByInternalName( mLibraryName ) );
   if( libraryGroup != NULL )
   {
      libraryGroup->addObject( this );
      return true;
   }

   libraryGroup = new SimGroup();
   if ( libraryGroup == NULL )
   {
      Con::errorf("ScriptClass::onAdd - Unable to create non-existent Script ClassLibrary %s!", mLibraryName );
      return false;
   }

   // Register the Script ClassLibrary SimGroup
   libraryGroup->registerObject();

   // Set Internal Name
   libraryGroup->setInternalName( StringTable->insert( mLibraryName ) );

   // Add to ScriptClassGroup SimGroup
   scriptClassGroup->addObject( libraryGroup );

   // Add ourselves to our new Script ClassLibrary
   libraryGroup->addObject( this );

   // Success!
   return true;

}
