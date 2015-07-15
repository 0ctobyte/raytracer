
-- materials
require('materials')

-- Scene root
scene = gr.node('scene')

radius = 3.0

sphere = gr.sphere('s')
scene:add_child(sphere)
sphere:set_material(white_cornell)
sphere:set_texture("textures/earthmap1k.png")
sphere:translate(0, 0, 15)
sphere:rotate('x', 90.0)
sphere:scale(radius, radius, radius)

-- lights
light_color = {0.780131, 0.780409, 0.775833}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}

-- on ceiling
light1 = gr.light({50, 0, 15}, light_color, {1, 0, 0})

gr.render(scene,
	  'texture.png', 512, 512,
	  {0, 0, -1}, {0, 0, 1}, {0, 1, 0}, 50,
	  {0.2,0.2,0.2}, {light1, light2},
    4, 4, 2, 1, 'backgrounds/starry.sky.png')

