
require('materials')

-- Scene root
scene = gr.node('scene')

radius = 2.0
height = 2.0

c1 = gr.cylinder('c1')
c1:set_material(green_cornell)
c1:rotate('x', 90)
c1:scale(0.5, 0.5, height)

c2 = gr.cylinder('c2')
c2:set_material(green_cornell)
c2:rotate('y', 90)
c2:scale(0.5, 0.5, height)

csg1 = gr.csg_union('csg1', c1, c2)

c3 = gr.cylinder('c3')
c3:set_material(green_cornell)
c3:scale(0.5, 0.5, height)

csg2 = gr.csg_union('csg2', csg1, c3)

s1 = gr.sphere('s4')
s1:set_material(white_cornell)
s1:scale(0.6, 0.6, 0.6)

b1 = gr.cube('b1')
b1:set_material(red_cornell)
b1:translate(-0.5, -0.5, -0.5)

csg3 = gr.csg_intersection('csg3', s1, b1)
csg3:scale(1.5, 1.5, 1.5)

csg4 = gr.csg_difference('csg4', csg3, csg2)
scene:add_child(csg4)
csg4:translate(0, 2, 0)
csg4:scale(2.0, 2.0, 2.0)

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
left_wall:translate(room_width/2.0-1, -1.0, -room_length/1.5)
left_wall:rotate('z', 90)
left_wall:scale(room_height, 1.0, room_length)

-- right wall
right_wall = gr.plane('right_wall')
room:add_child(right_wall)
right_wall:set_material(red_cornell)
right_wall:translate(-room_width/2.0+1, -1.0, -room_length/1.5)
right_wall:rotate('z', -90)
right_wall:scale(room_height, 1.0, room_length)

-- back wall
back_wall = gr.plane('back_wall')
room:add_child(back_wall)
back_wall:set_material(white_cornell)
back_wall:translate(0.0, -1.0, room_length/6)
back_wall:rotate('x', -90.0)
back_wall:scale(room_width, 1.0, room_height)

-- lights
light_color = {0.780131, 0.780409, 0.775833}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}

-- on ceiling
light1 = gr.light({0, room_height - 3.0, -3}, light_color_2, {1, 0, 0})

-- by camera
light2 = gr.light({-2.0, room_height - 3.0, -3}, light_color_2, {1, 0, 0})

-- Left wall
light3 = gr.light({room_width/2.0-2, room_height-6, -3}, light_color_2, {1, 0, 0})

--sqlight = gr.rect_light({0, room_height - 2.01, -2}, 3, 3, light_color, {1,0,0}, 10)

gr.render(scene,
	  'csg2.png', 400, 400,
	  {0, room_height/2.0, -room_length/2.0}, {0, -room_height/2.0, 30}, {0, 1, 0}, 50,
	  {0.3,0.3,0.3}, {light1, light2}, 4, 2, 1)

