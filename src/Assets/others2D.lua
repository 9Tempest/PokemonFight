
green = gr.material({0,1,0}, {0.1, 0.1, 0.1}, 10)
red = gr.material({1,0,0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.1,0.1,0.1}, {0.1, 0.1, 0.1}, 10)
SnorlaxText = gr.texture("snorlax_Icon.png")
PikachuText = gr.texture("pikachu_avatar.png")
rootnode = gr.node("root")

--avatar snorlax
avatar_snorlax = gr.mesh("plane_text", "ava_snorlax")
avatar_snorlax:set_material(green)
avatar_snorlax:rotate('x',90)
avatar_snorlax:rotate('z',180)
avatar_snorlax:scale(1,1,1)
avatar_snorlax:set_texture(SnorlaxText)

rootnode:add_child(avatar_snorlax)

--avatar pikachu
avatar_pikachu = gr.mesh("plane_text", "ava_pika")
avatar_pikachu:set_material(green)
avatar_pikachu:rotate('x',90)
avatar_pikachu:rotate('z',180)
avatar_pikachu:scale(1.5,1.5,1.5)
avatar_pikachu:translate(0, -2.5,0)

avatar_pikachu:set_texture(PikachuText)

rootnode:add_child(avatar_pikachu)

-- snorlax hp
hp_bar_snorlax = gr.mesh("plane_text", "hp_bar_Snorlax")
hp_bar_snorlax:scale(5, 0.5, 0.5)
hp_bar_snorlax:rotate('x',90)
hp_bar_snorlax:translate(5, 0, 0)
hp_bar_snorlax:set_material(red)
rootnode:add_child(hp_bar_snorlax)

hp_bar_blank = gr.mesh("plane_text", "hp_bar_blank1")
hp_bar_blank:scale(5, 0.5, 0.5)
hp_bar_blank:rotate('x',90)
hp_bar_blank:translate(5, 0, -0.01)
hp_bar_blank:set_material(black)
rootnode:add_child(hp_bar_blank)

-- pikachu hp
hp_bar_pikachu = gr.mesh("plane_text", "hp_bar_Pikachu")
hp_bar_pikachu:scale(5, 0.5, 0.5)
hp_bar_pikachu:rotate('x',90)
hp_bar_pikachu:translate(5, -2.5, 0)
hp_bar_pikachu:set_material(green)
rootnode:add_child(hp_bar_pikachu)

hp_bar_blank = gr.mesh("plane_text", "hp_bar_blank2")
hp_bar_blank:scale(5, 0.5, 0.5)
hp_bar_blank:rotate('x',90)
hp_bar_blank:translate(5, -2.5, -0.01)
hp_bar_blank:set_material(black)
rootnode:add_child(hp_bar_blank)

rootnode:translate(-14,10.5,0)
return rootnode
