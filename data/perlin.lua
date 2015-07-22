
-- materials
require('materials')

-- Scene root
scene = gr.node('scene')

radius = 0.7
length = 30

brown = gr.material({0.51953125, 0.3671875, 0.2578125}, {0, 0, 0}, 1)

s = gr.sphere('s')
scene:add_child(s)
s:set_material(brown)
s:set_perlin(1)

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

scene:translate(1, 1, -1)

-- lights
light_color = {0.780131, 0.780409, 0.775833}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}
light_color = {1.0, 1.0, 1.0}

light1 = gr.disc_light({0, 10, -10}, light_color, {1, 0, 0}, {0, -10, 10}, 5)

gr.render(scene,
    'perlin.png', 512, 512,
	  {1.25, 3, -6}, {1, -1.5, 5}, {0, 1, 0}, 50,
	  {0.1,0.1,0.1}, {light1},
    4, 4, 4, 1)

