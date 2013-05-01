---------------------------------------------------------------------
-- Torque DTS exporter utility for 3ds Max 6/7/8
---------------------------------------------------------------------
-- Written by Dark Industries
-- Version 1.3.1 RC3
-- Last Updated: 5/31/2006
---------------------------------------------------------------------

---------------------------------------------------------------------
-- convert to boolean
---------------------------------------------------------------------
function ConvertToBool state =
(
	if state == "true" then
	(
		newState = true
	)
	else newState = false
	newState
)

---------------------------------------------------------------------
-- create/load global variables
---------------------------------------------------------------------
Global gDTSUtility_rollout, DTSUtilityDialog, QuickExport_rollout, Bounds_rollout, Detail_rollout
Global Sequence_rollout, Node_rollout, Billboard_rollout, Trigger_rollout, Settings_rollout
Global Collision_rollout, Material_rollout, RenameMaterial_rollout, Help_rollout, NewSequence_rollout
Global BoundsTarget_rollout, CFGSettings_rollout, Visibility_rollout, DMP_rollout
Global SelectedNode, SelectedMaterial, SelectedMaterialID, SelectedMaterialType
Global About_rollout, GrayBar_bmp, UiHeight, UiX, UiY, UiMode, ShowDMP, UiMin, DmpPath, AutoCFG
Global BoundsPadX, BoundsPadY, BoundsPadY, NodeX, NodeY, NodeZ, ShapeX, ShapeY, ShapeZ
Global QuickExportOpen, BoundsOpen, DetaiOpenl, BillboardOpen, NodeOpen, SequenceOpen
Global TriggerOpen, VisibilityOpen, CollisionOpen, MaterialOpen, HelpPath
Global FreezeShape, FreezeMarkers

GrayBar_bmp = openBitMap "$scripts\dts\dts_bar.bmp"
DmpPath = ""

---------------------------------------------------------------------
-- update global variables from ini
---------------------------------------------------------------------
UiMin = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Interface" "UiMin" )
UiHeight = (getINISetting "$scripts\dts_prefs.ini" "Interface" "UiHeight" as integer)
UiX = (getINISetting "$scripts\dts_prefs.ini" "Interface" "UiX" as integer)
UiY = (getINISetting "$scripts\dts_prefs.ini" "Interface" "UiY" as integer)
BoundsPadX = (getINISetting "$scripts\dts_prefs.ini" "Settings" "BoundsPadX" as float)
BoundsPadY = (getINISetting "$scripts\dts_prefs.ini" "Settings" "BoundsPadY" as float)
BoundsPadZ = (getINISetting "$scripts\dts_prefs.ini" "Settings" "BoundsPadZ" as float)
NodeX = (getINISetting "$scripts\dts_prefs.ini" "Settings" "NodeX" as float)
NodeY = (getINISetting "$scripts\dts_prefs.ini" "Settings" "NodeY" as float)
NodeZ = (getINISetting "$scripts\dts_prefs.ini" "Settings" "NodeZ" as float)
ShapeX = (getINISetting "$scripts\dts_prefs.ini" "Settings" "ShapeX" as float)
ShapeY = (getINISetting "$scripts\dts_prefs.ini" "Settings" "ShapeY" as float)
ShapeZ = (getINISetting "$scripts\dts_prefs.ini" "Settings" "ShapeZ" as float)
FreezeShape = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Settings" "FreezeShape" )
FreezeMarkers = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Settings" "FreezeMarkers" )
AutoCFG = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Settings" "AutoCFG" )
ShowDMP = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Settings" "ShowDMP" )
HelpPath = getINISetting "$scripts\dts_prefs.ini" "Settings" "HelpPath"

---------------------------------------------------------------------
-- load rollout states from ini
---------------------------------------------------------------------
QuickExportOpen = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Interface" "QuickExport" )
BoundsOpen = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Interface" "Bounds" )
DetailOpen = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Interface" "Detail" )
BillboardOpen = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Interface" "Billboard" )
NodeOpen = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Interface" "Node" )
SequenceOpen = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Interface" "Sequence" )
TriggerOpen = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Interface" "Trigger" )
VisibilityOpen = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Interface" "Visibility" )
CollisionOpen = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Interface" "Collision" )
MaterialOpen = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Interface" "Material" )

---------------------------------------------------------------------
-- create DTS shape
---------------------------------------------------------------------
function CreateDTSShape =
(
	if $base01 == null then dummy name: "base01" boxsize:[ShapeX, ShapeY, ShapeZ]
	if $start01 == null then dummy name: "start01" boxsize:[ShapeX, ShapeY, ShapeZ]
	$start01.parent = $base01
	if FreezeShape == true then
	(
		$base01.isFrozen = true
		$start01.isFrozen = true
	)
	selection = $detail*
	selection.parent = $base01
)

---------------------------------------------------------------------
-- remove detail markers
---------------------------------------------------------------------
function DeleteDetailMarkers = 
(
	selection = $detail*
	if selection.count != 0 then delete selection
)

---------------------------------------------------------------------
-- create detail markers from array
---------------------------------------------------------------------
function CreateDetailMarkers arr = 
(
	for i = 1 to arr.count do
	(
		detailName = "detail" + arr[i]
		d = dummy name: detailName
		d.boxsize = [ShapeX, ShapeY, ShapeZ]
		d.parent = $base01
		if FreezeMarkers == true then d.isFrozen = true
	)
)

---------------------------------------------------------------------
-- create bounds object
---------------------------------------------------------------------
function MakeBounds align xPad yPad zPad = (
	if selection.count != 0 then
	(
		-- delete the current bounds
		try(delete $bounds) catch()
		
		-- get the bounds of the selected object(s)
		w = (selection.max.x - selection.min.x) + xPad
		l = (selection.max.y - selection.min.y) + yPad
		
		if align == "bottom" then (
			h = selection.max.z - selection.min.z
			c = selection.center - [0, 0, (h/2)]
			h = (h + zPad/2)
			if selection.min.z > 0 then (
				h = h + selection.min.z
				c.z = 0
			)
		)
		else if align == "center" then (
			h = selection.max.z - selection.min.z
			c = selection.center - [0, 0, (h/2)]
		)		
		-- create the bounds mesh
		b = Box length:l width:w height:h pos:c
		b.name = "bounds"
		b.boxmode = on
		converttomesh b
		
		setInheritanceFlags $bounds #{1,2}
	)
)

---------------------------------------------------------------------
-- remove custom user property
---------------------------------------------------------------------
function RemoveUserProp propObj propName propVal =
(
	curProp = getUserPropBuffer propObj
	searchStr = "\r\n" + propName + " = " + (propVal as string)

	starStr = findString curProp searchStr
	if starStr != undefined then
	(
		endStr = searchStr.count
		propNew = replace curProp starStr endStr ""
		setUserPropBuffer propObj propNew
	)
)

---------------------------------------------------------------------
-- apply node preset
---------------------------------------------------------------------
function ApplyNodePreset preset =
(
	setNames = getINISetting "$scripts\dts_nodes.ini" "NodeSets" preset
	presetList = (filterString setNames ", -")
	if presetList.count != 0 then
	(
		presetList
		CreateDTSShape()
		
		for name in presetList do
		(
			n = getNodeByName name
			if n == null then
			(
				n = dummy name: name
				n.boxsize = [NodeX, NodeY, NodeZ]
				n.parent = $start01
			)
			setUserProp n "alwaysExport" true
			setUserProp n "neverExport" false
			setUserProp n "neverAnimate" false
		)
		try( Node_rollout.UpdateNodes() ) catch() 
	)
)

---------------------------------------------------------------------
-- get size of detail
---------------------------------------------------------------------
function GetDetailSize detail =
(
	baseName = trimright detail "1234567890"
	detailNumber = trimleft detail baseName	as number
	detailNumber
)

---------------------------------------------------------------------
-- create .cfg file
---------------------------------------------------------------------
function CreateConfigFile savePath =
(
	clearSelection()
	select $Helpers
	
	if selection.count != 0 then
	(
		alwaysExportArr = #()
		neverExportArr = #()
		neverAnimateArr = #()
		
		for node in selection do
		(
			if getUserProp node "alwaysExport" == true then append alwaysExportArr node.name
			if getUserProp node "neverExport" == true then append neverExportArr node.name
			if getUserProp node "neverAnimate" == true then append neverAnimateArr node.name
		)
		if savePath != undefined then
		(
			cfgSavePath = savePath
		)
		else
		(
			cfgSavePath = getSavePath caption:"Choose the location to save your .cfg file:"
		)
		
		if cfgSavePath != undefined then
		(
			cfgFilePath = cfgSavePath + "\dtsScene.cfg"
			cfgFile = createfile cfgFilePath
			
				-- write node settings to the text file
				format "%\n" "AlwaysExport:" to:cfgFile
				for name in alwaysExportArr do
				(
					format "%\n" name to:cfgFile
				)
				format "%" "\n" to:cfgFile
				
				format "%\n" "NeverExport:" to:cfgFile
				for name in neverExportArr do
				(
					format "%\n" name to:cfgFile
				)
				format "%" "\n" to:cfgFile
				
				format "%\n" "NeverAnimate:" to:cfgFile
				for name in neverAnimateArr do
				(
					format "%\n" name to:cfgFile
				)
				format "%" "\n" to:cfgFile
				
				-- write cfg parameters to the text file
				if getNodeByName "cfg_settings" != null then
				(
					format "%" "//cfg settings\n" to:cfgFile
					cfg = getNodeByName "cfg_settings"
					
					nodeCollection = "-"
					if getUserProp cfg "NodeCollection" == true then nodeCollection = "+"
					format "%Dump::NodeCollection\n" nodeCollection to:cfgFile
	
					shapeConstruction = "-"
					if getUserProp cfg "ShapeConstruction" == true then shapeConstruction = "+"
					format "%Dump::ShapeConstruction\n" shapeConstruction to:cfgFile
					
					nodeCulling = "-"
					if getUserProp cfg "NodeCulling" == true then nodeCulling = "+"
					format "%Dump::NodeCulling\n" nodeCulling to:cfgFile
					
					nodeStates = "-"
					if getUserProp cfg "NodeStates" == true then nodeStates = "+"
					format "%Dump::NodeStates\n" nodeStates to:cfgFile
					
					nodeStateDetails = "-"
					if getUserProp cfg "NodeStateDetails" == true then nodeStateDetails = "+"
					format "%Dump::NodeStateDetails\n" nodeStateDetails to:cfgFile
					
					objectStates = "-"
					if getUserProp cfg "ObjectStates" == true then objectStates = "+"
					format "%Dump::ObjectStates\n" objectStates to:cfgFile
					
					objectStateDetails = "-"
					if getUserProp cfg "ObjectStateDetails" == true then objectStateDetails = "+"
					format "%Dump::ObjectStateDetails\n" objectStateDetails to:cfgFile
					
					objectOffsets = "-"
					if getUserProp cfg "ObjectOffsets" == true then objectOffsets = "+"
					format "%Dump::ObjectOffsets\n" objectOffsets to:cfgFile
					
					sequenceDetails = "-"
					if getUserProp cfg "SequenceDetails" == true then sequenceDetails = "+"
					format "%Dump::SequenceDetails\n" sequenceDetails to:cfgFile
					
					shapeHierarchy = "-"
					if getUserProp cfg "ShapeHierarchy" == true then shapeHierarchy = "+"
					format "%Dump::ShapeHierarchy\n" shapeHierarchy to:cfgFile
					
					allowEmptySubtrees = "-"
					if getUserProp cfg "AllowEmptySubtrees" == true then allowEmptySubtrees = "+"
					format "%Error::AllowEmptySubtrees\n" allowEmptySubtrees to:cfgFile
					
					allowCrossedDetails = "-"
					if getUserProp cfg "AllowCrossedDetails" == true then allowCrossedDetails = "+"
					format "%Error::AllowCrossedDetails\n" allowCrossedDetails to:cfgFile
					
					allowUnusedMeshes = "-"
					if getUserProp cfg "AllowUnusedMeshes" == true then allowUnusedMeshes = "+"
					format "%Error::AllowUnusedMeshes\n" allowUnusedMeshes to:cfgFile
					
					defaultCyclic = "-"
					if getUserProp cfg "defaultCyclic" == true then defaultCyclic = "+"
					format "%Sequence::defaultCyclic\n" defaultCyclic to:cfgFile
					
					defaultBlend = "-"
					if getUserProp cfg "defaultBlend" == true then defaultBlend = "+"
					format "%Sequence::defaultBlend\n" defaultBlend to:cfgFile
					
					defaultFirstLastFrameSame = "-"
					if getUserProp cfg "defaultFirstLastFrameSame" == true then defaultFirstLastFrameSame = "+"
					format "%Sequence::defaultFirstLastFrameSame\n" defaultFirstLastFrameSame to:cfgFile
					
					defaultUseFrameRate = "-"
					if getUserProp cfg "ObjectOffsets" == true then defaultUseFrameRate = "+"
					format "%Sequence::defaultUseFrameRate\n" defaultFirstLastFrameSame to:cfgFile
					
					defaultIgnoreGroundTransform = "-"
					if getUserProp cfg "defaultIgnoreGroundTransform" == true then defaultIgnoreGroundTransform = "+"
					format "%Sequence::defaultIgnoreGroundTransform\n" defaultIgnoreGroundTransform to:cfgFile
					
					defaultUseGroundFrameRate = "-"
					if getUserProp cfg "defaultUseGroundFrameRate" == true then defaultUseGroundFrameRate = "+"
					format "%Sequence::defaultUseGroundFrameRate\n" defaultUseGroundFrameRate to:cfgFile
					
					defaultEnableMorphAnimation = "-"
					if getUserProp cfg "defaultEnableMorphAnimation" == true then defaultEnableMorphAnimation = "+"
					format "%Sequence::defaultEnableMorphAnimation\n" defaultEnableMorphAnimation to:cfgFile
					
					defaultEnableVisAnimation = "-"
					if getUserProp cfg "defaultEnableVisAnimation" == true then defaultEnableVisAnimation = "+"
					format "%Sequence::defaultEnableVisAnimation\n" defaultEnableVisAnimation to:cfgFile
					
					defaultEnableTransformAnimation = "-"
					if getUserProp cfg "defaultEnableTransformAnimation" == true then defaultEnableTransformAnimation = "+"
					format "%Sequence::defaultEnableTransformAnimation\n" defaultEnableTransformAnimation to:cfgFile
					
					defaultForceMorphAnimation = "-"
					if getUserProp cfg "defaultForceMorphAnimation" == true then defaultForceMorphAnimation = "+"
					format "%Sequence::defaultForceMorphAnimation\n" defaultForceMorphAnimation to:cfgFile
					
					defaultForceVisAnimation = "-"
					if getUserProp cfg "defaultForceVisAnimation" == true then defaultForceVisAnimation = "+"
					format "%Sequence::defaultForceVisAnimation\n" defaultForceVisAnimation to:cfgFile
					
					defaultForceTransformAnimation = "-"
					if getUserProp cfg "defaultForceTransformAnimation" == true then defaultForceTransformAnimation = "+"
					format "%Sequence::defaultForceTransformAnimation\n" defaultForceTransformAnimation to:cfgFile
					
					defaultOverrideDuration = "-"
					if getUserProp cfg "defaultOverrideDuration" == true then defaultOverrideDuration = "+"
					format "%Sequence::defaultOverrideDuration\n" defaultOverrideDuration to:cfgFile
					
					format "=Sequence::defaultFrameRate %\n" (getUserProp cfg "defaultFrameRate") to:cfgFile
					format "=Sequence::defaultGroundFrameRate %\n" (getUserProp cfg "defaultGroundFrameRate") to:cfgFile
					format "=Sequence::defaultDuration %\n" (getUserProp cfg "defaultDuration") to:cfgFile
					format "=Sequence::defaultNumFrames %\n" (getUserProp cfg "defaultNumFrames") to:cfgFile
					format "=Sequence::defaultNumGroundFrames %\n" (getUserProp cfg "defaultNumGroundFrames ") to:cfgFile
					format "=Sequence::defaultDefaultSequencePriority %\n" (getUserProp cfg "defaultDefaultSequencePriority") to:cfgFile
					
					enableTwoSidedMaterials = "-"
					if getUserProp cfg "EnableTwoSidedMaterials" == true then enableTwoSidedMaterials = "+"
					format "%Param::EnableTwoSidedMaterials\n" enableTwoSidedMaterials to:cfgFile
					
					collapseTransforms = "-"
					if getUserProp cfg "CollapseTransforms" == true then collapseTransforms = "+"
					format "%Param::CollapseTransforms\n" collapseTransforms to:cfgFile
					
					sequenceExport = "-"
					if getUserProp cfg "SequenceExport" == true then sequenceExport = "+"
					format "%Param::SequenceExport\n" sequenceExport to:cfgFile
					
					format "=Params::AnimationDelta %\n" (getUserProp cfg "AnimationDelta") to:cfgFile
					format "=Params::MaxFrameRate %\n" (getUserProp cfg "MaxFrameRate") to:cfgFile
					format "=Params::SkinWeightThreshhold %\n" (getUserProp cfg "SkinWeightThreshhold") to:cfgFile
					format "=Params::weightsPerVertex %\n" (getUserProp cfg "weightsPerVertex") to:cfgFile
					format "=Params::baseTexturePath %" "." to:cfgFile
					
					IgnoreOnSkinMesh = "-"
					if getUserProp cfg "IgnoreOnSkinMesh" == true then IgnoreOnSkinMesh = "+"
					format "%SmoothingGroup::IgnoreOnSkinMesh\n" IgnoreOnSkinMesh to:cfgFile
					
					IgnoreDuringCollapse = "-"
					if getUserProp cfg "IgnoreDuringCollapse" == true then IgnoreDuringCollapse = "+"
					format "%SmoothingGroup::IgnoreDuringCollapse\n" IgnoreDuringCollapse to:cfgFile
				)
			close cfgFile
		)
	)
)

