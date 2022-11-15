-- snorlax.lua
-- model for Snorlax


Rackley = gr.material({0.3764706,0.5882353,0.6666667}, {0.2, 0.2, 0.2}, 10)
TealBlue = gr.material({0.1607843,0.4509804,0.5607843}, {0.2, 0.2, 0.2}, 10)
white = gr.material({1,1,1}, {0.2, 0.2, 0.2}, 10)
WhiteChocolate = gr.material({0.9450980,0.8980392,0.8470588}, {0.0, 0.0, 0.0}, 10)
PaleSilver = gr.material({0.8078431,0.7607843,0.7215686}, {0.2, 0.2, 0.2}, 10)
Shadow = gr.material({0.4980392,0.4352941,0.3843137},{0.2, 0.2, 0.2}, 10)


rootNode = gr.node('root')

turso = gr.mesh('sphere', 'turso')
rootNode:add_child(turso)

turso:scale(8,6.5,4)
turso:set_material(TealBlue)
abdomen = gr.mesh('sphere', 'abdomen')
turso:add_child(abdomen)
abdomen:set_material(WhiteChocolate)
abdomen:scale(7,5.5,4.5)
abdomen:translate(0,0.5,0.5)

-- Arm
-- large arm
shoulderr =  gr.joint('shoulderr', {-30,0, 30}, {0, 0, 0})
large_arm_r = gr.mesh('sphere', 'large_arm_r')
large_arm_r:set_material(TealBlue)
large_arm_r:translate(1.2,0,0)
large_arm_r:scale(3,1.5,2)
shoulderr:add_child(large_arm_r)
-- small arm
elbowr = gr.joint('elbowr', {0,0, 0}, {-60, 0, 60})
large_arm_r:add_child(elbowr)
small_arm_r = gr.mesh('sphere', 'small_arm_r')
elbowr:add_child(small_arm_r)
small_arm_r:set_material(TealBlue)
small_arm_r:scale(1,1,1)

-- pow
pow_poly = gr.mesh('cone', 'pow')
pow_poly:set_material(white)
pow_poly:scale(0.2, 0.2, 0.2)
pow_poly:rotate('z', -90)
pow_poly:translate(1.2,0,0)
pow1 = gr.node('pow1')
small_arm_r:add_child(pow1)
pow1:add_child_deepcpy(pow_poly)
pow2 = gr.node('pow2')
small_arm_r:add_child(pow2)
pow2:add_child_deepcpy(pow_poly)
pow2:translate(-0.2, 0, 0.4)
pow3 = gr.node('pow3')
small_arm_r:add_child(pow3)
pow3:add_child_deepcpy(pow_poly)
pow3:translate(-0.4, 0, 0.8)
pow3 = gr.node('pow4')
small_arm_r:add_child(pow3)
pow3:add_child_deepcpy(pow_poly)
pow3:translate(-0.2, 0, -0.4)
small_arm_r:translate(5.6,0,0)

armr = gr.node('armr')
armr:add_child_deepcpy(shoulderr)
armr:scale(1.6,1.3,1.2)
armr:rotate('z', -30)
armr:translate(2,6,0.0)
turso:add_child(armr)

arml = gr.node('arml')
arml:add_child_deepcpy(shoulderr)
arml:scale(1.6,1.3,1.2)
arml:rotate('z', -30)
arml:translate(2,6,0.5)
arml:rotate('y', 190)
turso:add_child(arml)


-- leg part
leg_joint_r = gr.joint('leg_joint_r', {-30, 0, 30}, {0,0,0})
turso:add_child(leg_joint_r)
leg_r = gr.mesh('sphere', 'leg_r')
leg_r:set_material(TealBlue)
leg_r:scale(3.8, 3.8, 2.8)

leg_joint_r:add_child(leg_r)

keen_joint_r = gr.joint('keen_joint_r',{0,0,0}, {0, 20, 60})
leg_r:add_child(keen_joint_r)
small_leg_r = gr.mesh('mcube', 'small_leg_r')
small_leg_r:set_material(WhiteChocolate)
small_leg_r:scale(1,0.3, 1.5)

toe_poly = gr.mesh('cone', 'toe')
toe_poly:set_material(white)
toe_poly:scale(0.2, 0.2, 0.2)
toe_poly:rotate('x', 90)
toe_poly:translate(0,0,3.5)
toe1 = gr.node('toe1')
small_leg_r:add_child(toe1)
toe1:add_child_deepcpy(toe_poly)
toe2 = gr.node('toe2')
small_leg_r:add_child(toe2)
toe2:add_child_deepcpy(toe_poly)
toe2:translate(1, 0, 0)
toe3 = gr.node('toe3')
small_leg_r:add_child(toe3)
toe3:add_child_deepcpy(toe_poly)
toe3:translate(-1, 0, 0)
small_leg_r:translate(0, -1.0, 0)
-- small_leg_r:rotate('x', -60)
keen_joint_r:add_child(small_leg_r)
keen_joint_r:translate(0.5, -2.5, 0)

