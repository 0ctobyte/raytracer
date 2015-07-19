
-- materials
require('materials')

-- need this to read obj files
require('readobj')

-- Scene root
scene = gr.node('scene')

radius = 0.7
length = 10

--teapot = gr.mesh('teapot', readobj('objs/teapot_n.obj'))
--scene:add_child(teapot)
--teapot:set_material(white_cornell)

--torus = gr.torus('torus')
--scene:add_child(torus)
--torus:set_material(red_cornell)
--torus:scale(radius, radius, radius)
--torus:rotate('y', -20) 
--torus:rotate('x', 70)
--torus:translate(0.5, -1, -1)

floor = gr.plane('floor')
scene:add_child(floor)
floor:set_material(white_cornell)
floor:set_texture('textures/boards02.png')
floor:set_bumpmap('bumps/boards02-bump.png', 0.005)
floor:scale(length, 1.0, length)

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
light1 = gr.disc_light({10, 10, 0}, light_color, {1, 0, 0}, {0, -1, 0}, 2)

gr.render(scene,
	  'mesh.png', 512, 512,
	  {0, 4, -4.5}, {0, -7.871199, 4.612095}, {0, 1, 0}, 50,
	  {0.1,0.1,0.1}, {light1},
    4, 4, 2, 1)

