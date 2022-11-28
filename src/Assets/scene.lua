white = gr.material({1,1,1}, {0.1, 0.1, 0.1}, 10)
Dirt = gr.material({0.6078431, 0.4627451, 0.3254902}, {0.2, 0.2, 0.2}, 10)
DirtTexture = gr.texture("dirt.png")

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



return rootnode