---------------------------------------------------------------------
-- load settings from ini
---------------------------------------------------------------------
function ReloadSettings =
(
	ShowDMP = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Settings" "ShowDMP" )
	BoundsPadX = (getINISetting "$scripts\dts_prefs.ini" "Settings" "BoundsPadX" as float)
	BoundsPadY = (getINISetting "$scripts\dts_prefs.ini" "Settings" "BoundsPadY" as float)
	BoundsPadZ = (getINISetting "$scripts\dts_prefs.ini" "Settings" "BoundsPadZ" as float)
	NodeX = (getINISetting "$scripts\dts_prefs.ini" "Settings" "NodeX" as float)
	NodeY = (getINISetting "$scripts\dts_prefs.ini" "Settings" "NodeY" as float)
	NodeZ = (getINISetting "$scripts\dts_prefs.ini" "Settings" "NodeZ" as float)
	ShapeX = (getINISetting "$scripts\dts_prefs.ini" "Settings" "ShapeX" as float)
	ShapeY = (getINISetting "$scripts\dts_prefs.ini" "Settings" "ShapeY" as float)
	ShapeZ = (getINISetting "$scripts\dts_prefs.ini" "Settings" "ShapeZ" as float)
	FreezeShape = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Settings" "FreezeShape" )
	FreezeMarkers = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Settings" "FreezeMarkers" )
	AutoCFG = ConvertToBool( getINISetting "$scripts\dts_prefs.ini" "Settings" "AutoCFG" )
	
	try( Bounds_rollout.spn_xPad.value = BoundsPadX ) catch()
	try( Bounds_rollout.spn_yPad.value = BoundsPadY ) catch()
	try( Bounds_rollout.spn_zPad.value = BoundsPadZ ) catch()
)
---------------------------------------------------------------------
-- Main Exporter UI
---------------------------------------------------------------------
rcmenu ConfigRCmenu
(
	menuItem mi_help "[User Reference]" checked:false
	menuItem mi_dmp "[Show latest DMP]" checked:false
	menuItem mi_quick " Quick Export" checked:false
	menuItem mi_bounds " Bounds" checked:false
	menuItem mi_detail " Details" checked:false
	menuItem mi_bill " Billboards" checked:false
	menuItem mi_nodes " Nodes" checked:false
	menuItem mi_seq " Sequences" checked:false
	menuItem mi_trig " Triggers" checked:false
	menuItem mi_vis " Visibility" checked:false
	menuItem mi_col " Collision" checked:false
	menuItem mi_mat " Materials" checked:false
	menuItem mi_about "[About...]" checked:false
	
	on ConfigRCmenu open do
	(
		mi_quick.checked = QuickExportOpen
		mi_bounds.checked = BoundsOpen
		mi_detail.checked = DetailOpen
		mi_bill.checked = BillboardOpen
		mi_nodes.checked = NodeOpen
		mi_seq.checked = SequenceOpen
		mi_trig.checked = TriggerOpen
		mi_vis.checked = VisibilityOpen
		mi_col.checked = CollisionOpen
		mi_mat.checked = MaterialOpen
	)
	
	on mi_help picked do
	(
		createDialog Help_rollout width:640 height:480
	)
	
	on mi_dmp picked do
	(
		if DmpPath != "" then
		(
			createDialog DMP_rollout width:640 height:480
		)
	)
	
	on mi_about picked do
	(
		createDialog About_rollout width:317 height:166
	)
	
	on mi_quick picked do
	(
		if mi_quick.checked == true then
		(
			mi_quick.checked = false
			QuickExportOpen = false
			removeSubRollout gDTSUtility_rollout.DTSUtilityDialog QuickExport_rollout
		)
		else
		(
			mi_quick.checked = true
			QuickExportOpen = true
			addSubRollout gDTSUtility_rollout.DTSUtilityDialog QuickExport_rollout
		)
		setINISetting "$scripts\dts_prefs.ini" "Interface" "QuickExport" (QuickExportOpen as string)
	)
	
	on mi_bounds picked do
	(
		if mi_bounds.checked == true then
		(
			mi_bounds.checked = false
			BoundsOpen = false
			removeSubRollout gDTSUtility_rollout.DTSUtilityDialog Bounds_rollout
		)
		else
		(
			mi_bounds.checked = true
			BoundsOpen = true
			addSubRollout gDTSUtility_rollout.DTSUtilityDialog Bounds_rollout
		)
		setINISetting "$scripts\dts_prefs.ini" "Interface" "Bounds" (BoundsOpen as string)
	)
	
	on mi_detail picked do
	(
		if mi_detail.checked == true then
		(
			mi_detail.checked = false
			DetailOpen = false
			removeSubRollout gDTSUtility_rollout.DTSUtilityDialog Detail_rollout
		)
		else
		(
			mi_detail.checked = true
			DetailOpen = true
			addSubRollout gDTSUtility_rollout.DTSUtilityDialog Detail_rollout
		)
		setINISetting "$scripts\dts_prefs.ini" "Interface" "Detail" (DetailOpen as string)
	)
	
	on mi_bill picked do
	(
		if mi_bill.checked == true then
		(
			mi_bill.checked = false
			BillboardOpen = false
			removeSubRollout gDTSUtility_rollout.DTSUtilityDialog Billboard_rollout
		)
		else
		(
			mi_bill.checked = true
			BillboardOpen = true
			addSubRollout gDTSUtility_rollout.DTSUtilityDialog Billboard_rollout
		)
		setINISetting "$scripts\dts_prefs.ini" "Interface" "Billboard" (BillboardOpen as string)
	)
	
	on mi_nodes picked do
	(
		if mi_nodes.checked == true then
		(
			mi_nodes.checked = false
			NodeOpen = false
			removeSubRollout gDTSUtility_rollout.DTSUtilityDialog Node_rollout
		)
		else
		(
			mi_nodes.checked = true
			NodeOpen = true
			addSubRollout gDTSUtility_rollout.DTSUtilityDialog Node_rollout
		)
		setINISetting "$scripts\dts_prefs.ini" "Interface" "Node" (NodeOpen as string)
	)
	
	on mi_seq picked do
	(
		if mi_seq.checked == true then
		(
			mi_seq.checked = false
			SequenceOpen = false
			removeSubRollout gDTSUtility_rollout.DTSUtilityDialog Sequence_rollout
		)
		else
		(
			mi_seq.checked = true
			SequenceOpen = true
			addSubRollout gDTSUtility_rollout.DTSUtilityDialog Sequence_rollout
		)
		setINISetting "$scripts\dts_prefs.ini" "Interface" "Sequence" (SequenceOpen as string)
	)
	
	on mi_trig picked do
	(
		if mi_trig.checked == true then
		(
			mi_trig.checked = false
			TriggerOpen = false
			removeSubRollout gDTSUtility_rollout.DTSUtilityDialog Trigger_rollout
		)
		else
		(
			mi_trig.checked = true
			TriggerOpen = true
			addSubRollout gDTSUtility_rollout.DTSUtilityDialog Trigger_rollout
		)
		setINISetting "$scripts\dts_prefs.ini" "Interface" "Trigger" (TriggerOpen as string)
	)
	
	on mi_vis picked do
	(
		if mi_vis.checked == true then
		(
			mi_vis.checked = false
			VisibilityOpen = false
			removeSubRollout gDTSUtility_rollout.DTSUtilityDialog Visibility_rollout
		)
		else
		(
			mi_vis.checked = true
			VisibilityOpen = true
			addSubRollout gDTSUtility_rollout.DTSUtilityDialog Visibility_rollout
		)
		setINISetting "$scripts\dts_prefs.ini" "Interface" "Visibility" (VisibilityOpen as string)
	)
	
	on mi_col picked do
	(
		if mi_col.checked == true then
		(
			mi_col.checked = false
			CollisionOpen = false
			removeSubRollout gDTSUtility_rollout.DTSUtilityDialog Collision_rollout
		)
		else
		(
			mi_col.checked = true
			CollisionOpen = true
			addSubRollout gDTSUtility_rollout.DTSUtilityDialog Collision_rollout
		)
		setINISetting "$scripts\dts_prefs.ini" "Interface" "Collision" (CollisionOpen as string)
	)
	
	on mi_mat picked do
	(
		if mi_mat.checked == true then
		(
			mi_mat.checked = false
			MaterialOpen = false
			removeSubRollout gDTSUtility_rollout.DTSUtilityDialog Material_rollout
		)
		else
		(
			mi_mat.checked = true
			MaterialOpen = true
			addSubRollout gDTSUtility_rollout.DTSUtilityDialog Material_rollout
		)
		setINISetting "$scripts\dts_prefs.ini" "Interface" "Material" (MaterialOpen as string)
	)
)

rollout Settings_rollout "Exporter Settings"
(
	imgTag uiBar "" bitmap:GrayBar_bmp pos:[0,0]
	group "Bounds Settings"
	(
		spinner boundsXpad_spn "X padding: " type: #float range:[0,100,BoundsPadX]
		spinner boundsYpad_spn "Y padding: " type: #float range:[0,100,BoundsPadY]
		spinner boundsZpad_spn "Z padding: " type: #float range:[0,100,BoundsPadZ]
	)
	
	group "DTS Shape\\Marker Settings"
	(
		spinner shapeX_spn "X size: " type: #float range:[0,100,ShapeX]
		spinner shapeY_spn "Y size: " type: #float range:[0,100,ShapeY]
		spinner shapeZ_spn "Z size: " type: #float range:[0,100,ShapeZ]
		checkbox freezeShape_chk "Freeze DTS Shape" checked:FreezeShape across:2
		checkbox freezeMarkers_chk "Freeze Markers" checked:FreezeMarkers
	)
	
	group "Node Settings"
	(
		spinner nodeX_spn "X size: " type: #float range:[0,100,NodeX]
		spinner nodeY_spn "Y size: " type: #float range:[0,100,NodeY]
		spinner nodeZ_spn "Z size: " type: #float range:[0,100,NodeZ]
	)
	
	group "Dump File\\CFG Settings"
	(
		checkbox showDMP_chk "Show DMP file after export" checked:ShowDMP across:2
		checkbox AutoCFG_chk "Create .CFG file on export" checked:AutoCFG
	)
	
	on Settings_rollout close do
	(
		ReloadSettings()
	)
	
	
	-- save bounds settings
	on boundsXpad_spn changed val do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "BoundsPadX" (boundsXpad_spn.value as string)
	)
	on boundsYpad_spn changed val do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "BoundsPadY" (boundsYpad_spn.value as string)
	)
	on boundsZpad_spn changed val do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "BoundsPadZ" (boundsZpad_spn.value as string)
	)
	
	
	-- save shape settings
	on shapeX_spn changed val do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "ShapeX" (shapeX_spn.value as string)
	)
	on shapeY_spn changed val do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "ShapeY" (shapeY_spn.value as string)
	)
	on shapeZ_spn changed val do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "ShapeZ" (shapeZ_spn.value as string)
	)
	on freezeShape_chk changed state do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "FreezeShape" (state as string)
	)
	
	on freezeMarkers_chk changed state do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "FreezeMarkers" (state as string)
	)
	
	
	-- save node settings
	on nodeX_spn changed val do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "NodeX" (nodeX_spn.value as string)
	)
	on nodeY_spn changed val do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "NodeY" (nodeY_spn.value as string)
	)
	on nodeZ_spn changed val do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "NodeZ" (nodeZ_spn.value as string)
	)
	
	
	-- save dmp cfg settings
	on showDMP_chk changed state do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "ShowDMP" (state as string)
	)
	
	on autoCFG_chk changed state do
	(
		setINISetting "$scripts\dts_prefs.ini" "Settings" "AutoCFG" (state as string)
	)
)

rollout gDTSUtility_rollout "DTS Exporter Utility" width:420 height:530
(
	imgTag uiHeader "" bitmap: (openBitMap "$scripts\dts\dts_header.bmp") pos:[0,0]
	imgTag uiBlank "" bitmap: (openBitMap "$scripts\dts\dts_blank.bmp") pos:[285,0]
	imgTag uiHelp "Help" bitmap: (openBitMap "$scripts\dts\dts_help.bmp") pos:[318,0]
	imgTag uiMaxMin "" bitmap: (openBitMap "$scripts\dts\dts_min.bmp") pos:[350,0]
	imgTag uiRefresh "" bitmap: (openBitMap "$scripts\dts\dts_refresh.bmp") pos:[382,0]
	subrollout DTSUtilityDialog "" pos:[4,38] width:415 height:(UiHeight - 72)
	imgTag uiDTS "" bitmap: (openBitMap "$scripts\dts\dts_dts.bmp") pos:[0,(UiHeight - 31)]
	imgTag uiDSQ "" bitmap: (openBitMap "$scripts\dts\dts_dsq.bmp") pos:[76,(UiHeight - 31)]
	imgTag uiSettings "" bitmap: (openBitMap "$scripts\dts\dts_settings.bmp") pos:[161,(UiHeight - 31)]
	imgTag uiLogo "" bitmap: (openBitMap "$scripts\dts\dts_gglogo2.bmp") pos:[264,(UiHeight - 31)]
	
	function RefreshUtil =
	(
		try( Bounds_rollout.InitBounds() ) catch()
		try( Detail_rollout.UpdateDetailMarkers() ) catch()
		try( Detail_rollout.UpdateDetails 0 ) catch()
		try( Node_rollout.UpdateNodes() ) catch()
		try( Sequence_rollout.UpdateSequences() ) catch()
		try( Sequence_rollout.EnableControls() ) catch()
		try( Trigger_rollout.UpdateSequences() ) catch()
		try( Billboard_rollout.UpdateDetails() ) catch()
		try( Visibility_rollout.UpdateDetails() ) catch()
		try( Collision_rollout.UpdateDetails() ) catch()
		try( Material_rollout.UpdateMaterials() ) catch()
	)
	
	on gDTSUtility_rollout open do
	(
		-- Reload Min/Max State
		if UiMin == false then
		(
			gDTSUtility_rollout.height = UiHeight
		)
		else
		(
			gDTSUtility_rollout.height = 69
			gDTSUtility_rollout.uiMaxMin.bitmap = (openBitMap "$scripts\dts\dts_max.bmp")
		)
		SetDialogPos gDTSUtility_rollout [UiX,UiY]
	)
	
	on gDTSUtility_rollout resized newSz do
	(
		newSize = newSz.y as integer
		pDialogHeight = newSz.y
		DTSUtilityDialog.height = pDialogHeight - 72
		buttonY = pDialogHeight - 31
		uiDTS.pos = [uiDTS.pos.x, buttonY]
		uiDSQ.pos = [uiDSQ.pos.x, buttonY]
		uiSettings.pos = [uiSettings.pos.x, buttonY]
		uiLogo.pos = [uiLogo.pos.x, buttonY]
		
		if newSize > 75 then
		(
			if newSize != UiHeight then
			(
				setINISetting "$scripts\dts_prefs.ini" "Interface" "UiHeight" (newSize as string)
				UiHeight = newSize
				UiMin = false
			)
		)
	)
	
	on gDTSUtility_rollout moved pos do
	(
		setINISetting "$scripts\dts_prefs.ini" "Interface" "UiX" ((pos.x as integer) as string)
		setINISetting "$scripts\dts_prefs.ini" "Interface" "UiY" ((pos.y as integer) as string)
	)
	
	-- export dts shape
	on uiDTS click do
	(
		if $bounds == undefined then messageBox "No bounds was found"
		else
		(
			dtsFileName = getSaveFileName \
			caption:"Select a File Name" \
			filename:"newobject.dts" \
			types:"Torque DTS Shape(*.DTS)|*.dts|"
			
			if dtsFileName != undefined then
			(
				if AutoCFG == true then CreateConfigFile ( getFilenamePath dtsFileName )
				exportFile dtsFileName
				DmpPath = ( getFilenamePath dtsFileName )
				if ShowDMP == true then createDialog DMP_rollout width:640 height:480
			)
		)
	)
	
	on uiDTS mousedown do
	(
		uiDTS.bitmap = (openBitMap "$scripts\dts\dts_dtsc.bmp")
	)
	
	on uiDTS mouseup do
	(
		uiDTS.bitmap = (openBitMap "$scripts\dts\dts_dts.bmp")
	)
	
	-- export sequence (dsq)
	on uiDSQ click do
	(
		numSeq = $sequence_*
		if numSeq.count == 0 then messageBox "No Sequences were found"
		else
		(
			dsqFileName = getSaveFileName \
			caption:"Select a File Name" \
			filename:"newsequence.dsq" \
			types:"Torque DSQ File(*.DSQ)|*.dsq|"
			
			if dsqFileName != undefined then
			(
				if AutoCFG == true then CreateConfigFile ( getFilenamePath dsqFileName )
				exportFile dsqFileName
				DmpPath = ( getFilenamePath dsqFileName )
				if ShowDMP == true then createDialog DMP_rollout width:640 height:480
			)
		)
	)
	
	on uiDSQ mousedown do
	(
		uiDSQ.bitmap = (openBitMap "$scripts\dts\dts_dsqc.bmp")
	)
	
	on uiDSQ mouseup do
	(
		uiDSQ.bitmap = (openBitMap "$scripts\dts\dts_dsq.bmp")
	)
	
	on uiSettings click do
	(
		createDialog Settings_rollout width:350 height:355
	)
	
	on uiSettings mousedown do
	(
		uiSettings.bitmap = (openBitMap "$scripts\dts\dts_settingsc.bmp")
	)
	
	on uiSettings mouseup do
	(
		uiSettings.bitmap = (openBitMap "$scripts\dts\dts_settings.bmp")
	)
	
	on uiRefresh mousedown do
	(
		uiRefresh.bitmap = (openBitMap "$scripts\dts\dts_refreshc.bmp")
	)
	
	on uiRefresh mouseup do
	(
		uiRefresh.bitmap = (openBitMap "$scripts\dts\dts_refresh.bmp")
		RefreshUtil()
	)
	
	
	on uiMaxMin click do
	(
		if gDTSUtility_rollout.inViewport == false then
		(
			if (getFilenameFile uiMaxMin.bitmap.filename) == "dts_max" then
			(
				gDTSUtility_rollout.height = 69
				setINISetting "$scripts\dts_prefs.ini" "Interface" "UiMin" "true"
			)
			else
			(
				gDTSUtility_rollout.height = UiHeight
				setINISetting "$scripts\dts_prefs.ini" "Interface" "UiMin" "false"
			)
		)
	)
	
	on uiMaxMin mousedown do
	(
		if (getFilenameFile uiMaxMin.bitmap.filename) == "dts_max" then
		(
			uiMaxMin.bitmap = (openBitMap "$scripts\dts\dts_maxc.bmp")
		)
		else
		(
			uiMaxMin.bitmap = (openBitMap "$scripts\dts\dts_minc.bmp")
		)
	)
	
	on uiMaxMin mouseup do
	(
		if (getFilenameFile uiMaxMin.bitmap.filename) == "dts_maxc" then
		(
			uiMaxMin.bitmap = (openBitMap "$scripts\dts\dts_min.bmp")
		)
		else
		(
			uiMaxMin.bitmap = (openBitMap "$scripts\dts\dts_max.bmp")
		)
	)
	
	on uiHelp mousedown do
	(
		uiHelp.bitmap = (openBitMap "$scripts\dts\dts_helpc.bmp")
	)
	
	on uiHelp mouseup do
	(
		uiHelp.bitmap = (openBitMap "$scripts\dts\dts_help.bmp")
	)
	
	on uiHelp click do
	(
		popupMenu ConfigRCmenu
	)
)

---------------------------------------------------------------------
-- Quick Export
---------------------------------------------------------------------
rollout QuickExport_rollout "Quick Export/Setup"
(
	imgTag uiBar "" bitmap:GrayBar_bmp pos:[1,1]
	group "options: "
	(
		checkBox skinned_chk "As Skinned Mesh" across:2
		checkBox cfg_chk "Create .CFG On Quick Export"
		checkBox bounds_chk "Use Existing Bounds"  across:2
		checkBox preset_chk "Use Node Preset"
		checkBox center_chk "Center Shape at Origin" across:2
		dropdownlist presets_ddl "" enabled:false
		checkBox scale_chk "Remove Object Scale (xform)"
	)
	imgTag uiquickSetup "" bitmap: (openBitMap "$scripts\dts\dts_quicksetup.bmp") across:2 align:#left offset:[-8,0]
	imgTag uiquickExport "" bitmap: (openBitMap "$scripts\dts\dts_quickexport.bmp") align:#left offset:[-100,0]
	
	function FindRootIK =
	(
		clearSelection()
		select rootNode.children
			try ( deselect $base01 ) catch ()
			try ( deselect $start01 ) catch ()
			try ( deselect $detail* ) catch ()
			try ( deselect $sequence_* ) catch ()
			try ( deselect $cfg_settings* ) catch ()
			try ( deselect $col* ) catch ()
			try ( deselect $los* ) catch ()
		
		nodeArr = #()
		
		for o in selection do
		(
			if classOf o == BoneGeometry then append nodeArr o
			if classOf o == Dummy then append nodeArr o
		)
		for node in nodeArr do
		(
			if node.parent == undefined then
			(
				node.parent = $start01
			)
		)
		clearSelection()
	)
	
	function quickExport setupType =
	(
		if selection.count == 0 then messageBox "Quick Setup/Export requires a selection"
		else
		(
			if selection[1].name != "bounds" then
			(
				CreateDTSShape()
				if skinned_chk.checked == false then
				(
					selection.parent = $start01
					
					-- should add check for duplicate object names
					
					for i = 1 to selection.count do (
						-- freeze scale transforms
						if scale_chk.checked == true then ResetXForm selection[i]
						
						-- clean object names
						objectName = selection[i].name
						baseName = trimright selection[i].name "1234567890"
						
						-- add detail size to object name
						selection[i].name = baseName + "2"
					)
				)
				else
				(
					-- parent the mesh to the scene root
					selection.parent = null
					
					-- clean objects name
					baseName = trimright selection[1].name "1234567890"
					
					-- add detail size to object name
					selection[1].name = baseName + "2"
					tmpSelect = GetNodeByName( baseName + "2" )
					
					FindRootIK()
					
					-- reselect the mesh object
					select tmpSelect
				)
				
				-- create detail markers
				detailArr = #("2")
				DeleteDetailMarkers()
				CreateDetailMarkers detailArr
				
				-- create bounds
				if bounds_chk.checked == false then
				(
					MakeBounds "bottom" 0.1 0.1 0.1
				)
				
				-- center the dts shape at the scene origin
				if center_chk.checked == true then
				(
					delta = $bounds.pos
					for i = 1 to selection.count do (
						selection[i].pos = selection[i].pos - delta
					)
					$bounds.pos= [0,0,0]
				)
				
				if preset_chk.checked == true then
				(
					if presets_ddl.items.count != 0 then
					(
						ApplyNodePreset presets_ddl.selected
					)
				)
				
				if setupType == "export" then
				(
					dtsFileName = getSaveFileName \
					caption:"Select a File Name" \
					filename:"quickexport.dts" \
					types:"Torque DTS Shape(*.DTS)|*.dts|"
					
					if dtsFileName != undefined then
					(
						if cfg_chk.checked == true then
						(
							CreateConfigFile ( getFilenamePath dtsFileName )
						)
						exportFile dtsFileName
					)
				)
			)
		)
		try( gDTSUtility_rollout.RefreshUtil() ) catch()
	)
	
	function UpdatePresets =
	(
		setNames = getINISetting "$scripts\dts_nodes.ini" "NodeSets" "SetNames"
		presetList = (filterString setNames ", -")
		if presetList.count != 0 then
		(
			presets_ddl.items = presetList
		)
	)
	
	on preset_chk changed state do
	(
		if state == true then
		(
			UpdatePresets()
			presets_ddl.enabled = true
		)
		else
		(
			presets_ddl.enabled = false
			presets_ddl.items = #()
		)	
	)
	
	on uiquickSetup click do
	(
		quickExport "setup"
	)
	
	on uiquickSetup mousedown do
	(
		uiquickSetup.bitmap = (openBitMap "$scripts\dts\dts_quicksetupc.bmp")
	)
	
	on uiquickSetup mouseup do
	(
		uiquickSetup.bitmap = (openBitMap "$scripts\dts\dts_quicksetup.bmp")
	)
	
	on uiquickExport click do
	(
		quickExport "export"
	)
	
	on uiquickExport mousedown do
	(
		uiquickExport.bitmap = (openBitMap "$scripts\dts\dts_quickexportc.bmp")
	)
	
	on uiquickExport mouseup do
	(
		uiquickExport.bitmap = (openBitMap "$scripts\dts\dts_quickexport.bmp")
	)
)

