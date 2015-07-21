-- materials
require('materials')

glass = gr.material({0, 0, 0}, {1, 1, 1}, 50000000, 1.52)

-- Scene root
scene = gr.node('scene')

radius = 2.0
height = 3.0

sphere = gr.sphere('sphere')
scene:add_child(sphere)
sphere:set_material(glass)
sphere:translate(8, radius-1, -7)
sphere:scale(radius, radius, radius)

-- Room
room_width = 10.0
room_height = 10.0
room_length = 30.0
room = gr.node('room')
scene:add_child(room)

-- floor
floor = gr.plane('floor')
room:add_child(floor)
floor:set_material(white_cornell)
floor:set_texture('textures/pavement04.png')
floor:set_bumpmap('bumps/pavement04-bump.png', 0.05)
floor:translate(room_width/2.0, -1.0, -room_width/2.0)
floor:scale(room_width*2, 1, room_width*2)

-- right wall
right_wall = gr.plane('right_wall')
room:add_child(right_wall)
right_wall:set_material(red_cornell)
right_wall:set_texture('textures/brickwork.png')
right_wall:set_bumpmap('bumps/brickwork-bump.png', 0.05)
right_wall:translate(-room_width/2.0+1, room_height/2.0-1.0, -room_width/2.0)
right_wall:rotate('y', -90)
right_wall:rotate('x', -90)
right_wall:scale(room_width+5, 1.0, room_width+5)

-- back wall
back_wall = gr.plane('back_wall')
room:add_child(back_wall)
back_wall:set_material(white_cornell)
back_wall:set_texture('textures/brickwork.png')
back_wall:set_bumpmap('bumps/brickwork-bump.png', 0.05)
back_wall:translate(room_width/2.0-3, room_height/2.0-1.0, 0)
back_wall:rotate('x', -90.0)
back_wall:scale(room_width+5, 1.0, room_width+5)

scene:rotate('y', 25.0)

-- lights
light_color = {0.780131, 0.780409, 0.775833}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}

light1 = gr.disc_light({0, room_height - 3.0, -3}, light_color, {1, 0, 0}, {0.0, -1.0, 0.0}, 5.0)
light2 = gr.disc_light({-2.0, room_height - 3.0, -3}, light_color_2, {1, 0, 0}, {0.0, 0.0, 1.0}, 1.0)

gr.render(scene,
	  'refract.png', 512, 512,
	  {0, room_height/2.0-2, -room_length/2.0 - 5}, {0, -room_height/2.0, 30}, {0, 1, 0}, 50,
	  {0.2,0.2,0.2}, {light1},
    4, 4, 4, 1)

