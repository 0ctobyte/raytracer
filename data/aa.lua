
-- materials
require('materials')

-- Scene root
scene = gr.node('scene')

radius = 0.7
length = 30

b1 = gr.cube('b1')
b1:set_material(red_cornell)
b1:translate(-0.5, -0.5, -0.5)

s1 = gr.sphere('s1')
s1:set_material(green_cornell)
s1:scale(0.6, 0.6, 0.6)

csg = gr.csg_union('csg', b1, s1)
scene:add_child(csg)
csg:translate(0, -0.2, 0)
csg:rotate('y', 45.0)
csg:scale(1.5, 1.5, 1.5)

floor = gr.plane('floor')
scene:add_child(floor)
floor:set_material(white_cornell)
floor:scale(length, 1.0, length)
floor:translate(0, -1, 0)

backwall = gr.plane('backwall')
scene:add_child(backwall)
backwall:set_material(white_cornell)
backwall:translate(0, 0, 2)
backwall:rotate('x', -70)
backwall:scale(length, 1.0, length)

-- lights
light_color = {0.780131, 0.780409, 0.775833}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}
light_color = {1.0, 1.0, 1.0}

-- on ceiling
light1 = gr.disc_light({5, 10, -10}, light_color, {1, 0, 0}, {-9.5, -11, -1}, 5)

aa = 4
ss = 1
gr.render(scene,
	  'aa.' .. tostring(aa) .. '.' .. tostring(ss) .. '.png', 512, 512,
	  {0, 2, -5}, {0, -3.011505, 5.56155}, {0, 1, 0}, 50,
	  {0.1,0.1,0.1}, {light1},
    4, 4, aa, ss)

