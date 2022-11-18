-- pikachu.lua
-- model for pikachu

white = gr.material({1,1,1}, {0.1, 0.1, 0.1}, 10)
yellow = gr.material({0.9568627,0.8627451,0.1490196}, {0.0, 0.0, 0.0}, 5)
red = gr.material({0.9137255,0.1607843,0.1607843}, {0.0, 0.0, 0.0}, 5)
black = gr.material({0.0,0.0,0.0}, {0.5, 0.5, 0.5}, 20)
brown = gr.material({0.3607843,0.2117647,0.0745098}, {0.0, 0.0, 0.0}, 5)
yellow2 = gr.material({1.0000000,0.9019608,0.1764706}, {0.0, 0.0, 0.0}, 5)

rootnode = gr.node('root')


-- construct body
turso = gr.mesh('mcube', 'turso')
rootnode:add_child(turso)
turso:set_material(yellow2)
turso:scale(1.3, 1.7, 1)

-- construct neck and head
neck = gr.joint("neck", {0,0, 0}, {-60, 0, 60})
turso:add_child(neck)
head = gr.mesh('sphere', 'head')
head:set_material(yellow2)
head:scale(3.5,3,3)
neck:add_child(head)


-- construct head components

--ear right
joint_earlhead1 = gr.joint('e2h1', {-30,0, 10}, {0, 0, 0})
earr = gr.mesh('sphere', 'earr')
earr:set_material(yellow2)
earr:scale(3, 0.8, 0.5)
earr:rotate('z', 30)
joint_earlhead1:add_child(earr)
head:add_child(joint_earlhead1)

--ear left
joint_earlhead2 = gr.joint('e2h2', {-10,0, 30}, {0, 0, 0})
earl = gr.mesh('sphere', 'earl')
earl:set_material(yellow2)
earl:scale(3, 0.8, 0.5)
earl:rotate('z', -30)
joint_earlhead2:add_child(earl)
head:add_child(joint_earlhead2)

earr:translate(2.5,3,0)
earl:translate(-2.5,3,0)


--cheek right
cheekr = gr.mesh('sphere', 'cheekr')
cheekr:set_material(red)
cheekr:translate(2.1,-0.5,1.5)
head:add_child(cheekr)

--cheek left
cheekl = gr.mesh('sphere', 'cheekl')
cheekl:set_material(red)
cheekl:translate(-2.1,-0.5,1.5)
head:add_child(cheekl)

-- eye right
eyer = gr.mesh('sphere', 'eyer')
eyer:set_material(black)
white_ballr = gr.mesh('sphere', 'white_balll')
white_ballr:set_material(white)
white_ballr:scale(0.5, 0.5, 0.5)
white_ballr:translate(-0.1,0,0.6)
eyer:add_child(white_ballr)
eyer:scale(0.5,0.5,0.5)
eyer:translate(1.4,0.5,2.5)
head:add_child(eyer)

-- eye left
eyel = gr.mesh('sphere', 'eyel')
eyel:set_material(black)
white_balll = gr.mesh('sphere', 'white_balll')
white_balll:set_material(white)
white_balll:scale(0.5, 0.5, 0.5)
white_balll:translate(0.1,0,0.6)
eyel:add_child(white_balll)
eyel:scale(0.5,0.5,0.5)
eyel:translate(-1.4,0.5,2.5)
head:add_child(eyel)

--nose
nose = gr.mesh('cone', 'nose')
nose:set_material(black)
nose:rotate('z', 180)
nose:scale(0.1, 0.1, 0.1)
nose:translate(0,-0.3,3)
head:add_child(nose)

--mouth
mouth1 = gr.mesh('torus', 'mouth1')
mouth1:set_material(black)
mouth1:scale(0.4,1,0.4)
mouth1:rotate('x', 60)
mouth1:translate(-0.32,-0.6,2.7)
head:add_child(mouth1)
mouth1 = gr.mesh('torus', 'mouth2')
mouth1:set_material(black)
mouth1:scale(0.4,1,0.4)
mouth1:rotate('x', 60)
mouth1:translate(0.32,-0.6,2.7)
head:add_child(mouth1)

-- todo: mouth
neck:translate(0, 5.5, 0)

