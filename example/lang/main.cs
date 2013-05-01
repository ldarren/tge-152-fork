function onStart()
{
	// setup localization:
	$I18N::starter.fps = new LangTable();
	exec("starter.fps/lang/tables/German.cs");
	exec("starter.fps/lang/tables/English.cs");
	$I18N::starter.fps.addLanguage("starter.fps/lang/tables/German.lso", "German");
	$I18N::starter.fps.addLanguage("starter.fps/lang/tables/English.lso", "English");
	$I18N::starter.fps.setDefaultLanguage(0); // German is default here
	$I18N::starter.fps.setCurrentLanguage(0); // German is current
	//$I18N::starter.fps.setCurrentLanguage(1); // this would set the current language to english
}