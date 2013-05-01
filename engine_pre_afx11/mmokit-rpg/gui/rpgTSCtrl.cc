


#include "rpgTSCtrl.h"
#include "console/consoleTypes.h"
#include "game/projectile.h"
#include "game/gameBase.h"
#include "game/gameConnection.h"
#include "game/shapeBase.h"
#include "game/tsStatic.h"
#include "interior/interiorInstance.h"
#include "rpg/game/rpgPlayer.h"


IMPLEMENT_CONOBJECT(rpgTSCtrl);


static bool gMouseLookToggle = false;
static rpgTSCtrl* grpgTSCtrl = NULL;

rpgTSCtrl::rpgTSCtrl()
{
   grpgTSCtrl = this;
}

rpgTSCtrl::~rpgTSCtrl()
{
   grpgTSCtrl = NULL;
}


//mouse events

ConsoleFunction(ToggleMouseLook,void,2,2,"ToggleMouseLook()")
{
   
   bool v = dAtob(argv[1]);

   if (!v)
      return;

   //if (v != Canvas->isCursorON())
   //   return;

   
   
   if (!grpgTSCtrl)
   {
      gMouseLookToggle = false;
      return;
   }

   gMouseLookToggle = !gMouseLookToggle;
   
   if (gMouseLookToggle)
   {
      Platform::setWindowLocked(true);
      Canvas->setCursorON(false);
      grpgTSCtrl->setFirstResponder();   
   }
   else
   {
      Platform::setWindowLocked(false);
      Canvas->setCursorON(true);
      grpgTSCtrl->clearFirstResponder();   
   }
}

ConsoleFunction(ToggleMouseLookBlah,void,2,2,"ToggleMouseLookBlah()")
{

   bool v = dAtob(argv[1]);

   if (v != Canvas->isCursorON())
      return;

   if (!grpgTSCtrl)
   {
      gMouseLookToggle = false;
      return;
   }

   gMouseLookToggle = !gMouseLookToggle;

   if (gMouseLookToggle)
   {
      Platform::setWindowLocked(true);
      Canvas->setCursorON(false);
      grpgTSCtrl->setFirstResponder();   
   }
   else
   {
      Platform::setWindowLocked(false);
      Canvas->setCursorON(true);
      grpgTSCtrl->clearFirstResponder();   
   }
}

bool gAutoWalkin = false;
void rpgTSCtrl::onMouseUp(const GuiEvent & event)
{
    if (!gAutoWalkin)
    {
        make3DMouseEvent(mLastEvent, event);
        on3DMouseDown(mLastEvent);
    }

    gAutoWalkin = false;
}

void rpgTSCtrl::onRightMouseDown(const GuiEvent & event)
{
   gMouseLookToggle = true;
   // ok, gotta disable the mouse
   // script functions are lockMouse(true); Canvas.cursorOff();
   Platform::setWindowLocked(true);
   Canvas->setCursorON(false);
   setFirstResponder();
}

void rpgTSCtrl::onRightMouseUp(const GuiEvent & event)
{

}


bool rpgTSCtrl::onInputEvent(const InputEvent & event)
{
   if(event.deviceType == MouseDeviceType &&
      event.objInst == KEY_BUTTON1 && event.action == SI_BREAK)
   {
      // if the right mouse pass thru is enabled,
      // we want to reactivate mouse on a right mouse button up
      gMouseLookToggle = false;
      Platform::setWindowLocked(false);
      Canvas->setCursorON(true);
      clearFirstResponder();
   }
   // we return false so that the canvas can properly process the right mouse button up...
   return false;
}



//original function

bool rpgTSCtrl::processCameraQuery(CameraQuery *camq)
{
   GameUpdateCameraFov();
   bool result = GameProcessCameraQuery(camq);

   smCamMatrix = camq->cameraMatrix;
   smCamMatrix.getColumn(3,&smCamPos);
   smCamRot.set(0,0,0);

   return result;
}


