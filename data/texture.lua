
-- materials
require('materials')

-- Scene root
scene = gr.node('scene')

radius = 4.0

sphere = gr.sphere('s')
scene:add_child(sphere)
sphere:set_material(white_cornell)
sphere:set_texture("textures/earthmap1k.png")
sphere:set_bumpmap('bumps/earthbump1k.png', 0.05)
sphere:translate(0, 0, 15)
sphere:rotate('x', 90.0)
sphere:scale(radius, radius, radius)

-- lights
light_color = {0.780131, 0.780409, 0.775833}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}
light_color = {1.0, 1.0, 1.0}

light1 = gr.disc_light({10, 10, 0}, light_color, {1, 0, 0}, {-10, -10, 15}, 1)
light2 = gr.disc_light({10, 10, 0}, light_color, {1, 0, 0}, {-10, -10, 15}, 1)

gr.render(scene,
	  'texture.png', 512, 512,
	  {0, 0, -1}, {0, 0, 1}, {0, 1, 0}, 50,
	  {0.1,0.1,0.1}, {light1, light2},
    4, 4, 4, 1, 1, 'backgrounds/starry.sky.png')

-- widescreen    
--gr.render(scene,
--	  'texture.2048.512.png', 2048, 512,
--	  {0, 0, -1}, {0, 0, 1}, {0, 1, 0}, 30,
--	  {0.1,0.1,0.1}, {light1, light2},
--    4, 4, 4, 1, 1, 'backgrounds/starry.sky.png')

