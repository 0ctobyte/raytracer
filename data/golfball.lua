-- materials
require('materials')

white = gr.material({0.7, 0.7, 0.7}, {1.0, 1.0, 1.0}, 32)

-- Scene root
scene = gr.node('scene')

radius = 3.0
height = 3.0

sphere = gr.sphere('s')
scene:add_child(sphere)
sphere:set_material(white)
sphere:set_bumpmap('bumps/golfball_bump.png')
sphere:translate(0, -radius, 0)
sphere:scale(radius, radius, radius)

-- lights
light_color = {0.780131, 0.780409, 0.775833}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}
light_color_3 = {1.0, 1.0, 1.0}

-- on ceiling
light1 = gr.light({5, 5, -5}, light_color, {1, 0, 0})
light1 = gr.light({7, 4, -15}, light_color_3, {1, 0, 0})

gr.render(scene,
	  'golfball.png', 512, 512,
	  {0, 0, -15}, {0, 0, 1}, {0, 1, 0}, 50,
	  {0.3,0.3,0.3}, {light1},
    4, 4, 2, 1, 'backgrounds/sky.png')