//from EditTSCtrl
/*
bool rpgTSCtrl::processCameraQuery(CameraQuery * query)
{

   GameConnection* connection = dynamic_cast<GameConnection *>(NetConnection::getConnectionToServer());
   if (connection)
   {
      if (connection->getControlCameraTransform(0.032,&query->cameraMatrix)) {
         query->nearPlane = 0.1;
         query->farPlane = getMax(smVisibleDistance, 50.f);
         query->fov = 3.1415 / 2;

         smCamMatrix = query->cameraMatrix;
         smCamMatrix.getColumn(3,&smCamPos);
         smCamRot.set(0,0,0);
         return(true);
      }
   }
   return(false);
}
*/

//---------------------------------------------------------------------------
void rpgTSCtrl::renderWorld(const RectI &updateRect)
{
   GameRenderWorld();
   dglSetClipRect(updateRect);
}


void rpgTSCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   // check if should bother with a render
   GameConnection * con = GameConnection::getConnectionToServer();
   bool skipRender = !con || (con->getWhiteOut() >= 1.f) || (con->getDamageFlash() >= 1.f) || (con->getBlackOut() >= 1.f);

   if(!skipRender)
   {
      Parent::onRender(offset, updateRect);
      renderChildControls(offset, updateRect);
   }

   dglSetViewport(updateRect);
   CameraQuery camq;
   if(GameProcessCameraQuery(&camq))  
      GameRenderFilters(camq);
}

void rpgTSCtrl::consoleInit()
{
 
}

class gmSelectionEvent : public NetEvent
{
   
public:
   
   Point3F m_startPnt;
   Point3F m_vec;
   U8      m_doubleClick;
   U8      m_modifier; //ctrl for corpse loot, etc
   U8      m_charIndex;//character index into party


   gmSelectionEvent()
      {
        mGuaranteeType = NetEvent::Unguaranteed;
      }
   ~gmSelectionEvent(){};
   
