-- materials
--stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
--grey_stone = gr.material({0.4, 0.4, 0.4}, {0.2, 0.2, 0.2}, 20)
--grass = gr.material({0.1, 0.0, 0.3}, {0.0, 0.0, 0.0}, 0)
--ground = gr.material({0.3, 0.3, 0.3}, {0.0, 0.0, 0.0}, 0)
--hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)
--glass = gr.material({1.0,1.0,1.0}, {0.1, 0.1, 0.1}, 50)
--shirt = gr.material({0.9,0.9,0.9}, {0, 0, 0}, 2)
--pants = gr.material({0.2,0.3,0.9}, {0, 0, 0}, 2)
--shoes = gr.material({0.1,0.1,0.1}, {0.3, 0.3, 0.3}, 20)

white = gr.material({0.740063, 0.742313, 0.733934}, {0, 0, 0}, 0)
white_shiny = gr.material({0.740063/2, 0.742313/2, 0.733934/2}, {1, 1, 1}, 10000.0)
mirror = gr.material({0,0,0}, {1, 1, 1}, 50)
red =   gr.material({0.366046, 0.037182, 0.041638}, {0, 0, 0}, 0)
green = gr.material({0.162928, 0.408903, 0.083375}, {0, 0, 0}, 0)

-- scene root
scene = gr.node('scene')

math.randomseed(1)

nspheres = 10
for i=1,nspheres do
	r = math.random()
	g = math.random()
	b = math.random()
	spheremat = gr.material({r,g,b}, {0.3, 0.3, 0.3}, 1000000000.0)

	sphere = gr.sphere('s')
	radius = math.random()
	x = math.random() * 10 - (5 + radius)
	z = math.random() * 10 - (5 + radius)
	sphere:translate(x,radius - 1,z)
	sphere:scale(radius, radius, radius)
	sphere:set_material(spheremat)
	scene:add_child(sphere)

end

-- Room
room = gr.node('cornell room')

room_width = 10.0
room_height = 7.0
room_length = 30.0

-- floor
floor = gr.plane('floor')
room:add_child(floor)
floor:set_material(white)
floor:translate(5.0,-1.0,0)
floor:scale(room_width,1,room_width)

-- ceiling
ceiling = gr.plane('ceiling')
room:add_child(ceiling)
ceiling:set_material(white)
ceiling:translate(0,5,0)
ceiling:rotate('z',180)
ceiling:scale(room_width,1,room_width)

-- left wall
left_wall = gr.plane('left_wall')
room:add_child(left_wall)
left_wall:set_material(green)
left_wall:translate(5,0,0)
left_wall:rotate('y',90)
left_wall:rotate('x',-90)
left_wall:scale(room_width,1,room_width)

-- right wall
right_wall = gr.plane('right_wall')
room:add_child(right_wall)
right_wall:set_material(red)
right_wall:translate(-5,0,0)
right_wall:rotate('y',90)
right_wall:rotate('x',90)
right_wall:scale(room_width,1,room_width)

-- back wall
back_wall = gr.plane('back_wall')
room:add_child(back_wall)
back_wall:set_material(white)
back_wall:translate(0.0, -1.0, room_length/6)
back_wall:rotate('x', -90.0)
back_wall:scale(room_width, 1.0, room_height)

scene:add_child(room)

-- lights
light_color = {1, 1, 1}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}

-- on ceiling
light1 = gr.light({4, 3.0, -1}, light_color_2, {1, 0, 0})

-- by camera
light2 = gr.light({-2.0, room_height - 3.0, -3}, light_color_2, {1, 0, 0})

--sqlight = gr.rect_light({0, room_height - 2.01, -2}, 3, 3, light_color, {1,0,0}, 3)
--sqlight2 = gr.rect_light({3, room_height - 2.01, -2}, 3, 3, light_color, {1,0,0}, 3)
--sqlight3 = gr.rect_light({-3, room_height - 2.01, -2}, 3, 3, light_color, {1,0,0}, 3)

gr.render(scene,
	  'yr_tone.png', 512, 512,
	  {0, 2, -14.0}, {0, -room_height/2.0, 30}, {0, 1, 0}, 50,
	  {0.2,0.2,0.2}, {light1,light2}, 2, 2)

