The GUI files are organized now based on broad categories:

   containers  Windows, layout helper controls, controls that contain
               other controls.
   controls    Buttons, text boxes, and so forth.
   core        This contains base GUI code, like GuiControl and GuiCanvas.
   editor      Specialized editor controls.
   game        Game specific controls, like progress bars or HUD elements.
   utility     Controls with specialized, usually invisible, purposes.

The goal is to provide developers with hints about what purpose code serves,
so that they can easily work with and extend GUI controls.