---------------------------------------------------------------------
-- Bounds UI
---------------------------------------------------------------------
rollout BoundsTarget_rollout "Select The Parent Object"
(
	imgTag uiBar "" bitmap:GrayBar_bmp pos:[0,0]
	label target_lbl "Available Nodes:" pos:[10,22]
	dropdownlist targetObject_ddl "" pos:[93,20] width:122
	button targetCancel_btn "cancel" pos:[93,44] width:59 height:20
	button targetApply_btn "apply" pos:[155,44] width:59 height:20
	
	function UpdateNodes =
	(
		nodeArr = #()
		clearSelection()
		select $*
		try ( deselect $bounds ) catch ()
		try ( deselect $detail* ) catch ()
		try ( deselect $sequence_* ) catch ()
		try ( deselect $cfg_settings* ) catch ()
		try ( deselect $col* ) catch ()
		try ( deselect $los* ) catch ()
		for node in selection do
		(
			append nodeArr node.name
		)
		clearSelection()
		targetObject_ddl.items = nodeArr
	)
	
	on BoundsTarget_rollout open do
	(
		UpdateNodes()
	)
	
	on targetCancel_btn pressed do
	(
		try(destroyDialog BoundsTarget_rollout) catch()
	)
	
	on targetApply_btn pressed do
	(
		try(Bounds_rollout.AddConstraint (getNodeByName targetObject_ddl.selected) ) catch() 
		try(destroyDialog BoundsTarget_rollout) catch()
	)
)

rollout Bounds_rollout "Bounds Manager"
(
	imgTag uiBar "" bitmap:GrayBar_bmp pos:[1,1]
	label lbl_bounds "Bounds Utilities: " align:#left offset:[-8,3] across:5
	checkBox chk_lockBounds "Freeze" offset:[0,3]
	checkBox chk_hideBounds "Hide" offset:[0,3]
	imgTag uiboundsDelete "" bitmap: (openBitMap "$scripts\dts\dts_deletebounds.bmp") offset:[-20,0]
	imgTag uiboundsApply "" bitmap: (openBitMap "$scripts\dts\dts_createbounds.bmp") offset:[-5,0]
	group "padding/alignment: "
	(
		spinner spn_xPad "X padding: " type: #float range:[0,100,BoundsPadX]
		spinner spn_yPad "Y padding: " type: #float range:[0,100,BoundsPadY]
		spinner spn_zPad "Z padding: " type: #float range:[0,100,BoundsPadZ]
		dropdownlist ddl_align "" items:#("bottom", "center") width:190 offset:[1,0] align:#right
	)
	group "link Inheritance: "
	( 
		checkBox inheritX_chk "X" across: 5
		checkBox inheritY_chk "Y"
		checkBox inheritZ_chk "Z" 
		imgTag uiRemoveLinkBounds "" bitmap: (openBitMap "$scripts\dts\dts_removelink.bmp") offset:[-20,-5]
		imgTag uiLinkBounds "" bitmap: (openBitMap "$scripts\dts\dts_linkbounds.bmp") offset:[-5,-5]
	)
	imgTag uiBounds "" bitmap: (openBitMap "$scripts\dts\dts_bounds.bmp") pos:[35,71] transparent:(color 197 197 197)
	
	function InitBounds =
	(
		if $bounds != null then
		(
			if $bounds.isHidden == true then chk_hideBounds.checked = true
			if $bounds.isFrozen == true then chk_lockBounds.checked = true
			
			boundsInherit = getInheritanceFlags $bounds
			
			if (findItem boundsInherit 1) != 0 then inheritX_chk.checked = true
			else
			(
				inheritX_chk.checked = false
			)
			
			if (findItem boundsInherit 2) != 0 then inheritY_chk.checked = true
			else
			(
				inheritY_chk.checked = false
			)
			
			if (findItem boundsInherit 3) != 0 then inheritZ_chk.checked = true
			else
			(
				inheritZ_chk.checked = false
			)
		)
	)
	
	function AddConstraint TargetObj =
	(
		if $bounds != undefined then
		(
			setInheritanceFlags $bounds #{1,2}
			$bounds.parent = TargetObj
			InitBounds()
		)
	)
	
	function RemoveConstraint =
	(
		if $bounds != undefined then
		(
			$bounds.parent = null
			InitBounds()
		)
	)
	
	on Bounds_rollout open do
	(
		InitBounds()
	)
	
	on uiboundsApply click do
	(
		if selection.count == 0 then messageBox "Create Bounds requires a selection"
		else
		(
			MakeBounds ddl_align.selected spn_xPad.value spn_yPad.value spn_zPad.value
		)
	)
	
	on uiboundsDelete click do
	(
		if $bounds != null then
		(
			delete $bounds
		)
	)
	
	-- hide/show the bounds
	on chk_hideBounds changed curState do
	(
		if $bounds == null then chk_hideBounds.checked = false
		else
		(
			if chk_hideBounds.checked == true then
			(
				$bounds.isHidden = true
			) else (
				$bounds.isHidden = false
			)
		)
	)
	
	-- lock bounds
	on chk_lockBounds changed curState do
	(
		if $bounds == null then chk_lockBounds.checked = false
		else
		(
			if chk_lockBounds.checked == true then
			(
				$bounds.isFrozen = true
			) else (
				$bounds.isFrozen = false
			)
		)
	)
	
	on uiLinkBounds click do
	(
		createDialog BoundsTarget_rollout width:225 height:74
	)
	
	on uiRemoveLinkBounds click do
	(
		RemoveConstraint()
	)
	
	on inheritX_chk changed state do
	(
		if $bounds != null then
		(	
			boundsInherit = getInheritanceFlags $bounds
			
			if inheritX_chk.checked == true then append boundsInherit 1
			else
			(
				deleteItem boundsInherit 1
			)
			setInheritanceFlags $bounds boundsInherit
		)
		else
		(
			inheritX_chk.checked = false
		)
	)
	
	on inheritY_chk changed state do
	(
		if $bounds != null then
		(	
			boundsInherit = getInheritanceFlags $bounds
			
			if inheritY_chk.checked == true then append boundsInherit 2
			else
			(
				deleteItem boundsInherit 2
			)
			setInheritanceFlags $bounds boundsInherit
		)
		else
		(
			inheritY_chk.checked = false
		)
	)
	
	on inheritZ_chk changed state do
	(
		if $bounds != null then
		(	
			boundsInherit = getInheritanceFlags $bounds
			
			if inheritZ_chk.checked == true then append boundsInherit 3
			else
			(
				deleteItem boundsInherit 3
			)
			setInheritanceFlags $bounds boundsInherit
		)
		else
		(
			inheritZ_chk.checked = false
		)
	)
	
	on uiboundsApply mousedown do
	(
		uiboundsApply.bitmap = (openBitMap "$scripts\dts\dts_createboundsc.bmp")
	)
	
	on uiboundsApply mouseup do
	(
		uiboundsApply.bitmap = (openBitMap "$scripts\dts\dts_createbounds.bmp")
	)
	
	on uiboundsDelete mousedown do
	(
		uiboundsDelete.bitmap = (openBitMap "$scripts\dts\dts_deleteboundsc.bmp")
	)
	
	on uiboundsDelete mouseup do
	(
		uiboundsDelete.bitmap = (openBitMap "$scripts\dts\dts_deletebounds.bmp")
	)
	
	on uiLinkBounds mousedown do
	(
		uiLinkBounds.bitmap = (openBitMap "$scripts\dts\dts_linkboundsc.bmp")
	)
	
	on uiLinkBounds mouseup do
	(
		uiLinkBounds.bitmap = (openBitMap "$scripts\dts\dts_linkbounds.bmp")
	)
	
	on uiRemoveLinkBounds mousedown do
	(
		uiRemoveLinkBounds.bitmap = (openBitMap "$scripts\dts\dts_removelinkc.bmp")
	)
	
	on uiRemoveLinkBounds mouseup do
	(
		uiRemoveLinkBounds.bitmap = (openBitMap "$scripts\dts\dts_removelink.bmp")
	)
)

---------------------------------------------------------------------
-- Detail Manager
---------------------------------------------------------------------
rollout DetailTarget_rollout "Select Parent Object"
(
	imgTag uiBar "" bitmap:GrayBar_bmp pos:[0,0]
	label target_lbl "Available Nodes:" pos:[10,22]
	dropdownlist targetObject_ddl "" pos:[93,20] width:122
	button targetCancel_btn "cancel" pos:[93,44] width:59 height:20
	button targetApply_btn "apply" pos:[155,44] width:59 height:20
	
	function UpdateNodes =
	(
		clearSelection()
		select $*
		try ( deselect $bounds ) catch ()
		try ( deselect $detail* ) catch ()
		try ( deselect $sequence_* ) catch ()
		try ( deselect $cfg_settings* ) catch ()
		try ( deselect $col* ) catch ()
		try ( deselect $los* ) catch ()
		
		nodeArr = #()
		for node in selection do
		(
			append nodeArr node.name
		)
		clearSelection()
		targetObject_ddl.items = nodeArr
	)
	
	on DetailTarget_rollout open do
	(
		UpdateNodes()
	)
	
	on targetCancel_btn pressed do
	(
		try(destroyDialog DetailTarget_rollout) catch()
	)
	
	on targetApply_btn pressed do
	(
		try(Detail_rollout.DetailParent targetObject_ddl.selected ) catch() 
		try(destroyDialog DetailTarget_rollout) catch()
	)
)

rollout Detail_rollout "Detail Manager"
(
	imgTag uiBar "" bitmap:GrayBar_bmp pos:[1,1]
	groupBox detailMarkers_grp "Detail Levels" pos:[9,20] width:381 height:107
	button detailDelete_btn "-" pos:[20,38] width:23 height:16
	button detailEdit_btn "edit" pos:[44,38] width:34 height:16
	button detailAuto_btn "auto" pos:[80,38] width:34 height:16
	button detailNew_btn "+" pos:[116,38] width:23 height:16
	listBox detailMarkers_lbx "" pos:[19,58] width:164 height:4
	
	groupBox detailSet_grp "Detail Level Settings" pos:[201,33] width:176 height:85
	label detailSize_lbl "Pixel Size:" pos:[228,69] width:53 height:16
	spinner detailSize_spn "" pos:[303,69] width:65 height:16 range:[0,1000,2] type:#integer
	button detailMarkerSave_btn "save" pos:[304,90] width:64 height:16
	
	groupBox details_grp "Detail Meshes" pos:[9,141] width:186 height:155
	multiListBox details_lbx "" pos:[19,162] width:164 height:8
	checkbox detailShowAll_chk "show all detail meshes" pos:[19,275] width:150 height:18 checked:true
	
	groupBox detailUtil_grp "Detail Utilities" pos:[203,141] width:187 height:155
	button deleteShape_btn "del shape" pos:[219,158] width:76 height:20
	button createShape_btn "create shape" pos:[298,158] width:76 height:20
	button registerDetails_btn "register detail(s)" pos:[219,180] width:155 height:20
	button renumberDetails_btn "renumber detail(s)" pos:[219,202] width:155 height:20
	button deleteMarkers_btn "remove detail markers" pos:[219,224] width:155 height:20
	button sort_btn "sort settings" pos:[219,246] width:155 height:20
	button delParent_btn "del parent" pos:[219,268] width:76 height:20
	button setParent_btn "set parent" pos:[298,268] width:76 height:20
	
	label renumber_lbl "Renumber Detail(s)" pos:[220,159] width:126 height:14 visible:false
	label detailNum_lbl "Detail Number:" pos:[220,180] width:75 height:17 visible:false
	spinner renumberSize_spn "" pos:[306,180] width:69 height:16 range:[0,1000,2] type:#integer visible:false
	groupBox options_grp "Options:" pos:[219,200] width:156 height:60 visible:false
	checkbox createNew_chk "Create new marker" pos:[228,216] width:128 height:18 checked:true visible:false
	checkbox removeOld_chk "Remove old marker(s)" pos:[228,237] width:138 height:18 checked:true visible:false
	button renumberCancel_btn "Cancel" pos:[223,267] width:70 height:19 visible:false
	button renumberUpdate_btn "Update" pos:[301,267] width:70 height:19 visible:false
	
	checkbox enableSort_chk "Enable Mesh Sorting" pos:[219,164] width:153 height:17 visible:false
	label maxDepth_lbl "Max Depth:" pos:[220,186] width:75 height:17 visible:false
	spinner maxDepth_spn "" pos:[304,185] width:65 height:16 range:[0,1000,2] type:#integer visible:false
	label numFaces_lbl "Num Big Faces:" pos:[220,205] width:75 height:17 visible:false
	spinner numFaces_spn "" pos:[304,205] width:65 height:16 range:[0,1000,4] type:#integer visible:false
	checkbox zUp_chk "Z Layer Up" pos:[220,225] width:90 height:18 visible:false
	checkbox zDown_chk "Z Layer Down" pos:[220,245] width:102 height:18 visible:false
	button sortCancel_btn "cancel" pos:[305,268] width:64 height:16 visible:false

	groupBox autoDetails_grp "Auto Details" pos:[9,309] width:381 height:116
	button autodDelete_btn "-" pos:[19,327] width:23 height:16
	button autodEdit_btn "edit" pos:[44,327] width:34 height:16
	button autodAuto_btn "auto" pos:[79,327] width:34 height:16
	button autodNew_btn "+" pos:[116,327] width:23 height:16
	listBox autoDetails_lbx "" pos:[19,344] width:164 height:5
	
	groupBox autodSet_grp "Auto Detail Settings" pos:[200,331] width:176 height:85
	label lbl2 "Reduction:" pos:[229,349] width:54 height:16
	spinner detailReduction_spn "" pos:[304,348] width:65 height:16 range:[0,1,1]
	label lbl3 "Pixel Size:" pos:[229,370] width:53 height:16
	spinner autodSize_spn "" pos:[304,368] width:65 height:16 range:[0,1000,2] type:#integer
	button autoDetailSave_btn "save" pos:[305,391] width:64 height:16
	
	function DetailParent targetObj =
	(
		selectionTmp = details_lbx.selection as array
		if selectionTmp.count != 0 then
		(
			if targetObj != null then
			(
				for i = 1 to selectionTmp.count do
				(
					tmpObj = getNodeByName details_lbx.items[selectionTmp[i]]
					tmpObj.parent = (getNodeByName targetObj)
				)
			)
			else
			(
				for i = 1 to selectionTmp.count do
				(
					tmpObj = getNodeByName details_lbx.items[selectionTmp[i]]
					tmpObj.parent = null
				)
			)
		)
	)
	
	function EnableSort =
	(
		maxDepth_lbl.enabled = true
		maxDepth_spn.enabled = true
		numFaces_lbl.enabled = true
		numFaces_spn.enabled = true
		zUp_chk.enabled = true
		zDown_chk.enabled = true
	)
	
	function DisableSort =
	(
		enableSort_chk.checked = false
		maxDepth_spn.value = 2
		numFaces_spn.value = 4
		zUp_chk.checked = false
		zDown_chk.checked = false
		
		maxDepth_lbl.enabled = false
		maxDepth_spn.enabled = false
		numFaces_lbl.enabled = false
		numFaces_spn.enabled = false
		zUp_chk.enabled = false
		zDown_chk.enabled = false
	)
	
	function SaveSort =
	(
		selectionTmp = details_lbx.selection as array
		if selectionTmp.count == 1 then
		(
			d = getNodeByName details_lbx.items[selectionTmp[1]]
			if d != null then
			(
				if enableSort_chk.checked == true then
				(
					setUserProp d "enableSort" true
					setUserProp d "max_depth" maxDepth_spn.value
					setUserProp d "num_big_faces" numFaces_spn.value
					
					setUserProp d "z_layer_up" zUp_chk.checked
					setUserProp d "z_layer_down" zDown_chk.checked
				)
				else
				(
					setUserPropBuffer d ""
				)
			)
			else
			(
				DisableSort()
			)
		)
		else
		(
			DisableSort()
		)
	)
	
	function EditSort =
	(
		selectionTmp = details_lbx.selection as array
		if selectionTmp.count == 1 then
		(
			d = getNodeByName details_lbx.items[selectionTmp[1]]
			if d != null then
			(
				if (getUserProp d "enableSort") == true then
				(
					enableSort_chk.checked = true
					maxDepth_spn.value = (getUserProp d "max_depth")
					numFaces_spn.value = (getUserProp d "num_big_faces")
					zUp_chk.checked = (getUserProp d "z_layer_up")
					zDown_chk.checked = (getUserProp d "z_layer_down")
					EnableSort()
				)
				else
				(	
					DisableSort()
				)
			)
			else
			(
				DisableSort()
			)
		)
		else
		(
			DisableSort()
		)
	)
	
	function SelectDetails =
	(
		clearSelection()
		select $geometry
		try ( deselect $bounds ) catch ()
		try ( deselect $col* ) catch ()
		try ( deselect $los* ) catch ()
	)
	
	function SelectDetailMarkers =
	(
		clearSelection()
		select $detail*
	)
	
	function GetDetailNumbers =
	(
		SelectDetailMarkers()
		detailArr = #()
		for o in selection do
		(
			append detailArr (GetDetailSize o.name as string)
		)
		detailArr
	)
	
	function UpdateDetails size =
	(
		SelectDetails()
		detailArr = #()
		if size != null then
		(
			if detailShowAll_chk.checked == false then
			(
				for o in selection do
				(
					if (GetDetailSize o.name) == (GetDetailSize size) then
					(
						append detailArr o.name
					)
				)
			)
			else
			(
				for o in selection do
				(
					if classOf o != BoneGeometry then
					(
						append detailArr o.name
					)
				)
			)
		)
		details_lbx.items = detailArr
		try( Billboard_rollout.UpdateDetails() ) catch()
		try( Visibility_rollout.UpdateDetails() ) catch()
		try( Collision_rollout.UpdateDetails() ) catch()
		ClearSelection()
	)
	
	function UpdateDetailMarkers =
	(
		SelectDetailMarkers()
		detailMarkersArr = #()
		for o in selection do
		(
			if classOf o != BoneGeometry then
			(
				append detailMarkersArr o.name
			)
		)
		detailMarkers_lbx.items = detailMarkersArr
		ClearSelection()
		
		UpdateDetails detailMarkers_lbx.selected
		EditSort()
	)
	
	function CreateDetailMarker size = 
	(
		detailArr = GetDetailNumbers()
		detailTest = findItem detailArr (size as string)
		if detailTest == 0 then
		(
			detailName = "detail" + size as string
			d = dummy name: detailName
			d.boxsize = [ShapeX, ShapeY, ShapeZ]
			d.parent = $base01
			if FreezeMarkers == true then d.isFrozen = true
		)
	)
	
	function RenumberDetails size =
	(
		selectionTmp = details_lbx.selection as array
		if selectionTmp.count != 0 then
		(
			detailArr = #()
			for i = 1 to selectionTmp.count do
			(
				tmpObj = getNodeByName details_lbx.items[selectionTmp[i]]
				baseName = trimright tmpObj.name "1234567890"
				detailNumber = trimleft tmpObj.name baseName 
				
				detailTest = findItem detailArr detailNumber
				if detailTest == 0 then
				(
					append detailArr detailNumber
				)
				newName = baseName + size
				tmpObj.name = newName
			)
				
			if removeOld_chk.checked  then
			(
				for i = 1 to detailArr.count do
				(
					obj = getNodeByName ( "detail" + detailArr[i] )
					if obj != null then delete obj
				)
			)
				
			if createNew_chk.checked  then
			(
				CreateDetailMarker size
			)
			UpdateDetailMarkers()
		)
	)
	
	function GetAutoDetails detail =
	(
		autoArr = #()
		detailObj = getNodeByName detail
		if detailObj != null then
		(
			numAutoD = getUserProp detailObj "numAutoDetails"
			if numAutoD != null then
			(
				for i = 0 to (numAutoD - 1) do
				(
					curDetailSize = "autoDetailSize" + i as string
					curDetailPercent = "autoDetailPercent" + i as string
					autoDetailSize = getUserProp detailObj curDetailSize as string
					autoDetailPercent = getUserProp detailObj curDetailPercent as string
					autoDTmp = "Size: " + autoDetailSize + " Reduction: " + autoDetailPercent
					append autoArr autoDTmp
				)
			)
		)
		else
		(
			UpdateDetails detailMarkers_lbx.selected
		)
		autoArr
	)
	
	function AddAutoDetails reduction size =
	(
		selectionTmp = details_lbx.selection as array
		if selectionTmp.count == 1 then
		(
			detailObj = getNodeByName details_lbx.items[selectionTmp[1]]
			numAutoD = getUserProp detailObj "numAutoDetails"
			if numAutoD == null then numAutoD = 0 
			curDetailSize = "autoDetailSize" + numAutoD as string
			curDetailPercent = "autoDetailPercent" + numAutoD as string
			setUserProp detailObj "numAutoDetails" (numAutoD + 1)
			setUserProp detailObj curDetailSize size
			setUserProp detailObj curDetailPercent reduction
		)
	)
	
	function EditAutoDetails autoNum =
	(
		selectionTmp = details_lbx.selection as array
		if selectionTmp.count == 1 then
		(
			autoNum -= 1
			detailObj = getNodeByName details_lbx.items[selectionTmp[1]]
			curDetailSize = "autoDetailSize" + autoNum as string
			curDetailPercent = "autoDetailPercent" + autoNum as string
			autodSize_spn.value = getUserProp detailObj curDetailSize
			detailReduction_spn.value = getUserProp detailObj curDetailPercent
		)
	)
	
	function UpdateAutoDetails autoNum =
	(
		selectionTmp = details_lbx.selection as array
		if selectionTmp.count == 1 then
		(
			autoNum -= 1
			detailObj = getNodeByName details_lbx.items[selectionTmp[1]]
			curDetailSize = "autoDetailSize" + autoNum as string
			curDetailPercent = "autoDetailPercent" + autoNum as string
			setUserProp detailObj curDetailSize autodSize_spn.value
			setUserProp detailObj curDetailPercent detailReduction_spn.value
		)
	)
	
	function DeleteAutoDetails num =
	(
		selectionTmp = details_lbx.selection as array
		if selectionTmp.count == 1 then
		(
			autoDetails = #()
			detailObj = getNodeByName details_lbx.items[selectionTmp[1]]
			numAutoD = getUserProp detailObj "numAutoDetails"
			for i = 0 to (numAutoD -= 1) do
			(
				curDetailSize = "autoDetailSize" + i as string
				curDetailPercent = "autoDetailPercent" + i as string
				curSize = getUserProp detailObj curDetailSize
				curPercent = getUserProp detailObj curDetailPercent
				
				append autoDetails  #(curSize, curPercent)
				if i == numAutoD then
				(
					RemoveUserProp detailObj curDetailSize curSize
					RemoveUserProp detailObj curDetailPercent curPercent
					setUserProp detailObj "numAutoDetails" numAutoD
				)
			)
			deleteItem autoDetails num
			for i = 0 to (autoDetails.count - 1) do
			(
				curDetailSize = "autoDetailSize" + i as string
				curDetailPercent = "autoDetailPercent" + i as string
				setUserProp detailObj curDetailSize autoDetails[i + 1][1]
				setUserProp detailObj curDetailPercent autoDetails[i + 1][2]
			)
		)
	)
	
	on Detail_rollout open do 
	(
		UpdateDetailMarkers()
		UpdateDetails 0
	)
	
	on detailMarkers_lbx selected selectedMarker do
	(
		detailMarkerSave_btn.text = "save"
		if detailMarkers_lbx.selected != null then
		(
			detailShowAll_chk.checked = false
			UpdateDetails detailMarkers_lbx.selected
		)
	)
	
	on detailDelete_btn pressed do
	(
		if detailMarkers_lbx.selected != null then
		(
			delete (getNodeByName detailMarkers_lbx.selected)
			UpdateDetailMarkers()
		)
	)
	
	on detailEdit_btn pressed do
	(
		if detailMarkers_lbx.selected != null then
		(
			detailSize_spn.value = GetDetailSize(detailMarkers_lbx.selected)
			detailMarkerSave_btn.text = "update"
		)
	)
	
	on detailAuto_btn pressed do
	(
		SelectDetails()
		detailArr = #()
		for i = 1 to selection.count do
		(
			objectName = selection[i].name
			baseName = trimright selection[i].name "1234567890"
			detailNumber = trimleft selection[i].name baseName 
				
			detailTest = findItem detailArr detailNumber
			if detailTest == 0 then
			(
				if classOf selection[i] != BoneGeometry then
				(
					append detailArr detailNumber
				)
			)
		)
		DeleteDetailMarkers()
		CreateDetailMarkers detailArr
		UpdateDetailMarkers()
	)
	
	on detailNew_btn pressed do
	(
		detailMarkerSave_btn.text = "save"
	)
	
	on detailMarkerSave_btn pressed do
	(
		if detailMarkerSave_btn.text == "save" then
		(
			CreateDetailMarker detailSize_spn.value
		)
		else
		(
			slectedMarker = getNodeByName detailMarkers_lbx.selected
			slectedMarker.name = "detail" + detailSize_spn.value as string
			detailMarkerSave_btn.text = "save"
		)
		UpdateDetailMarkers()
	)
	
	on details_lbx selected selectedDetail do
	(
		selectionTmp = details_lbx.selection as array
		if selectionTmp.count == 1 then
		(
			autoDetails_lbx.items = GetAutoDetails details_lbx.items[selectionTmp[1]]
		)
		else
		(
			autoDetails_lbx.items = #()
		)
		EditSort()
	)
	
	on detailShowAll_chk changed state do
	(
		if state == true then
		(
			UpdateDetails 0
		)
		else
		(
			UpdateDetails detailMarkers_lbx.selected
		)
		DisableSort()
	)
	
	on createShape_btn pressed do
	(
		CreateDTSShape()
		try( Node_rollout.UpdateNodes() ) catch()
	)
	
	on registerDetails_btn pressed do
	(
		CreateDTSShape()
		selectionTmp = details_lbx.selection as array
		for i = 1 to selectionTmp.count do
		(
			tmpObj = getNodeByName details_lbx.items[selectionTmp[i]]
			tmpObj.parent = $start01
		)
		try( Node_rollout.UpdateNodes() ) catch()
	)
	
	on renumberDetails_btn pressed do
	(
		createShape_btn.visible = false
		deleteShape_btn.visible = false
		registerDetails_btn.visible = false
		registerDetails_btn.visible = false
		renumberDetails_btn.visible = false
		deleteMarkers_btn.visible = false
		sort_btn.visible = false
		delParent_btn.visible = false
		setParent_btn.visible = false
		
		renumber_lbl.visible = true
		detailNum_lbl.visible = true
		renumberSize_spn.visible = true
		options_grp.visible = true
		createNew_chk.visible = true
		removeOld_chk.visible = true
		renumberCancel_btn.visible = true
		renumberUpdate_btn.visible = true
	)
	
	on renumberCancel_btn pressed do
	(
		renumber_lbl.visible = false
		detailNum_lbl.visible = false
		renumberSize_spn.visible = false
		options_grp.visible = false
		createNew_chk.visible = false
		removeOld_chk.visible = false
		renumberCancel_btn.visible = false
		renumberUpdate_btn.visible = false
		
		createShape_btn.visible = true
		deleteShape_btn.visible = true
		registerDetails_btn.visible = true
		registerDetails_btn.visible = true
		renumberDetails_btn.visible = true
		deleteMarkers_btn.visible = true
		sort_btn.visible = true
		delParent_btn.visible = true
		setParent_btn.visible = true
	)
	
	on renumberUpdate_btn pressed do
	(
		RenumberDetails (renumberSize_spn.value as string)
	)
	
	on deleteMarkers_btn pressed do
	(
		DeleteDetailMarkers()
		UpdateDetailMarkers()
	)
	
	on deleteShape_btn pressed do
	(
		if $base01 != null then delete $base01
		if $start01 != null then delete $start01
		try( Node_rollout.UpdateNodes() ) catch()
	)
	
	on sort_btn pressed do
	(
		createShape_btn.visible = false
		deleteShape_btn.visible = false
		registerDetails_btn.visible = false
		registerDetails_btn.visible = false
		renumberDetails_btn.visible = false
		deleteMarkers_btn.visible = false
		sort_btn.visible = false
		delParent_btn.visible = false
		setParent_btn.visible = false
		
		enableSort_chk.visible = true
		maxDepth_lbl.visible = true
		maxDepth_spn.visible = true
		numFaces_lbl.visible = true
		numFaces_spn.visible = true
		zUp_chk.visible = true
		zDown_chk.visible = true
		sortCancel_btn.visible = true
	)
	
	on sortCancel_btn pressed do
	(
		enableSort_chk.visible = false
		maxDepth_lbl.visible = false
		maxDepth_spn.visible = false
		numFaces_lbl.visible = false
		numFaces_spn.visible = false
		zUp_chk.visible = false
		zDown_chk.visible = false
		sortCancel_btn.visible = false
		
		createShape_btn.visible = true
		deleteShape_btn.visible = true
		registerDetails_btn.visible = true
		registerDetails_btn.visible = true
		renumberDetails_btn.visible = true
		deleteMarkers_btn.visible = true
		sort_btn.visible = true
		delParent_btn.visible = true
		setParent_btn.visible = true
	)
	
	on enableSort_chk changed state do
	(
		if state == true then EnableSort()
		else
		(
			DisableSort()
		)
		SaveSort()
	)
	
	on maxDepth_spn changed value do
	(
		SaveSort()
	)
	
	on numFaces_spn changed value do
	(
		SaveSort()
	)
	
	on zUp_chk changed state do
	(
		SaveSort()
	)
	
	on zDown_chk changed state do
	(
		SaveSort()
	)
	
	on delParent_btn pressed do
	(
		DetailParent null
	)
	
	on setParent_btn pressed do
	(
		createDialog DetailTarget_rollout width:225 height:74
	)
	
	on autodNew_btn pressed do
	(
		autoDetailSave_btn.text = "save"
	)
	
	on autoDetails_lbx selected selectedAutoD do
	(
		--autoDetailSave_btn.text = "update"
		if autoDetails_lbx.selection != 0 then
		(
			EditAutoDetails(autoDetails_lbx.selection)
			selectionTmp = details_lbx.selection as array
			autoDetails_lbx.items = GetAutoDetails details_lbx.items[selectionTmp[1]]
			autoDetailSave_btn.text = "update"
		)
	)
	
	on autoDetailSave_btn pressed do
	(
		selectionTmp = details_lbx.selection as array
		if selectionTmp.count == 1 then
		(
			if autoDetailSave_btn.text == "save" then
			(
				AddAutoDetails detailReduction_spn.value autodSize_spn.value
			)
			else
			(
				UpdateAutoDetails(autoDetails_lbx.selection)
			)
			selectionTmp = details_lbx.selection as array
			autoDetails_lbx.items = GetAutoDetails details_lbx.items[selectionTmp[1]]
			CreateDetailMarker autodSize_spn.value
			UpdateDetailMarkers()
		)
	)
	
	on autodEdit_btn pressed do
	(
		if autoDetails_lbx.selection != 0 then
		(
			EditAutoDetails(autoDetails_lbx.selection)
			selectionTmp = details_lbx.selection as array
			autoDetails_lbx.items = GetAutoDetails details_lbx.items[selectionTmp[1]]
			autoDetailSave_btn.text = "update"
		)
	)
	
	on autodDelete_btn pressed do
	(
		if autoDetails_lbx.selected != null then
		(
			DeleteAutoDetails(autoDetails_lbx.selection)
			selectionTmp = details_lbx.selection as array
			autoDetails_lbx.items = GetAutoDetails details_lbx.items[selectionTmp[1]]
		)
		autoDetailSave_btn.text = "save"
	)
)

