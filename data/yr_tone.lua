

-- materials
stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
--grey_stone = gr.fancy_material({0.4, 0.4, 0.4}, {0.2, 0.2, 0.2}, 20, 0.5, 1.0, 0.0)
grass = gr.material({0.1, 0.0, 0.3}, {0.0, 0.0, 0.0}, 0)
ground = gr.material({0.3, 0.3, 0.3}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)

glass = gr.material({1.0,1.0,1.0}, {0.1, 0.1, 0.1}, 50)
shirt = gr.material({0.9,0.9,0.9}, {0, 0, 0}, 2)
pants = gr.material({0.2,0.3,0.9}, {0, 0, 0}, 2)
shoes = gr.material({0.1,0.1,0.1}, {0.3, 0.3, 0.3}, 20)


white_cornell = gr.material({0.740063, 0.742313, 0.733934}, {0, 0, 0}, 0)
white_cornell_shiny = gr.material({0.740063/2, 0.742313/2, 0.733934/2}, {1, 1, 1}, 10000.0)
mirror = gr.material({0,0,0}, {1, 1, 1}, 50)
red_cornell =   gr.material({0.366046, 0.037182, 0.041638}, {0, 0, 0}, 0)
green_cornell = gr.material({0.162928, 0.408903, 0.083375}, {0, 0, 0}, 0)


--tex_test = gr.fancy_material({0.740063, 0.742313, 0.733934}, {0, 0, 0}, 10000000.0)
--tex_test:set_bump_map("textures/moonbump2.png", 0.05)

scene = gr.node('scene')


--orig
-- sphere = gr.sphere('s')
-- radius = 1.5
-- sphere:translate(-2,radius - 1,0)
-- sphere:scale(radius, radius, radius)
-- sphere:set_material(white_cornell)
-- scene:add_child(sphere)

-- box = gr.cube('c')
-- box:translate(1,-1,0)
-- box:scale(2,4,2)
-- box:rotate('y', 30)
-- box:set_material(white_cornell)
-- scene:add_child(box)

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





-- Cornell Box
cornell_box = gr.node('cornell box')

box_width = 10.0
box_height = 7.0
box_length = 30.0
	--floor
floor = gr.plane('floor', 30.0)
floor:set_material(white_cornell)
floor:translate(5.0,-1.0,0)
--floor:scale(box_width,1,box_width)
cornell_box:add_child(floor)

	--ceiling

ceiling = gr.plane('ceiling', 30.0)
ceiling:set_material(white_cornell)
ceiling:translate(0,5,0)
ceiling:rotate('z',180)
--ceiling:scale(box_width,1,box_width)
cornell_box:add_child(ceiling)

	--left wall

left_wall = gr.plane('left_wall', 30.0)
left_wall:set_material(green_cornell)
left_wall:translate(5,0,0)
left_wall:rotate('y',90)
left_wall:rotate('x',-90)
--left_wall:scale(box_width,1,box_width)
cornell_box:add_child(left_wall)

-- left_wall = gr.cube('left_wall')
-- left_wall:set_material(green_cornell)
-- left_wall:translate(box_width/2.0 - 1,-1.0,-box_length/1.5)
-- left_wall:scale(1,box_height,box_length)
-- cornell_box:add_child(left_wall)

	--right wall
-- right_wall = gr.cube('right_wall')
-- right_wall:set_material(red_cornell)
-- right_wall:translate(-box_width/2.0,-1.0,-box_length/1.5)
-- right_wall:scale(1,box_height,box_length)
-- cornell_box:add_child(right_wall)

right_wall = gr.plane('right_wall', 30.0)
right_wall:set_material(red_cornell)
right_wall:translate(-5,0,0)
right_wall:rotate('y',90)
right_wall:rotate('x',90)
--right_wall:scale(box_width,1,box_width)
cornell_box:add_child(right_wall)


	--back wall
back_wall = gr.cube('back_wall')
back_wall:set_material(white_cornell)
back_wall:translate(-box_width/2.0,-1.0,box_length/6)
back_wall:scale(box_width,box_height,1.0)
cornell_box:add_child(back_wall)

scene:add_child(cornell_box)


-- lights
light_color = {1, 1, 1}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}
-- on ceiling
light1 = gr.light({4, 3.0, -1}, light_color_2, {1, 0, 0})
-- by camera
light2 = gr.light({-2.0, box_height - 3.0, -3}, light_color_2, {1, 0, 0})


--sqlight = gr.rect_light({0, box_height - 2.01, -2}, 3, 3, light_color, {1,0,0}, 3)
--sqlight2 = gr.rect_light({3, box_height - 2.01, -2}, 3, 3, light_color, {1,0,0}, 3)
--sqlight3 = gr.rect_light({-3, box_height - 2.01, -2}, 3, 3, light_color, {1,0,0}, 3)


-- double mAperature = 0.01;
-- 					double mFocalLen = 7.0;
-- 					int mDofSamples = 4; 
--far
gr.render(scene,
	  'yr_tone.png', 512, 512,
	  {0, 1, -7.0}, {0, -box_height/2.0, 30}, {0, 1, 0}, 50,
	  {0.2,0.2,0.2}, {light1,light2}, 1, 1)


--close
-- gr.render(scene,
-- 	  'bump2.png', 700, 700, 2,
-- 	  {0, box_height/100.0, -box_length/100.0}, {0, -10, 30}, {0, 1, 0}, 50,
-- 	  {0.2,0.2,0.2}, {light1, light2})