-- large arm
shoulderr =  gr.joint('shoulderr', {-30,0, 30}, {0, 0, 0})
turso:add_child(shoulderr)
large_arm_r = gr.mesh('sphere', 'large_arm_r')
large_arm_r:set_material(yellow2)
large_arm_r:translate(1.2,0,0)
large_arm_r:scale(2,1,1)
shoulderr:add_child(large_arm_r)
-- small arm
elbowr = gr.joint('elbowr', {0,0, 0}, {-60, 0, 60})
large_arm_r:add_child(elbowr)
small_arm_r = gr.mesh('sphere', 'small_arm_r')
small_arm_r:set_material(yellow2)
small_arm_r:translate(0.8,0,0)
small_arm_r:scale(0.5,0.5,0.5)

-- fingers
right_finger1_joint = gr.joint('f1_jointr', {0,0, 0}, {-10, 0, 30})
small_arm_r:add_child(right_finger1_joint)
right_finger1 = gr.mesh('sphere', 'rfinger1')
right_finger1_joint:add_child(right_finger1)
right_finger1:set_material(yellow2)
right_finger1:scale(0.1, 0.1, 0.1)
right_finger1:translate(0.95, 0, 0)

right_finger2_joint = gr.joint('f2_jointr', {0,0, 0}, {-10, 0, 30})
small_arm_r:add_child(right_finger2_joint)
right_finger2 = gr.mesh('sphere', 'rfinger2')
right_finger2_joint:add_child(right_finger2)
right_finger2:set_material(yellow2)
right_finger2:scale(0.1, 0.1, 0.1)
right_finger2:translate(0.9, 0.2, 0)

right_finger3_joint = gr.joint('f3_jointr', {0,0, 0}, {-10, 0, 30})
small_arm_r:add_child(right_finger3_joint)
right_finger3 = gr.mesh('sphere', 'rfinger3')
right_finger3_joint:add_child(right_finger3)
right_finger3:set_material(yellow2)
right_finger3:scale(0.1, 0.1, 0.1)
right_finger3:translate(0.9, -0.2, 0)

elbowr:add_child(small_arm_r)
elbowr:translate(3.8,0,0)
shoulderr:translate(1, 2.5, 0.5)

-- large arm left
shoulderl =  gr.joint('shoulderl', {-30,0, 30}, {0, 0, 0})
turso:add_child(shoulderl)
large_arm_l = gr.mesh('sphere', 'large_arm_l')
large_arm_l:set_material(yellow2)
large_arm_l:translate(-1.2,0,0)
large_arm_l:scale(2,1,1)
shoulderl:add_child(large_arm_l)
-- small arm
elbowl = gr.joint('elbowr', {0,0, 0}, {-30, 0, 60})
large_arm_l:add_child(elbowl)
small_arm_l = gr.mesh('sphere', 'small_arm_r')
small_arm_l:set_material(yellow2)
small_arm_l:translate(-0.8,0,0)
small_arm_l:scale(0.5,0.5,0.5)

-- fingers
left_finger1_joint = gr.joint('f1_joint', {0,0, 0}, {-10, 0, 30})
small_arm_l:add_child(left_finger1_joint)
left_finger1 = gr.mesh('sphere', 'lfinger1')
left_finger1_joint:add_child(left_finger1)
left_finger1:set_material(yellow2)
left_finger1:scale(0.1, 0.1, 0.1)
left_finger1:translate(-0.95, 0, 0)

left_finger2_joint = gr.joint('f2_joint', {0,0, 0}, {-10, 0, 30})
small_arm_l:add_child(left_finger2_joint)
left_finger2 = gr.mesh('sphere', 'lfinger2')
left_finger2_joint:add_child(left_finger2)
left_finger2:set_material(yellow2)
left_finger2:scale(0.1, 0.1, 0.1)
left_finger2:translate(-0.9, 0.2, 0)

left_finger3_joint = gr.joint('f3_joint', {0,0, 0}, {-10, 0, 30})
small_arm_l:add_child(left_finger3_joint)
left_finger3 = gr.mesh('sphere', 'lfinger3')
left_finger3_joint:add_child(left_finger3)
left_finger3:set_material(yellow2)
left_finger3:scale(0.1, 0.1, 0.1)
left_finger3:translate(-0.9, -0.2, 0)
elbowl:add_child(small_arm_l)
elbowl:translate(-3.8,0,0)
shoulderl:translate(-1, 2.5, 0.5)


