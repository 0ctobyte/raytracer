
-- materials
stone = gr.material({0.8, 0.7, 0.7}, {0.0, 0.0, 0.0}, 0)
grass = gr.material({0.1, 0.0, 0.3}, {0.0, 0.0, 0.0}, 0)
ground = gr.material({0.3, 0.3, 0.3}, {0.0, 0.0, 0.0}, 0)
hide = gr.material({0.84, 0.6, 0.53}, {0.3, 0.3, 0.3}, 20)
mirror = gr.material({0,0,0}, {1, 1, 1}, 50000000)
glass = gr.material({1.0,1.0,1.0}, {0.1, 0.1, 0.1}, 50)
shirt = gr.material({0.9,0.9,0.9}, {0, 0, 0}, 2)
pants = gr.material({0.2,0.3,0.9}, {0, 0, 0}, 2)
shoes = gr.material({0.1,0.1,0.1}, {0.3, 0.3, 0.3}, 20)

white_cornell = gr.material({0.740063, 0.742313, 0.733934}, {0, 0, 0}, 0)
white_cornell_shiny = gr.material({0.740063/2, 0.742313/2, 0.733934/2}, {1, 1, 1}, 40000000)
red_cornell =   gr.material({0.366046, 0.037182, 0.041638}, {0, 0, 0}, 0)
green_cornell = gr.material({0.162928, 0.408903, 0.083375}, {0, 0, 0}, 0)



scene = gr.node('scene')



sphere = gr.sphere('s')
radius = 1.5
sphere:translate(-2,radius - 1,0)
sphere:scale(radius, radius, radius)
sphere:set_material(mirror)
scene:add_child(sphere)

box = gr.cube('c')
box:translate(1,-1,0)
box:scale(2,4,2)
box:rotate('y', 30)
box:set_material(white_cornell)
scene:add_child(box)

-- Cornell Box
cornell_box = gr.node('cornell box')

box_width = 10.0
box_height = 10.0
box_length = 30.0
	--floor
floor = gr.cube('floor')
floor:set_material(white_cornell_shiny)
floor:translate(-box_width/2.0,-2.0,-box_length/1.5)
floor:scale(box_width,1,box_length)
cornell_box:add_child(floor)

	--ceiling
ceiling = gr.cube('ceiling')
ceiling:set_material(white_cornell)
ceiling:translate(-box_width/2.0,box_height - 2.0,-box_length/1.5)
ceiling:scale(box_width,1,box_length)
cornell_box:add_child(ceiling)


	--left wall
left_wall = gr.cube('left_wall')
left_wall:set_material(green_cornell)
left_wall:translate(box_width/2.0 - 1,-1.0,-box_length/1.5)
left_wall:scale(1,box_height,box_length)
cornell_box:add_child(left_wall)

	--right wall
right_wall = gr.cube('right_wall')
right_wall:set_material(red_cornell)
right_wall:translate(-box_width/2.0,-1.0,-box_length/1.5)
right_wall:scale(1,box_height,box_length)
cornell_box:add_child(right_wall)

	--back wall
back_wall = gr.cube('back_wall')
back_wall:set_material(white_cornell)
back_wall:translate(-box_width/2.0,-1.0,box_length/6)
back_wall:scale(box_width,box_height,1.0)
cornell_box:add_child(back_wall)

scene:add_child(cornell_box)


-- lights
light_color = {0.780131, 0.780409, 0.775833}
light_color_2 = {0.780131/2, 0.780409/2, 0.775833/2}
-- on ceiling
light1 = gr.light({0, box_height - 3.0, -3}, light_color_2, {1, 0, 0})
-- by camera
light2 = gr.light({-2.0, box_height - 3.0, -3}, light_color_2, {1, 0, 0})


--sqlight = gr.rect_light({0, box_height - 2.01, -2}, 3, 3, light_color, {1,0,0}, 10)


--far
-- gr.render(scene,
-- 	  'reflect.png', 800, 800,
-- 	  {-4, 4, -20}, {2, -2, 10}, {0, 1, 0}, 50,
-- 	  {0.2, 0.2, 0.2}, {light1, light2})

-- --close
-- gr.render(scene,
-- 	  'reflect.png', 800, 800,
-- 	  {-2, 4, -10}, {2, -2, 10}, {0, 1, 0}, 50,
-- 	  {0.2, 0.2, 0.2}, {light1, light2})
gr.render(scene,
	  'yr_reflect.png', 1024, 1024,
	  {0, box_height/2.0, -box_length/2.0}, {0, -box_height/2.0, 30}, {0, 1, 0}, 50,
	  {0.2,0.2,0.2}, {light1, light2}, 4, 4)

--top
--close
-- gr.render(scene,
-- 	  'reflect.png', 800, 800,
-- 	  {0, 10, 0}, {0, -9, -5}, {0, 1, 0}, 50,
-- 	  {0.2, 0.2, 0.2}, {light1, light2})


