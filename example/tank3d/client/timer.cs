// ============================================================
// Project            :  TankAttack3D
// File               :  .\tank3d\client\timer.cs
// Copyright          :  
// Author             :  Ezham
// Created on         :  Thursday, June 07, 2007 12:26 PM
//
// Editor             :  Codeweaver v. 1.2.2685.32755
//
// Description        :  
//                    :  
//                    :  
// ============================================================

function clientCmdUnpauseCDC()
{
	CDM();
}
function clientCmdPauseCDC()
{
	cancel($CDS);
}
function clientCmdStopCDC()
{
	cancel($CDS);
	CDTime.visible = 0;
	CDBG.visible = 0;
	CDBGR.visible = 0;
}
function clientCmdSetCDC(%min)
{
	cancel($CDS);
	CDTime.visible = 1;
	//CDBG.visible = 1;
	CDTime.setValue(%min@":00");
	$Min = %min;
	$Sec = "a";
	CDM();
}

function CDM()
{
	if($Sec $= "a")
	{
		$Min--;
		$Sec = "59";
	}
	else
	{
		$Sec--;
	}

	if($Sec $= "0" && $Min $= "0")		// END GAME
	{
		CDTime.visible = 0;
		CDBGR.visible = 0;
		cancel($CDS);
		CDTime.setValue($Zero2@$Min@":"@$Zero@$sec);
		commandToServer('StopGameLoop');						// END GAME LOOP (server/engine.cs)
	}
	else
	{
		if($Sec $= "-1")
		{
			$Sec = "59";
			$Min--;
		}
		if($Sec <= "9")
		{
			$Zero = "0";
		}
		else
		{
			$Zero = "";
		}
		if($min <= "9")
		{
			$Zero2 = "0";
		}
		else
		{
			$Zero2 = "";
		}
		if($Min $= "0" && $Sec $= "10")
		{
			//CDBGR.visible = 1;
			//CDBG.visible = 0;
		}

		CDTime.setValue($Zero2@$Min@":"@$Zero@$sec);
		%cdschedule = schedule(1000,0,"CDM");
		$CDS = %cdschedule;
	}
}