-- large leg left
leg_joint_l = gr.joint('leg_joint_l',{-30, 0, 30},{0,0,0})
turso:add_child(leg_joint_l)
leg_l = gr.mesh('sphere', 'leg_l')
leg_l:set_material(yellow2)
leg_l:scale(1.8, 1.8, 1.8)
leg_joint_l:add_child(leg_l)
keen_joint_l = gr.joint('keen_joint_l',{0,0,0}, {-60, -30, 0})
leg_l:add_child(keen_joint_l)
small_leg_l = gr.mesh('sphere', 'small_leg_l')
small_leg_l:set_material(yellow2)
small_leg_l:scale(0.5,1.5, 0.5)
small_leg_l:rotate('x', -60)

--foot_joint_l = gr.joint('foot_joint_l',{0,0,0}, {-30, 30, 60})
-- foot_l = gr.mesh('sphere', 'foot_l')
-- foot_l:set_material(yellow2)
-- foot_l:scale(1, 0.2, 0.5)
-- foot_joint_l:add_child(foot_l)
-- foot_joint_l:translate(0, -0.5, 0)
-- small_leg_l:add_child(foot_joint_l)
keen_joint_l:add_child(small_leg_l)
keen_joint_l:translate(0, -2, 0)
leg_l:translate(-1, -1.0, 0)
leg_joint_l:translate(-0.8, -1.5, 0.5)


-- large leg right
leg_joint_r = gr.joint('leg_joint_r', {-30, 0, 30}, {0,0,0})
turso:add_child(leg_joint_r)
leg_r = gr.mesh('sphere', 'leg_r')
leg_r:set_material(yellow2)
leg_r:scale(1.8, 1.8, 1.8)

leg_joint_r:add_child(leg_r)

keen_joint_r = gr.joint('keen_joint_r',{0,0,0}, {0, 30, 60})
leg_r:add_child(keen_joint_r)
small_leg_r = gr.mesh('sphere', 'small_leg_r')
small_leg_r:set_material(yellow2)
small_leg_r:scale(0.5,1.5, 0.5)
small_leg_r:rotate('x', -60)
keen_joint_r:add_child(small_leg_r)
keen_joint_r:translate(0, -2, 0)

leg_r:translate(1, -1, 0)
leg_joint_r:translate(0.8, -1.5, 0.5)


-- tail
tail_joint = gr.joint('tail_joint',{-30,0,30}, {0, 0, 0})
turso:add_child(tail_joint)
tail1 = gr.mesh('cube', 'tail1')
tail_joint:add_child(tail1)
tail1:set_material(brown)
tail1:scale(5,1,0.2)
tail1:rotate('z', 20)

tail2 = gr.mesh('cube', 'tail2')
tail1:add_child(tail2)
tail2:set_material(brown)
tail2:scale(1.3,5,0.2)
tail2:rotate('z', 20)


tail3 = gr.mesh('cube', 'tail3')
tail2:add_child(tail3)
tail3:set_material(brown)
tail3:scale(5,1.5,0.2)
tail3:rotate('z', 20)

tail4 = gr.mesh('cube', 'tail4')
tail3:add_child(tail4)
tail4:set_material(yellow2)
tail4:scale(1.8,5,0.2)
tail4:rotate('z', 20)

tail5 = gr.mesh('cube', 'tail5')
tail4:add_child(tail5)
tail5:set_material(yellow2)
tail5:scale(6,2,0.2)
tail5:rotate('z', 20)

tail6 = gr.mesh('cube', 'tail6')
tail5:add_child(tail6)
tail6:set_material(yellow2)
tail6:scale(2.5,9,0.2)
tail6:rotate('z', 20)

tail7 = gr.mesh('cube', 'tail7')
tail6:add_child(tail7)
tail7:set_material(yellow2)
tail7:rotate('z', 20)
tail7:scale(10,3,0.2)
tail7:rotate('x', 180)
tail7:rotate('z', 30)


tail7:translate(2.35,7.6,0)
tail6:translate(1.5,4.25,0)
tail5:translate(1.5, 2.2, 0)
tail4:translate(0.9, 2.2, 0.05)
tail3:translate(0.95, 2.8, 0)
tail2:translate(1.8, 2.8, 0)
tail1:translate(1, 0, 0)

tail_joint:scale(0.7,0.7,0.7)
tail_joint:rotate('x', -10)
tail_joint:rotate('z', -30)
tail_joint:translate(0, -2, -1.8)

rootnode:translate(10, -2, -10.0)

return rootnode