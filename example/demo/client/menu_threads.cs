
// Stack used to track what gui thread we are viewing
new SimSet(menu_thread_stack);

// Define various topic threads
new SimSet(main_menu_thread);
   main_menu_thread.add(MainMenuDlg);

new SimSet(overview_thread);
   //overview_thread.add(overview_main);

new SimSet(features_thread);
   features_thread.add(features_main);
  
new SimSet(products_thread);
   products_thread.add(product_main);

package AutoLoad_GuiMLTextCtrl_Contents
{
   function GuiMLTextCtrl::onWake(%this)
   {
      if (%this.filename !$= "")
      {
         %fo = new FileObject();
         %fo.openForRead(%this.filename);
         %text = "";
         while(!%fo.isEOF())
            %text = %text @ %fo.readLine() @ "\n";

         %fo.delete();
         %this.setText(%text);
      }
      //parent::onWake(%this);
   }

   function gotoWebPage(%url)
   {
      if(isFullScreen())
         toggleFullScreen();
      Parent::gotoWebPage(%url);
   }

   function GuiMLTextCtrl::onURL(%this, %url)
   {
      if (getSubStr(%url, 0, 9) $= "gamelink ")
         eval( getSubStr(%url, 9, 1024) );
      else
         gotoWebPage( %url );
   }   

};

activatePackage(AutoLoad_GuiMLTextCtrl_Contents);

