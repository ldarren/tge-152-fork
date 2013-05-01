<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
<!--Torque Constructor Scene document http://www.garagegames.com-->
<ConstructorScene version="4" creator="Torque Constructor" date="2007/10/09 22:55:30">
    <Sunlight azimuth="180" elevation="35" color="255 255 255" ambient="64 64 64" />
    <LightingOptions lightingSystem="" ineditor_defaultLightmapSize="256" ineditor_maxLightmapSize="256" ineditor_lightingPerformanceHint="0" ineditor_shadowPerformanceHint="1" ineditor_TAPCompatibility="0" ineditor_useSunlight="0" export_defaultLightmapSize="256" export_maxLightmapSize="256" export_lightingPerformanceHint="0" export_shadowPerformanceHint="1" export_TAPCompatibility="0" export_useSunlight="0" />
    <GameTypes>
        <GameType name="Constructor" />
        <GameType name="Torque" />
    </GameTypes>
    <SceneShapes nextShapeID="1">
        <SceneShape id="0" type="0" file="../../shapes/Npc/cubixmale/player.dts" pos="0 -8.5 0" rot="1 0 0 0" scale="1 1 1" transform="1 0 0 0 0 1 0 -8.5 0 0 1 0 0 0 0 1" group="-1" locked="0" />
    </SceneShapes>
    <DetailLevels current="0">
        <DetailLevel minPixelSize="0" actionCenter="0 0 0">
            <InteriorMap brushScale="32" lightScale="8" ambientColor="0 0 0" ambientColorEmerg="0 0 0">
                <Entities nextEntityID="1">
                    <Entity id="0" classname="worldspawn" gametype="Torque" isPointEntity="0">
                        <Properties detail_number="0" min_pixels="250" geometry_scale="32.0" light_geometry_scale="8.0" light_smoothing_scale="4.0" light_mesh_scale="1.0" ambient_color="0 0 0" emergency_ambient_color="0 0 0" mapversion="220" />
                    </Entity>
                </Entities>
                <Brushes nextBrushID="100">
                    <Brush id="24" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="12 -8 1" />
                            <Vertex pos="12 -8 -2" />
                            <Vertex pos="4 -8 -2" />
                            <Vertex pos="4 -8 1" />
                            <Vertex pos="4 -7.5 -2" />
                            <Vertex pos="4 -7.5 1" />
                            <Vertex pos="11.5 -7.5 1" />
                            <Vertex pos="11.5 -7.5 -2" />
                        </Vertices>
                        <Face id="0" plane="0 -1 0 -8" album="showroom" material="EMPORIUM_1" texgens="1 0 0 384 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-1 -0 -0 4" album="showroom" material="CLUB_2" texgens="0 1 0 176 0 0 -1 16 90 1 1" texRot="90" texScale="1 1" texDiv="128 128">
                            <Indices indices=" 4 5 3 2" />
                        </Face>
                        <Face id="2" plane="0 0 1 -1" album="Auxiliary" material="NULL" texgens="1 0 0 128 0 -1 0 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 6 0 3" />
                        </Face>
                        <Face id="3" plane="-0 1 0 7.5" album="showroom" material="EMPORIUM_2" texgens="1.04348 0 0 384 0 0 -1.25 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 6 5 4 7" />
                        </Face>
                        <Face id="4" plane="0 0 -1 -2" album="Auxiliary" material="NULL" texgens="-1 0 0 128 0 -1 0 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 2 1 7 4" />
                        </Face>
                        <Face id="5" plane="0.707107 0.707107 -0 -2.82843" album="Auxiliary" material="NULL" texgens="0.707102 -0.707112 0 128 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 1 0 6" />
                        </Face>
                    </Brush>
                    <Brush id="72" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="12 8 2.5" />
                            <Vertex pos="12 -8 2.5" />
                            <Vertex pos="-12 -8 2.5" />
                            <Vertex pos="-12 8 2.5" />
                            <Vertex pos="-11.5 7.5 2" />
                            <Vertex pos="11.5 7.5 2" />
                            <Vertex pos="-11.5 -7.5 2" />
                            <Vertex pos="11.5 -7.5 2" />
                        </Vertices>
                        <Face id="0" plane="0 -0 1 -2.5" album="showroom" material="CLUB_2" texgens="1 0 0 384 0 -1 0 -256 0 1 1" texRot="0" texScale="1 1" texDiv="128 128">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 0.707107 -0.707107 -3.88909" album="Auxiliary" material="NULL" texgens="-0.707107 0 0 256 0 0.5 0.5 -80 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 5 0 3" />
                        </Face>
                        <Face id="2" plane="-0.707107 -0 -0.707107 -6.71751" album="Auxiliary" material="NULL" texgens="0 -0.707112 0 384 -0.5 0 0.500007 -80 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 4 3 2" />
                        </Face>
                        <Face id="3" plane="-0 -0 -1 2" album="showroom" material="CLUB_2" texgens="1.04348 0 0 384 0 -1.06667 0 -256 0 1 1" texRot="0" texScale="1 1" texDiv="128 128">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                        <Face id="4" plane="0.707107 0 -0.707107 -6.71751" album="Auxiliary" material="NULL" texgens="0 -0.707112 0 384 0.5 0 0.500007 -80 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 7 1 0" />
                        </Face>
                        <Face id="5" plane="0 -0.707107 -0.707107 -3.88909" album="Auxiliary" material="NULL" texgens="-0.707107 0 0 384 0 -0.5 0.5 -80 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 2 1" />
                        </Face>
                    </Brush>
                    <Brush id="76" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="12 -8 -2.5" />
                            <Vertex pos="12 8 -2.5" />
                            <Vertex pos="-12 8 -2.5" />
                            <Vertex pos="-12 -8 -2.5" />
                            <Vertex pos="-11.5 7.5 -2" />
                            <Vertex pos="11.5 7.5 -2" />
                            <Vertex pos="-11.5 -7.5 -2" />
                            <Vertex pos="11.5 -7.5 -2" />
                        </Vertices>
                        <Face id="0" plane="-0 -0 -1 -2.5" album="Auxiliary" material="NULL" texgens="-1 0 0 384 0 -1 0 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-0 0.707107 0.707107 -3.88909" album="Auxiliary" material="NULL" texgens="-0.707107 0 0 256 0 -0.5 0.5 -80 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1 5" />
                        </Face>
                        <Face id="2" plane="-0.707107 0 0.707107 -6.71751" album="Auxiliary" material="NULL" texgens="0 -0.707112 0 384 0.5 0 0.500007 -80 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 4 6" />
                        </Face>
                        <Face id="3" plane="-0 0 1 2" album="showroom" material="FLOOR" texgens="-1.04348 0 0 384 0 -1.06667 0 -256 0 1 1" texRot="0" texScale="1 1" texDiv="128 128">
                            <Indices indices=" 6 4 5 7" />
                        </Face>
                        <Face id="4" plane="0.707107 0 0.707107 -6.71751" album="Auxiliary" material="NULL" texgens="0 -0.70711 0 384 -0.5 0 0.500005 -80 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 7" />
                        </Face>
                        <Face id="5" plane="0 -0.707107 0.707107 -3.88909" album="Auxiliary" material="NULL" texgens="-0.707105 0 0 384 0 0.5 0.499998 -80 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 0 3 6" />
                        </Face>
                    </Brush>
                    <Brush id="80" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-11.5 -7.49999 1.99999" />
                            <Vertex pos="-12 -8 2.5" />
                            <Vertex pos="-12 8 2.5" />
                            <Vertex pos="-11.5 7.49999 1.99999" />
                            <Vertex pos="-12 8 1" />
                            <Vertex pos="-12 -8 1" />
                            <Vertex pos="-11.5 7.49999 1" />
                            <Vertex pos="-11.5 -7.49999 1" />
                        </Vertices>
                        <Face id="0" plane="0.707114 -0 0.7071 6.71762" album="Auxiliary" material="NULL" texgens="0 -0.707112 0 384 -0.5 0 0.500007 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-1 -0 -0 -12" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 256 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 4 2 1 5" />
                        </Face>
                        <Face id="2" plane="0.707114 0.7071 0 2.82857" album="Auxiliary" material="NULL" texgens="0.707102 -0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 3 2 4" />
                        </Face>
                        <Face id="3" plane="1 0 -0 11.5" album="showroom" material="EMPORIUM_2" texgens="0 -1.06667 0 256 0 0 -1.25 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 3 6 7 0" />
                        </Face>
                        <Face id="4" plane="0.707114 -0.7071 0 2.82857" album="Auxiliary" material="NULL" texgens="0.707102 0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 7" />
                        </Face>
                        <Face id="5" plane="-0 -0 -1 1" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 256 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                    </Brush>
                    <Brush id="82" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="11.5 7.5 2" />
                            <Vertex pos="-11.5 7.5 2" />
                            <Vertex pos="-12 8 2.5" />
                            <Vertex pos="12 8 2.5" />
                            <Vertex pos="12 8 1" />
                            <Vertex pos="-12 8 1" />
                            <Vertex pos="-11.5 7.5 1" />
                            <Vertex pos="11.5 7.5 1" />
                        </Vertices>
                        <Face id="0" plane="0 -0.707107 0.707107 3.88909" album="Auxiliary" material="NULL" texgens="-0.707107 0 0 384 0 0.5 0.5 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 1 0 -8" album="showroom" material="EMPORIUM_1" texgens="-1 0 0 384 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 4 3 2 5" />
                        </Face>
                        <Face id="2" plane="-0.707107 -0.707107 -0 -2.82843" album="Auxiliary" material="NULL" texgens="0.707102 -0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 2 1 6" />
                        </Face>
                        <Face id="3" plane="0 -1 0 7.5" album="showroom" material="EMPORIUM_2" texgens="-1.04348 0 0 384 0 0 -1.25 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 0 7 6 1" />
                        </Face>
                        <Face id="4" plane="0.707107 -0.707107 0 -2.82843" album="Auxiliary" material="NULL" texgens="0.707102 0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 0 3 4" />
                        </Face>
                        <Face id="5" plane="-0 0 -1 1" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 384 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 7 4 5 6" />
                        </Face>
                    </Brush>
                    <Brush id="84" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="11.5 -7.5 1.99999" />
                            <Vertex pos="11.5 7.5 1.99999" />
                            <Vertex pos="12 8 2.5" />
                            <Vertex pos="12 -8 2.5" />
                            <Vertex pos="12 8 1" />
                            <Vertex pos="12 -8 1" />
                            <Vertex pos="11.5 7.5 1" />
                            <Vertex pos="11.5 -7.5 1" />
                        </Vertices>
                        <Face id="0" plane="-0.707114 0 0.7071 6.71762" album="Auxiliary" material="NULL" texgens="0 -0.707112 0 384 0.5 0 0.500007 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="1 0 -0 -12" album="showroom" material="EMPORIUM_1" texgens="0 1 0 256 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 2 4 5 3" />
                        </Face>
                        <Face id="2" plane="-0.707107 0.707107 0 2.82843" album="Auxiliary" material="NULL" texgens="0.707102 0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1 6" />
                        </Face>
                        <Face id="3" plane="-1 -0 -0 11.5" album="showroom" material="EMPORIUM_2" texgens="0 1.06667 0 256 0 0 -1.25 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 6 1 0 7" />
                        </Face>
                        <Face id="4" plane="-0.707107 -0.707107 -0 2.82843" album="Auxiliary" material="NULL" texgens="0.707102 -0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 0 3 5" />
                        </Face>
                        <Face id="5" plane="0 0 -1 1" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 256 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                    </Brush>
                    <Brush id="86" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-11.5 -7.5 2" />
                            <Vertex pos="-4 -7.5 2" />
                            <Vertex pos="-4 -8 2.5" />
                            <Vertex pos="-12 -8 2.5" />
                            <Vertex pos="-4 -8 1" />
                            <Vertex pos="-12 -8 1" />
                            <Vertex pos="-4 -7.5 1" />
                            <Vertex pos="-11.5 -7.5 1" />
                        </Vertices>
                        <Face id="0" plane="0 0.707107 0.707107 3.88909" album="Auxiliary" material="NULL" texgens="-0.707107 0 0 -224 0 -0.5 0.5 -64 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-0 -1 -0 -8" album="showroom" material="EMPORIUM_1" texgens="1 0 0 384 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 3 2 4 5" />
                        </Face>
                        <Face id="2" plane="1 0 0 4" album="Auxiliary" material="NULL" texgens="0 -1 0 -224 0 0 -1 -64 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1 6" />
                        </Face>
                        <Face id="3" plane="0 1 0 7.5" album="showroom" material="EMPORIUM_2" texgens="1.04348 0 0 384 0 0 -1.25 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 6 1 0 7" />
                        </Face>
                        <Face id="4" plane="-0.707107 0.707107 0 -2.82843" album="Auxiliary" material="NULL" texgens="0.707102 0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 0 3 5" />
                        </Face>
                        <Face id="5" plane="0 0 -1 1" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 384 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                    </Brush>
                    <Brush id="88" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-11.5 7.49999 1" />
                            <Vertex pos="-11.5 7.49999 -1.99999" />
                            <Vertex pos="-11.5 -7.49999 -1.99999" />
                            <Vertex pos="-11.5 -7.49999 1" />
                            <Vertex pos="-12 -8 1" />
                            <Vertex pos="-12 8 1" />
                            <Vertex pos="-12 8 -1.99997" />
                            <Vertex pos="-12 -8 -1.99997" />
                        </Vertices>
                        <Face id="0" plane="1 0 -0 11.5" album="showroom" material="EMPORIUM_2" texgens="0 -1.06667 0 256 0 0 -1.25 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 0 1 -1" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 256 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 3 4 5 0" />
                        </Face>
                        <Face id="2" plane="-1 -0 -0 -12" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 256 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 6 5 4 7" />
                        </Face>
                        <Face id="3" plane="0.707114 0.7071 0 2.82857" album="Auxiliary" material="NULL" texgens="0.707102 -0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 0 5 6" />
                        </Face>
                        <Face id="4" plane="0.707114 -0.7071 0 2.82857" album="Auxiliary" material="NULL" texgens="0.707102 0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 4 3 2" />
                        </Face>
                        <Face id="5" plane="-4.00543e-005 0 -1 -2.00045" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 256 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 7 2 1 6" />
                        </Face>
                    </Brush>
                    <Brush id="89" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="6" nextVertexID="7">
                        <Vertices>
                            <Vertex pos="-11.5 -7.49999 -1.99999" />
                            <Vertex pos="-11.5 7.49999 -1.99999" />
                            <Vertex pos="-12 8 -2.5" />
                            <Vertex pos="-12 -8 -2.5" />
                            <Vertex pos="-12 8 -1.99997" />
                            <Vertex pos="-12 -8 -1.99997" />
                        </Vertices>
                        <Face id="0" plane="0.707114 0 -0.7071 6.71762" album="Auxiliary" material="NULL" texgens="0 -0.707112 0 384 0.5 0 0.500007 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-1 0 0 -12" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 256 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 3 2 4 5" />
                        </Face>
                        <Face id="2" plane="0.707114 0.7071 0 2.82857" album="Auxiliary" material="NULL" texgens="0.707102 -0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 4 2" />
                        </Face>
                        <Face id="3" plane="0.707114 -0.7071 0 2.82857" album="Auxiliary" material="NULL" texgens="0.707102 0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 5 0" />
                        </Face>
                        <Face id="4" plane="4.00543e-005 0 1 2.00045" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 256 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 5 4 1 0" />
                        </Face>
                    </Brush>
                    <Brush id="90" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="11.5 7.5 1" />
                            <Vertex pos="11.5 7.5 -2" />
                            <Vertex pos="-11.5 7.5 -2" />
                            <Vertex pos="-11.5 7.5 1" />
                            <Vertex pos="-12 8 1" />
                            <Vertex pos="12 8 1" />
                            <Vertex pos="12 8 -2.00006" />
                            <Vertex pos="-12 8 -1.99997" />
                        </Vertices>
                        <Face id="0" plane="0 -1 0 7.5" album="showroom" material="EMPORIUM_2" texgens="-1.04348 0 0 384 0 0 -1.25 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 -0 1 -1" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 384 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 0 3 4 5" />
                        </Face>
                        <Face id="2" plane="0 1 0 -8" album="showroom" material="EMPORIUM_1" texgens="-1 0 0 384 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 6 5 4 7" />
                        </Face>
                        <Face id="3" plane="-0.707107 -0.707107 -0 -2.82843" album="Auxiliary" material="NULL" texgens="0.707102 -0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 4 3 2" />
                        </Face>
                        <Face id="4" plane="0.707107 -0.707107 0 -2.82843" album="Auxiliary" material="NULL" texgens="0.707102 0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 0 5 6" />
                        </Face>
                        <Face id="5" plane="0 6.00815e-005 -1 -2.00045" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 384 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 7 2 1 6" />
                        </Face>
                    </Brush>
                    <Brush id="91" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="6" nextVertexID="7">
                        <Vertices>
                            <Vertex pos="11.5 7.5 -2" />
                            <Vertex pos="12 8 -2.5" />
                            <Vertex pos="-12 8 -2.5" />
                            <Vertex pos="-11.5 7.5 -2" />
                            <Vertex pos="12 8 -2.00006" />
                            <Vertex pos="-12 8 -1.99997" />
                        </Vertices>
                        <Face id="0" plane="-0 -0.707107 -0.707107 3.88909" album="Auxiliary" material="NULL" texgens="-0.707107 0 0 384 0 -0.5 0.5 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 1 -0 -8" album="showroom" material="EMPORIUM_1" texgens="-1 0 0 384 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 2 1 4 5" />
                        </Face>
                        <Face id="2" plane="-0.707107 -0.707107 -0 -2.82843" album="Auxiliary" material="NULL" texgens="0.707102 -0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 2 5 3" />
                        </Face>
                        <Face id="3" plane="0.707107 -0.707107 0 -2.82843" album="Auxiliary" material="NULL" texgens="0.707102 0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 4 1" />
                        </Face>
                        <Face id="4" plane="3.75509e-006 0.000116408 1 1.99908" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 384 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 5 4 0 3" />
                        </Face>
                    </Brush>
                    <Brush id="92" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="11.5 7.5 -1.99999" />
                            <Vertex pos="11.5 7.5 1" />
                            <Vertex pos="11.5 -7.5 1" />
                            <Vertex pos="11.5 -7.5 -1.99999" />
                            <Vertex pos="12 8 1" />
                            <Vertex pos="12 -8 1" />
                            <Vertex pos="12 8 -2.00006" />
                            <Vertex pos="12 -8 -2.00006" />
                        </Vertices>
                        <Face id="0" plane="-1 -0 -0 11.5" album="showroom" material="EMPORIUM_2" texgens="0 1.06667 0 256 0 0 -1.25 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-0 0 1 -1" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 256 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 2 1 4 5" />
                        </Face>
                        <Face id="2" plane="1 0 -0 -12" album="showroom" material="EMPORIUM_1" texgens="0 1 0 256 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 4 6 7 5" />
                        </Face>
                        <Face id="3" plane="-0.707107 0.707107 0 2.82843" album="Auxiliary" material="NULL" texgens="0.707102 0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 4 1 0" />
                        </Face>
                        <Face id="4" plane="-0.707107 -0.707107 -0 2.82843" album="Auxiliary" material="NULL" texgens="0.707102 -0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 5 7" />
                        </Face>
                        <Face id="5" plane="-0.00014019 -0 -1 -1.99838" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 256 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 7 6 0 3" />
                        </Face>
                    </Brush>
                    <Brush id="93" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="6" nextVertexID="7">
                        <Vertices>
                            <Vertex pos="11.5 -7.5 -1.99999" />
                            <Vertex pos="12 -8 -2.5" />
                            <Vertex pos="12 8 -2.5" />
                            <Vertex pos="11.5 7.5 -1.99999" />
                            <Vertex pos="12 8 -2.00006" />
                            <Vertex pos="12 -8 -2.00006" />
                        </Vertices>
                        <Face id="0" plane="-0.707114 0 -0.7071 6.71762" album="Auxiliary" material="NULL" texgens="0 -0.70711 0 384 -0.5 0 0.500005 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="1 0 -0 -12" album="showroom" material="EMPORIUM_1" texgens="0 1 0 256 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 4 2 1 5" />
                        </Face>
                        <Face id="2" plane="-0.707107 0.707107 0 2.82843" album="Auxiliary" material="NULL" texgens="0.707102 0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 2 4 3" />
                        </Face>
                        <Face id="3" plane="-0.707107 -0.707107 -0 2.82843" album="Auxiliary" material="NULL" texgens="0.707102 -0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 5 1" />
                        </Face>
                        <Face id="4" plane="0.00014019 -0 1 1.99838" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 256 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 5 0 3 4" />
                        </Face>
                    </Brush>
                    <Brush id="94" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-4 -7.5 -2" />
                            <Vertex pos="-4 -7.5 1" />
                            <Vertex pos="-11.5 -7.5 1" />
                            <Vertex pos="-11.5 -7.5 -2" />
                            <Vertex pos="-4 -8 1" />
                            <Vertex pos="-12 -8 1" />
                            <Vertex pos="-4 -8 -2" />
                            <Vertex pos="-12 -8 -1.99997" />
                        </Vertices>
                        <Face id="0" plane="0 1 0 7.5" album="showroom" material="EMPORIUM_2" texgens="1.04348 0 0 384 0 0 -1.25 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 0 1 -1" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 384 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 2 1 4 5" />
                        </Face>
                        <Face id="2" plane="0 -1 0 -8" album="showroom" material="EMPORIUM_1" texgens="1 0 0 384 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 4 6 7 5" />
                        </Face>
                        <Face id="3" plane="1 -0 0 4" album="showroom" material="CLUB_2" texgens="0 -1 0 -304 0 0 -1 16 90 1 1" texRot="90" texScale="1 1" texDiv="128 128">
                            <Indices indices=" 6 4 1 0" />
                        </Face>
                        <Face id="4" plane="-0.707107 0.707107 0 -2.82843" album="Auxiliary" material="NULL" texgens="0.707102 0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 5 7" />
                        </Face>
                        <Face id="5" plane="-3.75509e-006 0 -1 -2.00002" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 384 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 7 6 0 3" />
                        </Face>
                    </Brush>
                    <Brush id="95" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="6" nextVertexID="7">
                        <Vertices>
                            <Vertex pos="-11.5 -7.5 -2" />
                            <Vertex pos="-12 -8 -2.5" />
                            <Vertex pos="-4 -8 -2.5" />
                            <Vertex pos="-4 -7.5 -2" />
                            <Vertex pos="-4 -8 -2" />
                            <Vertex pos="-12 -8 -1.99997" />
                        </Vertices>
                        <Face id="0" plane="0 0.707107 -0.707107 3.88909" album="Auxiliary" material="NULL" texgens="-0.707105 0 0 -224 0 0.5 0.499998 -64 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 -1 0 -8" album="showroom" material="EMPORIUM_1" texgens="1 0 0 384 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 4 2 1 5" />
                        </Face>
                        <Face id="2" plane="1 -0 0 4" album="Auxiliary" material="NULL" texgens="0 -1 0 -224 0 0 -1 -64 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 2 4 3" />
                        </Face>
                        <Face id="3" plane="-0.707107 0.707107 0 -2.82843" album="Auxiliary" material="NULL" texgens="0.707102 0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 5 1" />
                        </Face>
                        <Face id="4" plane="-0 6.00815e-005 1 2.00045" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 384 1 0 0 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 5 0 3 4" />
                        </Face>
                    </Brush>
                    <Brush id="96" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-4 -7.5 1" />
                            <Vertex pos="-4 -7.5 2" />
                            <Vertex pos="-4 -8 2.5" />
                            <Vertex pos="-4 -8 1" />
                            <Vertex pos="4 -8 2.5" />
                            <Vertex pos="4 -8 1" />
                            <Vertex pos="4 -7.5 1" />
                            <Vertex pos="4 -7.5 2" />
                        </Vertices>
                        <Face id="0" plane="-1 -0 -0 -4" album="Auxiliary" material="NULL" texgens="0 -1 0 -224 0 0 -1 -64 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-0 -1 -0 -8" album="showroom" material="EMPORIUM_1" texgens="1 0 0 384 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 2 4 5 3" />
                        </Face>
                        <Face id="2" plane="0 0 -1 1" album="showroom" material="CLUB_2" texgens="1 0 0 128 0 -1 0 -256 0 1 1" texRot="0" texScale="1 1" texDiv="128 128">
                            <Indices indices=" 0 3 5 6" />
                        </Face>
                        <Face id="3" plane="0 1 0 7.5" album="showroom" material="EMPORIUM_2" texgens="1.04348 0 0 384 0 0 -1.25 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 1 0 6 7" />
                        </Face>
                        <Face id="4" plane="-0 0.707107 0.707107 3.88909" album="Auxiliary" material="NULL" texgens="-0.707107 0 0 -224 0 -0.5 0.5 -64 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1 7" />
                        </Face>
                        <Face id="5" plane="1 0 -0 -4" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 384 0 0 1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 7 6 5 4" />
                        </Face>
                    </Brush>
                    <Brush id="97" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="11.5 -7.5 1" />
                            <Vertex pos="12 -8 1" />
                            <Vertex pos="12 -8 2.5" />
                            <Vertex pos="11.5 -7.5 2" />
                            <Vertex pos="4 -8 2.5" />
                            <Vertex pos="4 -8 1" />
                            <Vertex pos="4 -7.5 1" />
                            <Vertex pos="4 -7.5 2" />
                        </Vertices>
                        <Face id="0" plane="0.707107 0.707107 -0 -2.82843" album="Auxiliary" material="NULL" texgens="0.707102 -0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-0 -1 -0 -8" album="showroom" material="EMPORIUM_1" texgens="1 0 0 384 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 4 2 1 5" />
                        </Face>
                        <Face id="2" plane="0 0 -1 1" album="Auxiliary" material="NULL" texgens="1 0 0 128 0 -1 0 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 6" />
                        </Face>
                        <Face id="3" plane="0 1 -0 7.5" album="showroom" material="EMPORIUM_2" texgens="1.04348 0 0 384 0 0 -1.25 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 6 0 3 7" />
                        </Face>
                        <Face id="4" plane="0 0.707107 0.707107 3.88909" album="Auxiliary" material="NULL" texgens="-0.707107 0 0 -224 0 -0.5 0.5 -64 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 4 7" />
                        </Face>
                        <Face id="5" plane="-1 0 0 4" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 384 0 0 1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 7 4 5 6" />
                        </Face>
                    </Brush>
                    <Brush id="98" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="6" nextVertexID="7">
                        <Vertices>
                            <Vertex pos="-4 -7.5 -2" />
                            <Vertex pos="-4 -8 -2" />
                            <Vertex pos="-4 -8 -2.5" />
                            <Vertex pos="4 -8 -2.5" />
                            <Vertex pos="4 -8 -2" />
                            <Vertex pos="4 -7.5 -2" />
                        </Vertices>
                        <Face id="0" plane="-1 0 0 -4" album="Auxiliary" material="NULL" texgens="0 -1 0 -224 0 0 -1 -64 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2" />
                        </Face>
                        <Face id="1" plane="0 -1 0 -8" album="showroom" material="EMPORIUM_1" texgens="1 0 0 384 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 3 2 1 4" />
                        </Face>
                        <Face id="2" plane="0 0 1 2" album="showroom" material="CLUB_2" texgens="-1 0 0 128 0 -1 0 -256 0 1 1" texRot="0" texScale="1 1" texDiv="128 128">
                            <Indices indices=" 0 5 4 1" />
                        </Face>
                        <Face id="3" plane="0 0.707107 -0.707107 3.88909" album="Auxiliary" material="NULL" texgens="-0.707105 0 0 -224 0 0.5 0.499998 -64 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 0 2 3" />
                        </Face>
                        <Face id="4" plane="1 4.76837e-007 -0 -4" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 384 0 0 1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 5 3 4" />
                        </Face>
                    </Brush>
                    <Brush id="99" owner="0" type="0" pos="0 0 2.5" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 2.5 0 0 0 1" group="-1" locked="0" nextFaceID="6" nextVertexID="7">
                        <Vertices>
                            <Vertex pos="11.5 -7.5 -2" />
                            <Vertex pos="12 -8 -2.5" />
                            <Vertex pos="12 -8 -2" />
                            <Vertex pos="4 -8 -2.5" />
                            <Vertex pos="4 -8 -2" />
                            <Vertex pos="4 -7.5 -2" />
                        </Vertices>
                        <Face id="0" plane="0.707107 0.707107 -0 -2.82843" album="Auxiliary" material="NULL" texgens="0.707102 -0.707112 0 384 0 0 1 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2" />
                        </Face>
                        <Face id="1" plane="0 -1 0 -8" album="showroom" material="EMPORIUM_1" texgens="1 0 0 384 0 0 -1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 2 1 3 4" />
                        </Face>
                        <Face id="2" plane="0 0 1 2" album="Auxiliary" material="NULL" texgens="-1 0 0 128 0 -1 0 -256 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 0 2 4" />
                        </Face>
                        <Face id="3" plane="0 0.707107 -0.707107 3.88909" album="Auxiliary" material="NULL" texgens="-0.707105 0 0 -224 0 0.5 0.499998 -64 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 5 3 1" />
                        </Face>
                        <Face id="4" plane="-1 -4.76837e-007 -0 4" album="showroom" material="EMPORIUM_1" texgens="0 -1 0 384 0 0 1 -80 0 1 1" texRot="0" texScale="1 1" texDiv="128 256">
                            <Indices indices=" 5 4 3" />
                        </Face>
                    </Brush>
                </Brushes>
            </InteriorMap>
        </DetailLevel>
    </DetailLevels>
</ConstructorScene>