---------------------------------------------------------------------
-- Sequence Manager
---------------------------------------------------------------------
rollout NewSequence_rollout "Create New Sequence"
(
	label seqName_lbl "Sequence Name:" pos:[8,14] width:85 height:15
	editText seqName_edt "" pos:[91,12] width:122 height:20
	button newSeqCancel_btn "cancel" pos:[95,36] width:58 height:20
	button newSeqApply_btn "apply" pos:[155,36] width:58 height:20
	
	on NewSequence_rollout open do
	(
		--seqName_edt.text = SelectedNode.name
		setFocus seqName_edt
	)
	
	on newSeqCancel_btn pressed do
	(
		try(destroyDialog NewSequence_rollout) catch()
	)
	
	on newSeqApply_btn pressed do
	(
		if seqName_edt.text != "" then
		(
			try(Sequence_rollout.name_edt.text = seqName_edt.text) catch()
			try(Sequence_rollout.SaveSequence()) catch()
			try(Sequence_rollout.UpdateSequences()) catch() 
		)
		try(destroyDialog NewSequence_rollout) catch()
	)
)

rollout RenameSequence_rollout "Rename Sequence"
(
	label rename_lbl "Sequence Name:" pos:[8,14] width:85 height:15
	editText seqName_edt "" pos:[91,12] width:122 height:20
	button renameCancel_btn "cancel" pos:[95,36] width:58 height:20
	button renameApply_btn "apply" pos:[155,36] width:58 height:20
	
	on RenameSequence_rollout open do
	(
		tmpStr = SelectedNode.name
		seqName_edt.text = (substring tmpStr 10 tmpStr.count)
		setFocus seqName_edt
	)
	
	on renameCancel_btn pressed do
	(
		try(destroyDialog RenameSequence_rollout) catch()
	)
	
	on renameApply_btn pressed do
	(
		if seqName_edt.text != "" then
		(
			SelectedNode.name = ("sequence_" + seqName_edt.text)
		)
		SelectedNode = null
			
		try(Sequence_rollout.UpdateSequences()) catch() 
		try(destroyDialog RenameSequence_rollout) catch()
	)
)

