white = gr.material({1,1,1}, {0.1, 0.1, 0.1}, 10)
green = gr.material({1,1,1}, {0.1, 1.0, 0.1}, 25)
Dirt = gr.material({0.6078431, 0.4627451, 0.3254902}, {0.2, 0.2, 0.2}, 10)
DirtTexture = gr.texture("dirt.png")
GrassTexture = gr.texture("grass.png")
FenceTexture = gr.texture("fence.png")
-- set skybox
rootnode = gr.node("root")

skybox = gr.skybox("skybox3.png")



DirtParticle = gr.mesh('cube_text', 'dirt')
DirtParticle:set_material(Dirt)
DirtParticle:set_particle()
DirtParticle:set_texture(DirtTexture)

lightning = gr.mesh('cube', 'lightning')
lightning:set_material(white)
lightning:scale(3.5, 0.5, 0.5)
lightning:set_particle()

tile_grass = gr.mesh('plane_text', 'tile_grass')
tile_grass:scale(10,10,10)
tile_grass:rotate('y', 180)
tile_grass:translate(-80, 0, -50)
tile_grass:set_material(green)
tile_grass:set_texture(GrassTexture)

for i = 0, 15 do
    for j = 0, 6 do
        tile = gr.node('tile' .. tostring(i) .. tostring(j))
        tile:add_child_deepcpy(tile_grass)
        tile:translate(i*10, 0, j*10)
        rootnode:add_child(tile)
    end
 end



-- rootnode:add_child(tile_grass)

tile_mud = gr.mesh('plane_text', 'tile_mud')
tile_mud:scale(5,5,5)
tile_mud:translate(-35, 0.01, -50)
tile_mud:set_material(white)
tile_mud:set_texture(DirtTexture)

for i = 0, 15 do
    for j = 7, 8 do
        tile = gr.node('tilemud' .. tostring(i) .. tostring(j))
        tile:add_child_deepcpy(tile_mud)
        tile:translate(i*5, 0, j*5)
        rootnode:add_child(tile)
    end
 end

-- rootnode:add_child(tile_mud)


fence = gr.mesh('cube_text', 'fence_body')
fence:set_material(white)
fence:set_texture(FenceTexture)
fence:scale(5, 30, 2)
fence_top = gr.mesh('cube_text', 'fence_head')
fence_top:set_material(white)
fence_top:set_texture(FenceTexture)
fence_top:scale(3.5, 3.5, 1)
fence_top:rotate('z', 45)
fence_top:translate(0, 15, 0)
fence_connect1 = gr.mesh('cube_text', 'fence_conn1')
fence_connect1:set_material(white)
fence_connect1:set_texture(FenceTexture)
fence_connect1:scale(10, 1, 1)
fence_connect1:translate(0, 6, 0)
fence_connect2 = gr.mesh('cube_text', 'fence_conn1')
fence_connect2:set_material(white)
fence_connect2:set_texture(FenceTexture)
fence_connect2:scale(10, 1, 1)
fence_connect2:translate(0, 12, 0)
fence:add_child(fence_connect1)
fence:add_child(fence_connect2)
fence:add_child(fence_top)
fence:translate(-80, 0, -50)
for i = 0, 15 do
    fence_instance = gr.node('fence' .. tostring(i))
    fence_instance:add_child_deepcpy(fence)
    fence_instance:translate(i*10, 0, 0)
    rootnode:add_child(fence_instance)
 end

rootnode:translate(0,-8,-10.0)

return rootnode

