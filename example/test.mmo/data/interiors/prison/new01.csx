<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
<!--Torque Constructor Scene document http://www.garagegames.com-->
<ConstructorScene version="4" creator="Torque Constructor" date="2007/11/02 18:36:11">
    <Sunlight azimuth="180" elevation="35" color="255 255 255" ambient="64 64 64" />
    <LightingOptions lightingSystem="" ineditor_defaultLightmapSize="256" ineditor_maxLightmapSize="256" ineditor_lightingPerformanceHint="0" ineditor_shadowPerformanceHint="1" ineditor_TAPCompatibility="0" ineditor_useSunlight="0" export_defaultLightmapSize="256" export_maxLightmapSize="256" export_lightingPerformanceHint="0" export_shadowPerformanceHint="1" export_TAPCompatibility="0" export_useSunlight="0" />
    <GameTypes>
        <GameType name="Constructor" />
        <GameType name="Torque" />
    </GameTypes>
    <SceneGroups nextGroupID="1">
        <SceneGroup id="0" />
    </SceneGroups>
    <SceneShapes nextShapeID="5">
        <SceneShape id="0" type="0" file="../../../Program Files/Torque/Constructor/referenceshapes/orc/player.dts" pos="0 0 0.5" rot="1 0 0 0" scale="1 1 1" transform="1 0 0 0 0 1 0 0 0 0 1 0.5 0 0 0 1" group="-1" locked="0" />
        <SceneShape id="1" type="1" entityID="2" dl="0" group="-1" locked="0" />
        <SceneShape id="2" type="1" entityID="3" dl="0" group="-1" locked="0" />
        <SceneShape id="3" type="1" entityID="4" dl="0" group="-1" locked="0" />
        <SceneShape id="4" type="1" entityID="5" dl="0" group="-1" locked="0" />
    </SceneShapes>
    <DetailLevels current="0">
        <DetailLevel minPixelSize="0" actionCenter="0 0 0">
            <InteriorMap brushScale="32" lightScale="8" ambientColor="0 0 0" ambientColorEmerg="0 0 0">
                <Entities nextEntityID="6">
                    <Entity id="0" classname="worldspawn" gametype="Torque" isPointEntity="0">
                        <Properties detail_number="0" min_pixels="250" geometry_scale="32.0" light_geometry_scale="8.0" light_smoothing_scale="4.0" light_mesh_scale="1.0" ambient_color="0 0 0" emergency_ambient_color="0 0 0" mapversion="220" />
                    </Entity>
                    <Entity id="1" classname="portal" gametype="Torque" isPointEntity="0">
                        <Properties ambient_light="0" />
                    </Entity>
                    <Entity id="2" classname="light_point" gametype="Constructor" isPointEntity="1" origin="0 0 5.5" direction="0 -0 -1.74533">
                        <Properties color="255 255 255" intensity="100.0" falloff_inner="1.0" falloff_outer="10.0" />
                    </Entity>
                    <Entity id="3" classname="light_point" gametype="Constructor" isPointEntity="1" origin="0 0 5.5" direction="0 -0 -1.74533">
                        <Properties color="255 255 255" intensity="100.0" falloff_inner="1.0" falloff_outer="10.0" />
                    </Entity>
                    <Entity id="4" classname="light_point" gametype="Constructor" isPointEntity="1" origin="0 0 5.5" direction="0 -0 -1.74533">
                        <Properties color="255 255 255" intensity="100.0" falloff_inner="1.0" falloff_outer="10.0" />
                    </Entity>
                    <Entity id="5" classname="light_point" gametype="Constructor" isPointEntity="1" origin="0 0 5.5" direction="0 -0 -1.74533">
                        <Properties color="255 255 255" intensity="100.0" falloff_inner="1.0" falloff_outer="10.0" />
                    </Entity>
                </Entities>
                <Brushes nextBrushID="82">
                    <Brush id="16" owner="0" type="0" pos="0 0 0.25" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.25 0 0 0 1" group="-1" locked="0" nextFaceID="2429" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-6 -6 -0.25" />
                            <Vertex pos="6 -6 -0.25" />
                            <Vertex pos="6 6 -0.25" />
                            <Vertex pos="-6 6 -0.25" />
                            <Vertex pos="-5.5 -5.5 0.25" />
                            <Vertex pos="5.5 -5.5 0.25" />
                            <Vertex pos="5.5 5.5 0.25" />
                            <Vertex pos="-5.5 5.5 0.25" />
                        </Vertices>
                        <Face id="2422" plane="0 0 -1 -0.25" album="Auxiliary" material="NULL" texgens="-1 0 0 192 0 -1 0 -192 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="2423" plane="0 -0.707105 0.707108 -4.06586" album="Auxiliary" material="NULL" texgens="-0.707105 0 0 192 0 0.5 0.499998 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 5 1 0" />
                        </Face>
                        <Face id="2424" plane="0.707105 0 0.707108 -4.06586" album="Auxiliary" material="NULL" texgens="0 -0.707105 0 192 -0.5 0 0.499998 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 6 2 1" />
                        </Face>
                        <Face id="2425" plane="0 -0 1 -0.25" album="Starter" material="S22" texgens="1 0 0 256 0 -1 0 256 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 6 5 4 7" />
                        </Face>
                        <Face id="2426" plane="-0.707105 0 0.707108 -4.06585" album="Auxiliary" material="NULL" texgens="0 -0.707105 0 192 0.5 0 0.499998 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 3 7 4" />
                        </Face>
                        <Face id="2427" plane="-0 0.707105 0.707108 -4.06585" album="Auxiliary" material="NULL" texgens="-0.707105 0 0 192 0 -0.5 0.499998 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 2 6" />
                        </Face>
                    </Brush>
                    <Brush id="18" owner="0" type="0" pos="1.19209e-007 0 9.75" rot="0 -1 0 3.14159" scale="" transform="-1 0 -8.74228e-008 1.19209e-007 0 1 0 0 8.74228e-008 0 -1 9.75 0 0 0 1" group="-1" locked="0" nextFaceID="2429" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-6 -6 -0.25" />
                            <Vertex pos="6 -6 -0.25" />
                            <Vertex pos="6 6 -0.25" />
                            <Vertex pos="-6 6 -0.25" />
                            <Vertex pos="-5.5 -5.5 0.25" />
                            <Vertex pos="5.5 -5.5 0.25" />
                            <Vertex pos="5.5 5.5 0.25" />
                            <Vertex pos="-5.5 5.5 0.25" />
                        </Vertices>
                        <Face id="2422" plane="0 0 -1 -0.25" album="Starter" material="CEMENT10" texgens="-1 0 0 192 0 -1 0 -192 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="2423" plane="0 -0.707105 0.707108 -4.06586" album="Auxiliary" material="NULL" texgens="-0.707105 0 0 192 0 0.5 0.499998 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 5 1 0" />
                        </Face>
                        <Face id="2424" plane="0.707105 0 0.707108 -4.06586" album="Auxiliary" material="NULL" texgens="0 -0.707105 0 192 -0.5 0 0.499998 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 6 2 1" />
                        </Face>
                        <Face id="2425" plane="0 -0 1 -0.25" album="Starter" material="CEMENT10" texgens="1 0 0 192 0 -1 0 -192 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 6 5 4 7" />
                        </Face>
                        <Face id="2426" plane="-0.707105 0 0.707108 -4.06585" album="Auxiliary" material="NULL" texgens="0 -0.707105 0 192 0.5 0 0.499998 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 3 7 4" />
                        </Face>
                        <Face id="2427" plane="-0 0.707105 0.707108 -4.06585" album="Auxiliary" material="NULL" texgens="-0.707105 0 0 192 0 -0.5 0.499998 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 2 6" />
                        </Face>
                    </Brush>
                    <Brush id="34" owner="0" type="0" pos="5.75 0 5" rot="0 0 -1 3.14159" scale="" transform="-1 8.74228e-008 0 5.75 -8.74228e-008 -1 0 0 0 0 1 5 0 0 0 1" group="-1" locked="0" nextFaceID="985" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-0.25 -6 -5" />
                            <Vertex pos="-0.25 6 -5" />
                            <Vertex pos="-0.25 6 5" />
                            <Vertex pos="-0.25 -6 5" />
                            <Vertex pos="0.25 5.5 -4.5" />
                            <Vertex pos="0.25 5.5 4.50001" />
                            <Vertex pos="0.25 -5.5 4.50001" />
                            <Vertex pos="0.25 -5.5 -4.5" />
                        </Vertices>
                        <Face id="978" plane="-1 0 0 -0.25" album="Starter" material="WALL5" texgens="0 -1 0 192 0 0 -1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="979" plane="0.707107 0.707107 0 -4.06586" album="Auxiliary" material="NULL" texgens="0.707107 -0.707107 0 192 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 5 2 1" />
                        </Face>
                        <Face id="980" plane="0.707102 -0 0.707112 -3.35878" album="Auxiliary" material="NULL" texgens="0 -0.707102 0 192 -0.5 0 0.499993 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 6 3 2" />
                        </Face>
                        <Face id="981" plane="1 0 0 -0.25" album="Starter" material="WOOD2" texgens="0 1 0 256 0 0 -1 256 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 6 5 4 7" />
                        </Face>
                        <Face id="982" plane="0.707105 0 -0.707108 -3.35876" album="Auxiliary" material="NULL" texgens="0 -0.707105 0 192 0.5 0 0.499998 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 0 7 4" />
                        </Face>
                        <Face id="983" plane="0.707107 -0.707107 0 -4.06586" album="Auxiliary" material="NULL" texgens="0.707107 0.707107 0 192 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 0 3 6" />
                        </Face>
                    </Brush>
                    <Brush id="37" owner="0" type="0" pos="-5.75 0 5" rot="1 0 0 0" scale="" transform="1 0 0 -5.75 0 1 0 0 0 0 1 5 0 0 0 1" group="-1" locked="0" nextFaceID="985" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-0.25 -6 -5" />
                            <Vertex pos="-0.25 6 -5" />
                            <Vertex pos="-0.25 6 5" />
                            <Vertex pos="-0.25 -6 5" />
                            <Vertex pos="0.25 5.5 -4.5" />
                            <Vertex pos="0.25 5.5 4.50001" />
                            <Vertex pos="0.25 -5.5 4.50001" />
                            <Vertex pos="0.25 -5.5 -4.5" />
                        </Vertices>
                        <Face id="978" plane="-1 0 0 -0.25" album="Starter" material="WALL5" texgens="0 -1 0 192 0 0 -1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="979" plane="0.707107 0.707107 0 -4.06586" album="Auxiliary" material="NULL" texgens="0.707107 -0.707107 0 192 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 5 2 1" />
                        </Face>
                        <Face id="980" plane="0.707102 -0 0.707112 -3.35878" album="Auxiliary" material="NULL" texgens="0 -0.707102 0 192 -0.5 0 0.499993 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 6 3 2" />
                        </Face>
                        <Face id="981" plane="1 0 0 -0.25" album="Starter" material="WOOD2" texgens="0 1 0 8 0 0 -1 8 0 22 18" texRot="0" texScale="22 18" texDiv="16 16">
                            <Indices indices=" 6 5 4 7" />
                        </Face>
                        <Face id="982" plane="0.707105 0 -0.707108 -3.35876" album="Auxiliary" material="NULL" texgens="0 -0.707105 0 192 0.5 0 0.499998 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 0 7 4" />
                        </Face>
                        <Face id="983" plane="0.707107 -0.707107 0 -4.06586" album="Auxiliary" material="NULL" texgens="0.707107 0.707107 0 192 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 0 3 6" />
                        </Face>
                    </Brush>
                    <Brush id="49" owner="0" type="0" pos="0 5.75 5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 5.75 0 0 1 5 0 0 0 1" group="-1" locked="0" nextFaceID="1117" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-6 0.25 -5" />
                            <Vertex pos="6 0.25 -5" />
                            <Vertex pos="6 0.25 5" />
                            <Vertex pos="-6 0.25 5" />
                            <Vertex pos="-5.5 -0.25 -4.5" />
                            <Vertex pos="5.5 -0.25 -4.5" />
                            <Vertex pos="-5.5 -0.25 4.50001" />
                            <Vertex pos="5.5 -0.25 4.50001" />
                        </Vertices>
                        <Face id="1110" plane="0 1 -0 -0.25" album="Starter" material="WALL5" texgens="-1 0 0 192 0 0 -1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1111" plane="-0 -0.707105 -0.707108 -3.35876" album="Auxiliary" material="NULL" texgens="-0.707105 0 0 8 0 -0.5 0.499998 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 5 1 0" />
                        </Face>
                        <Face id="1112" plane="-0.707107 -0.707107 -0 -4.06586" album="Auxiliary" material="NULL" texgens="0.707107 -0.707107 0 192 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 3 6 4" />
                        </Face>
                        <Face id="1113" plane="0 -1 0 -0.25" album="Starter" material="WOOD2" texgens="1 0 0 8 0 0 -1 8 0 22 18" texRot="0" texScale="22 18" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                        <Face id="1114" plane="0.707106 -0.707107 0 -4.06586" album="Auxiliary" material="NULL" texgens="0.707107 0.707107 0 192 0 0 1 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 7 2 1" />
                        </Face>
                        <Face id="1115" plane="0 -0.707102 0.707112 -3.35878" album="Auxiliary" material="NULL" texgens="-0.707102 0 0 192 0 0.5 0.499993 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 3 2" />
                        </Face>
                    </Brush>
                    <Brush id="59" owner="0" type="0" pos="-2.38418e-007 -5.75 5" rot="1 0 0 0" scale="" transform="-1 8.74228e-008 0 -2.38418e-007 -8.74228e-008 -1 0 -5.75 0 0 1 5 0 0 0 1" group="-1" locked="0" nextFaceID="1123" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="5.5 -0.25 4.50001" />
                            <Vertex pos="-5.5 -0.25 4.50001" />
                            <Vertex pos="-6 0.25 5" />
                            <Vertex pos="6 0.25 5" />
                            <Vertex pos="6 0.25 1" />
                            <Vertex pos="-6 0.25 1" />
                            <Vertex pos="-5.5 -0.25 1" />
                            <Vertex pos="5.5 -0.25 1" />
                        </Vertices>
                        <Face id="1116" plane="0 -0.707102 0.707112 -3.35878" album="Auxiliary" material="NULL" texgens="-0.707102 0 0 192 0 0.5 0.499993 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1117" plane="0 1 0 -0.25" album="Starter" material="WALL5" texgens="-1 0 0 256 0 0 -1 256 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 4 3 2 5" />
                        </Face>
                        <Face id="1118" plane="-0.707107 -0.707107 -0 -4.06586" album="Auxiliary" material="NULL" texgens="0.707107 -0.707107 0 192 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 2 1 6" />
                        </Face>
                        <Face id="1119" plane="0 -1 0 -0.25" album="Starter" material="WOOD2" texgens="1 0 0 256 0 0 -1 256 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 0 7 6 1" />
                        </Face>
                        <Face id="1120" plane="0.707106 -0.707107 0 -4.06586" album="Auxiliary" material="NULL" texgens="0.707107 0.707107 0 192 0 0 1 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 0 3 4" />
                        </Face>
                        <Face id="1121" plane="9.93411e-009 5.86112e-007 -1 1" album="Auxiliary" material="NULL" texgens="0 1 0 192 1 0 0 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 4 5 6" />
                        </Face>
                    </Brush>
                    <Brush id="64" owner="0" type="0" pos="-2.38418e-007 -5.75 5" rot="1 0 0 0" scale="" transform="-1 8.74228e-008 0 -2.38418e-007 -8.74228e-008 -1 0 -5.75 0 0 1 5 0 0 0 1" group="-1" locked="0" nextFaceID="1134" nextVertexID="7">
                        <Vertices>
                            <Vertex pos="3 -0.25 -4.5" />
                            <Vertex pos="5.5 -0.25 -4.5" />
                            <Vertex pos="6 0.25 -5" />
                            <Vertex pos="3 0.25 -5" />
                            <Vertex pos="6 0.25 -4.5" />
                            <Vertex pos="3 0.25 -4.5" />
                        </Vertices>
                        <Face id="1128" plane="-0 -0.707105 -0.707108 -3.35876" album="Auxiliary" material="NULL" texgens="-0.707105 0 0 8 0 -0.5 0.499998 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1129" plane="0.707106 -0.707107 0 -4.06586" album="Auxiliary" material="NULL" texgens="0.707107 0.707107 0 192 0 0 1 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 4 2" />
                        </Face>
                        <Face id="1130" plane="0 1 -0 -0.25" album="Starter" material="WALL5" texgens="-1 0 0 256 0 0 -1 256 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 3 2 4 5" />
                        </Face>
                        <Face id="1131" plane="-1 9.53674e-007 0 3" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 3 5" />
                        </Face>
                        <Face id="1132" plane="0 -3.8147e-006 1 4.5" album="Auxiliary" material="NULL" texgens="0 1 0 192 1 0 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 4 1 0" />
                        </Face>
                    </Brush>
                    <Brush id="65" owner="0" type="0" pos="-2.38418e-007 -5.75 5" rot="1 0 0 0" scale="" transform="-1 8.74228e-008 0 -2.38418e-007 -8.74228e-008 -1 0 -5.75 0 0 1 5 0 0 0 1" group="-1" locked="0" nextFaceID="1135" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="5.5 -0.25 1" />
                            <Vertex pos="5.5 -0.25 -4.5" />
                            <Vertex pos="3 -0.25 -4.5" />
                            <Vertex pos="3 -0.25 1" />
                            <Vertex pos="3 0.25 1" />
                            <Vertex pos="6 0.25 1" />
                            <Vertex pos="6 0.25 -4.5" />
                            <Vertex pos="3 0.25 -4.5" />
                        </Vertices>
                        <Face id="1128" plane="0 -1 0 -0.25" album="Starter" material="WOOD2" texgens="1 0 0 256 0 0 -1 256 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1129" plane="0 -5.96046e-007 1 -1" album="Auxiliary" material="NULL" texgens="0 1 0 192 1 0 0 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 3 4 5" />
                        </Face>
                        <Face id="1130" plane="0.707106 -0.707107 0 -4.06586" album="Auxiliary" material="NULL" texgens="0.707107 0.707107 0 192 0 0 1 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 0 5 6" />
                        </Face>
                        <Face id="1131" plane="0 1 0 -0.25" album="Starter" material="WALL5" texgens="-1 0 0 256 0 0 -1 256 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 6 5 4 7" />
                        </Face>
                        <Face id="1132" plane="-1 9.53674e-007 0 3" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 7 4" />
                        </Face>
                        <Face id="1133" plane="0 3.8147e-006 -1 -4.5" album="Auxiliary" material="NULL" texgens="0 1 0 192 1 0 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 2 1 6" />
                        </Face>
                    </Brush>
                    <Brush id="66" owner="0" type="0" pos="-2.38418e-007 -5.75 5" rot="1 0 0 0" scale="" transform="-1 8.74228e-008 0 -2.38418e-007 -8.74228e-008 -1 0 -5.75 0 0 1 5 0 0 0 1" group="-1" locked="0" nextFaceID="1140" nextVertexID="7">
                        <Vertices>
                            <Vertex pos="-3 -0.25 -4.5" />
                            <Vertex pos="3 -0.25 -4.5" />
                            <Vertex pos="3 0.25 -5" />
                            <Vertex pos="-3 0.25 -5" />
                            <Vertex pos="3 0.25 -4.5" />
                            <Vertex pos="-3 0.25 -4.5" />
                        </Vertices>
                        <Face id="1134" plane="-0 -0.707105 -0.707108 -3.35876" album="Auxiliary" material="NULL" texgens="-0.707105 0 0 8 0 -0.5 0.499998 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1135" plane="1 -9.53674e-007 0 -3" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1" />
                        </Face>
                        <Face id="1136" plane="0 1 -0 -0.25" album="Starter" material="WALL5" texgens="-1 0 0 256 0 0 -1 256 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 3 2 4 5" />
                        </Face>
                        <Face id="1137" plane="-1 -4.76837e-007 0 -3" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 3 5" />
                        </Face>
                        <Face id="1138" plane="0 -3.8147e-006 1 4.5" album="Starter" material="WOOD1" texgens="0 1 0 8 1 0 0 -160 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 5 4 1 0" />
                        </Face>
                    </Brush>
                    <Brush id="68" owner="0" type="0" pos="-2.38418e-007 -5.75 5" rot="1 0 0 0" scale="" transform="-1 8.74228e-008 0 -2.38418e-007 -8.74228e-008 -1 0 -5.75 0 0 1 5 0 0 0 1" group="-1" locked="0" nextFaceID="1140" nextVertexID="7">
                        <Vertices>
                            <Vertex pos="-5.5 -0.25 -4.5" />
                            <Vertex pos="-3 -0.25 -4.5" />
                            <Vertex pos="-3 0.25 -5" />
                            <Vertex pos="-6 0.25 -5" />
                            <Vertex pos="-6 0.25 -4.5" />
                            <Vertex pos="-3 0.25 -4.5" />
                        </Vertices>
                        <Face id="1134" plane="-0 -0.707105 -0.707108 -3.35876" album="Auxiliary" material="NULL" texgens="-0.707105 0 0 8 0 -0.5 0.499998 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1135" plane="-0.707107 -0.707107 -0 -4.06586" album="Auxiliary" material="NULL" texgens="0.707107 -0.707107 0 192 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 4 0" />
                        </Face>
                        <Face id="1136" plane="0 1 -0 -0.25" album="Starter" material="WALL5" texgens="-1 0 0 256 0 0 -1 256 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 3 2 5 4" />
                        </Face>
                        <Face id="1137" plane="1 4.76837e-007 -0 3" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 2 1" />
                        </Face>
                        <Face id="1138" plane="0 -3.8147e-006 1 4.5" album="Auxiliary" material="NULL" texgens="0 1 0 192 1 0 0 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 4" />
                        </Face>
                    </Brush>
                    <Brush id="69" owner="0" type="0" pos="-2.38418e-007 -5.75 5" rot="1 0 0 0" scale="" transform="-1 8.74228e-008 0 -2.38418e-007 -8.74228e-008 -1 0 -5.75 0 0 1 5 0 0 0 1" group="-1" locked="0" nextFaceID="1141" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-3 -0.25 -4.5" />
                            <Vertex pos="-5.5 -0.25 -4.5" />
                            <Vertex pos="-5.5 -0.25 1" />
                            <Vertex pos="-3 -0.25 1" />
                            <Vertex pos="-6 0.25 1" />
                            <Vertex pos="-3 0.25 1" />
                            <Vertex pos="-6 0.25 -4.5" />
                            <Vertex pos="-3 0.25 -4.5" />
                        </Vertices>
                        <Face id="1134" plane="0 -1 0 -0.25" album="Starter" material="WOOD2" texgens="1 0 0 256 0 0 -1 256 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1135" plane="0 -3.57628e-007 1 -1" album="Auxiliary" material="NULL" texgens="0 1 0 192 1 0 0 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 4 5" />
                        </Face>
                        <Face id="1136" plane="-0.707107 -0.707107 -0 -4.06586" album="Auxiliary" material="NULL" texgens="0.707107 -0.707107 0 192 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 4 2 1" />
                        </Face>
                        <Face id="1137" plane="0 1 0 -0.25" album="Starter" material="WALL5" texgens="-1 0 0 256 0 0 -1 256 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                        <Face id="1138" plane="1 4.76837e-007 0 3" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 1 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 5 7 0" />
                        </Face>
                        <Face id="1139" plane="0 3.8147e-006 -1 -4.5" album="Auxiliary" material="NULL" texgens="0 1 0 192 1 0 0 -160 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 1 0" />
                        </Face>
                    </Brush>
                    <Brush id="70" owner="0" type="0" pos="-2.67188 -5.75 1.59375" rot="1 0 0 0" scale="" transform="1 0 0 -2.67188 0 1 0 -5.75 0 0 1 1.59375 0 0 0 1" group="-1" locked="0" nextFaceID="55" nextVertexID="73">
                        <Vertices>
                            <Vertex pos="-0.328125 -0.25 1.09375" />
                            <Vertex pos="-0.328125 -0.25 -1.09375" />
                            <Vertex pos="-0.328125 0.25 1.09375" />
                            <Vertex pos="-0.328125 0.25 -1.09375" />
                            <Vertex pos="0.328125 -0.25 1.09375" />
                            <Vertex pos="0.171875 -0.25 -1.09375" />
                            <Vertex pos="0.328125 0.25 1.09375" />
                            <Vertex pos="0.171875 0.25 -1.09375" />
                        </Vertices>
                        <Face id="48" plane="-1 -0 -0 -0.328125" album="Auxiliary" material="NULL" texgens="0 -1 0 8 0 0 -1 -35 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 0 1" />
                        </Face>
                        <Face id="49" plane="0 -1 0 -0.25" album="Starter" material="WALL5" texgens="1 0 0 142 0 0 -1 430.4 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 5 1 0 4" />
                        </Face>
                        <Face id="50" plane="0 1 0 -0.25" album="Starter" material="WOOD2" texgens="-1 0 0 380.364 0 0 -1 449.778 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 7 6 2 3" />
                        </Face>
                        <Face id="51" plane="0 -0 1 -1.09375" album="Auxiliary" material="NULL" texgens="1 0 0 10.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 4 0 2" />
                        </Face>
                        <Face id="52" plane="0.997459 0 -0.071247 -0.249364" album="Starter" material="WOOD1" texgens="0 1 0 8 -0.0712471 0 -0.997459 -34.5192 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                        <Face id="53" plane="0 0 -1 -1.09375" album="Auxiliary" material="NULL" texgens="-1 0 0 85.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 1 5" />
                        </Face>
                    </Brush>
                    <Brush id="71" owner="0" type="0" pos="2.67188 -5.75 1.59375" rot="1 0 0 0" scale="" transform="1 0 0 2.67188 0 1 0 -5.75 0 0 1 1.59375 0 0 0 1" group="-1" locked="0" nextFaceID="55" nextVertexID="73">
                        <Vertices>
                            <Vertex pos="0.328125 -0.25 1.09375" />
                            <Vertex pos="0.328125 -0.25 -1.09375" />
                            <Vertex pos="0.328125 0.25 1.09375" />
                            <Vertex pos="0.328125 0.25 -1.09375" />
                            <Vertex pos="-0.328125 -0.25 1.09375" />
                            <Vertex pos="-0.171875 -0.25 -1.09375" />
                            <Vertex pos="-0.328125 0.25 1.09375" />
                            <Vertex pos="-0.171875 0.25 -1.09375" />
                        </Vertices>
                        <Face id="48" plane="1 0 0 -0.328125" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 -1 -35 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 1 0 2" />
                        </Face>
                        <Face id="49" plane="0 -1 0 -0.25" album="Starter" material="WALL5" texgens="1 0 0 370 0 0 -1 430.4 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 5 4 0 1" />
                        </Face>
                        <Face id="50" plane="0 1 -0 -0.25" album="Starter" material="WOOD2" texgens="-1 0 0 131.636 0 0 -1 449.778 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 7 3 2 6" />
                        </Face>
                        <Face id="51" plane="0 0 1 -1.09375" album="Auxiliary" material="NULL" texgens="1 0 0 181.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 2 0 4" />
                        </Face>
                        <Face id="52" plane="-0.997459 -0 -0.071247 -0.249364" album="Starter" material="WOOD1" texgens="0 -1 0 8 0.0712471 0 -0.997459 -34.5192 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                        <Face id="53" plane="0 0 -1 -1.09375" album="Auxiliary" material="NULL" texgens="-1 0 0 -85.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 1 3" />
                        </Face>
                    </Brush>
                    <Brush id="72" owner="0" type="0" pos="-2.10938 -5.75 4.34375" rot="1 0 0 0" scale="" transform="1 0 0 -2.10938 0 1 0 -5.75 0 0 1 4.34375 0 0 0 1" group="-1" locked="0" nextFaceID="55" nextVertexID="73">
                        <Vertices>
                            <Vertex pos="-0.234375 -0.25 1.65625" />
                            <Vertex pos="0.234375 -0.25 1.65625" />
                            <Vertex pos="-0.234375 -0.25 -1.65625" />
                            <Vertex pos="0.234375 -0.25 -0.093749" />
                            <Vertex pos="-0.234375 0.25 1.65625" />
                            <Vertex pos="0.234375 0.25 1.65625" />
                            <Vertex pos="-0.234375 0.25 -1.65625" />
                            <Vertex pos="0.234375 0.25 -0.093749" />
                        </Vertices>
                        <Face id="48" plane="0 -1 0 -0.25" album="Starter" material="WALL5" texgens="1 0 0 166 0 0 -1 289.6 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 3 2 0 1" />
                        </Face>
                        <Face id="49" plane="0 1 0 -0.25" album="Starter" material="WOOD2" texgens="-1 0 0 354.182 0 0 -1 293.333 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                        <Face id="50" plane="0 -0 1 -1.65625" album="Auxiliary" material="NULL" texgens="1 0 0 28.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 4" />
                        </Face>
                        <Face id="51" plane="-1 -0 -0 -0.234375" album="Auxiliary" material="NULL" texgens="0 -1 0 8 0 0 -1 -123 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 4 0 2" />
                        </Face>
                        <Face id="52" plane="1 0 0 -0.234375" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 -1 -123 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 1 5" />
                        </Face>
                        <Face id="53" plane="0.957826 0 -0.287348 -0.251429" album="Starter" material="WOOD1" texgens="0 1 0 8 -0.287348 0 -0.957826 -123.098 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 2 3" />
                        </Face>
                    </Brush>
                    <Brush id="73" owner="0" type="0" pos="2.10938 -5.75 4.34375" rot="1 0 0 0" scale="" transform="1 0 0 2.10938 0 1 0 -5.75 0 0 1 4.34375 0 0 0 1" group="-1" locked="0" nextFaceID="55" nextVertexID="73">
                        <Vertices>
                            <Vertex pos="0.234375 -0.25 1.65625" />
                            <Vertex pos="-0.234375 -0.25 1.65625" />
                            <Vertex pos="0.234375 -0.25 -1.65625" />
                            <Vertex pos="-0.234375 -0.25 -0.093749" />
                            <Vertex pos="0.234375 0.25 1.65625" />
                            <Vertex pos="-0.234375 0.25 1.65625" />
                            <Vertex pos="0.234375 0.25 -1.65625" />
                            <Vertex pos="-0.234375 0.25 -0.093749" />
                        </Vertices>
                        <Face id="48" plane="0 -1 0 -0.25" album="Starter" material="WALL5" texgens="1 0 0 346 0 0 -1 289.6 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 3 1 0 2" />
                        </Face>
                        <Face id="49" plane="0 1 -0 -0.25" album="Starter" material="WOOD2" texgens="-1 0 0 157.818 0 0 -1 293.333 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                        <Face id="50" plane="0 0 1 -1.65625" album="Auxiliary" material="NULL" texgens="1 0 0 163.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 4 0 1" />
                        </Face>
                        <Face id="51" plane="1 0 0 -0.234375" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 -1 -123 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 2 0 4" />
                        </Face>
                        <Face id="52" plane="-1 -0 -0 -0.234375" album="Auxiliary" material="NULL" texgens="0 -1 0 8 0 0 -1 -123 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 1 3" />
                        </Face>
                        <Face id="53" plane="-0.957826 0 -0.287348 -0.251429" album="Starter" material="WOOD1" texgens="0 -1 0 8 0.287348 0 -0.957826 -123.098 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 2 6" />
                        </Face>
                    </Brush>
                    <Brush id="74" owner="0" type="0" pos="-1.48438 -5.75 5.125" rot="1 0 0 0" scale="" transform="1 0 0 -1.48438 0 1 0 -5.75 0 0 1 5.125 0 0 0 1" group="-1" locked="0" nextFaceID="55" nextVertexID="73">
                        <Vertices>
                            <Vertex pos="-0.390625 -0.25 0.875" />
                            <Vertex pos="0.390625 -0.25 0.875" />
                            <Vertex pos="-0.390625 -0.25 -0.875" />
                            <Vertex pos="0.390625 -0.25 0.0625005" />
                            <Vertex pos="-0.390625 0.25 0.875" />
                            <Vertex pos="0.390625 0.25 0.875" />
                            <Vertex pos="-0.390625 0.25 -0.875" />
                            <Vertex pos="0.390625 0.25 0.0625005" />
                        </Vertices>
                        <Face id="48" plane="0 -1 0 -0.25" album="Starter" material="WALL5" texgens="1 0 0 192.667 0 0 -1 249.6 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 3 2 0 1" />
                        </Face>
                        <Face id="49" plane="0 1 0 -0.25" album="Starter" material="WOOD2" texgens="-1 0 0 325.091 0 0 -1 248.889 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                        <Face id="50" plane="0 -0 1 -0.875" album="Auxiliary" material="NULL" texgens="1 0 0 48.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 4" />
                        </Face>
                        <Face id="51" plane="-1 -0 -0 -0.390625" album="Auxiliary" material="NULL" texgens="0 -1 0 8 0 0 -1 -148 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 4 0 2" />
                        </Face>
                        <Face id="52" plane="1 0 0 -0.390625" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 -1 -148 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 1 5" />
                        </Face>
                        <Face id="53" plane="0.768221 0 -0.640184 -0.260075" album="Starter" material="WOOD1" texgens="0 1 0 8 -0.640184 0 -0.768221 -151.892 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 2 3" />
                        </Face>
                    </Brush>
                    <Brush id="75" owner="0" type="0" pos="1.48438 -5.75 5.125" rot="1 0 0 0" scale="" transform="1 0 0 1.48438 0 1 0 -5.75 0 0 1 5.125 0 0 0 1" group="-1" locked="0" nextFaceID="55" nextVertexID="73">
                        <Vertices>
                            <Vertex pos="0.390625 -0.25 0.875" />
                            <Vertex pos="-0.390625 -0.25 0.875" />
                            <Vertex pos="0.390625 -0.25 -0.875" />
                            <Vertex pos="-0.390625 -0.25 0.0625005" />
                            <Vertex pos="0.390625 0.25 0.875" />
                            <Vertex pos="-0.390625 0.25 0.875" />
                            <Vertex pos="0.390625 0.25 -0.875" />
                            <Vertex pos="-0.390625 0.25 0.0625005" />
                        </Vertices>
                        <Face id="48" plane="0 -1 0 -0.25" album="Starter" material="WALL5" texgens="1 0 0 319.333 0 0 -1 249.6 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 3 1 0 2" />
                        </Face>
                        <Face id="49" plane="0 1 -0 -0.25" album="Starter" material="WOOD2" texgens="-1 0 0 186.909 0 0 -1 248.889 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                        <Face id="50" plane="0 0 1 -0.875" album="Auxiliary" material="NULL" texgens="1 0 0 143.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 4 0 1" />
                        </Face>
                        <Face id="51" plane="1 0 0 -0.390625" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 -1 -148 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 2 0 4" />
                        </Face>
                        <Face id="52" plane="-1 -0 -0 -0.390625" album="Auxiliary" material="NULL" texgens="0 -1 0 8 0 0 -1 -148 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 1 3" />
                        </Face>
                        <Face id="53" plane="-0.768221 0 -0.640184 -0.260075" album="Starter" material="WOOD1" texgens="0 -1 0 8 0.640184 0 -0.768221 -151.892 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 2 6" />
                        </Face>
                    </Brush>
                    <Brush id="76" owner="0" type="0" pos="-0.546875 -5.75 5.59375" rot="1 0 0 0" scale="" transform="1 0 0 -0.546875 0 1 0 -5.75 0 0 1 5.59375 0 0 0 1" group="-1" locked="0" nextFaceID="55" nextVertexID="73">
                        <Vertices>
                            <Vertex pos="-0.546875 -0.25 0.40625" />
                            <Vertex pos="0.546875 -0.25 0.40625" />
                            <Vertex pos="-0.546875 -0.25 -0.40625" />
                            <Vertex pos="0.546875 -0.25 -0.09375" />
                            <Vertex pos="-0.546875 0.25 0.40625" />
                            <Vertex pos="0.546875 0.25 0.40625" />
                            <Vertex pos="-0.546875 0.25 -0.40625" />
                            <Vertex pos="0.546875 0.25 -0.09375" />
                        </Vertices>
                        <Face id="48" plane="0 -1 0 -0.25" album="Starter" material="WALL5" texgens="1 0 0 232.667 0 0 -1 225.6 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 3 2 0 1" />
                        </Face>
                        <Face id="49" plane="0 1 0 -0.25" album="Starter" material="WOOD2" texgens="-1 0 0 281.455 0 0 -1 222.222 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                        <Face id="50" plane="0 -0 1 -0.40625" album="Auxiliary" material="NULL" texgens="1 0 0 78.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 4" />
                        </Face>
                        <Face id="51" plane="-1 -0 -0 -0.546875" album="Auxiliary" material="NULL" texgens="0 -1 0 8 0 0 -1 -163 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 4 0 2" />
                        </Face>
                        <Face id="52" plane="1 0 0 -0.546875" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 -1 -163 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 1 5" />
                        </Face>
                        <Face id="53" plane="0.274721 0 -0.961524 -0.240381" album="Starter" material="WOOD1" texgens="0 1 0 8 -0.961524 0 -0.274721 -181.829 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 2 3" />
                        </Face>
                    </Brush>
                    <Brush id="77" owner="0" type="0" pos="0.546875 -5.75 5.59375" rot="1 0 0 0" scale="" transform="1 0 0 0.546875 0 1 0 -5.75 0 0 1 5.59375 0 0 0 1" group="-1" locked="0" nextFaceID="55" nextVertexID="73">
                        <Vertices>
                            <Vertex pos="0.546875 -0.25 0.40625" />
                            <Vertex pos="-0.546875 -0.25 0.40625" />
                            <Vertex pos="0.546875 -0.25 -0.40625" />
                            <Vertex pos="-0.546875 -0.25 -0.09375" />
                            <Vertex pos="0.546875 0.25 0.40625" />
                            <Vertex pos="-0.546875 0.25 0.40625" />
                            <Vertex pos="0.546875 0.25 -0.40625" />
                            <Vertex pos="-0.546875 0.25 -0.09375" />
                        </Vertices>
                        <Face id="48" plane="0 -1 0 -0.25" album="Starter" material="WALL5" texgens="1 0 0 279.333 0 0 -1 225.6 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 3 1 0 2" />
                        </Face>
                        <Face id="49" plane="0 1 -0 -0.25" album="Starter" material="WOOD2" texgens="-1 0 0 230.545 0 0 -1 222.222 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                        <Face id="50" plane="0 0 1 -0.40625" album="Auxiliary" material="NULL" texgens="1 0 0 113.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 4 0 1" />
                        </Face>
                        <Face id="51" plane="1 0 0 -0.546875" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 -1 -163 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 2 0 4" />
                        </Face>
                        <Face id="52" plane="-1 -0 -0 -0.546875" album="Auxiliary" material="NULL" texgens="0 -1 0 8 0 0 -1 -163 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 1 3" />
                        </Face>
                        <Face id="53" plane="-0.274721 0 -0.961524 -0.240381" album="Starter" material="WOOD1" texgens="0 -1 0 8 0.961524 0 -0.274721 -181.829 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 2 6" />
                        </Face>
                    </Brush>
                    <Brush id="78" owner="0" type="0" pos="-2.67188 -5.75 4.34375" rot="1 0 0 0" scale="" transform="1 0 0 -2.67188 0 1 0 -5.75 0 0 1 4.34375 0 0 0 1" group="-1" locked="0" nextFaceID="55" nextVertexID="73">
                        <Vertices>
                            <Vertex pos="-0.328125 -0.25 1.65625" />
                            <Vertex pos="-0.328125 -0.25 -1.65625" />
                            <Vertex pos="-0.328125 0.25 1.65625" />
                            <Vertex pos="-0.328125 0.25 -1.65625" />
                            <Vertex pos="0.328125 -0.25 1.65625" />
                            <Vertex pos="0.328125 -0.25 -1.65625" />
                            <Vertex pos="0.328125 0.25 1.65625" />
                            <Vertex pos="0.328125 0.25 -1.65625" />
                        </Vertices>
                        <Face id="48" plane="-1 -0 -0 -0.328125" album="Auxiliary" material="NULL" texgens="0 -1 0 8 0 0 -1 -123 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 0 1" />
                        </Face>
                        <Face id="49" plane="0 -1 0 -0.25" album="Starter" material="WALL5" texgens="1 0 0 142 0 0 -1 289.6 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 5 1 0 4" />
                        </Face>
                        <Face id="50" plane="0 1 0 -0.25" album="Starter" material="WOOD2" texgens="-1 0 0 380.364 0 0 -1 293.333 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 7 6 2 3" />
                        </Face>
                        <Face id="51" plane="1 0 0 -0.328125" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 -1 -123 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                        <Face id="52" plane="0 -0 1 -1.65625" album="Auxiliary" material="NULL" texgens="1 0 0 10.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 4 0 2" />
                        </Face>
                        <Face id="53" plane="0 0 -1 -1.65625" album="Auxiliary" material="NULL" texgens="-1 0 0 85.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 1 5" />
                        </Face>
                    </Brush>
                    <Brush id="79" owner="0" type="0" pos="2.67188 -5.75 4.34375" rot="1 0 0 0" scale="" transform="1 0 0 2.67188 0 1 0 -5.75 0 0 1 4.34375 0 0 0 1" group="-1" locked="0" nextFaceID="55" nextVertexID="73">
                        <Vertices>
                            <Vertex pos="0.328125 -0.25 1.65625" />
                            <Vertex pos="0.328125 -0.25 -1.65625" />
                            <Vertex pos="0.328125 0.25 1.65625" />
                            <Vertex pos="0.328125 0.25 -1.65625" />
                            <Vertex pos="-0.328125 -0.25 1.65625" />
                            <Vertex pos="-0.328125 -0.25 -1.65625" />
                            <Vertex pos="-0.328125 0.25 1.65625" />
                            <Vertex pos="-0.328125 0.25 -1.65625" />
                        </Vertices>
                        <Face id="48" plane="1 0 0 -0.328125" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 -1 -123 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 1 0 2" />
                        </Face>
                        <Face id="49" plane="0 -1 0 -0.25" album="Starter" material="WALL5" texgens="1 0 0 370 0 0 -1 289.6 0 0.75 0.625" texRot="0" texScale="0.75 0.625" texDiv="512 512">
                            <Indices indices=" 5 4 0 1" />
                        </Face>
                        <Face id="50" plane="0 1 -0 -0.25" album="Starter" material="WOOD2" texgens="-1 0 0 131.636 0 0 -1 293.333 0 0.6875 0.562501" texRot="0" texScale="0.6875 0.562501" texDiv="512 512">
                            <Indices indices=" 7 3 2 6" />
                        </Face>
                        <Face id="51" plane="-1 -0 -0 -0.328125" album="Auxiliary" material="NULL" texgens="0 -1 0 8 0 0 -1 -123 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                        <Face id="52" plane="0 0 1 -1.65625" album="Auxiliary" material="NULL" texgens="1 0 0 181.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 2 0 4" />
                        </Face>
                        <Face id="53" plane="0 0 -1 -1.65625" album="Auxiliary" material="NULL" texgens="-1 0 0 -85.5 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 1 3" />
                        </Face>
                    </Brush>
                    <Brush id="80" owner="1" type="3" pos="0 -5.75 3" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 -5.75 0 0 1 3 0 0 0 1" group="-1" locked="0" nextFaceID="1279" nextVertexID="1705">
                        <Vertices>
                            <Vertex pos="2.5 0.25 2.5" />
                            <Vertex pos="2.5 0.25 -2.5" />
                            <Vertex pos="2.5 -0.25 2.5" />
                            <Vertex pos="2.5 -0.25 -2.5" />
                            <Vertex pos="-2.5 0.25 2.5" />
                            <Vertex pos="-2.5 0.25 -2.5" />
                            <Vertex pos="-2.5 -0.25 2.5" />
                            <Vertex pos="-2.5 -0.25 -2.5" />
                        </Vertices>
                        <Face id="1272" plane="1 -0 0 -2.5" album="Auxiliary" material="NULL" texgens="0 1 0 8 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 0 1" />
                        </Face>
                        <Face id="1273" plane="-1 0 0 -2.5" album="Auxiliary" material="NULL" texgens="0 -1 0 8 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                        <Face id="1274" plane="0 1 -0 -0.25" album="Auxiliary" material="NULL" texgens="-1 0 0 80 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 4" />
                        </Face>
                        <Face id="1275" plane="0 -1 0 -0.25" album="Auxiliary" material="NULL" texgens="1 0 0 80 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 2 3" />
                        </Face>
                        <Face id="1276" plane="-0 0 1 -2.5" album="Auxiliary" material="NULL" texgens="1 0 0 80 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 4 0 2" />
                        </Face>
                        <Face id="1277" plane="0 0 -1 -2.5" album="Auxiliary" material="NULL" texgens="-1 0 0 80 0 -1 0 -8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 1 5" />
                        </Face>
                    </Brush>
                    <Brush id="81" owner="0" type="0" pos="0 0 -1" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 -1 0 0 0 1" group="-1" locked="0" nextFaceID="133" nextVertexID="177">
                        <Vertices>
                            <Vertex pos="6 -6 1" />
                            <Vertex pos="-6 -6 1" />
                            <Vertex pos="6 6 1" />
                            <Vertex pos="-6 6 1" />
                            <Vertex pos="6 -6 -1" />
                            <Vertex pos="-6 -6 -1" />
                            <Vertex pos="6 6 -1" />
                            <Vertex pos="-6 6 -1" />
                        </Vertices>
                        <Face id="126" plane="0 0 1 -1" album="Auxiliary" material="NULL" texgens="0 1 0 192 -1 0 0 192 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 0 1" />
                        </Face>
                        <Face id="127" plane="0 0 -1 -1" album="Auxiliary" material="NULL" texgens="0 1 0 192 -1 0 0 192 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                        <Face id="128" plane="0 -1 0 -6" album="Auxiliary" material="NULL" texgens="0 0 -1 32 -1 0 0 192 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 4" />
                        </Face>
                        <Face id="129" plane="1 0 0 -6" album="Auxiliary" material="NULL" texgens="0 0 -1 32 0 -1 0 192 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 4 0 2" />
                        </Face>
                        <Face id="130" plane="0 1 -0 -6" album="Auxiliary" material="NULL" texgens="0 0 -1 32 1 0 0 192 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 2 3" />
                        </Face>
                        <Face id="131" plane="-1 -0 -0 -6" album="Auxiliary" material="NULL" texgens="0 0 -1 32 0 1 0 192 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 1 5" />
                        </Face>
                    </Brush>
                </Brushes>
            </InteriorMap>
        </DetailLevel>
    </DetailLevels>
</ConstructorScene>