rollout Sequence_rollout "Sequence Manager"
(
	imgTag uiGrayBar "" bitmap:GrayBar_bmp pos:[1,1] 
	button delete_btn "-" pos:[9,33] width:22 height:15
	button edit_btn "rename" pos:[33,33] width:43 height:15
	button copy_btn "copy" pos:[78,33] width:33 height:15
	button add_btn "+" pos:[113,33] width:22 height:15
	listBox sequences_lbx "" pos:[10,49] width:124 height:6
	button setTime_btn "Set Slider Range" pos:[9,136] width:109 height:18
	button playAni_btn "P" pos:[120,136] width:15 height:18
	button getTime_btn "Get Slider Range" pos:[9,156] width:109 height:18
	button stopAni_btn "S" pos:[120,156] width:15 height:18
	
	groupBox settings_grp "Sequence Settings" pos:[143,27] width:250 height:86
	editText name_edt "name: " pos:[176,46] width:200 height:15 readOnly:true
	spinner startFrame_spn "start frame: " pos:[195,68] width:75 height:16 range:[0,10000,0] type:#integer
	spinner endFrame_spn "end frame: " pos:[195,88] width:75 height:16 range:[0,10000,1] type:#integer 
	checkbox cyclic_chk "cycle sequence" pos:[278,68] width:100 height:15 
	checkbox complete_chk "complete cycle" pos:[278,88] width:100 height:15 
	
	groupBox blend_grp "Blend Settings" pos:[143,117] width:250 height:57 
	checkbox blend_chk "blend sequence" pos:[153,133] width:100 height:15 
	spinner referenceFrame_spn "Reference Frame: " pos:[300,150] width:75 height:16 range:[0,1000,0] type:#integer enabled:false 
	
	groupBox frameRate_grp "Frame Sampleing Rate" pos:[143,178] width:250 height:66
	radioButtons frames_rdo "" pos:[153,196] width:231 width:220 height:16 labels:#("Use frame rate", "Use N frames") columns:2 
	spinner frameRate_spn "frame rate: " pos:[195,218] width:60 height:16 range:[0,1000,30] type:#integer
	spinner numFrames_spn "num frames: " pos:[315,218] width:60 height:16 range:[0,1000,2] type:#integer enabled:false 
	
	groupBox duration_grp "Priority/ Duration" pos:[144,247] width:250 height:62
	spinner priority_spn "priority: " pos:[193,265] width:60 height:16 range:[0,1000,5] type:#integer
	checkbox overrideDuration_chk "override duration" pos:[275,265] width:100 height:15 
	spinner duration_spn "" pos:[326,284] width:50 height:16 range:[0,1000,1] type:#float enabled:false 
	
	groupBox ground_grp "Ground Transform" pos:[143,312] width:250 height:97
	checkbox ignoreGround_chk "Ignore Ground Transform" pos:[153,331] width:150 height:15
	radioButtons groundFrames_rdo "" pos:[153,354] width:231 height:16 labels:#("Use frame rate", "Use N frames") columns:2 
	spinner groundFrameRate_spn "frame rate: " pos:[195,378] width:60 height:16 range:[0,1000,15] type:#integer
	spinner groundNumFrames_spn "num frames: " pos:[315,378] width:60 height:16 range:[0,1000,2] type:#integer enabled:false 

	groupBox enable_grp "Enable Animation" pos:[10,178] width:124 height:131
	checkbox morph_chk "morph" pos:[20,197] width:70 height:15
	checkbox vis_chk "visibility" pos:[20,212] width:70 height:15 checked:true
	checkbox trans_chk "transform" pos:[20,227] width:70 height:15 checked:true
	checkbox uni_chk "uniform scale" pos:[20,242] width:100 height:15
	checkbox arb_chk "arbitrary Scale" pos:[20,257] width:100 height:15
	checkbox texture_chk "texture" pos:[20,272] width:70 height:15
	checkbox ILF_chk "IFL" pos:[20,287] width:70 height:15 checked:true

	groupBox force_grp "Force Animation" pos:[10,312] width:124 height:97
	checkbox morphForce_chk "morph" pos:[19,328] width:70 height:15
	checkbox visForce_chk "visibility" pos:[19,343] width:70 height:15 
	checkbox transForce_chk "transform" pos:[19,358] width:70 height:15 
	checkbox scaleForce_chk "scale" pos:[19,373] width:100 height:15 
	checkbox textureForce_chk "texture" pos:[19,388] width:70 height:15
	
	button load_btn "Create from File" pos:[190,414] width:100 height:18
	button save_btn "Save to File" pos:[293,414] width:100 height:18 
	
	function EnableControls =
	(
		if blend_chk.checked == false then
		(
			referenceFrame_spn.enabled = false
		)
		else
		(
			referenceFrame_spn.enabled = true
		)
		
		if frames_rdo.state == 1 then
		(
			frameRate_spn.enabled = true
			numFrames_spn.enabled = false
		)
		else
		(
			frameRate_spn.enabled = false
			numFrames_spn.enabled = true
		)
		
		if overrideDuration_chk.checked == false then
		(
			duration_spn.enabled = false
		)
		else
		(
			duration_spn.enabled = true
		)
		
		if ignoreGround_chk.checked == false then
		(
			groundFrames_rdo.enabled = true
			if groundFrames_rdo.state == 1 then
			(
				groundFrameRate_spn.enabled = true
				groundNumFrames_spn.enabled = false
			)
			else
			(
				groundFrameRate_spn.enabled = false
				groundNumFrames_spn.enabled = true
			)
		)
		else
		(
			groundFrames_rdo.enabled = false
			groundFrameRate_spn.enabled = false
			groundNumFrames_spn.enabled = false
		)
	)
	
	function SelectSequences =
	(
		clearSelection()
		select $sequence_*
	)
	
	function UpdateSequences =
	(
		SelectSequences()
		seqArr = #()
		for o in selection do
		(
			append seqArr (substring o.name 10 o.name.count )
		)
		sequences_lbx.items = seqArr
		try( Trigger_rollout.sequences_lbx.items = seqArr ) catch()
		clearSelection()
	)
	
	function ResetSequence =
	(
		name_edt.text = ""
		-- sequence info
		startFrame_spn.value = 0
		endFrame_spn.value = 1
		numFrames_spn.value = 1
		frames_rdo.state = 1
		frameRate_spn.value = 30
		priority_spn.value = 5
		cyclic_chk.checked = false
		complete_chk.checked = false
		overrideDuration_chk.checked = false
		duration_spn.value = 1.0
		-- blend settings
		blend_chk.checked = false
		referenceFrame_spn.value = 0
		-- ground transform
		groundFrames_rdo.state = 1
		groundNumFrames_spn.value = 2
		groundFrameRate_spn.value = 15
		ignoreGround_chk.checked = false
		-- enable animation types
		morph_chk.checked = false
		texture_chk.checked = false
		vis_chk.checked = true
		trans_chk.checked = true
		uni_chk.checked = false
		arb_chk.checked = false
		ILF_chk.checked = true
		-- force animation types
		morphForce_chk.checked = false
		textureForce_chk.checked = false
		visForce_chk.checked = false
		transForce_chk.checked = false
		scaleForce_chk.checked = false
		
		EnableControls()
	)
	
	function SaveSequence = 
	(
		if name_edt.text != "" then
		(
			sequenceName = "sequence_" + name_edt.text
			s = getNodeByName sequenceName
			if s == null then
			(
				s = dummy name: sequenceName
				s.boxsize = [NodeX, NodeY, NodeZ]
			)
			-- sequence info
			setUserProp s "startFrame" startFrame_spn.value
			setUserProp s "endFrame" endFrame_spn.value
			setUserProp s "cyclic" cyclic_chk.checked
			setUserProp s "completeCycle" complete_chk.checked
			-- blend settings
			if blend_chk.checked == false then
			(
				setUserProp s "blend" false
				RemoveUserProp s "blendReferenceFrame" (getUserProp s "blendReferenceFrame")
			)
			else
			(
				setUserProp s "blend" true
				setUserProp s "blendReferenceFrame" referenceFrame_spn.value
			)
			-- frame sampling rate
			setUserProp s "frameType" frames_rdo.state
			if frames_rdo.state == 1 then
			(
				setUserProp s "frameRate" frameRate_spn.value
				RemoveUserProp s "numFrames" (getUserProp s "numFrames")
			)
			else
			(
				setUserProp s "numFrames" numFrames_spn.value
				RemoveUserProp s "frameRate" (getUserProp s "frameRate")
			)
			-- priority
			setUserProp s "priority" priority_spn.value
			-- duration
			if overrideDuration_chk.checked == true then
			(
				setUserProp s "overrideDuration" true
				setUserProp s "duration" duration_spn.value
			)
			else
			(
				RemoveUserProp s "overrideDuration" (getUserProp s "overrideDuration")
				RemoveUserProp s "duration" (getUserProp s "duration")
			)
			-- ground transform
			setUserProp s "groundType" groundFrames_rdo.state
			if ignoreGround_chk.checked == false then
			(
				if groundFrames_rdo.state == 1 then
				(
					setUserProp s "groundFrameRate" groundFrameRate_spn.value
					RemoveUserProp s "groundNumFrames" (getUserProp s "groundNumFrames")
				)
				else
				(
					setUserProp s "groundNumFrames" groundNumFrames_spn.value
					RemoveUserProp s "groundFrameRate" (getUserProp s "groundFrameRate")
				)
				RemoveUserProp s "ignoreGround" true
			)
			else
			(
				setUserProp s "ignoreGround" ignoreGround_chk.checked
				RemoveUserProp s "groundNumFrames" (getUserProp s "groundNumFrames")
				RemoveUserProp s "groundFrameRate" (getUserProp s "groundFrameRate")
			)
			-- enable animation types
			setUserProp s "enableMorph" morph_chk.checked
			setUserProp s "enableTVert" texture_chk.checked
			setUserProp s "enableVis" vis_chk.checked
			setUserProp s "enableTransform" trans_chk.checked
			setUserProp s "enableScale" true
			setUserProp s "enableUniformScale" uni_chk.checked
			setUserProp s "enableAlignedScale" true
			setUserProp s "enableArbitraryScale" arb_chk.checked
			setUserProp s "enableIFL" ILF_chk.checked
			-- force animation types
			setUserProp s "forceMorph" morphForce_chk.checked
			setUserProp s "forceTVert" textureForce_chk.checked
			setUserProp s "forceVis" visForce_chk.checked
			setUserProp s "forceTransform" transForce_chk.checked
			setUserProp s "forceScale" scaleForce_chk.checked		

			s.isFrozen = true
		)
		UpdateSequences()
	)
	
	function EditSequence =
	(
		name_edt.text = sequences_lbx.selected
		sequenceName = "sequence_" + name_edt.text
		s = getNodeByName sequenceName
		
		startFrame_spn.value = getUserProp s "startFrame" 
		endFrame_spn.value = getUserProp s "endFrame" 
		cyclic_chk.checked = getUserProp s "cyclic" 
		complete_chk.checked = getUserProp s "completeCycle" 
		-- blend settings
		if (getUserProp s "blend") == true then
		( 
			blend_chk.checked = getUserProp s "blend" 
			referenceFrame_spn.value = getUserProp s "blendReferenceFrame"
		)
		else
		(
			blend_chk.checked = false
			referenceFrame_spn.value = 0
		)
		-- frame sampleing rate
		frames_rdo.state = getUserProp s "frameType"
		if frames_rdo.state == 1 then
		(
			frameRate_spn.value = getUserProp s "frameRate"
			numFrames_spn.value = 1
		)
		else
		(
			numFrames_spn.value = getUserProp s "numFrames"
			frameRate_spn.value = 30
		)
		-- priority
		priority_spn.value = getUserProp s "priority" 
		-- duration
		if (getUserProp s "overrideDuration") == true then
		(
			overrideDuration_chk.checked = getUserProp s "overrideDuration" 
			duration_spn.value = getUserProp s "duration"
		)
		else
		(
			overrideDuration_chk.checked = false
			duration_spn.value = 1
		)
		-- ground transform
		groundFrames_rdo.state = getUserProp s "groundType"
		
		if (getUserProp s "ignoreGround") == true then
		(
			ignoreGround_chk.checked = true
		)
		else
		(
			ignoreGround_chk.checked = false
			
			if groundFrames_rdo.state == 1 then
			(
				groundFrameRate_spn.value = getUserProp s "groundFrameRate"
				--groundNumFrames_spn.value = 2
			)
			else
			(
				groundNumFrames_spn.value = getUserProp s "groundNumFrames"
				--groundFrameRate_spn.value = 15
			)
		)
		-- enable animation types
		morph_chk.checked = getUserProp s "enableMorph" 
		texture_chk.checked = getUserProp s "enableTVert" 
		vis_chk.checked = getUserProp s "enableVis" 
		trans_chk.checked = getUserProp s "enableTransform" 
		uni_chk.checked = getUserProp s "enableUniformScale" 
		arb_chk.checked = getUserProp s "enableArbitraryScale" 
		ILF_chk.checked = getUserProp s "enableIFL" 
		-- force animation types
		morphForce_chk.checked = getUserProp s "forceMorph" 
		textureForce_chk.checked = getUserProp s "forceTVert" 
		visForce_chk.checked = getUserProp s "forceVis" 
		transForce_chk.checked = getUserProp s "forceTransform" 
		scaleForce_chk.checked = getUserProp s "forceScale"
		
		EnableControls()	
	)
	
	function SetAmimationTime =
	(
		animationRange = interval startFrame_spn.value endFrame_spn.value
	)
	
	function GetAmimationTime =
	(
		startFrame_spn.value = animationRange.start
		endFrame_spn.value = animationRange.end
	)
	
	on Sequence_rollout open do
	(
		UpdateSequences()
		EnableControls()
		if sequences_lbx.selection != 0 then
		(
			EditSequence()
		)
	)
	
	on sequences_lbx selected selSeq  do
	(
		EditSequence()
	)
	
	on delete_btn pressed do
	(
		if sequences_lbx.selection != 0 then
		(
			objTmp = getNodeByName ("sequence_" + sequences_lbx.selected)
			delete objTmp
			UpdateSequences()
			ResetSequence()
		)
	)
	
	on edit_btn pressed do
	(
		if sequences_lbx.selection != 0 then
		(
			SelectedNode = getNodeByName ("sequence_" + sequences_lbx.selected)
			createDialog RenameSequence_rollout width:225 height:69
		)
	)
	
	on copy_btn pressed do
	(
		if sequences_lbx.selection != 0 then
		(
			objTmp = getNodeByName ("sequence_" + sequences_lbx.selected)
			curProp = getUserPropBuffer objTmp
			newObj = copy objTmp
			newObj.name = ("sequence_" + sequences_lbx.selected + "_copy")
			newObj.isFrozen = true
			setUserPropBuffer newObj curProp
			UpdateSequences()
		)
	)
	
	on add_btn pressed do
	(
		ResetSequence()
		--setFocus name_edt
		createDialog NewSequence_rollout width:225 height:69
	)
	
	on setTime_btn pressed do
	(
		SetAmimationTime ()
	)
	
	on getTime_btn pressed do
	(
		GetAmimationTime ()
	)
	
	on playAni_btn pressed do
	(
		playAnimation()
	)
	
	on stopAni_btn pressed do
	(
		stopAnimation()
	)
	
	on startFrame_spn changed curState do
	(
		SaveSequence()
	)
	
	on endFrame_spn changed curState do
	(
		SaveSequence()
	)
	
	on cyclic_chk changed curState do
	(
		SaveSequence()
	)
	
	on complete_chk changed curState do
	(
		SaveSequence()
	)
	
	on blend_chk changed blendState do
	(
		referenceFrame_spn.enabled = blendState
		SaveSequence()
	)
	
	on referenceFrame_spn changed do
	(
		SaveSequence()
	)
	
	on frames_rdo changed curState do
	(
		if curState == 2 then
		(
			numFrames_spn.enabled = true
			frameRate_spn.enabled = false
		)
		else
		(
			numFrames_spn.enabled = false
			frameRate_spn.enabled = true
		)
		SaveSequence()
	)
	
	on frameRate_spn changed curState do
	(
		SaveSequence()
	)
	
	on numFrames_spn changed curState do
	(
		SaveSequence()
	)
	
	on priority_spn changed curState do
	(
		SaveSequence()
	)
	
	on overrideDuration_chk changed durState do
	(
		duration_spn.enabled = durState
		SaveSequence()
	)
	
	on duration_spn changed curState do
	(
		SaveSequence()
	)
	
	on ignoreGround_chk changed ignoreState do
	(
		if ignoreState == true then
		(
			groundFrames_rdo.enabled = false
			groundFrames_rdo.state = 1
			groundFrameRate_spn.enabled = false
			groundNumFrames_spn.enabled = false
		)
		else
		(
			groundFrames_rdo.enabled = true
			groundFrames_rdo.state = 1
			groundFrameRate_spn.enabled = true
		)
		SaveSequence()
	)
	
	on groundFrames_rdo changed curState do
	(
		if curState == 2 then
		(
			groundNumFrames_spn.enabled = true
			groundFrameRate_spn.enabled = false
		)
		else
		(
			groundNumFrames_spn.enabled = false
			groundFrameRate_spn.enabled = true
		)
		SaveSequence()
	)
	
	on groundFrameRate_spn changed curState do
	(
		SaveSequence()
	)
	
	on groundNumFrames_spn changed curState do
	(
		SaveSequence()
	)
	
	on morph_chk changed curState do
	(
		SaveSequence()
	)
	
	on vis_chk changed curState do
	(
		SaveSequence()
	)
	
	on trans_chk changed curState do
	(
		SaveSequence()
	)
	
	on uni_chk changed curState do
	(
		SaveSequence()
	)
	
	on arb_chk changed curState do
	(
		SaveSequence()
	)
	
	on texture_chk changed curState do
	(
		SaveSequence()
	)
	
	on ILF_chk changed curState do
	(
		SaveSequence()
	)

	on morphForce_chk changed curState do
	(
		SaveSequence()
	)

	on visForce_chk changed curState do
	(
		SaveSequence()
	)

	on transForce_chk changed curState do
	(
		SaveSequence()
	)

	on scaleForce_chk changed curState do
	(
		SaveSequence()
	)

	on textureForce_chk changed curState do
	(
		SaveSequence()
	)
	
	on load_btn pressed do
	(
		seqOpenPath = getOpenFileName caption:"Load Sequence Settings:" filename:"$scripts/dts/sequences/"
		
		if seqOpenPath != undefined then
		(
			seqFile = openFile seqOpenPath
			seek seqFile #eof
			maxlen=filepos seqFile
			seek seqFile 0
			
			curProp = readChars seqFile maxlen errorAtEOF:false
			
			seqName = (getFilenameFile seqOpenPath)
			sequenceName = ("sequence_" + seqName)
			s = dummy name: sequenceName
			s.boxsize = [NodeX, NodeY, NodeZ]
			s.isFrozen = true
			setUserPropBuffer s curProp
		)
		UpdateSequences()
	)
	
	on save_btn pressed do
	(
		if sequences_lbx.selection != 0 then
		(
			seqName = sequences_lbx.selected
			objTmp = getNodeByName ("sequence_" + seqName)
			curProp = getUserPropBuffer objTmp
			
			seqSavePath = getSaveFileName caption:"Save Sequence Settings:" filename:("$scripts/dts/sequences/" + seqName + ".txt")
		
			if seqSavePath != undefined then
			(
				seqFilePath = seqSavePath
				seqFile = createfile seqFilePath
				format "%" curProp to:seqFile
				
				close seqFile
			)
		)
		UpdateSequences()
	)
)

---------------------------------------------------------------------
-- Node Manager
---------------------------------------------------------------------
rollout CFGSettings_rollout "CFG Settings"
(
	imgTag uiBar "" bitmap:GrayBar_bmp pos:[0,0]
	group "Dump File Settings"
	(
		checkbox NodeCollection_chk "Node Collection" across:2
		checkbox ShapeConstruction_chk "Shape Construction"
		checkbox NodeCulling_chk "Node Culling" across:2
		checkbox NodeStates_chk "Node States"
		checkbox NodeStateDetails_chk "Node State Details" across:2
		checkbox ObjectStates_chk "Object States"
		checkbox ObjectStateDetails_chk "Object State Details" across:2
		checkbox ObjectOffsets_chk "Object Offsets"
		checkbox SequenceDetails_chk "Sequence Details" across:2
		checkbox ShapeHierarchy_chk "Shape Hierarchy"
	)
	group "Error Control"
	(
		checkbox AllowEmptySubtrees_chk "Allow Empty Subtrees " across:2
		checkbox AllowCrossedDetails_chk "Allow Crossed Details"
		checkbox AllowUnusedMeshes_chk "Allow Unused Meshes"
	)
	group "Default Sequence Settings"
	(
		checkbox defaultCyclic_chk "Default Cyclic" across:2
		checkbox defaultBlend_chk "Default Blend"
		checkbox defaultFirstLastFrameSame_chk "First Last Frame Same" across:2
		checkbox defaultUseFrameRate_chk "Use FrameRate"
		checkbox defaultIgnoreGroundTransform_chk "Ignore GroundTransform" across:2
		checkbox defaultUseGroundFrameRate_chk "Use Ground FrameRate"
		checkbox defaultEnableMorphAnimation_chk "Enable Morph Animation" across:2
		checkbox defaultEnableVisAnimation_chk "Enable Vis Animation"
		checkbox defaultEnableTransformAnimation_chk "Enable Transform Animation" across:2
		checkbox defaultForceMorphAnimation_chk "Force Morph Animation"
		checkbox defaultForceVisAnimation_chk "Force Vis Animation" across:2
		checkbox defaultForceTransformAnimation_chk "Force Transform Animation"
		checkbox defaultOverrideDuration_chk "Override Duration"
		--defaultFrameRate
		--defaultGroundFrameRate
		--defaultDuration
		--defaultNumFrames
		--defaultNumGroundFrames
		--defaultDefaultSequencePriority
	)
	group "Paramaters"
	(
		checkbox EnableTwoSidedMaterials_chk "Enable TwoSided Materials" across:2
		checkbox CollapseTransforms_chk "Collapse Transforms"
		checkbox SequenceExport_chk "Sequence Export"
		--AnimationDelta
		--MaxFrameRate
		--SkinWeightThreshhold
		--weightsPerVertex
		--baseTexturePath
	)
	group "Smoothing Groups"
	(
		checkbox IgnoreOnSkinMesh_chk "Ignore On Skined Mesh" across:2
		checkbox IgnoreDuringCollapse_chk "Ignore During Collapse "
	)
	button cfgCancel_btn "cancel" width:75 height:20 across:2
	button cfgApply_btn "apply" width:75 height:20
	
	function UpdateCFGSettings =
	(
		n = getNodeByName "cfg_settings"
		-- dmp file settings
		NodeCollection_chk.checked = getUserProp n "NodeCollection"
		ShapeConstruction_chk.checked = getUserProp n "ShapeConstruction"
		NodeCulling_chk.checked = getUserProp n "NodeCulling"
		NodeStates_chk.checked = getUserProp n "NodeStates"
		NodeStateDetails_chk.checked = getUserProp n "NodeStateDetails"
		ObjectStates_chk.checked = getUserProp n "ObjectStates"
		ObjectStateDetails_chk.checked = getUserProp n "ObjectStateDetails"
		ObjectOffsets_chk.checked = getUserProp n "ObjectOffsets"
		SequenceDetails_chk.checked = getUserProp n "SequenceDetails"
		ShapeHierarchy_chk.checked = getUserProp n "ShapeHierarchy"
		-- error control
		AllowEmptySubtrees_chk.checked = getUserProp n "AllowEmptySubtrees"
		AllowCrossedDetails_chk.checked = getUserProp n "AllowCrossedDetails"
		AllowUnusedMeshes_chk.checked = getUserProp n "AllowUnusedMeshes"
		-- sequence settings
		defaultCyclic_chk.checked = getUserProp n "defaultCyclic"
		defaultBlend_chk.checked = getUserProp n "defaultBlend"
		defaultFirstLastFrameSame_chk.checked = getUserProp n "defaultFirstLastFrameSame"
		defaultUseFrameRate_chk.checked = getUserProp n "defaultUseFrameRate"
		defaultIgnoreGroundTransform_chk.checked = getUserProp n "defaultIgnoreGroundTransform"
		defaultUseGroundFrameRate_chk.checked = getUserProp n "defaultUseGroundFrameRate"
		defaultEnableMorphAnimation_chk.checked = getUserProp n "defaultEnableMorphAnimation"
		defaultEnableVisAnimation_chk.checked = getUserProp n "defaultEnableVisAnimation"
		defaultEnableTransformAnimation_chk.checked = getUserProp n "defaultEnableTransformAnimation"
		defaultForceMorphAnimation_chk.checked = getUserProp n "defaultForceMorphAnimation"
		defaultForceVisAnimation_chk.checked = getUserProp n "defaultForceVisAnimation"
		defaultForceTransformAnimation_chk.checked = getUserProp n "defaultForceTransformAnimation"
		defaultOverrideDuration_chk.checked = getUserProp n "defaultOverrideDuration"
		-- param settings
		EnableTwoSidedMaterials_chk.checked = getUserProp n "EnableTwoSidedMaterials"
		CollapseTransforms_chk.checked = getUserProp n "CollapseTransforms"
		SequenceExport_chk.checked = getUserProp n "SequenceExport"
		-- smoothing groups
		IgnoreOnSkinMesh_chk.checked = getUserProp n "IgnoreOnSkinMesh"
		IgnoreDuringCollapse_chk.checked = getUserProp n "IgnoreDuringCollapse"
	)
	
	function SaveCFGSettings =
	(
		n = getNodeByName "cfg_settings"
		-- dmp file settings
		setUserProp n "NodeCollection" NodeCollection_chk.checked
		setUserProp n "ShapeConstruction" ShapeConstruction_chk.checked
		setUserProp n "NodeCulling" NodeCulling_chk.checked
		setUserProp n "NodeStates" NodeStates_chk.checked
		setUserProp n "NodeStateDetails" NodeStateDetails_chk.checked
		setUserProp n "ObjectStates" ObjectStates_chk.checked
		setUserProp n "ObjectStateDetails" ObjectStateDetails_chk.checked
		setUserProp n "ObjectOffsets" ObjectOffsets_chk.checked
		setUserProp n "SequenceDetails" SequenceDetails_chk.checked
		setUserProp n "ShapeHierarchy" ShapeHierarchy_chk.checked
		-- error control
		setUserProp n "AllowEmptySubtrees" AllowEmptySubtrees_chk.checked
		setUserProp n "AllowCrossedDetails" AllowCrossedDetails_chk.checked
		setUserProp n "AllowUnusedMeshes" AllowUnusedMeshes_chk.checked
		-- sequence settings
		setUserProp n "defaultCyclic" defaultCyclic_chk.checked
		setUserProp n "defaultBlend" defaultBlend_chk.checked
		setUserProp n "defaultFirstLastFrameSame" defaultFirstLastFrameSame_chk.checked
		setUserProp n "defaultUseFrameRate" defaultUseFrameRate_chk.checked
		setUserProp n "defaultIgnoreGroundTransform" defaultIgnoreGroundTransform_chk.checked
		setUserProp n "defaultUseGroundFrameRate" defaultUseGroundFrameRate_chk.checked
		setUserProp n "defaultEnableMorphAnimation" defaultEnableMorphAnimation_chk.checked
		setUserProp n "defaultEnableVisAnimation" defaultEnableVisAnimation_chk.checked
		setUserProp n "defaultEnableTransformAnimation" defaultEnableTransformAnimation_chk.checked
		setUserProp n "defaultForceMorphAnimation" defaultForceMorphAnimation_chk.checked
		setUserProp n "defaultForceVisAnimation" defaultForceVisAnimation_chk.checked
		setUserProp n "defaultForceTransformAnimation" defaultForceTransformAnimation_chk.checked
		setUserProp n "defaultOverrideDuration" defaultOverrideDuration_chk.checked
		-- param settings
		setUserProp n "EnableTwoSidedMaterials" EnableTwoSidedMaterials_chk.checked 
		setUserProp n "CollapseTransforms" CollapseTransforms_chk.checked
		setUserProp n "SequenceExport" SequenceExport_chk.checked
		-- smoothing groups
		setUserProp n "IgnoreOnSkinMesh" IgnoreOnSkinMesh_chk.checked 
		setUserProp n "IgnoreDuringCollapse" IgnoreDuringCollapse_chk.checked
	)
	
	on CFGSettings_rollout open do
	(
		UpdateCFGSettings()
	)
	
	on cfgCancel_btn pressed do
	(
		try(destroyDialog CFGSettings_rollout) catch()
	)
	
	on cfgApply_btn pressed do
	(
		SaveCFGSettings()
		try(destroyDialog CFGSettings_rollout) catch()
	)
)

rollout RenameNode_rollout "Rename Node"
(
	label rename_lbl "Node Name:" pos:[13,14] width:77 height:15
	editText nodeName_edt "" pos:[91,12] width:122 height:20
	button renameCancel_btn "cancel" pos:[95,36] width:58 height:20
	button renameApply_btn "apply" pos:[155,36] width:58 height:20
	
	on RenameNode_rollout open do
	(
		nodeName_edt.text = SelectedNode.name
		setFocus nodeName_edt
	)
	
	on renameCancel_btn pressed do
	(
		try(destroyDialog RenameNode_rollout) catch()
	)
	
	on renameApply_btn pressed do
	(
		
		SelectedNode.name = nodeName_edt.text
		SelectedNode = null
		try(Node_rollout.UpdateNodes()) catch() 
		try(destroyDialog RenameNode_rollout) catch()
	)
)

