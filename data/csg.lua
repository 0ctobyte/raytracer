
-- materials
--stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
--grass = gr.material({0.1, 0.0, 0.3}, {0.0, 0.0, 0.0}, 0)
--ground = gr.material({0.3, 0.3, 0.3}, {0.0, 0.0, 0.0}, 0)
--hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)
--glass = gr.material({1.0,1.0,1.0}, {0.1, 0.1, 0.1}, 50)
--shirt = gr.material({0.9,0.9,0.9}, {0, 0, 0}, 2)
--pants = gr.material({0.2,0.3,0.9}, {0, 0, 0}, 2)
--shoes = gr.material({0.1,0.1,0.1}, {0.3, 0.3, 0.3}, 20)

mirror = gr.material({0,0,0}, {1, 1, 1}, 50000000)
white = gr.material({0.740063, 0.742313, 0.733934}, {0, 0, 0}, 0)
white_shiny = gr.material({0.740063/2, 0.742313/2, 0.733934/2}, {1, 1, 1}, 40000000)
red = gr.material({0.366046, 0.037182, 0.041638}, {0, 0, 0}, 0)
green = gr.material({0.162928, 0.408903, 0.083375}, {0, 0, 0}, 0)

-- Scene root
scene = gr.node('scene')

radius = 2.0
height = 3.0

s1 = gr.sphere('s1')
s1:set_material(red)

s2 = gr.sphere('s2')
s2:set_material(green)
s2:translate(-0.5, 0, 0)

csg = gr.csg_union('csg', s1, s2)
scene:add_child(csg)
csg:translate(3, 0, 0)

s3 = gr.sphere('s3')
s3:set_material(red)
s3:scale(0.5, 1.5, 0.5)

c1 = gr.cylinder('c1')
c1:set_material(green)
c1:rotate('x', 90)

csg2 = gr.csg_union('csg2', s3, c1)
scene:add_child(csg2)
csg2:translate(-3, 1, 0)

s4 = gr.sphere('s4')
s4:set_material(green)
s4:scale(0.6, 0.6, 0.6)

b1 = gr.cube('b1')
b1:set_material(red)
b1:translate(-0.5, -0.5, -0.5)

csg3 = gr.csg_union('csg3', s4, b1)
scene:add_child(csg3)
csg3:translate(0, 0, -2)
csg3:rotate('y', 45.0)
csg3:scale(1.5, 1.5, 1.5)

csg4 = gr.csg_intersection('csg4', s4, b1)
scene:add_child(csg4)
csg4:translate(-3, 0, -4)
csg4:rotate('y', 45.0)
csg4:scale(1.5, 1.5, 1.5)

-- Room
room_width = 10.0
room_height = 10.0
room_length = 30.0
room = gr.node('room')
scene:add_child(room)

-- floor
floor = gr.plane('floor')
room:add_child(floor)
floor:set_material(white_shiny)
floor:translate(0.0, -1.0, 0.0)
floor:scale(room_width, 1, room_length)

-- ceiling
ceiling = gr.plane('ceiling')
room:add_child(ceiling)
ceiling:set_material(white)
ceiling:translate(0.0, room_height-2.0, 0.0)
ceiling:rotate('z', 180.0)
ceiling:scale(room_width, 1, room_length)

-- left wall
left_wall = gr.plane('left_wall')
room:add_child(left_wall)
left_wall:set_material(green)
left_wall:translate(room_width/2.0-1, -1.0, -room_length/1.5)
left_wall:rotate('z', 90)
left_wall:scale(room_height, 1.0, room_length)

-- right wall
right_wall = gr.plane('right_wall')
room:add_child(right_wall)
right_wall:set_material(red)
right_wall:translate(-room_width/2.0+1, -1.0, -room_length/1.5)
right_wall:rotate('z', -90)
right_wall:scale(room_height, 1.0, room_length)

-- back wall
back_wall = gr.plane('back_wall')
room:add_child(back_wall)
back_wall:set_material(white)
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
	  'csg.png', 256, 256,
	  {0, room_height/2.0, -room_length/2.0}, {0, -room_height/2.0, 30}, {0, 1, 0}, 50,
	  {0.3,0.3,0.3}, {light1, light2}, 2, 1)

