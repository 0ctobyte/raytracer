-- materials
require('materials')

-- Scene root
scene = gr.node('scene')

radius = 2.0
height = 3.0

box1 = gr.cube('b1')
scene:add_child(box1)
box1:set_material(white_cornell)
box1:set_texture('textures/crate01.png')
box1:set_bumpmap('bumps/crate01-bump.png')
box1:translate(0, 0.5, 0)
box1:scale(height, height, height)
box1:rotate('y', 45.0)
box1:translate(-0.5, -0.5, -0.5)

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
floor:set_bumpmap('bumps/pavement04-bump.png')
floor:translate(0.0, -1.0, 0.0)
floor:scale(room_width+4, 1, room_width+4)

-- ceiling
--ceiling = gr.plane('ceiling')
--room:add_child(ceiling)
--ceiling:set_material(white_cornell)
--ceiling:set_texture('textures/ceiling.png')
--ceiling:translate(0.0, room_height-2.0, 0.0)
--ceiling:rotate('z', 180.0)
--ceiling:scale(room_width, 1, room_width)

-- left wall
--left_wall = gr.plane('left_wall')
--room:add_child(left_wall)
--left_wall:set_material(green_cornell)
--left_wall:translate(room_width/2.0-1, room_height/2.0-1, 0.0)
--left_wall:rotate('z', 90)
--left_wall:scale(room_height, 1.0, room_length)

-- right wall
right_wall = gr.plane('right_wall')
room:add_child(right_wall)
right_wall:set_material(red_cornell)
right_wall:set_texture('textures/brickwork.png')
right_wall:set_bumpmap('bumps/brickwork-bump.png')
right_wall:translate(-room_width/2.0+1, room_height/2.0-1.0, 0.0)
right_wall:rotate('x', 90)
right_wall:rotate('z', -90)
right_wall:scale(room_width+4, 1.0, room_width+4)

-- back wall
back_wall = gr.plane('back_wall')
room:add_child(back_wall)
back_wall:set_material(white_cornell)
back_wall:set_texture('textures/brickwork.png')
back_wall:set_bumpmap('bumps/brickwork-bump.png')
back_wall:translate(0.0, room_height/2.0-1.0, room_length/6)
back_wall:rotate('x', -90.0)
back_wall:scale(room_width, 1.0, room_width)

scene:rotate('y', 45.0)

-- lights
light_color = {0.780131, 0.780409, 0.775833}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}

-- on ceiling
light1 = gr.disc_light({0, 1.0, -7.0}, light_color, {1, 0, 0}, {0.0, -1.0, 0.0}, 5.0)

-- by camera
--light2 = gr.disc_light({-2.0, room_height - 3.0, -3}, light_color_2, {1, 0, 0}, {0.0, 0.0, 1.0}, 1.0)

--sqlight = gr.rect_light({0, room_height - 2.01, -2}, 3, 3, light_color, {1,0,0}, 10)

gr.render(scene,
	  'bump.png', 512, 512,
	  {0, room_height/2.0-2, -room_length/2.0 + 5}, {0, -room_height/2.0, 30}, {0, 1, 0}, 50,
	  {0.2,0.2,0.2}, {light1},
    4, 4, 2, 1)