rollout Node_rollout "Node Manager"
(
	imgTag uiGrayBar "" bitmap:GrayBar_bmp pos:[1,1]
	groupBox nodes_grp "Nodes" pos:[10,20] width:381 height:175
	button deleteNode_btn "-" pos:[22,39] width:23 height:16
	button editNode_btn "edit" pos:[46,39] width:34 height:16
	button renameNode_btn "rename" pos:[81,39] width:50 height:16
	button addNode_btn "+" pos:[132,39] width:23 height:16
	multiListBox nodes_lbx "" pos:[23,56] width:151 height:9
	
	groupBox nodeSettings_grp "Node Settings" pos:[195,37] width:181 height:143
	label nodeName_lbl "Name:" pos:[209,61] width:58 height:15
	editText nodeName_edt "" pos:[240,58] width:99 height:21
	checkbox alwaysExport_chk "always export" pos:[244,88] width:111 height:16 checked: true
	checkbox neverExport_chk "never export" pos:[244,107] width:111 height:16
	checkbox neverAnimate_chk "never animate" pos:[244,127] width:111 height:16
	button saveNode_btn "save" pos:[302,150] width:64 height:16
	
	groupBox nodePresets_grp "Node Presets" pos:[11,205] width:173 height:222
	button deletePreset_btn "-" pos:[22,226] width:23 height:16
	button editPreset_btn "edit" pos:[46,226] width:34 height:16
	button addPreset_btn "+" pos:[82,226] width:23 height:16
	button applyPreset_btn "apply" pos:[132,226] width:42 height:16
	listBox presets_lbx "" pos:[23,243] width:150 height:5
	
	groupBox presetSettings_grp "Preset Settings" pos:[23,319] width:150 height:99
	label presetName_lbl "name:" pos:[32,345] width:36 height:15
	editText presetName_edt "" pos:[63,342] width:99 height:21 enabled:false
	label presetNodes_lbl "nodes:" pos:[32,370] width:36 height:15
	editText presetNodes_edt "" pos:[63,367] width:99 height:21 enabled:false
	button savePreset_btn "save" pos:[99,393] width:64 height:16 enabled:false
	
	groupBox nodeUtilities_grp "Node Utilities" pos:[195,205] width:195 height:222
	button createCFG_btn "create .CFG" pos:[216,231] width:155 height:20
	button setAlwaysExport_btn "set always export" pos:[216,257] width:155 height:20
	button setNeverExport_btn "set never export" pos:[216,283] width:155 height:20
	button setNeverAnimate_btn "set never animate" pos:[216,309] width:155 height:20
	button removeSettings_btn "remove .CFG settings" pos:[216,335] width:155 height:20
	button cfgSettings_btn ".CFG settings" pos:[216,360] width:155 height:20
	
	function SelectNodes =
	(
		clearSelection()
		select $*
		try ( deselect $detail* ) catch ()
		try ( deselect $collision* ) catch ()
		try ( deselect $loscol* ) catch ()
		try ( deselect $sequence_* ) catch () 
		try ( deselect $cfg_settings ) catch ()
		--print( classOf $Base01 )
	)
	
	function UpdateNodes =
	(
		SelectNodes()
		nodeArr = #()
		for o in selection do
		(
			if classOf o == BoneGeometry then append nodeArr o.name
			if classOf o == Dummy then append nodeArr o.name
		)
		nodes_lbx.items = nodeArr
		clearSelection()
	)
	
	function ResetNodes =
	(
		nodeName_edt.text = ""
		alwaysExport_chk.checked = true
		neverExport_chk.checked = false
		neverAnimate_chk.checked = false
	)
	
	function GetNode selNode =
	(
		n = getNodeByName selNode
		if n != null then
		(
			nodeName_edt.text = selNode
			
			if (getUserProp n "alwaysExport") == true then
			(
				alwaysExport_chk.checked = true
			)
			else
			(
				alwaysExport_chk.checked = false
			)
			
			if (getUserProp n "neverExport") == true then
			(
				neverExport_chk.checked	= true
			)
			else
			(
				neverExport_chk.checked	= false
			)
			
			if (getUserProp n "neverAnimate") == true then
			(
				neverAnimate_chk.checked = true
			)
			else
			(
				neverAnimate_chk.checked = false
			)
		)
		else
		(
			UpdateNodes()
		)
	)
	
	function CreateNode name alwaysX neverX neverA =
	(
		if name != "" then
		(
			CreateDTSShape()
			n = getNodeByName name
			if n == null then
			(
				n = dummy name: name
				n.boxsize = [NodeX, NodeY, NodeZ]
				n.parent = $start01
			)
			setUserProp n "alwaysExport" alwaysX
			setUserProp n "neverExport" neverX
			setUserProp n "neverAnimate" neverA
		)
		UpdateNodes()
	)
	
	function DeleteNode tmpNodes =
	(
		--if nodes_lbx.selection != 0 then
		if tmpNodes.count != 0 then
		(
			for node in tmpNodes do
			(
				n = getNodeByName nodes_lbx.items[node]
				delete n
			)
			UpdateNodes()
		)
	)
	
	function UpdatePresets =
	(
		setNames = getINISetting "$scripts\dts_nodes.ini" "NodeSets" "SetNames"
		presetList = (filterString setNames ", -")
		if presetList.count != 0 then
		(
			presets_lbx.items = presetList
		)
	)
	
	function EditPreset preset =
	(
		presetName_edt.text = preset
		presetNodes_edt.text = (getINISetting "$scripts\dts_nodes.ini" "NodeSets" preset)
	)
	
	function CreateNodeSet setName setNodes  =
	(
		tmpSet = filterString setNodes ", -"
		tmpNames = ""
		for name in tmpSet do
		(
			tmpNames += ( name + "," )
		)
		setINISetting "$scripts\dts_nodes.ini" "NodeSets" setName (trimRight tmpNames ",")
		
		
		setNames = getINISetting "$scripts\dts_nodes.ini" "NodeSets" "SetNames"
		tmpSetNames = filterString setNames ", -"
		
		nameTest = findItem tmpSetNames setName
		if nameTest == 0 then
		(
			append tmpSetNames setName
		)
		
		tmpNames = ""
		for name in tmpSetNames do
		(
			tmpNames += ( name + "," )
		)
		setINISetting "$scripts\dts_nodes.ini" "NodeSets" "SetNames" (trimRight tmpNames ",")
		UpdatePresets()
	)
	
	function DeleteNodeSet preset =
	(
		delIniSetting "$scripts\dts_nodes.ini" "NodeSets" preset
		
		setNames = getINISetting "$scripts\dts_nodes.ini" "NodeSets" "SetNames"
		tmpSetNames = filterString setNames ", -"
		deleteItem tmpSetNames (findItem tmpSetNames preset)
		
		tmpNames = ""
		for name in tmpSetNames do
		(
			tmpNames += ( name + "," )
		)
		setINISetting "$scripts\dts_nodes.ini" "NodeSets" "SetNames" (trimRight tmpNames ",")
		presetName_edt.text = ""
		presetNodes_edt.text = ""
		
		UpdatePresets()
	)
	
	function EnablePreset =
	(
		presetName_edt.enabled = true
		presetNodes_edt.enabled = true
		savePreset_btn.enabled = true
	)
	
	function DisablePreset =
	(
		presetName_edt.enabled = false
		presetNodes_edt.enabled = false
		savePreset_btn.enabled = false
	)
	
	function DisableSettings =
	(
		nodeName_edt.enabled = false
		alwaysExport_chk.enabled = false
		neverExport_chk.enabled = false
		neverAnimate_chk.enabled = false
	)
	
	function EnableSettings =
	(
		nodeName_edt.enabled = true
		alwaysExport_chk.enabled = true
		neverExport_chk.enabled = true
		neverAnimate_chk.enabled = true
	)
	
	function CreateCFGNode =
	(
		if getNodeByName "cfg_settings" == undefined then
		(
			n = dummy name: "cfg_settings" boxsize:[NodeX, NodeY, NodeZ]
			n.isFrozen = true
			
			cfg_text = "NodeCollection = true\r\n"
			-- dmp settings
			cfg_text = cfg_text + "ShapeConstruction = true\r\n"
			cfg_text = cfg_text + "NodeCulling = true\r\n"
			cfg_text = cfg_text + "NodeStates = true\r\n"
			cfg_text = cfg_text + "NodeStateDetails = true\r\n"
			cfg_text = cfg_text + "ObjectStates = true\r\n"
			cfg_text = cfg_text + "ObjectStateDetails = true\r\n"
			cfg_text = cfg_text + "ObjectOffsets = true\r\n"
			cfg_text = cfg_text + "SequenceDetails = true\r\n"
			cfg_text = cfg_text + "ShapeHierarchy = true\r\n"
			-- error control
			cfg_text = cfg_text + "AllowEmptySubtrees = true\r\n"
			cfg_text = cfg_text + "AllowCrossedDetails = false\r\n"
			cfg_text = cfg_text + "AllowUnusedMeshes = true\r\n"
			-- sequence defaults
			cfg_text = cfg_text + "defaultCyclic = true\r\n"
			cfg_text = cfg_text + "defaultBlend = false\r\n"
			cfg_text = cfg_text + "defaultFirstLastFrameSame = false\r\n"
			cfg_text = cfg_text + "defaultUseFrameRate = true\r\n"
			cfg_text = cfg_text + "defaultIgnoreGroundTransform = false\r\n"
			cfg_text = cfg_text + "defaultUseGroundFrameRate = false\r\n"
			cfg_text = cfg_text + "defaultEnableMorphAnimation = false\r\n"
			cfg_text = cfg_text + "defaultEnableVisAnimation = false\r\n"
			cfg_text = cfg_text + "defaultEnableTransformAnimation = true\r\n"
			cfg_text = cfg_text + "defaultForceMorphAnimation = false\r\n"
			cfg_text = cfg_text + "defaultForceVisAnimation = false\r\n"
			cfg_text = cfg_text + "defaultForceTransformAnimation = false\r\n"
			cfg_text = cfg_text + "defaultOverrideDuration = false\r\n"
			cfg_text = cfg_text + "defaultFrameRate = 15.000000\r\n"
			cfg_text = cfg_text + "defaultGroundFrameRate = 15.000000\r\n"
			cfg_text = cfg_text + "defaultDuration = 1.000000\r\n"
			cfg_text = cfg_text + "defaultNumFrames = 2\r\n"
			cfg_text = cfg_text + "defaultNumGroundFrames = 2\r\n"
			cfg_text = cfg_text + "defaultDefaultSequencePriority = 0\r\n"
			-- parameter settings
			cfg_text = cfg_text + "EnableTwoSidedMaterials = true\r\n"
			cfg_text = cfg_text + "CollapseTransforms = true\r\n"
			cfg_text = cfg_text + "SequenceExport = true\r\n"
			cfg_text = cfg_text + "AnimationDelta = 0.000100\r\n"
			cfg_text = cfg_text + "MaxFrameRate = 30.000000\r\n"
			cfg_text = cfg_text + "SkinWeightThreshhold = 0.001000\r\n"
			cfg_text = cfg_text + "weightsPerVertex = 10\r\n"
			cfg_text = cfg_text + "baseTexturePath = .\r\n"
			-- smoothing groups
			cfg_text = cfg_text + "IgnoreOnSkinMesh = true\r\n"
			cfg_text = cfg_text + "IgnoreDuringCollapse = false\r\n"
			
			setUserPropBuffer n cfg_text
		)
	)
	
	on Node_rollout open do
	(
		UpdateNodes()
		EnableSettings()
		UpdatePresets()
	)
	
	on nodes_lbx selected selNode do
	(
		selectionTmp = nodes_lbx.selection as array
		if selectionTmp.count == 1 then
		(
			EnableSettings()
			GetNode nodes_lbx.items[selectionTmp[1]]
		)
		else
		(
			DisableSettings()
		)
	)
	
	on nodes_lbx doubleClicked selNode do
	(
		UpdateNodes()
	)
	
	on saveNode_btn pressed do
	(
		CreateNode nodeName_edt.text alwaysExport_chk.checked neverExport_chk.checked neverAnimate_chk.checked
	)
	
	on deleteNode_btn pressed do
	(
		DeleteNode (nodes_lbx.selection as array)
	)
	
	on editNode_btn pressed do
	(
		selectionTmp = nodes_lbx.selection as array
		if selectionTmp.count != 0 then
		(
			GetNode nodes_lbx.items[selectionTmp[1]]
		)
	)
	
	on renameNode_btn pressed do
	(
		selectionTmp = nodes_lbx.selection as array
		if selectionTmp.count != 0 then
		(
			SelectedNode = getNodeByName nodes_lbx.items[selectionTmp[1]]
			createDialog RenameNode_rollout width:225 height:69
		)
	)
	
	on addNode_btn pressed do
	(
		ResetNodes()
		setFocus nodeName_edt
	)
	
	on presets_lbx selected selPreset do
	(
		EditPreset presets_lbx.selected
		DisablePreset()
	)
	
	on deletePreset_btn pressed do
	(
		if presets_lbx.selection != 0 then
		(
			DeleteNodeSet presets_lbx.selected
		)
	)
	
	on editPreset_btn pressed do
	(
		if presets_lbx.selection != 0 then
		(
			EditPreset presets_lbx.selected
			EnablePreset()
		)
	)
	
	on addPreset_btn pressed do
	(
		presetName_edt.text = ""
		presetNodes_edt.text = ""
		EnablePreset()
	)
	
	on applyPreset_btn pressed do
	(
		if presets_lbx.selection != 0 then
		(
			ApplyNodePreset presets_lbx.selected
		)
	)
	
	on savePreset_btn pressed do
	(
		if presetName_edt.text != "" then
		(
			CreateNodeSet presetName_edt.text presetNodes_edt.text
		)
		DisablePreset()
	)
	
	on createCFG_btn pressed do
	(
		CreateConfigFile undefined
	)
	
	on setAlwaysExport_btn pressed do
	(
		selectionTmp = nodes_lbx.selection as array
		if selectionTmp.count != 0 then
		(
			for i = 1 to selectionTmp.count do
			(
				n = getNodeByName nodes_lbx.items[selectionTmp[i]]
				setUserProp n "alwaysExport" true
			)
		)
	)
	
	on setNeverExport_btn pressed do
	(
		selectionTmp = nodes_lbx.selection as array
		if selectionTmp.count != 0 then
		(
			for i = 1 to selectionTmp.count do
			(
				n = getNodeByName nodes_lbx.items[selectionTmp[i]]
				setUserProp n "neverExport" true
			)
		)
	)
	
	on setNeverAnimate_btn pressed do
	(
		selectionTmp = nodes_lbx.selection as array
		if selectionTmp.count != 0 then
		(
			for i = 1 to selectionTmp.count do
			(
				n = getNodeByName nodes_lbx.items[selectionTmp[i]]
				setUserProp n "neverAnimate" true
			)
		)
	)
	
	on removeSettings_btn pressed do
	(
		selectionTmp = nodes_lbx.selection as array
		if selectionTmp.count != 0 then
		(
			for i = 1 to selectionTmp.count do
			(
				n = getNodeByName nodes_lbx.items[selectionTmp[i]]
				setUserProp n "alwaysExport" false
				setUserProp n "neverExport" false
				setUserProp n "neverAnimate" false
			)
		)
		try( delete $cfg_settings ) catch()
	)
	
	on cfgSettings_btn pressed do
	(
		CreateCFGNode()
		createDialog CFGSettings_rollout width:350 height:515
	)
)

---------------------------------------------------------------------
-- Billboard Manager
---------------------------------------------------------------------
rollout Billboard_rollout "Billboard Manager"
(
	imgTag uiGrayBar "" bitmap:GrayBar_bmp pos:[1,1]
	groupBox details_grp "Detail Meshes" pos:[7,20] width:380 height:130
	listBox details_lbx "" pos:[18,41] width:164 height:7
	
	groupBox detailMarkers_grp "Detail Markers" pos:[7,155] width:380 height:194
	listBox detailMarkers_lbx "" pos:[18,175] width:164 height:12
	
	groupBox detailUtil_grp "Billboard Settings" pos:[200,35] width:170 height:104
	checkbox billboard_chk "Set as billboard" pos:[221,50] width:128 height:18	
	
	groupBox btype_grp "Billboard Type:" pos:[220,70] width:131 height:58
	radioButtons bbtype_rdo "" pos:[244,88] width:67 height:32 labels:#("billboard", "billboardz") default:1 columns:1 enabled: false
	
	groupBox settings_grp "Auto Billboard Settings:" pos:[200,168] width:170 height:169
	checkbox autoBillboard_chk "Set as auto billboard" pos:[210,185] width:128 height:18
	
	label equatorSteps_lbl "Equator Steps:" pos:[210,208] width:75 height:17 enabled: false
	spinner equatorSteps_spn "" pos:[285,207] width:68 height:16 range:[0,25,12] type:#integer scale:0.1 enabled: false
	
	label polarSteps_lbl "Polar Steps:" pos:[210,229] width:75 height:17 enabled: false
	spinner polarSteps_spn "" pos:[285,228] width:68 height:16 range:[0,25,6] type:#integer scale:0.1 enabled: false

	label detailLvl_lbl "Detail Level:" pos:[210,250] width:75 height:17 enabled: false
	spinner detailLevel_spn "" pos:[285,249] width:68 height:16 range:[0,1000,2] type:#integer scale:0.1 enabled: false
	
	label textureRes_lbl "Texture Res:" pos:[210,271] width:75 height:17 enabled: false
	spinner dim_spn "" pos:[285,270] width:68 height:16 range:[0,1024,128] type:#integer scale:0.1 enabled: false
	
	checkbox incPoles_chk "Include Poles" pos:[210,290] width:101 height:18 enabled: false
	label polarAng_lbl "Polar Angle:" pos:[210,311] width:75 height:17 enabled: false
	spinner polarAngle_spn "" pos:[285,310] width:68 height:16 range:[0,90,30] type:#integer scale:0.1 enabled: false
	
	function SelectDetailMarkers =
	(
		clearSelection()
		select $detail*
	)
	
	function SelectDetails =
	(
		clearSelection()
		select $geometry
		try ( deselect $bounds ) catch ()
		try ( deselect $col* ) catch ()
		try ( deselect $los* ) catch ()
	)
	
	function UpdateDetailMarkers =
	(
		SelectDetailMarkers()
		detailMarkersArr = #()
		for o in selection do
		(
			append detailMarkersArr o.name
		)
		detailMarkers_lbx.items = detailMarkersArr
		ClearSelection()
	)
	
	function UpdateDetails =
	(
		SelectDetails()
		detailArr = #()
		for o in selection do
		(
			if classOf o != BoneGeometry then
			(
				append detailArr o.name
			)
		)
		details_lbx.items = detailArr
		clearSelection()
		UpdateDetailMarkers()
	)
	
	function EnableBillboard =
	(
		bbtype_rdo.enabled = true
	)
	
	function EnableAutoBillboard =
	(
		equatorSteps_lbl.enabled = true
		equatorSteps_spn.enabled = true
		polarSteps_lbl.enabled = true
		polarSteps_spn.enabled = true
		detailLvl_lbl.enabled = true
		detailLevel_spn.enabled = true
		textureRes_lbl.enabled = true
		dim_spn.enabled = true
		incPoles_chk.enabled = true
	)
	
	function DisableBillboard =
	(
		billboard_chk.checked = false
		bbtype_rdo.enabled = false
		bbtype_rdo.state = 1
	)
	
	function DisableAutoBillboard =
	(
		autoBillboard_chk.checked = false
		equatorSteps_lbl.enabled = false
		equatorSteps_spn.enabled = false
		polarSteps_lbl.enabled = false
		polarSteps_spn.enabled = false
		detailLvl_lbl.enabled = false
		detailLevel_spn.enabled = false
		textureRes_lbl.enabled = false
		dim_spn.enabled = false
		incPoles_chk.enabled = false
		incPoles_chk.checked = false
		polarAng_lbl.enabled = false
		polarAngle_spn.enabled = false
	)
	
	function SaveBillboard =
	(
		if details_lbx.selected != null then
		(
			b = getNodeByName details_lbx.selected
			if billboard_chk.checked == true then
			(
				setUserProp b "enableBillboard" true
				setUserProp b "bb_Type" bbtype_rdo.state
			)
			else
			(
				setUserPropBuffer b ""
			)
		)
		else
		(
			DisableBillboard()
		)
	)
	
	function SaveAutoBillboard =
	(
		if detailMarkers_lbx.selected != null then
		(
			b = getNodeByName detailMarkers_lbx.selected
			if autoBillboard_chk.checked == true then
			(
				setUserProp b "enableAutoBillboard" true
				setUserProp b "bb_equator_steps" equatorSteps_spn.value
				setUserProp b "bb_polar_steps" polarSteps_spn.value
				setUserProp b "bb_dl" detailLevel_spn.value
				setUserProp b "bb_dim" dim_spn.value
				
				if incPoles_chk.checked == false then
				(
					RemoveUserProp b "bb_include_poles" (getUserProp b "bb_include_poles")
					RemoveUserProp b "bb_polar_angle" (getUserProp b "bb_polar_angle")
				)
				else
				(
					setUserProp b "bb_include_poles" true
					setUserProp b "bb_polar_angle" polarAngle_spn.value
				)
			)
			else
			(
				setUserPropBuffer b ""
			)
		)
		else
		(
			DisableAutoBillboard()
		)
	)
	
	function EditBillboard =
	(
		if details_lbx.selected != null then
		(
			b = getNodeByName details_lbx.selected
			if b != null then
			(
				if (getUserProp b "enableBillboard") == true then
				(
					billboard_chk.checked = true
					bbtype_rdo.state = (getUserProp b "bb_Type")
					EnableBillboard()
				)
				else
				(	
					DisableBillboard()
				)
			)
			else
			(
				UpdateDetails()
				EditBillboard()
			)
		)
	)
	
	function EditAutoBillboard =
	(
		if detailMarkers_lbx.selected != null then
		(
			b = getNodeByName detailMarkers_lbx.selected
			if b != null then
			(
				if (getUserProp b "enableAutoBillboard") == true then
				(
					autoBillboard_chk.checked = true
					equatorSteps_spn.value = (getUserProp b "bb_equator_steps")
					polarSteps_spn.value = (getUserProp b "bb_polar_steps")
					detailLevel_spn.value = (getUserProp b "bb_dl")
					dim_spn.value = (getUserProp b "bb_dim")
					
					if (getUserProp b "bb_include_poles") == true then
					(
						incPoles_chk.checked = true
						polarAng_lbl.enabled = true
						polarAngle_spn.enabled = true
						polarAngle_spn.value = (getUserProp b "bb_polar_angle")
					)
					else
					(
						incPoles_chk.checked = false
					)
					EnableAutoBillboard()
				)
				else
				(	
					DisableAutoBillboard()
				)
			)
			else
			(
				UpdateDetails()
				EditBillboard()
			)
		)
	)
	
	on Billboard_rollout open do
	(
		UpdateDetails()
		EditBillboard()
		EditAutoBillboard()
	)
	
	on details_lbx selected detail do
	(
		EnableBillboard()
		EditBillboard()
	)
	
	on billboard_chk changed state do
	(
		if state == true then EnableBillboard()
		else
		(
			DisableBillboard()
		)
		SaveBillboard()
	)
	
	on bbtype_rdo changed state do
	(
		SaveBillboard()
	)
	
	on detailMarkers_lbx selected detail do
	(
		EnableAutoBillboard()
		EditAutoBillboard()
	)
	
	on autoBillboard_chk changed state do
	(
		if state == true then EnableAutoBillboard()
		else
		(
			DisableAutoBillboard()
		)
		SaveAutoBillboard()
	)
	
	on incPoles_chk changed state do
	(
		if state == true then
		(
			polarAng_lbl.enabled = true
			polarAngle_spn.enabled = true
			SaveAutoBillboard()
		)
		else
		(
			polarAng_lbl.enabled = false
			polarAngle_spn.enabled = false
			SaveAutoBillboard()
		)
	)
	
	on equatorSteps_spn changed val do
	(
		SaveAutoBillboard()
	)
	
	on polarSteps_spn changed val do
	(
		SaveAutoBillboard()
	)
	
	on detailLevel_spn changed val do
	(
		SaveAutoBillboard()
	)
	
	on dim_spn changed val do
	(
		SaveAutoBillboard()
	)
	
	on polarAngle_spn changed val do
	(
		SaveAutoBillboard()
	)
)

