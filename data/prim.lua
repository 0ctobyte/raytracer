-- materials
require('materials')

-- Scene root
scene = gr.node('scene')

radius = 2.0
height = 3.0

sphere = gr.sphere('s')
scene:add_child(sphere)
sphere:set_material(mirror)
sphere:translate(-2,radius - 1,0)
sphere:scale(radius, radius, radius)

cylinder = gr.cylinder('c')
scene:add_child(cylinder)
cylinder:set_material(white_cornell)
cylinder:translate(2, 0, 0)
cylinder:rotate('x', 90.0)
cylinder:scale(radius, radius, height)

torus = gr.torus('t')
scene:add_child(torus)
torus:set_material(red_cornell)
torus:translate(1, 0, -3)
torus:rotate('x', 50.0)
torus:rotate('y', 20.0)

cone = gr.cone('cone')
scene:add_child(cone)
cone:set_material(green_cornell)
cone:translate(3, 1.6, 0)
cone:rotate('x', 60)
cone:rotate('y', 60.0)
cone:scale(1.0, 1.0, 2.0)

disc = gr.disc('disc')
scene:add_child(disc)
disc:set_material(mirror)
disc:translate(-3.0, 7.0, 0.0)
disc:rotate('y', -45.0)
disc:rotate('x', -45.0)

-- Room
room_width = 10.0
room_height = 10.0
room_length = 30.0
room = gr.node('room')
scene:add_child(room)

-- floor
floor = gr.plane('floor')
room:add_child(floor)
floor:set_material(white_cornell_shiny)
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

-- lights
light_color = {0.780131, 0.780409, 0.775833}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}

light1 = gr.disc_light({0, room_height - 3.0, -3}, light_color_2, {1, 0, 0}, {0.0, -1.0, 0.0}, 1.0)
light2 = gr.disc_light({-2.0, room_height - 3.0, -3}, light_color_2, {1, 0, 0}, {0.0, -1.0, 0.0}, 1.0)

gr.render(scene,
	  'prim.png', 512, 512,
	  {0, room_height/2.0, -room_length/2.0}, {0, -room_height/2.0, 30}, {0, 1, 0}, 50,
	  {0.2,0.2,0.2}, {light1, light2},
    4, 4, 4, 1)