   virtual void pack(NetConnection* ps, BitStream *bstream)
      {
         bstream->writeCompressedPoint(m_startPnt);
         bstream->writeNormalVector(m_vec,10);
         bstream->write(m_modifier);
         bstream->write(m_doubleClick);
         bstream->write(m_charIndex);
      }
   virtual void write(NetConnection* n, BitStream *bstream)
      {  
        pack(n,bstream);
     }
   virtual void unpack(NetConnection* ps, BitStream *bstream)
      {
         bstream->readCompressedPoint(&m_startPnt);
         bstream->readNormalVector(&m_vec,10);
         bstream->read(&m_modifier);
         bstream->read(&m_doubleClick);
         bstream->read(&m_charIndex);
      }
   virtual void process(NetConnection *connection)
      {
#ifdef DARREN_MMO
         SceneObject* so=NULL;
         GameConnection* gc=NULL;
         
         if(connection)
         {
            gc = (dynamic_cast<GameConnection*> (connection));
         }

         AssertFatal(gc, "SelectionEvent::process - no game connection?");

         so = (dynamic_cast<SceneObject*>(gc->getControlObject()));

         // turn off the collsion with the control object
         Point3F avatarPos;
         avatarPos.set(0,0,0);
         
         if (so)
         {
            so->disableCollision();
            avatarPos=so->getPosition();
         }

         //
         bool  corpseSelect = false;
         F32  mProjectDistance = 120.0f; //move to member

         if (m_modifier&SI_CTRL)
         {
            mProjectDistance = 10.0f;
            corpseSelect = true;
         }
                

         Point3F startPnt = m_startPnt;
         Point3F endPnt = m_startPnt + m_vec * mProjectDistance;

         //Con::printf("%f %f %f : %f %f %f\n",startPnt.x,startPnt.y,startPnt.z,m_vec.x,m_vec.y,m_vec.z);

         //
         RayInfo ri;
         ri.includeInsideBox = true;
         bool hit=false;
         int x;

         

         U32 mask = 0;
         SceneObject* selected = NULL;
         
         mask = PlayerObjectType;
         if (corpseSelect)
            mask = CorpseObjectType;

         Vector<SceneObject*> hitPlayers;
         Vector<SceneObject*> hitCorpses;
         Vector<SceneObject*> pets;

            if (mask != CorpseObjectType)
            {
                rpgPlayer *ap = dynamic_cast<rpgPlayer*>(so);
                while (gServerContainer.collideBox(startPnt, endPnt, mask, &ri))
                {
                    ri.object->disableCollision();
                    
                    rpgPlayer* p = dynamic_cast<rpgPlayer*>(ri.object);
                    
                    if (p->mPlayerPet && !p->allowHarmful(ap))
                        pets.push_back(ri.object);
                    else
                        hitPlayers.push_back(ri.object);
                }
                
                //enabled collision
                for (x = 0; x < hitPlayers.size(); x++)
                    hitPlayers[x]->enableCollision();
                
                for (x = 0; x < pets.size(); x++)
                    pets[x]->enableCollision();
            }

         if (!hitPlayers.size() && (mask==CorpseObjectType || m_doubleClick))
         {
            while (gServerContainer.collideBox(startPnt, endPnt, CorpseObjectType, &ri))
            {
               ri.object->disableCollision();
               hitCorpses.push_back(ri.object);
            }

            //enabled collision
            for (int x = 0; x < hitCorpses.size(); x++)
               hitCorpses[x]->enableCollision();
         }
         

         if (hitPlayers.size())
         {
            //select one that is not in last select if possible
            for(Vector<SceneObject*>::iterator itr = hitPlayers.begin(); itr != hitPlayers.end(); itr++)
            {
               for (x = 0; x < gc->mLastSelect.size(); x++)
                  if ((*itr)->getId() == gc->mLastSelect[x])
                     break;

               if (x == gc->mLastSelect.size())
               {
                  //got one
                  selected = (*itr);
                  gc->mLastSelect.push_back(selected->getId());
                  break;
               }
            }
            //loop to first
            if (!selected)
            {
               gc->mLastSelect.clear();
               selected = hitPlayers[0];
               gc->mLastSelect.push_back(selected->getId());
            }
         }
     
         if (!selected && hitCorpses.size())
         {
            //select one that is not in last select if possible
            for(Vector<SceneObject*>::iterator itr = hitCorpses.begin(); itr != hitCorpses.end(); itr++)
            {
               for (x = 0; x < gc->mLastSelectCorpse.size(); x++)
                  if ((*itr)->getId() == gc->mLastSelectCorpse[x])
                     break;

               if (x == gc->mLastSelectCorpse.size())
               {
                  //got one
                  selected = (*itr);
                  gc->mLastSelectCorpse.push_back(selected->getId());
                  break;
               }
            }
            //loop to first
            if (!selected)
            {
               gc->mLastSelectCorpse.clear();
               selected = hitCorpses[0];
               gc->mLastSelectCorpse.push_back(selected->getId());
            }
         }

         

         if(selected)
         {
            //REMEMBER WE ARE SERVER SIDE HERE... 

            //getIdString uses a static buffer so it is the same
            //value when we do both the object and connection..
            //thing is, in a member function I thought a static
            //was unique?  compiler dependent?
            char wtf[64];
            dSprintf(wtf,64,"%s",selected->getIdString());

            //call the Console select function
            
            int doubleClick = m_doubleClick;
            const char* vret = Con::evaluatef("Py::Select(\"%s\",\"%s\",%i, %i, %i);",so->getIdString(),wtf,m_charIndex,doubleClick,m_modifier&SI_SHIFT);
            
            AssertFatal(ri.object, "SelectionEvent::collide - client container returned non SceneObject");
         }
         else
         {
            if (m_doubleClick)
            {
               //check if we clicked a trigger tsstatic
               hit = gServerContainer.castRay(startPnt, endPnt, StaticTSObjectType, &ri);
               if (hit)
               {
                  TSStatic* obj = dynamic_cast< TSStatic *>(ri.object);
                  if (!obj)
                     hit = false;
                  else                  
                  {
                     Point3F dist = avatarPos - ri.point;

                     if (obj->mDialogTrigger == "" || dist.len() > obj->mDialogRange)
                        hit = false;
                  
                     else
                     {
                        Con::evaluatef("Py::OnDialogTrigger(\"%s\",\"%s\");",so->getIdString(),obj->mDialogTrigger);
                        if (so)
                           so->enableCollision();
                        return;
                     }
                  }
               }

               //now check for a bindstone... HHHHAAACCCCKKKKK
               //endPnt = m_startPnt + m_vec * 10.f;
               hit = gServerContainer.castRay(startPnt, endPnt, InteriorObjectType, &ri);
               if (hit)
               {
                  InteriorInstance* obj = dynamic_cast< InteriorInstance *>(ri.object);
                  if (!obj)
                  {
                     hit = false;
                  }
                  else
                  {
                     Point3F dist = avatarPos - ri.point;
                     if (dist.len() < 10.f)
                     {
                     
                        const AbstractClassRep::Field* f;    
                        f = obj->getClassRep()->findField(StringTable->insert("interiorFile"));
                        if (f)
                        {
                           const char* filename = Con::getData(f->type, (void *) (S32(obj) + f->offset),0,  f->table, f->flag);
                           if (filename)
                           {
                              if (!dStricmp("minions.of.mirth/data/interiors/architecture/bindpoint.dif",filename))
                              {                              
                                 Con::evaluatef("Py::OnBindTrigger(\"%s\");",so->getIdString());
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
            
            
         if(so)
            so->enableCollision();
#endif //DARREN_MMO
     }

   DECLARE_CONOBJECT(gmSelectionEvent);
};

IMPLEMENT_CO_SERVEREVENT_V1(gmSelectionEvent);

/*
ConsoleMethod(NetConnection, sendMsg, void, 3, 3, "con.sendMsg(messageString)")
{
   (NetConnection *) object)->postNetEvent(new SimpleMessageEvent(argv[2]));
}
*/

void rpgTSCtrl::onLoseFirstResponder()
{
   Platform::setWindowLocked(false);
   Canvas->setCursorON(true);
   
   Parent::onLoseFirstResponder();
}

void rpgTSCtrl::on3DMouseDown(const gm3DMouseEvent & event)
{
    //mouseLock(); is this necessary?

   NetConnection * connection=NetConnection::getConnectionToServer();
   if (!connection)
      return;
   
   
   StringTableEntry PyCharacterIndex = Con::getVariable("$Py::SelectionCharacterIndex");
   if (!PyCharacterIndex[0])
      return;
   //AssertISV(PyCharacterIndex[0],"$Py::SelectionCharacterIndex is not initialized");

   U8 charIndex = (U8) dAtoi(PyCharacterIndex);

   gmSelectionEvent* selev= new gmSelectionEvent;
   selev->m_startPnt=event.pos;
   selev->m_vec=event.vec;
   selev->m_modifier = event.modifier;
   selev->m_charIndex = charIndex;
   selev->m_doubleClick = event.mouseClickCount > 1;

   connection->postNetEvent(selev);
}

void rpgTSCtrl::make3DMouseEvent(gm3DMouseEvent & gui3DMouseEvent, const GuiEvent & event)
{
   (GuiEvent&)(gui3DMouseEvent) = event;
   
   // get the eye pos and the mouse vec from that...
   //is this the corner of the cursor or the middle?
   Point3F sp(event.mousePoint.x, event.mousePoint.y, 1);

   Point3F wp;
   unproject(sp, &wp);
   
   gui3DMouseEvent.pos = smCamPos;
   gui3DMouseEvent.vec = wp - smCamPos;
   gui3DMouseEvent.vec.normalize();
   
   gui3DMouseEvent.modifier=event.modifier;
   gui3DMouseEvent.mouseClickCount = event.mouseClickCount;

}

bool rpgTSCtrl::onMouseWheelUp(const GuiEvent &event)
{
   if ( !mAwake || !mVisible )
      return( false );

//DARREN_MMO   MoveManager::mZoom+=.1f;

   return( true );
}

bool rpgTSCtrl::onMouseWheelDown(const GuiEvent &event)
{
   if ( !mAwake || !mVisible )
      return( false );
   
//DARREN_MMO   MoveManager::mZoom-=.1f;

   return( true );
}



