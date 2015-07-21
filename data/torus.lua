
-- materials
require('materials')

-- Scene root
scene = gr.node('scene')

radius = 0.7
length = 30

torus = gr.torus('torus')
scene:add_child(torus)
torus:set_material(red_cornell)
torus:scale(radius, radius, radius)
torus:rotate('y', -20) 
torus:rotate('x', 70)
torus:translate(0.5, -1, -1)

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

light1 = gr.disc_light({10, 10, 0}, light_color, {1, 0, 0}, {-9.5, -11, -1}, 5)

aa = 4
ss = 1
gr.render(scene,
	  'torus.' .. tostring(aa) .. '.' .. tostring(ss) .. '.png', 512, 512,
	  {0, 2, -5}, {0, -3.011505, 5.56155}, {0, 1, 0}, 50,
	  {0.1,0.1,0.1}, {light1},
    4, 4, aa, ss)

