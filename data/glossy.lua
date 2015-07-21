-- materials
require('materials')

-- Scene root
scene = gr.node('scene')

-- light blue: {0.4, 0.698, 1.0}
mirror0 = gr.material({0, 0, 0}, {1.0, 1.0, 1.0}, 0.1)
mirror1 = gr.material({0, 0, 0}, {1.0, 1.0, 1.0}, 1.0)
mirror2 = gr.material({0, 0, 0}, {1.0, 1.0, 1.0}, 10.0)
mirror3 = gr.material({0, 0, 0}, {1.0, 1.0, 1.0}, 100.0)
mirror4 = gr.material({0, 0, 0}, {1.0, 1.0, 1.0}, 1000.0)

radius = 1.0
height = 3.0

s0 = gr.sphere('s0')
scene:add_child(s0)
s0:set_material(mirror0)
s0:translate(2, radius-1, -8)
s0:scale(radius, radius, radius)

s1 = gr.sphere('s1')
scene:add_child(s1)
s1:set_material(mirror1)
s1:translate(-2, radius-1, -8)
s1:scale(radius, radius, radius)

s2 = gr.sphere('s2')
scene:add_child(s2)
s2:set_material(mirror2)
s2:translate(-2, radius-1, -12)
s2:scale(radius, radius, radius)

s3 = gr.sphere('s3')
scene:add_child(s3)
s3:set_material(mirror4)
s3:translate(2, radius-1, -12)
s3:scale(radius, radius, radius)

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
floor:translate(0.0, -1.0, 0.0)
floor:scale(room_width, 1, room_length)

-- ceiling
ceiling = gr.plane('ceiling')
room:add_child(ceiling)
ceiling:set_material(white_cornell)
ceiling:translate(0.0, room_height-2.0, 0.0)
ceiling:rotate('z', 180.0)
ceiling:scale(room_width, 1, room_length)

-- left wall
left_wall = gr.plane('left_wall')
room:add_child(left_wall)
left_wall:set_material(green_cornell)
left_wall:translate(room_width/2.0-1, room_height/2.0-1, 0.0)
left_wall:rotate('z', 90)
left_wall:scale(room_height, 1.0, room_length)

-- right wall
right_wall = gr.plane('right_wall')
room:add_child(right_wall)
right_wall:set_material(red_cornell)
right_wall:translate(-room_width/2.0+1, room_height/2.0-1.0, 0.0)
right_wall:rotate('z', -90)
right_wall:scale(room_height, 1.0, room_length)

-- back wall
back_wall = gr.plane('back_wall')
room:add_child(back_wall)
back_wall:set_material(white_cornell)
back_wall:translate(0.0, room_height/2.0-1.0, room_length/6)
back_wall:rotate('x', -90.0)
back_wall:scale(room_width, 1.0, room_height)

-- back wall
front_wall = gr.plane('front_wall')
room:add_child(front_wall)
front_wall:set_material(white_cornell)
front_wall:translate(0.0, room_height/2.0-1.0, -room_length/2)
front_wall:rotate('x', 90.0)
front_wall:scale(room_width, 1.0, room_height)

-- lights
light_color = {0.780131, 0.780409, 0.775833}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}

light1 = gr.disc_light({0, room_height - 3.0, -3}, light_color_2, {1, 0, 0}, {0.0, -1.0, 0.0}, 1.0)
light2 = gr.disc_light({-2.0, room_height - 3.0, -3}, light_color_2, {1, 0, 0}, {0.0, -1.0, 0.0}, 1.0)

gr.render(scene,
	  'glossy.png', 512, 512,
	  {0, room_height/2.0, 0}, {0, -room_height, -room_length}, {0, 0, -1}, 50,
	  {0.2,0.2,0.2}, {light1, light2},
    4, 4, 4, 1, 16)