---------------------------------------------------------------------
-- Trigger Manager
---------------------------------------------------------------------
rollout Trigger_rollout "Trigger Manager"
(
	imgTag uiGrayBar "" bitmap:GrayBar_bmp pos:[1,1] 
	groupBox sequences_grp "Sequences" pos:[9,25] width:186 height:220
	listBox sequences_lbx "" pos:[20,45] width:164 height:14
	
	groupBox triggers_grp "Trigger States" pos:[201,25] width:189 height:220
	button trigDelete_btn "-" pos:[212,45] width:23 height:16
	button trigEdit_btn "edit" pos:[237,45] width:34 height:16
	button trigNew_btn "+" pos:[273,45] width:23 height:16
	listBox triggers_lbx "" pos:[212,63] width:164 height:5
	
	groupBox trigSet_grp "Trigger Settings" pos:[212,144] width:164 height:90
	label trigFrame_lbl "Key Frame:" pos:[223,166] width:70 height:16
	spinner trigFrame_spn "" pos:[302,166] width:65 height:16 range:[0,1000,0] type:#integer
	label trigState_lbl "Trigger State:" pos:[223,194] width:70 height:16
	dropDownList trigState_ddl "" pos:[304,185] width:65 height:21 \
		items:#("1","-1","2","-2","3","-3","4","-4","5","-5","6","-6","7","-7","8","-8","9","-9","10","-10" \
			,"11","-11","12","-12","13","-13","14","-14","15","-15","16","-16","17","-17","18","-18","19","-19","20","-20" \
			,"21","-21","22","-22","23","-23","24","-24","25","-25","26","-26","27","-27","28","-28","29","-29","30","-30","31","-31","32","-32")
	button trigSave_btn "save" pos:[303,210] width:64 height:16 range:[0,1,1]
	
	function SelectSequences =
	(
		clearSelection()
		select $sequence_*
	)
	
	function UpdateSequences =
	(
		SelectSequences()
		seqArr = #()
		for o in selection do
		(
			append seqArr (substring o.name 10 o.name.count )
		)
		sequences_lbx.items = seqArr
		clearSelection()
	)
	
	function GetTriggers seq =
	(
		trigArr = #()
		seqObj = getNodeByName ("sequence_" + seq as string)
		if seqObj != null then
		(
			numTrig = getUserProp seqObj "numTriggers"
			if numTrig != null then
			(
				for i = 0 to (numTrig - 1) do
				(
					curTrigFrame = "triggerFrame" + i as string
					curTrigState = "triggerState" + i as string
					trigFrame = getUserProp seqObj curTrigFrame as string
					trigState = getUserProp seqObj curTrigState as string
					trigTmp = "Key Frame: " + trigFrame + " State: " + trigState 
					append trigArr trigTmp
				)
			)
		)
		else
		(
			UpdateSequences()
		)
		trigArr
	)
	
	function AddTrigger frame state =
	(
		if sequences_lbx.selected != null then
		(
			seqObj = getNodeByName ("sequence_" + sequences_lbx.selected)
			numTrig = getUserProp seqObj "numTriggers"
			if numTrig == null then numTrig = 0 
			curTrigFrame = "triggerFrame" + numTrig as string
			curTrigState = "triggerState" + numTrig as string
			setUserProp seqObj "numTriggers" (numTrig + 1)
			setUserProp seqObj curTrigFrame frame
			setUserProp seqObj curTrigState state
		)
	)
	
	function EditTrigger trigNum =
	(
		if sequences_lbx.selected != null then
		(
			trigNum -= 1
			seqObj = getNodeByName ("sequence_" + sequences_lbx.selected)
			curTrigFrame = "triggerFrame" + trigNum as string
			curTrigState = "triggerState" + trigNum as string
			trigFrame_spn.value = getUserProp seqObj curTrigFrame
			trigState_ddl.selection = findItem trigState_ddl.items (getUserProp seqObj curTrigState as string)
		)
	)
	
	function UpdateTriggers trigNum =
	(
		if sequences_lbx.selected != null then
		(
			trigNum -= 1
			seqObj = getNodeByName ("sequence_" + sequences_lbx.selected)
			curTrigFrame = "triggerFrame" + trigNum as string
			curTrigState = "triggerState" + trigNum as string
			setUserProp seqObj curTrigFrame trigFrame_spn.value
			setUserProp seqObj curTrigState trigState_ddl.selected
		)
	)
	
	function DeleteTrigger num =
	(
		if sequences_lbx.selected != null then
		(
			triggers = #()
			seqObj = getNodeByName ("sequence_" + sequences_lbx.selected)
			numTrig = getUserProp seqObj "numTriggers"
			for i = 0 to (numTrig -= 1) do
			(
				curTrigFrame = "triggerFrame" + i as string
				curTrigState = "triggerState" + i as string
				curFrame = getUserProp seqObj curTrigFrame
				curState = getUserProp seqObj curTrigState
				
				append triggers #(curFrame, curState)
				if i == numTrig then
				(
					RemoveUserProp seqObj curTrigFrame curFrame 
					RemoveUserProp seqObj curTrigState curState 
					setUserProp seqObj "numTriggers" numTrig
				)
			)
			deleteItem triggers num
			for i = 0 to (triggers.count - 1) do
			(
				curTriggerFrame = "triggerFrame" + i as string
				curTriggerState = "triggerState" + i as string
				setUserProp seqObj curTriggerFrame triggers[i + 1][1]
				setUserProp seqObj curTriggerState triggers[i + 1][2]
			)
		)
	)
	
	function ResetControls =
	(
		trigSave_btn.text = "save"
		trigFrame_spn.value = 0
		trigState_ddl.selection = 1
	)
	
	on Trigger_rollout open do
	(
		UpdateSequences()
		triggers_lbx.items = (GetTriggers sequences_lbx.selected)
	)
	
	on sequences_lbx selected seq do
	(
		triggers_lbx.items = (GetTriggers sequences_lbx.selected)
		ResetControls()
	)
	
	on sequences_lbx doubleclicked seq do
	(
		UpdateSequences()
	)
	
	on trigDelete_btn pressed do
	(
		if triggers_lbx.selection != 0 then
		(
			DeleteTrigger triggers_lbx.selection
			triggers_lbx.items = (GetTriggers sequences_lbx.selected)
			ResetControls()
		)
	)
	
	on trigEdit_btn pressed do
	(
		if triggers_lbx.selection != 0 then
		(
			EditTrigger triggers_lbx.selection
			trigSave_btn.text = "update"
		)
	)
	
	on trigNew_btn pressed do
	(
		ResetControls()
	)
	
	on trigSave_btn pressed do
	(
		if trigSave_btn.text == "save" then
		(
			AddTrigger trigFrame_spn.value trigState_ddl.selected
			triggers_lbx.items = (GetTriggers sequences_lbx.selected)
			ResetControls()
		)
		else
		(
			UpdateTriggers triggers_lbx.selection
			ResetControls()
		)
	)
)

---------------------------------------------------------------------
-- Visibility Manager
---------------------------------------------------------------------
rollout Visibility_rollout "Visibility Manager"
(
	imgTag uiGrayBar "" bitmap:GrayBar_bmp pos:[1,1] 
	groupBox details_grp "Detail Meshes" pos:[9,25] width:186 height:220
	listBox details_lbx "" pos:[20,45] width:164 height:14
	
	groupBox visibility_grp "Visibility States" pos:[201,25] width:189 height:220
	button visDelete_btn "-" pos:[212,45] width:23 height:16
	button visEdit_btn "edit" pos:[237,45] width:34 height:16
	button vis_btn "+" pos:[273,45] width:23 height:16
	listBox visibility_lbx "" pos:[212,63] width:164 height:5
	
	groupBox visSet_grp "Visibility Settings" pos:[212,144] width:164 height:90
	label visFrame_lbl "key Frame:" pos:[223,166] width:70 height:16
	spinner visFrame_spn "" pos:[302,166] width:65 height:16 range:[0,1000,0] type:#integer
	label visValue_lbl "Visibility State:" pos:[223,188] width:70 height:16
	spinner visValue_spn "" pos:[302,185] width:65 height:21 range:[0,1,1]
	button visSave_btn "save" pos:[303,210] width:64 height:16
	
	function SelectDetails =
	(
		clearSelection()
		select $geometry
		try ( deselect $bounds ) catch ()
		try ( deselect $col* ) catch ()
		try ( deselect $los* ) catch ()
	)
	
	function UpdateDetails =
	(
		SelectDetails()
		detailArr = #()
		for o in selection do
		(
			if classOf o != BoneGeometry then
			(
				append detailArr o.name
			)
		)
		details_lbx.items = detailArr
		clearSelection()
	)
	
	function GetVisibility detail =
	(
		visArr = #()
		visObj = getNodeByName (detail as string)
		if visObj != null then
		(
			numVis = getUserProp visObj "numVis"
			if numVis != null then
			(
				for i = 0 to (numVis - 1) do
				(
					curVisFrame = "visFrame" + i as string
					curVisValue = "visValue" + i as string
					visFrame = getUserProp visObj curVisFrame as string
					visValue = getUserProp visObj curVisValue as string
					visTmp = "Key Frame: " + visFrame + " Value: " + visValue 
					append visArr visTmp
				)
			)
		)
		else
		(
			UpdateDetails()
		)
		visArr
	)
	
	function AddVisibility frame value =
	(
		if details_lbx.selected != null then
		(
			visObj = getNodeByName details_lbx.selected
			numVis = getUserProp visObj "numVis"
			if numVis == null then numVis = 0 
			curVisFrame = "visFrame" + numVis as string
			curVisValue = "visValue" + numVis as string
			setUserProp visObj "numVis" (numVis + 1)
			setUserProp visObj curVisFrame frame
			setUserProp visObj curVisValue value
		)
	)
	
	function EditVisibility visNum =
	(
		if details_lbx.selected != null then
		(
			visNum -= 1
			visObj = getNodeByName details_lbx.selected
			curVisFrame = "visFrame" + visNum as string
			curVisValue = "visValue" + visNum as string
			visFrame_spn.value = getUserProp visObj curVisFrame
			visValue_spn.value = getUserProp visObj curVisValue
		)
	)
	
	function UpdateVisibility visNum =
	(
		if details_lbx.selected != null then
		(
			visNum -= 1
			visObj = getNodeByName details_lbx.selected
			curVisFrame = "visFrame" + visNum as string
			curVisValue = "visValue" + visNum as string
			setUserProp visObj curVisFrame visFrame_spn.value
			setUserProp visObj curVisValue visValue_spn.value
		)
	)
	
	function DeleteVisibility num =
	(
		if details_lbx.selected != null then
		(
			visibility = #()
			visObj = getNodeByName details_lbx.selected
			numVis = getUserProp visObj "numVis"
			for i = 0 to (numVis -= 1) do
			(
				curVisFrame = "visFrame" + i as string
				curVisValue = "visValue" + i as string
				curFrame = getUserProp visObj curVisFrame
				curValue = getUserProp visObj curVisValue
				
				append visibility #(curFrame, curValue)
				if i == numVis then
				(
					RemoveUserProp visObj curVisFrame curFrame 
					RemoveUserProp visObj curVisValue curValue 
					setUserProp visObj "numVis" numVis
				)
			)
			deleteItem visibility num
			for i = 0 to (visibility.count - 1) do
			(
				curVisFrame = "visFrame" + i as string
				curVisValue = "visValue" + i as string
				setUserProp visObj curVisFrame visibility[i + 1][1]
				setUserProp visObj curVisValue visibility[i + 1][2]
			)
		)
	)
	
	function ResetControls =
	(
		visSave_btn.text = "save"
		visFrame_spn.value = 0
		visValue_spn.value = 1
	)
	
	function MakeVisibility =
	(
		visibility = #()
		visObj = getNodeByName details_lbx.selected
		numVis = getUserProp visObj "numVis"
		if getVisController visObj == null then
		(
			visObj.visibility = bezier_float()
		)
		for i = 0 to (numVis -= 1) do
		(
			curVisFrame = "visFrame" + i as string
			curVisValue = "visValue" + i as string
			curFrame = getUserProp visObj curVisFrame
			curValue = getUserProp visObj curVisValue
			
			append visibility #(curFrame, curValue)
		)
		deleteKeys visObj.visibility.controller
		visObj.visibility.controller.value = 1
		for i = 0 to (visibility.count - 1) do
		(
			animate on at time (visibility[i + 1][1]) visObj.visibility.controller.value = (visibility[i + 1][2])
		)
	)
	
	on Visibility_rollout open do
	(
		UpdateDetails()
		visibility_lbx.items = (GetVisibility details_lbx.selected)
	)
	
	on details_lbx selected seq do
	(
		visibility_lbx.items = (GetVisibility details_lbx.selected)
		ResetControls()
	)
	
	on details_lbx doubleclicked seq do
	(
		UpdateDetails()
	)
	
	on visDelete_btn pressed do
	(
		if visibility_lbx.selection != 0 then
		(
			DeleteVisibility visibility_lbx.selection
			visibility_lbx.items = (GetVisibility details_lbx.selected)
			MakeVisibility()
			ResetControls()
		)
	)
	
	on visEdit_btn pressed do
	(
		if visibility_lbx.selection != 0 then
		(
			EditVisibility visibility_lbx.selection
			visSave_btn.text = "update"
		)
	)
	
	on visNew_btn pressed do
	(
		ResetControls()
	)
	
	on visSave_btn pressed do
	(
		if details_lbx.selection != 0 then
		(
			if visSave_btn.text == "save" then
			(
				AddVisibility visFrame_spn.value visValue_spn.value
				visibility_lbx.items = (GetVisibility details_lbx.selected)
				MakeVisibility()
				ResetControls()
			)
			else
			(
				UpdateVisibility visibility_lbx.selection
				MakeVisibility()
				ResetControls()
			)
		)
	)
)

---------------------------------------------------------------------
-- Collision Manager
---------------------------------------------------------------------
rollout Collision_rollout "Collision Manager"
(
	imgTag uiGrayBar "" bitmap:GrayBar_bmp pos:[0,0] 
	groupBox details_grp "Detail Meshes:" pos:[7,21] width:132 height:259
	MultiListBox details_lbx "" pos:[17,38] width:111 height:14
	radioButtons colType_rdo "" pos:[17,230] width:100 height:32 \
			labels:#("use copy of mesh", "use actual mesh", "use volume") columns:1 default:3
	
	groupBox col_grp "Collision:" pos:[148,21] width:89 height:259
	button col1_btn "Col-1" pos:[157,40] width:70 height:20
	button col2_btn "Col-2" pos:[157,69] width:70 height:20
	button col3_btn "Col-3" pos:[157,99] width:70 height:20
	button col4_btn "Col-4" pos:[157,129] width:70 height:20
	button col5_btn "Col-5" pos:[157,160] width:70 height:20
	button col6_btn "Col-6" pos:[157,191] width:70 height:20
	button col7_btn "Col-7" pos:[157,222] width:70 height:20
	button col8_btn "Col-8" pos:[157,252] width:70 height:20
	
	button copy1_btn "copy >" pos:[245,40] width:50 height:20
	button copy2_btn "copy >" pos:[245,69] width:50 height:20
	button copy3_btn "copy >" pos:[245,99] width:50 height:20
	button copy4_btn "copy >" pos:[245,129] width:50 height:20
	button copy5_btn "copy >" pos:[245,160] width:50 height:20
	button copy6_btn "copy >" pos:[245,191] width:50 height:20
	button copy7_btn "copy >" pos:[245,222] width:50 height:20
	
	groupBox los_grp "LOS Collision:" pos:[304,21] width:88 height:259
	button los9_btn "LOScol-9" pos:[314,40] width:70 height:20
	button los10_btn "LOScol-10" pos:[313,69] width:70 height:20
	button los11_btn "LOScol-11" pos:[313,99] width:70 height:20
	button los12_btn "LOScol-12" pos:[313,129] width:70 height:20
	button los13_btn "LOScol-13" pos:[313,160] width:70 height:20
	button los14_btn "LOScol-14" pos:[313,191] width:70 height:20
	button los15_btn "LOScol-15" pos:[313,222] width:70 height:20
	
	--groupBox colUtil_grp "Collision Utilities:" pos:[7,283] width:385 height:61
	
	function SelectDetails =
	(
		clearSelection()
		select $geometry
		try ( deselect $bounds ) catch ()
		--try ( deselect $col* ) catch ()
		--try ( deselect $los* ) catch ()
	)
	
	function UpdateDetails =
	(
		SelectDetails()
		detailArr = #()
		for o in selection do
		(
			if classOf o != BoneGeometry then
			(
				append detailArr o.name
			)
		)
		details_lbx.items = detailArr
		clearSelection()
	)
	
	function CreateCollisionMarker colType colNum = 
	(
		colName = "Collision-" + colNum as string
		if colType == "los" then colName = "LOS-" + colNum as string
		findCol = getNodeByName colName
		if findCol == undefined then
		(
			c = dummy name: colName
			c.boxsize = [ShapeX, ShapeY, ShapeZ]
			c.parent = $base01
			if FreezeMarkers == true then c.isFrozen = true
		)
	)
	
	function CreateColision colType colNum =
	(
		selectionTmp = details_lbx.selection as array
		if selectionTmp.count != 0 then
		(
			colName = ("Col-" + colNum as string)
			if colType == "los" then colName = ("LOScol-" + colNum as string)
			findCol = getNodeByName colName
			if findCol == undefined then
			(
				if selectionTmp.count == 1 and colType_rdo.state == 1 then
				(
					objTmp = getNodeByName details_lbx.items[selectionTmp[1]]
					newObj = copy objTmp
					newObj.name = colName
					CreateDtsShape()
					newObj.parent = $start01
					CreateCollisionMarker colType colNum
				)
				if selectionTmp.count == 1 and colType_rdo.state == 2 then
				(
					objTmp = getNodeByName details_lbx.items[selectionTmp[1]]
					objTmp.name = colName
					CreateDtsShape()
					objTmp.parent = $start01
					CreateCollisionMarker colType colNum
				)
				if selectionTmp.count > 1 or colType_rdo.state == 3 then
				(
					tmpArr = details_lbx.items
					for obj in tmpArr do
					(
						selectMore (getNodeByName obj)
					)
					if selection.count != 0 then
					(
						w = (selection.max.x - selection.min.x)
						l = (selection.max.y - selection.min.y)
						h = selection.max.z - selection.min.z
						c = selection.center - [0, 0, (h/2)]
			
						-- create the collision box
						col = Box length:l width:w height:h pos:c
						col.name = colName
						converttomesh col
						col.boxmode = on
						CreateDtsShape()
						col.parent = $start01
						CreateCollisionMarker colType colNum
					)
				)
			)
		)
		UpdateDetails()
	)
	
	function CopyCollision losNum colNum =
	(
		losName = "LOScol-" + losNum as string
		colName = "Col-" + colNum as string
		findLos = getNodeByName losName
		findCol = getNodeByName colName
		if findLos == undefined and findCol != undefined then
		(
			objTmp = getNodeByName colName
			newObj = copy objTmp
			newObj.name = losName
			CreateDtsShape()
			newObj.parent = $start01
			if objTmp.boxmode == on then newObj.boxmode = on
			CreateCollisionMarker "los" losNum
		)
		UpdateDetails()
	)
	
	on Collision_rollout open do
	(
		UpdateDetails()
	)
	
	on col1_btn pressed do
	(
		CreateColision "col" 1
	)
	
	on col2_btn pressed do
	(
		CreateColision "col" 2
	)
	
	on col3_btn pressed do
	(
		CreateColision "col" 3
	)
	
	on col4_btn pressed do
	(
		CreateColision "col" 4
	)
	
	on col5_btn pressed do
	(
		CreateColision "col" 5
	)
	
	on col6_btn pressed do
	(
		CreateColision "col" 6
	)
	
	on col7_btn pressed do
	(
		CreateColision "col" 7
	)
	
	on col8_btn pressed do
	(
		CreateColision "col" 8
	)
	
	on los9_btn pressed do
	(
		CreateColision "los" 9
	)
	
	on los10_btn pressed do
	(
		CreateColision "los" 10
	)
	
	on los11_btn pressed do
	(
		CreateColision "los" 11
	)
	
	on los12_btn pressed do
	(
		CreateColision "los" 12
	)
	
	on los13_btn pressed do
	(
		CreateColision "los" 13
	)
	
	on los14_btn pressed do
	(
		CreateColision "los" 14
	)
	
	on los15_btn pressed do
	(
		CreateColision "los" 15
	)
	
	on copy1_btn pressed do
	(
		CopyCollision 9 1
	)
	
	on copy2_btn pressed do
	(
		CopyCollision 10 2
	)
	
	on copy3_btn pressed do
	(
		CopyCollision 11 3
	)
	
	on copy4_btn pressed do
	(
		CopyCollision 12 4
	)
	
	on copy5_btn pressed do
	(
		CopyCollision 13 5
	)
	
	on copy6_btn pressed do
	(
		CopyCollision 14 6
	)
	
	on copy7_btn pressed do
	(
		CopyCollision 15 7
	)
)

