<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
<!--Torque Constructor Scene document http://www.garagegames.com-->
<ConstructorScene version="4" creator="Torque Constructor" date="2007/10/07 16:20:52">
    <Sunlight azimuth="180" elevation="35" color="255 255 255" ambient="64 64 64" />
    <LightingOptions lightingSystem="" ineditor_defaultLightmapSize="256" ineditor_maxLightmapSize="256" ineditor_lightingPerformanceHint="0" ineditor_shadowPerformanceHint="1" ineditor_TAPCompatibility="0" ineditor_useSunlight="0" export_defaultLightmapSize="256" export_maxLightmapSize="256" export_lightingPerformanceHint="0" export_shadowPerformanceHint="1" export_TAPCompatibility="0" export_useSunlight="0" />
    <GameTypes>
        <GameType name="Constructor" />
        <GameType name="Torque" />
    </GameTypes>
    <SceneShapes nextShapeID="1">
        <SceneShape id="0" type="0" file="../../Shapes/Npc/cubixmale/CubixStudioMale.dts" pos="0 0 1" rot="1 0 0 0" scale="1 1 1" transform="1 0 0 0 0 1 0 0 0 0 1 1 0 0 0 1" group="-1" locked="0" />
    </SceneShapes>
    <DetailLevels current="0">
        <DetailLevel minPixelSize="0" actionCenter="0 0 0">
            <InteriorMap brushScale="32" lightScale="8" ambientColor="0 0 0" ambientColorEmerg="0 0 0">
                <Entities nextEntityID="1">
                    <Entity id="0" classname="worldspawn" gametype="Torque" isPointEntity="0">
                        <Properties detail_number="0" min_pixels="250" geometry_scale="32.0" light_geometry_scale="8.0" light_smoothing_scale="4.0" light_mesh_scale="1.0" ambient_color="0 0 0" emergency_ambient_color="0 0 0" mapversion="220" />
                    </Entity>
                </Entities>
                <Brushes nextBrushID="57">
                    <Brush id="22" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-5 -5 -0.75" />
                            <Vertex pos="-5 5 -0.75" />
                            <Vertex pos="-40 5 -0.75" />
                            <Vertex pos="-40 -5 -0.75" />
                            <Vertex pos="-5 5 0.25" />
                            <Vertex pos="-40 5 0.25" />
                            <Vertex pos="-40 -5 0.25" />
                            <Vertex pos="-5 -5 0.25" />
                        </Vertices>
                        <Face id="0" plane="-0 -0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 1 0 -5" album="Auxiliary" material="NULL" texgens="-1 0 0 1376 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 4 5 2" />
                        </Face>
                        <Face id="2" plane="-1 0 0 -40" album="Auxiliary" material="NULL" texgens="0 -1 0 960 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 5 6" />
                        </Face>
                        <Face id="3" plane="1 0 -0 5" album="Auxiliary" material="NULL" texgens="0 -1 0 960 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 1 0 7" />
                        </Face>
                        <Face id="4" plane="0 -1 0 -5" album="Auxiliary" material="NULL" texgens="1 0 0 1360 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 3 6 7" />
                        </Face>
                        <Face id="5" plane="0 0 1 -0.25" album="street" material="RoadTex_n_diffuse" texgens="-1 0 0 128 0 -1 0 292.571 90 1.25 1" texRot="90" texScale="1.25 1" texDiv="256 256">
                            <Indices indices=" 7 6 5 4" />
                        </Face>
                    </Brush>
                    <Brush id="30" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-5 5 -0.75" />
                            <Vertex pos="-5 30 -0.75" />
                            <Vertex pos="-5 30 0.25" />
                            <Vertex pos="-5 5 0.25" />
                            <Vertex pos="5 30 -0.75" />
                            <Vertex pos="5 30 0.25" />
                            <Vertex pos="5 5 -0.75" />
                            <Vertex pos="5 5 0.25" />
                        </Vertices>
                        <Face id="0" plane="-1 0 0 -5" album="Auxiliary" material="NULL" texgens="0 -1 0 960 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 1 -0 -30" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 4 5 2" />
                        </Face>
                        <Face id="2" plane="0 0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 1 0 6" />
                        </Face>
                        <Face id="3" plane="-0 0 1 -0.25" album="street" material="RoadTex_n_diffuse" texgens="-1 0 0 128 0 -1 0 -976 0 1.25 1" texRot="0" texScale="1.25 1" texDiv="256 256">
                            <Indices indices=" 3 2 5 7" />
                        </Face>
                        <Face id="4" plane="-0 -1 -0 5" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 1 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 7 6 0" />
                        </Face>
                        <Face id="5" plane="1 0 0 -5" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                    </Brush>
                    <Brush id="33" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="5 -30 -0.75" />
                            <Vertex pos="-5 -30 -0.75" />
                            <Vertex pos="-5 -30 0.25" />
                            <Vertex pos="5 -30 0.25" />
                            <Vertex pos="-5 -5 -0.75" />
                            <Vertex pos="-5 -5 0.25" />
                            <Vertex pos="5 -5 -0.75" />
                            <Vertex pos="5 -5 0.25" />
                        </Vertices>
                        <Face id="0" plane="0 -1 0 -30" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-1 0 0 -5" album="Auxiliary" material="NULL" texgens="0 -1 0 960 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 4 5 2" />
                        </Face>
                        <Face id="2" plane="0 0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 0 6 4" />
                        </Face>
                        <Face id="3" plane="-0 0 1 -0.25" album="street" material="RoadTex_n_diffuse" texgens="-1 0 0 128 0 -1 0 -976 0 1.25 1" texRot="0" texScale="1.25 1" texDiv="256 256">
                            <Indices indices=" 2 5 7 3" />
                        </Face>
                        <Face id="4" plane="1 0 0 -5" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 0 3 7" />
                        </Face>
                        <Face id="5" plane="-0 1 0 5" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                    </Brush>
                    <Brush id="34" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-5 5 0.25" />
                            <Vertex pos="-5 5 -0.75" />
                            <Vertex pos="5 5 -0.75" />
                            <Vertex pos="5 5 0.25" />
                            <Vertex pos="-5 -5 -0.75" />
                            <Vertex pos="-5 -5 0.25" />
                            <Vertex pos="5 -5 -0.75" />
                            <Vertex pos="5 -5 0.25" />
                        </Vertices>
                        <Face id="0" plane="0 1 0 -5" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 1 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-1 0 0 -5" album="Auxiliary" material="NULL" texgens="0 -1 0 960 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 1 0 5" />
                        </Face>
                        <Face id="2" plane="-0 -0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 2 1 4" />
                        </Face>
                        <Face id="3" plane="-0 0 1 -0.25" album="street" material="RoadTex_j_diffuse" texgens="-1 0 0 128 0 -1 0 128 0 1.25 1.25" texRot="0" texScale="1.25 1.25" texDiv="256 256">
                            <Indices indices=" 5 0 3 7" />
                        </Face>
                        <Face id="4" plane="1 0 -0 -5" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 6 7" />
                        </Face>
                        <Face id="5" plane="0 -1 0 -5" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 8 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                    </Brush>
                    <Brush id="36" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="5 5 0.25" />
                            <Vertex pos="5 30 0.25" />
                            <Vertex pos="5 30 0.75" />
                            <Vertex pos="5 5 0.75" />
                            <Vertex pos="10 30 0.75" />
                            <Vertex pos="10 30 0.25" />
                            <Vertex pos="10 5 0.75" />
                            <Vertex pos="10 5 0.25" />
                        </Vertices>
                        <Face id="0" plane="-1 0 0 5" album="street" material="cement" texgens="0 1 0 976 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-0 1 0 -30" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1 5" />
                        </Face>
                        <Face id="2" plane="0 0 1 -0.75" album="street" material="cement" texgens="1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 2 4 6 3" />
                        </Face>
                        <Face id="3" plane="0 0 -1 0.25" album="Auxiliary" material="NULL" texgens="-1 0 0 160 0 -1 0 -976 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 7" />
                        </Face>
                        <Face id="4" plane="0 -1 0 5" album="street" material="cement" texgens="-1 0 0 1376 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 3 6 7" />
                        </Face>
                        <Face id="5" plane="1 -0 0 -10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                    </Brush>
                    <Brush id="38" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="5 -30 0.25" />
                            <Vertex pos="5 -5 0.25" />
                            <Vertex pos="5 -5 0.75" />
                            <Vertex pos="5 -30 0.75" />
                            <Vertex pos="10 -30 0.75" />
                            <Vertex pos="10 -30 0.25" />
                            <Vertex pos="10 -5 0.75" />
                            <Vertex pos="10 -5 0.25" />
                        </Vertices>
                        <Face id="0" plane="-1 0 0 5" album="street" material="cement" texgens="0 1 0 976 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-0 -1 -0 -30" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 4 5 0" />
                        </Face>
                        <Face id="2" plane="0 0 1 -0.75" album="street" material="cement" texgens="1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 4 3 2 6" />
                        </Face>
                        <Face id="3" plane="0 0 -1 0.25" album="Auxiliary" material="NULL" texgens="-1 0 0 160 0 -1 0 -976 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 5 7 1" />
                        </Face>
                        <Face id="4" plane="0 1 -0 5" album="street" material="cement" texgens="1 0 0 1360 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 1 7 6 2" />
                        </Face>
                        <Face id="5" plane="1 0 0 -10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                    </Brush>
                    <Brush id="39" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="40 5 0.25" />
                            <Vertex pos="40 30 0.25" />
                            <Vertex pos="40 30 -0.75" />
                            <Vertex pos="40 5 -0.75" />
                            <Vertex pos="10 30 -0.75" />
                            <Vertex pos="10 30 0.25" />
                            <Vertex pos="10 5 -0.75" />
                            <Vertex pos="10 5 0.25" />
                        </Vertices>
                        <Face id="0" plane="1 0 0 -40" album="Auxiliary" material="NULL" texgens="0 1 0 960 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 1 -0 -30" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1 5" />
                        </Face>
                        <Face id="2" plane="-0 -0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 4 6" />
                        </Face>
                        <Face id="3" plane="0 0 1 -0.25" album="Auxiliary" material="NULL" texgens="-1 0 0 160 0 -1 0 -976 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 7" />
                        </Face>
                        <Face id="4" plane="-0 -1 -0 5" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 1 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 0 3 6" />
                        </Face>
                        <Face id="5" plane="-1 0 0 10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                    </Brush>
                    <Brush id="40" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="5 5 0.25" />
                            <Vertex pos="5 5 -0.75" />
                            <Vertex pos="5 30 -0.75" />
                            <Vertex pos="5 30 0.25" />
                            <Vertex pos="10 30 -0.75" />
                            <Vertex pos="10 30 0.25" />
                            <Vertex pos="10 5 -0.75" />
                            <Vertex pos="10 5 0.25" />
                        </Vertices>
                        <Face id="0" plane="-1 0 0 5" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 1 -0 -30" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 2 4 5 3" />
                        </Face>
                        <Face id="2" plane="0 0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1 6" />
                        </Face>
                        <Face id="3" plane="0 0 1 -0.25" album="Auxiliary" material="NULL" texgens="-1 0 0 160 0 -1 0 -976 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 5 7 0" />
                        </Face>
                        <Face id="4" plane="-0 -1 -0 5" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 1 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 7 6 1" />
                        </Face>
                        <Face id="5" plane="1 0 0 -10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                    </Brush>
                    <Brush id="41" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="40 -30 -0.75" />
                            <Vertex pos="40 -30 0.25" />
                            <Vertex pos="40 -5 0.25" />
                            <Vertex pos="40 -5 -0.75" />
                            <Vertex pos="10 -30 -0.75" />
                            <Vertex pos="10 -30 0.25" />
                            <Vertex pos="10 -5 -0.75" />
                            <Vertex pos="10 -5 0.25" />
                        </Vertices>
                        <Face id="0" plane="1 -0 0 -40" album="Auxiliary" material="NULL" texgens="0 1 0 960 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 -1 0 -30" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 0 4 5" />
                        </Face>
                        <Face id="2" plane="0 0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 0 3 6" />
                        </Face>
                        <Face id="3" plane="0 -0 1 -0.25" album="Auxiliary" material="NULL" texgens="-1 0 0 160 0 -1 0 -976 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 2 1 5 7" />
                        </Face>
                        <Face id="4" plane="0 1 -0 5" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 3 2 7" />
                        </Face>
                        <Face id="5" plane="-1 0 0 10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                    </Brush>
                    <Brush id="42" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="5 -5 0.25" />
                            <Vertex pos="5 -30 0.25" />
                            <Vertex pos="5 -30 -0.75" />
                            <Vertex pos="5 -5 -0.75" />
                            <Vertex pos="10 -30 -0.75" />
                            <Vertex pos="10 -30 0.25" />
                            <Vertex pos="10 -5 -0.75" />
                            <Vertex pos="10 -5 0.25" />
                        </Vertices>
                        <Face id="0" plane="-1 0 0 5" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 -1 0 -30" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1 5" />
                        </Face>
                        <Face id="2" plane="0 0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 2 4 6 3" />
                        </Face>
                        <Face id="3" plane="0 0 1 -0.25" album="Auxiliary" material="NULL" texgens="-1 0 0 160 0 -1 0 -976 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 7" />
                        </Face>
                        <Face id="4" plane="0 1 0 5" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 3 6 7" />
                        </Face>
                        <Face id="5" plane="1 0 -0 -10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                    </Brush>
                    <Brush id="43" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="40 -5 0.25" />
                            <Vertex pos="40 5 0.25" />
                            <Vertex pos="40 5 -0.75" />
                            <Vertex pos="40 -5 -0.75" />
                            <Vertex pos="10 5 -0.75" />
                            <Vertex pos="10 5 0.25" />
                            <Vertex pos="10 -5 -0.75" />
                            <Vertex pos="10 -5 0.25" />
                        </Vertices>
                        <Face id="0" plane="1 0 0 -40" album="Auxiliary" material="NULL" texgens="0 1 0 960 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 1 -0 -5" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 1 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1 5" />
                        </Face>
                        <Face id="2" plane="-0 -0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 4 6" />
                        </Face>
                        <Face id="3" plane="0 0 1 -0.25" album="street" material="RoadTex_n_diffuse" texgens="-1 0 0 128 0 -1 0 -976 90 1.25 1" texRot="90" texScale="1.25 1" texDiv="256 256">
                            <Indices indices=" 5 1 0 7" />
                        </Face>
                        <Face id="4" plane="-0 -1 -0 -5" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 0 3 6" />
                        </Face>
                        <Face id="5" plane="-1 0 0 10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                    </Brush>
                    <Brush id="44" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="5 5 0.25" />
                            <Vertex pos="5 -5 0.25" />
                            <Vertex pos="5 -5 -0.75" />
                            <Vertex pos="5 5 -0.75" />
                            <Vertex pos="10 5 -0.75" />
                            <Vertex pos="10 5 0.25" />
                            <Vertex pos="10 -5 -0.75" />
                            <Vertex pos="10 -5 0.25" />
                        </Vertices>
                        <Face id="0" plane="-1 0 0 5" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 1 -0 -5" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 1 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 4 5 0" />
                        </Face>
                        <Face id="2" plane="0 0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 3 2 6" />
                        </Face>
                        <Face id="3" plane="0 0 1 -0.25" album="street" material="RoadTex_pedes_diffuse" texgens="-1 0 0 128 0 -1 0 -256 90 1.25 0.625" texRot="90" texScale="1.25 0.625" texDiv="256 256">
                            <Indices indices=" 0 5 7 1" />
                        </Face>
                        <Face id="4" plane="-0 -1 -0 -5" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 7 6 2" />
                        </Face>
                        <Face id="5" plane="1 0 0 -10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                    </Brush>
                    <Brush id="45" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-5 5 0.75" />
                            <Vertex pos="-5 30 0.75" />
                            <Vertex pos="-5 30 -0.75" />
                            <Vertex pos="-5 5 -0.75" />
                            <Vertex pos="-10 30 -0.75" />
                            <Vertex pos="-10 30 0.75" />
                            <Vertex pos="-10 5 0.75" />
                            <Vertex pos="-10 5 -0.75" />
                        </Vertices>
                        <Face id="0" plane="1 0 0 5" album="street" material="cement" texgens="0 -1 0 960 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="0 1 -0 -30" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1 5" />
                        </Face>
                        <Face id="2" plane="0 0 1 -0.75" album="street" material="cement" texgens="1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 5 1 0 6" />
                        </Face>
                        <Face id="3" plane="-0 -0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 4 7" />
                        </Face>
                        <Face id="4" plane="0 -1 0 5" album="street" material="cement" texgens="-1 0 0 1376 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 3 7 6 0" />
                        </Face>
                        <Face id="5" plane="-1 0 0 -10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 4 5 6" />
                        </Face>
                    </Brush>
                    <Brush id="47" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-5 -30 -0.75" />
                            <Vertex pos="-5 -30 0.75" />
                            <Vertex pos="-5 -5 0.75" />
                            <Vertex pos="-5 -5 -0.75" />
                            <Vertex pos="-10 -30 0.75" />
                            <Vertex pos="-10 -30 -0.75" />
                            <Vertex pos="-10 -5 0.75" />
                            <Vertex pos="-10 -5 -0.75" />
                        </Vertices>
                        <Face id="0" plane="1 -0 0 5" album="street" material="cement" texgens="0 -1 0 960 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-0 -1 -0 -30" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 1 0 5" />
                        </Face>
                        <Face id="2" plane="0 0 1 -0.75" album="street" material="cement" texgens="1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 6 2 1 4" />
                        </Face>
                        <Face id="3" plane="0 0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 0 3 7" />
                        </Face>
                        <Face id="4" plane="0 1 0 5" album="street" material="cement" texgens="1 0 0 1360 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 3 2 6 7" />
                        </Face>
                        <Face id="5" plane="-1 -0 -0 -10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                    </Brush>
                    <Brush id="49" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="10 -30 0.75" />
                            <Vertex pos="40 -30 0.75" />
                            <Vertex pos="40 -30 0.25" />
                            <Vertex pos="10 -30 0.25" />
                            <Vertex pos="40 -10 0.75" />
                            <Vertex pos="40 -10 0.25" />
                            <Vertex pos="10 -10 0.75" />
                            <Vertex pos="10 -10 0.25" />
                        </Vertices>
                        <Face id="0" plane="-0 -1 -0 -30" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="1 -0 0 -40" album="Auxiliary" material="NULL" texgens="0 1 0 960 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 2 1 4 5" />
                        </Face>
                        <Face id="2" plane="0 -0 1 -0.75" album="street" material="Floor_set_stone" texgens="1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="128 128">
                            <Indices indices=" 4 1 0 6" />
                        </Face>
                        <Face id="3" plane="0 0 -1 0.25" album="Auxiliary" material="NULL" texgens="-1 0 0 160 0 -1 0 -976 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 2 5 7" />
                        </Face>
                        <Face id="4" plane="-1 0 0 10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 0 3 7" />
                        </Face>
                        <Face id="5" plane="0 1 -0 10" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                    </Brush>
                    <Brush id="50" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="10 -5 0.25" />
                            <Vertex pos="40 -5 0.25" />
                            <Vertex pos="40 -5 0.75" />
                            <Vertex pos="10 -5 0.75" />
                            <Vertex pos="40 -10 0.75" />
                            <Vertex pos="40 -10 0.25" />
                            <Vertex pos="10 -10 0.75" />
                            <Vertex pos="10 -10 0.25" />
                        </Vertices>
                        <Face id="0" plane="0 1 -0 5" album="street" material="cement" texgens="1 0 0 1360 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="1 0 0 -40" album="Auxiliary" material="NULL" texgens="0 1 0 960 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1 5" />
                        </Face>
                        <Face id="2" plane="0 -0 1 -0.75" album="street" material="cement" texgens="1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 2 4 6 3" />
                        </Face>
                        <Face id="3" plane="-0 -0 -1 0.25" album="Auxiliary" material="NULL" texgens="-1 0 0 160 0 -1 0 -976 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 1 0 7" />
                        </Face>
                        <Face id="4" plane="-1 -0 -0 10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 3 6 7" />
                        </Face>
                        <Face id="5" plane="0 -1 0 -10" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                    </Brush>
                    <Brush id="51" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-40 -30 -0.75" />
                            <Vertex pos="-40 -30 0.75" />
                            <Vertex pos="-10 -30 0.75" />
                            <Vertex pos="-10 -30 -0.75" />
                            <Vertex pos="-40 -10 -0.75" />
                            <Vertex pos="-40 -10 0.75" />
                            <Vertex pos="-10 -10 0.75" />
                            <Vertex pos="-10 -10 -0.75" />
                        </Vertices>
                        <Face id="0" plane="0 -1 0 -30" album="Auxiliary" material="NULL" texgens="1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-1 0 0 -40" album="Auxiliary" material="NULL" texgens="0 -1 0 960 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 4 5 1" />
                        </Face>
                        <Face id="2" plane="-0 0 1 -0.75" album="street" material="diban_5" texgens="1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="128 128">
                            <Indices indices=" 1 5 6 2" />
                        </Face>
                        <Face id="3" plane="0 0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 3 7 4" />
                        </Face>
                        <Face id="4" plane="1 0 0 10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 2 6" />
                        </Face>
                        <Face id="5" plane="0 1 0 10" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 5 4" />
                        </Face>
                    </Brush>
                    <Brush id="52" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-10 -5 0.75" />
                            <Vertex pos="-40 -5 0.75" />
                            <Vertex pos="-40 -5 -0.75" />
                            <Vertex pos="-10 -5 -0.75" />
                            <Vertex pos="-40 -10 -0.75" />
                            <Vertex pos="-40 -10 0.75" />
                            <Vertex pos="-10 -10 0.75" />
                            <Vertex pos="-10 -10 -0.75" />
                        </Vertices>
                        <Face id="0" plane="-0 1 0 5" album="street" material="cement" texgens="1 0 0 1360 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-1 0 0 -40" album="Auxiliary" material="NULL" texgens="0 -1 0 960 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 2 1 5" />
                        </Face>
                        <Face id="2" plane="-0 0 1 -0.75" album="street" material="cement" texgens="1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 5 1 0 6" />
                        </Face>
                        <Face id="3" plane="-0 -0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 2 4" />
                        </Face>
                        <Face id="4" plane="1 0 0 10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 7 6 0" />
                        </Face>
                        <Face id="5" plane="0 -1 0 -10" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 4 5 6" />
                        </Face>
                    </Brush>
                    <Brush id="53" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="10 5 0.75" />
                            <Vertex pos="40 5 0.75" />
                            <Vertex pos="40 5 0.25" />
                            <Vertex pos="10 5 0.25" />
                            <Vertex pos="40 10 0.75" />
                            <Vertex pos="40 10 0.25" />
                            <Vertex pos="10 10 0.75" />
                            <Vertex pos="10 10 0.25" />
                        </Vertices>
                        <Face id="0" plane="-0 -1 -0 5" album="street" material="cement" texgens="-1 0 0 1376 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="1 0 0 -40" album="Auxiliary" material="NULL" texgens="0 1 0 960 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 4 5 2" />
                        </Face>
                        <Face id="2" plane="0 -0 1 -0.75" album="street" material="cement" texgens="1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 4 1 0 6" />
                        </Face>
                        <Face id="3" plane="-0 -0 -1 0.25" album="Auxiliary" material="NULL" texgens="-1 0 0 160 0 -1 0 -976 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 2 5 7 3" />
                        </Face>
                        <Face id="4" plane="-1 0 0 10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 3 7 6 0" />
                        </Face>
                        <Face id="5" plane="0 1 -0 -10" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 5 4 6" />
                        </Face>
                    </Brush>
                    <Brush id="54" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="40 30 0.25" />
                            <Vertex pos="40 30 0.75" />
                            <Vertex pos="10 30 0.75" />
                            <Vertex pos="10 30 0.25" />
                            <Vertex pos="40 10 0.75" />
                            <Vertex pos="40 10 0.25" />
                            <Vertex pos="10 10 0.75" />
                            <Vertex pos="10 10 0.25" />
                        </Vertices>
                        <Face id="0" plane="0 1 0 -30" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="1 0 0 -40" album="Auxiliary" material="NULL" texgens="0 1 0 960 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 1 0 5" />
                        </Face>
                        <Face id="2" plane="0 0 1 -0.75" album="street" material="pa025" texgens="1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="256 256">
                            <Indices indices=" 2 1 4 6" />
                        </Face>
                        <Face id="3" plane="-0 -0 -1 0.25" album="Auxiliary" material="NULL" texgens="-1 0 0 160 0 -1 0 -976 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 5 0 3 7" />
                        </Face>
                        <Face id="4" plane="-1 0 0 10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 3 2 6" />
                        </Face>
                        <Face id="5" plane="0 -1 0 10" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 4 5" />
                        </Face>
                    </Brush>
                    <Brush id="55" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-10 5 -0.75" />
                            <Vertex pos="-40 5 -0.75" />
                            <Vertex pos="-40 5 0.75" />
                            <Vertex pos="-10 5 0.75" />
                            <Vertex pos="-40 10 -0.75" />
                            <Vertex pos="-40 10 0.75" />
                            <Vertex pos="-10 10 0.75" />
                            <Vertex pos="-10 10 -0.75" />
                        </Vertices>
                        <Face id="0" plane="0 -1 0 5" album="street" material="cement" texgens="-1 0 0 1376 0 0 -1 8 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-1 0 0 -40" album="Auxiliary" material="NULL" texgens="0 -1 0 960 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 4 5 2" />
                        </Face>
                        <Face id="2" plane="-0 0 1 -0.75" album="street" material="cement" texgens="1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="512 512">
                            <Indices indices=" 2 5 6 3" />
                        </Face>
                        <Face id="3" plane="0 0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 1 0 7" />
                        </Face>
                        <Face id="4" plane="1 -0 0 10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 3 6 7" />
                        </Face>
                        <Face id="5" plane="0 1 0 -10" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 6 5 4" />
                        </Face>
                    </Brush>
                    <Brush id="56" owner="0" type="0" pos="0 0 0.75" rot="1 0 0 0" scale="" transform="1 0 0 0 0 1 0 0 0 0 1 0.75 0 0 0 1" group="-1" locked="0" nextFaceID="7" nextVertexID="9">
                        <Vertices>
                            <Vertex pos="-40 30 -0.75" />
                            <Vertex pos="-10 30 -0.75" />
                            <Vertex pos="-10 30 0.75" />
                            <Vertex pos="-40 30 0.75" />
                            <Vertex pos="-40 10 -0.75" />
                            <Vertex pos="-40 10 0.75" />
                            <Vertex pos="-10 10 0.75" />
                            <Vertex pos="-10 10 -0.75" />
                        </Vertices>
                        <Face id="0" plane="0 1 -0 -30" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 0 1 2 3" />
                        </Face>
                        <Face id="1" plane="-1 0 0 -40" album="Auxiliary" material="NULL" texgens="0 -1 0 960 0 0 -1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 4 0 3 5" />
                        </Face>
                        <Face id="2" plane="-0 0 1 -0.75" album="street" material="Filler_tile1_02" texgens="1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="128 128">
                            <Indices indices=" 5 3 2 6" />
                        </Face>
                        <Face id="3" plane="0 0 -1 -0.75" album="Auxiliary" material="NULL" texgens="-1 0 0 1280 0 -1 0 -960 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 1 0 4 7" />
                        </Face>
                        <Face id="4" plane="1 0 0 10" album="Auxiliary" material="NULL" texgens="0 1 0 1280 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 6 2 1 7" />
                        </Face>
                        <Face id="5" plane="0 -1 0 10" album="Auxiliary" material="NULL" texgens="1 0 0 960 0 0 1 -24 0 1 1" texRot="0" texScale="1 1" texDiv="16 16">
                            <Indices indices=" 7 4 5 6" />
                        </Face>
                    </Brush>
                </Brushes>
            </InteriorMap>
        </DetailLevel>
    </DetailLevels>
</ConstructorScene>
