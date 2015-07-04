
-- materials
require('materials')

-- Scene root
scene = gr.node('scene')

radius = 3.0

--sphere = gr.sphere('s')
--scene:add_child(sphere)
--sphere:set_material(white_cornell)
--sphere:set_texture("textures/earthmap1k.png")
--sphere:translate(0, radius, 0)
--sphere:rotate('x', 90.0)
--sphere:scale(radius, radius, radius)

--radius = 1.5
--cylinder = gr.cylinder('c')
--scene:add_child(cylinder)
--cylinder:set_material(white_cornell)
--cylinder:set_texture("textures/blackmetal.png")
--cylinder:translate(0, radius, 0)
--cylinder:rotate('y', 45)
--cylinder:scale(radius, radius, radius*2)

box = gr.cube('b')
scene:add_child(box)
box:set_material(white_cornell)
box:set_texture("textures/crate2_diffuse.png")
box:translate(0, radius, 0)
box:rotate('y', 45)
box:rotate('x', 45)
box:scale(radius, radius, radius)
box:translate(-0.5, -0.5, -0.5)

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

-- on ceiling
light1 = gr.light({0, room_height - 3.0, -3}, light_color_2, {1, 0, 0})

-- by camera
light2 = gr.light({-2.0, room_height - 3.0, -3}, light_color_2, {1, 0, 0})

--sqlight = gr.rect_light({0, room_height - 2.01, -2}, 3, 3, light_color, {1,0,0}, 10)

gr.render(scene,
	  'texture.png', 512, 512,
	  {0, room_height/2.0, -room_length/2.0}, {0, -room_height/2.0, 30}, {0, 1, 0}, 50,
	  {0.2,0.2,0.2}, {light1, light2},
    4, 4, 2, 1)