---------------------------------------------------------------------
-- Material Manager
---------------------------------------------------------------------
rollout RenameMaterial_rollout "Rename Material"
(
	imgTag uiGrayBar "" bitmap:GrayBar_bmp pos:[0,0] 
	label rename_lbl "Material Name:" pos:[13,28] width:77 height:15
	editText materialName_edt "" pos:[91,26] width:122 height:20
	button renameCancel_btn "cancel" pos:[95,50] width:58 height:20
	button renameApply_btn "apply" pos:[155,50] width:58 height:20
	
	on RenameMaterial_rollout open do
	(
		materialName_edt.text = SelectedMaterial
		setFocus materialName_edt
	)
	
	on renameCancel_btn pressed do
	(
		try(destroyDialog RenameMaterial_rollout) catch()
	)
	
	on renameApply_btn pressed do
	(
		if SelectedMaterialType == 1 then
		(
			materials = meditMaterials
		)
		else
		(
			materials = sceneMaterials
		)
		materials[SelectedMaterialID].name = materialName_edt.text
		try(Material_rollout.UpdateMaterials()) catch() 
		try(destroyDialog RenameMaterial_rollout) catch()
	)
)

rollout Material_rollout "Material Manager"
(
	imgTag uiGrayBar "" bitmap:GrayBar_bmp pos:[1,1] 
	groupBox materials_grp "Materials" pos:[7,25] width:157 height:330
	button matDelete_btn "-" pos:[16,42] width:23 height:16
	button matRename_btn "rename" pos:[40,42] width:45 height:16
	button matApply_btn "apply" pos:[86,42] width:44 height:16
	button matNew_btn "+" pos:[131,42] width:23 height:16
	listBox materials_lbx "" pos:[17,60] width:137 height:15
	radioButtons materialType_rdo "" pos:[18,269] width:150 height:32 labels:#("Material Editor", "Scene Materials") default:1 columns:1
	label showTexture_lbl "Show Material in Viewport" pos:[18,309] width:130 height:18 checked:false
	button showTextureOff_btn "no" pos:[18,329] width:50 height:17
	button showTextureOn_btn "yes" pos:[90,329] width:50 height:17
	
	groupBox materialProp_grp "Material Properties" pos:[172,25] width:216 height:330
	
	groupBox texture_grp "Texture:" pos:[185,44] width:193 height:114
	checkbox enableTexture_chk "enable texture" pos:[195,60] width:100 height:18 checked:false
	button chooseTexture_btn "choose a texture" pos:[195,82] width:105 height:25
	bitmap texture_bmp pos:[310,59] width:60 height:50 bitmap:(bitmap 60 50 color:gray)
	checkbox uwrap_chk "UWrap" pos:[195,115] width:80 height:18
	checkbox vwrap_chk "VWrap" pos:[285,115] width:80 height:18
	checkbox twoSided_chk "Two Sided" pos:[195,135] width:80 height:18 checked:false
	checkbox selfIllum_chk "Self Illuminate" pos:[285,135] width:87 height:18 checked:false
	
	groupBox trans_grp "Transparency:" pos:[184,164] width:193 height:108
	checkbox enableTrans_chk "enable transparency" pos:[195,184] width:167 height:18 checked:false
	button chooseTrans_btn "choose a texture" pos:[195,204] width:100 height:25
	radioButtons transType_rdo "" pos:[212,235] width:150 height:32 labels:#("normal", "subtractive", "additive") default:3 columns:2
	
	groupBox reflection_grp "Reflection:" pos:[184,277] width:193 height:68
	checkbox enableReflection_chk "enable reflection" pos:[196,298] width:167 height:18 checked:false
	spinner reflection_spn "reflection amount:" pos:[278,318] width:75 height:16 range:[0,100,100] type:#float
	
	function UpdateMaterials =
	(
		matArr = #()
		
		if materialType_rdo.state == 1 then
		(
			for mat in meditMaterials do
			(
				append matArr mat.name
			)
		)
		else
		(
			for mat in sceneMaterials do
			(
				append matArr mat.name
			)
		)
		materials_lbx.items = matArr
	)
	
	function ResetMaterial =
	(
		blankImg = bitmap 60 50 color:gray
		enableTexture_chk.checked = false
		chooseTexture_btn.text = "choose a texture"
		texture_bmp.bitmap = blankImg
		uwrap_chk.checked = false
		vwrap_chk.checked = false
		twoSided_chk.checked = false
		selfIllum_chk.checked = false
		enableTrans_chk.checked = false
		chooseTrans_btn.text = "choose a texture"
		transType_rdo.state = 1
		enableReflection_chk.checked = false
		reflection_spn.value = 100
		enableTexture_chk.enabled = true
		chooseTexture_btn.enabled = true
		texture_bmp.enabled = true
		uwrap_chk.enabled = true
		vwrap_chk.enabled = true
		twoSided_chk.enabled = true
		selfIllum_chk.enabled = true
		enableTrans_chk.enabled = true
		chooseTrans_btn.enabled = true
		transType_rdo.enabled = true
		enableReflection_chk.enabled = true
		reflection_spn.enabled = true
	)
	
	function DisableMaterial =
	(
		enableTexture_chk.enabled = false
		chooseTexture_btn.enabled = false
		texture_bmp.enabled = false
		uwrap_chk.enabled = false
		vwrap_chk.enabled = false
		twoSided_chk.enabled = false
		selfIllum_chk.enabled = false
		enableTrans_chk.enabled = false
		chooseTrans_btn.enabled = false
		transType_rdo.enabled = false
		enableReflection_chk.enabled = false
		reflection_spn.enabled = false
	)
	
	function GetMaterial num =
	(
		if num != null then
		(	
			ResetMaterial()
			if materialType_rdo.state == 1 then
			(
				materials = meditMaterials
			)
			else
			(
				materials = sceneMaterials
			)
			if classof materials[num] == Standardmaterial then
			(
				enableTexture_chk.checked = materials[num].diffuseMapEnable
				if classof materials[num].diffusemap == BitmapTexture then
				(
					chooseTexture_btn.text = (filenameFromPath materials[num].diffusemap.filename)
					texture_bmp.filename = materials[num].diffusemap.filename
					uwrap_chk.checked = materials[num].diffusemap.coordinates.U_Tile
					Vwrap_chk.checked = materials[num].diffusemap.coordinates.V_Tile
				)
				
				twoSided_chk.checked = materials[num].twoSided
				--selfIllum_chk.checked = materials[num].selfIllumMapEnable
				if materials[num].selfIllumAmount == 100 then
				(
					selfIllum_chk.checked = true
				)
				else
				(
					selfIllum_chk.checked = false
				)
				enableTrans_chk.checked = materials[num].opacityMapEnable
				if classof materials[num].opacitymap == BitmapTexture then
				(
					chooseTrans_btn.text = (filenameFromPath materials[num].opacitymap.filename)
				)
				
				transType_rdo.state = (materials[num].opacityType + 1)
				enableReflection_chk.checked = materials[num].reflectionMapEnable
				reflection_spn.value = materials[num].reflectionMapAmount
			)
			else
			(
				ResetMaterial()
				DisableMaterial()
			)
		)

	)
	
	function SetMaterial num =
	(
		if materialType_rdo.state == 1 then
		(
			materials = meditMaterials
		)
		else
		(
			materials = sceneMaterials
		)
		materials[num].diffuseMapEnable = enableTexture_chk.checked
		if enableTexture_chk.checked == false then
		(
			materials[num].diffusemap = undefined
		)
		if classof materials[num].diffusemap == BitmapTexture then
		(
			materials[num].diffusemap.coordinates.U_Tile = uwrap_chk.checked
			materials[num].diffusemap.coordinates.V_Tile = Vwrap_chk.checked
		)
		else
		(
			uwrap_chk.checked = false
			vwrap_chk.checked = false
		)
		materials[num].twoSided = twoSided_chk.checked
		--materials[num].selfIllumMapEnable = selfIllum_chk.checked
		materials[num].opacityMapEnable = enableTrans_chk.checked
		if enableTrans_chk.checked == true then
		(
			if materials[num].opacitymap == undefined then
			(
				materials[num].opacitymap = materials[num].diffusemap
				try( materials[num].opacityMap.alphaSource = 0 ) catch()
				try( materials[num].opacityMap.monoOutput = 1 ) catch()
			)
		)
		else
		(
			materials[num].opacitymap = undefined
		)
		materials[num].opacityType = (transType_rdo.state - 1)
		materials[num].reflectionMapEnable = enableReflection_chk.checked
		materials[num].reflectionMapAmount = reflection_spn.value
		
		GetMaterial num
	)
	
	on Material_rollout open do
	(
		UpdateMaterials()
		GetMaterial materials_lbx.selection
	)
	
	on materials_lbx selected state do
	(
		GetMaterial materials_lbx.selection
	)
	
	on materials_lbx doubleclicked state do
	(
		GetMaterial materials_lbx.selection
	)
	
	on materialType_rdo changed state do
	(
		UpdateMaterials()
		ResetMaterial()
	)
	
	on matRename_btn pressed do
	(
		if materials_lbx.selection != 0 then
		(
			SelectedMaterial = materials_lbx.selected
			SelectedMaterialID = materials_lbx.selection
			SelectedMaterialType = materialType_rdo.state
			createDialog RenameMaterial_rollout width:225 height:83
		)
	)
	
	on enableTexture_chk changed state do
	(
		if materials_lbx.selection != 0 then
		(
			SetMaterial materials_lbx.selection
		)
		else
		(
			enableTexture_chk.checked = false
		)
	)
	
	on uwrap_chk changed state do
	(
		if materials_lbx.selection != 0 then
		(
			SetMaterial materials_lbx.selection
		)
	)
	
	on vwrap_chk changed state do
	(
		if materials_lbx.selection != 0 then
		(
			SetMaterial materials_lbx.selection
		)
	)
	
	on twoSided_chk changed state do
	(
		if materials_lbx.selection != 0 then
		(
			SetMaterial materials_lbx.selection
		)
		else
		(
			twoSided_chk.checked = false
		)
	)
	
	on selfIllum_chk changed state do
	(
		if materials_lbx.selection != 0 then
		(
			--SetMaterial materials_lbx.selection
			num = materials_lbx.selection
			if materialType_rdo.state == 1 then
			(
				materials = meditMaterials
			)
			else
			(
				materials = sceneMaterials
			)
			if selfIllum_chk.checked == true then
			(
				materials[num].selfIllumAmount = 100
			)
			else
			(
				materials[num].selfIllumAmount = 0
			)
		)
		else
		(
			selfIllum_chk.checked = false
		)
	)
	
	on enableTrans_chk changed state do
	(
		if materials_lbx.selection != 0 then
		(
			SetMaterial materials_lbx.selection
		)
		else
		(
			enableTrans_chk.checked = false
		)
	)
	
	on transType_rdo changed state do
	(
		if materials_lbx.selection != 0 then
		(
			SetMaterial materials_lbx.selection
		)
		else
		(
			transType_rdo.state = 1
		)
	)
	
	on enableReflection_chk changed state do
	(
		if materials_lbx.selection != 0 then
		(
			SetMaterial materials_lbx.selection
		)
		else
		(
			enableReflection_chk.checked = false
		)
	)
	
	on reflection_spn changed state do
	(
		if materials_lbx.selection != 0 then
		(
			SetMaterial materials_lbx.selection
		)
		else
		(
			reflection_spn.value = 100
		)
	)
	
	on chooseTexture_btn pressed do
	(
		if materials_lbx.selection != 0 then
		(
			num = materials_lbx.selection
			if materialType_rdo.state == 1 then
			(
				materials = meditMaterials
			)
			else
			(
				materials = sceneMaterials
			)
			
			baseTexture = getOpenFileName \
			caption:"Select a Texture" \
			filename:"*.jpg" \
			types:"JPEG(*.jpg)|*.jpg|PNG File(*.png)|*.png|TGA File(*.tga)|*.tga|IFL File|*.ifl|All|*.*|"
				
			if baseTexture != undefined then
			(
				materials[num].diffuseMap = Bitmaptexture fileName:baseTexture
				materials[num].diffuseMap.name = (getFilenameFile baseTexture)
				materials[num].name = (getFilenameFile baseTexture)
				showTextureMap materials[num] on
				GetMaterial num
				UpdateMaterials()
			)
		)
	)
	
	on chooseTrans_btn pressed do
	(
		if materials_lbx.selection != 0 then
		(
			num = materials_lbx.selection
			if materialType_rdo.state == 1 then
			(
				materials = meditMaterials
			)
			else
			(
				materials = sceneMaterials
			)
			
			baseTexture = getOpenFileName \
			caption:"Select a Transparency Mask" \
			filename:"*.jpg" \
			types:"JPEG(*.jpg)|*.jpg|PNG File(*.png)|*.png|TGA File(*.tga)|*.tga|IFL File|*.ifl|All|*.*|"
				
			if baseTexture != undefined then
			(
				materials[num].opacitymap = Bitmaptexture fileName:baseTexture
				materials[num].opacitymap.name = (getFilenameFile baseTexture)
				materials[num].opacityMap.alphaSource = 0
				materials[num].opacityMap.monoOutput = 1
				GetMaterial num
			)
		)
	)
	
	on matApply_btn pressed do
	(
		if selection.count == 0 then messageBox "Apply Material requires a selection"
		else
		(
			if materials_lbx.selection != 0 then
			(
				num = materials_lbx.selection
				if materialType_rdo.state == 1 then
				(
					materials = meditMaterials
				)
				else
				(
					materials = sceneMaterials
				)
				$.material = materials[num]
			)
		)
	)
	
	on showTextureOn_btn pressed do
	(
		if materials_lbx.selection != 0 then
		(
			num = materials_lbx.selection
			if materialType_rdo.state == 1 then
			(
				materials = meditMaterials
			)
			else
			(
				materials = sceneMaterials
			)
			showTextureMap materials[num] on
		)
	)
	
	on showTextureOff_btn pressed do
	(
		if materials_lbx.selection != 0 then
		(
			num = materials_lbx.selection
			if materialType_rdo.state == 1 then
			(
				materials = meditMaterials
			)
			else
			(
				materials = sceneMaterials
			)
			showTextureMap materials[num] off
		)
	)
)

---------------------------------------------------------------------
-- DMP viewer
---------------------------------------------------------------------
rollout DMP_rollout "View DMP File"
(	
	imgTag uiBar "" bitmap:GrayBar_bmp pos:[0,0] width:640
	activeXControl ie_ax "Shell.Explorer.2" width:620 height:425 align:#center pos:[10,27]
	checkbox showDmp_chk "Always show DMP file after export" pos:[10,460]
	
	function OpenDmpFile dmpLocation =
	(
		ie_ax.Navigate ( dmpLocation + "dump.html" )
	)
	
	on DMP_rollout open do
	(
		showDmp_chk.checked = ShowDMP
		OpenDmpFile( DmpPath )
	)
	
	on showDmp_chk changed state do
	(
		ShowDMP = state
		setINISetting "$scripts\dts_prefs.ini" "Settings" "ShowDMP" (state as string)
	)
)

---------------------------------------------------------------------
-- User Guide
---------------------------------------------------------------------
rollout Help_rollout "User Guide"
(
	imgTag uiBar "" bitmap:GrayBar_bmp pos:[0,0] width:640
	activeXControl ie_ax "Shell.Explorer.2" width:620 height:425 align:#center pos:[10,27]
	
	on Help_rollout open do
	(
		ie_ax.Navigate ( HelpPath )
	)
)

---------------------------------------------------------------------
-- About...
---------------------------------------------------------------------
rollout About_rollout "About this tool..."
(
	imgTag uiAbout "" bitmap:(openBitMap "$scripts\dts\dts_abouttop.bmp") \
		pos:[0,0] transparent:(color 192 192 192)
	imgTag uiDIlogo "" bitmap:(openBitMap "$scripts\dts\dts_dilogo.bmp") \
		pos:[3,38] transparent:(color 192 192 192)

	label version_lbl "Torque (DTS) Exporter Version 1.3" pos:[143, 55]
	label copyright_lbl "Copyright 2005 Dark Industries" pos:[143, 70]
	
	label moreinfo_lbl "For more info visit:" pos:[143, 95]
	hyperLink diWeb_hl "http://www.darkindustries.com" address:"http://www.darkindustries.com" \
		pos:[143, 107] color:(color 0 0 255) hoverColor:(color 215 28 57)
	hyperLink ggWeb_hl "http://www.garagegames.com" address:"http://www.garagegames.com" \
		pos:[143, 120] color:(color 0 0 255) hoverColor:(color 215 28 57)
)

---------------------------------------------------------------------
-- Create the exporter UI
---------------------------------------------------------------------

-- Load Main Exporter UI
createdialog gDTSUtility_rollout lockWidth:true \
	style:#(#style_sysmenu,#style_titlebar,#style_minimizebox,#style_resizing)

-- Load SubRollout UIs
if QuickExportOpen == true then AddSubRollout gDTSUtility_rollout.DTSUtilityDialog QuickExport_rollout rolledUp:false
if BoundsOpen == true then AddSubRollout gDTSUtility_rollout.DTSUtilityDialog Bounds_rollout rolledUp:false
if DetailOpen == true then AddSubRollout gDTSUtility_rollout.DTSUtilityDialog Detail_rollout rolledUp:true
if BillboardOpen == true then AddSubRollout gDTSUtility_rollout.DTSUtilityDialog Billboard_rollout rolledUp:true
if NodeOpen == true then AddSubRollout gDTSUtility_rollout.DTSUtilityDialog Node_rollout rolledUp:true
if SequenceOpen == true then AddSubRollout gDTSUtility_rollout.DTSUtilityDialog Sequence_rollout rolledUp:true
if TriggerOpen == true then AddSubRollout gDTSUtility_rollout.DTSUtilityDialog Trigger_rollout rolledUp:true
if VisibilityOpen == true then AddSubRollout gDTSUtility_rollout.DTSUtilityDialog Visibility_rollout rolledUp:true
if CollisionOpen == true then AddSubRollout gDTSUtility_rollout.DTSUtilityDialog Collision_rollout rolledUp:true
if MaterialOpen == true then AddSubRollout gDTSUtility_rollout.DTSUtilityDialog Material_rollout rolledUp:true

registerViewWindow gDTSUtility_rollout

---------------------------------------------------------------------
-- End exporter UI
---------------------------------------------------------------------