leg_r:translate(1, -0.8, 0)
leg_joint_r:translate(3.0, -2.0, 0.0)
leg_instance_r = gr.node('leg_instance_r')
leg_instance_r:add_child_deepcpy(leg_joint_r)
turso:add_child(leg_instance_r)


leg_joint_l = gr.joint('leg_joint_l', {-30, 0, 30}, {0,0,0})
turso:add_child(leg_joint_l)
leg_l = gr.mesh('sphere', 'leg_l')
leg_l:set_material(TealBlue)
leg_l:scale(3.8, 3.8, 2.8)

leg_joint_l:add_child(leg_l)

keen_joint_l = gr.joint('keen_joint_l',{0,0,0}, {-60, -20, 0})
leg_l:add_child(keen_joint_l)
small_leg_l = gr.mesh('mcube', 'small_leg_l')
small_leg_l:set_material(WhiteChocolate)
small_leg_l:scale(1,0.3, 1.5)

toe_poly = gr.mesh('cone', 'toe')
toe_poly:set_material(white)
toe_poly:scale(0.2, 0.2, 0.2)
toe_poly:rotate('x', 90)
toe_poly:translate(0,0,3.5)
toe1 = gr.node('toe1')
small_leg_l:add_child(toe1)
toe1:add_child_deepcpy(toe_poly)
toe2 = gr.node('toe2')
small_leg_l:add_child(toe2)
toe2:add_child_deepcpy(toe_poly)
toe2:translate(1, 0, 0)
toe3 = gr.node('toe3')
small_leg_l:add_child(toe3)
toe3:add_child_deepcpy(toe_poly)
toe3:translate(-1, 0, 0)
small_leg_l:translate(0, -1.0, 0)
-- small_leg_l:rotate('x', -60)
keen_joint_l:add_child(small_leg_l)
keen_joint_l:translate(-0.5, -2.5, 0)

leg_l:translate(-1, -0.8, 0)
leg_joint_l:translate(-3.0, -2.0, 0.0)
leg_instance_l = gr.node('leg_instance_l')
leg_instance_l:add_child_deepcpy(leg_joint_l)
turso:add_child(leg_instance_l)


-- head part
neck = gr.joint("neck", {0,0, 0}, {-60, 0, 60})
turso:add_child(neck)
head = gr.mesh('sphere', 'head')
head:scale(4,4,3)
head:set_material(TealBlue)
face = gr.mesh('sphere', 'face')
head:add_child(face)
face:set_material(WhiteChocolate)
face:scale(3.5,3.5,3.5)
face:translate(0,-0.5,0.0)
space = gr.mesh('cube','space')
face:add_child(space)
space:set_material(TealBlue)
space:scale(1.5,0.1,1.5)
space:rotate('y',45)
space:rotate('x',52)
space:translate(0,1.55,2.8)

-- eyes
eye_poly = gr.mesh("cube", "eye")
eye_poly:scale(1.2,0.1, 0.1)
eye1 = gr.node("eyerl")
face:add_child(eye1)
eye2 = gr.node("eyer")
face:add_child(eye2)
eye1:add_child_deepcpy(eye_poly)
eye2:add_child_deepcpy(eye_poly)
eye1:rotate('z', 10)
eye1:translate(-1.2, 0, 3.4)
eye2:rotate('z', -10)
eye2:translate(1.2, 0, 3.4)

-- mouth
mouth = gr.mesh("cube", "mouth")
mouth:scale(1.8, 0.1, 0.1)
face:add_child(mouth)


-- teeth
tooth_poly = gr.mesh("cone", "tooth_poly")
tooth_poly:set_material(white)
tooth_poly:scale(0.15, 0.3, 0.01)
toothl = gr.node("toothl")
toothl:add_child_deepcpy(tooth_poly)
toothl:translate(-0.95, 0.21, 0.05)
toothr = gr.node("toothr")
toothr:add_child_deepcpy(tooth_poly)
toothr:translate(0.95, 0.21, 0.05)
mouth:add_child(toothl)
mouth:add_child(toothr)
mouth:translate(0,-1.5,3.5)

-- ears
ear_poly = gr.mesh("cone", "ear")
ear_poly:scale(1.5,1,1)
ear_poly:rotate('x', 10)
ear_poly:set_material(TealBlue)
ear1 = gr.node("earl")
ear2 = gr.node("earr")
ear1:add_child_deepcpy(ear_poly)
ear1:rotate('z', -30)
ear1:translate(2.3,3.2,1)

ear2:add_child_deepcpy(ear_poly)
ear2:rotate('z', 30)
ear2:translate(-2.3,3.2,1)
head:add_child(ear1)
head:add_child(ear2)
head:scale(1.2, 1,1)
head:rotate('x', -10)


neck:add_child(head)
neck:translate(0,8,0)


-- Body part





rootNode:translate(0, -2, -10.0)

return rootNode


