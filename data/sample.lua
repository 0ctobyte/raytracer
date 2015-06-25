--
-- CS488 -- Introduction to Computer Graphics
-- 
-- puppet.lua

rootnode = gr.node('root')

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
blue = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 10)
green = gr.material({0.0, 1.0, 0.0}, {0.1, 0.1, 0.1}, 10)
white = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 10)
white2 = gr.material({1.0, 1.0, 1.0}, {0.1, 0.1, 0.1}, 40)
blue2 = gr.material({0.0, 0.0, 1.0}, {0.1, 0.1, 0.1}, 80)

torso = gr.sphere('torso')
rootnode:add_child(torso)
torso:set_material(white)
torso:scale(1.0, 4.0, 1.0);
torso:translate(0.0, 0.3, 0.0);

-- Hips
hips = gr.sphere('hips')
torso:add_child(hips)
hips:set_material(blue)
hips:scale(1.0, 0.25, 1.0)
hips:scale(1.5, 0.5, 0.5)
hips:translate(0.0, -8.5, 0.0)

-- Right leg
rthigh_j = gr.joint('rthigh_j', {-90.0, 0.0, 45.0}, {0.0, 0.0, 0.0})
hips:add_child(rthigh_j)
rthigh_j:scale(1.0/1.5, 2.0, 2.0)
rthigh_j:translate(1.5, 0.0, 0.0)

rthigh = gr.sphere('rthigh')
rthigh_j:add_child(rthigh)
rthigh:set_material(white)
rthigh:scale(0.4, 1.7, 0.4)
rthigh:translate(0.0, -1.0, 0.0)

rcalf_j = gr.joint('rcalf_j', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0})
rthigh:add_child(rcalf_j)
rcalf_j:scale(2.5, 1.0/1.7, 2.5)
rcalf_j:translate(0.0, -1.5, 0.0)

rcalf = gr.sphere('rcalf')
rcalf_j:add_child(rcalf)
rcalf:set_material(red)
rcalf:scale(0.4, 1.7, 0.4)
rcalf:translate(0.0, -1.0, 0.0)

rfoot_j = gr.joint('rfoot_j', {-45.0, 0.0, 45.0}, {0.0, 0.0, 0.0})
rcalf:add_child(rfoot_j)
rfoot_j:scale(2.5, 1.0/1.7, 2.5)
rfoot_j:translate(0.0, -1.5, 0.0)

rfoot = gr.sphere('rfoot')
rfoot_j:add_child(rfoot)
rfoot:set_material(green)
rfoot:scale(0.4, 0.4, 0.8)
rfoot:translate(0.0, -0.6, 0.8)

-- Left leg
lthigh_j = gr.joint('lthigh_j', {-90.0, 0.0, 45.0}, {0.0, 0.0, 0.0})
hips:add_child(lthigh_j)
lthigh_j:scale(1.0/1.5, 2.0, 2.0)
lthigh_j:translate(-1.5, 0.0, 0.0)

lthigh = gr.sphere('lthigh')
lthigh_j:add_child(lthigh)
lthigh:set_material(white)
lthigh:scale(0.4, 1.7, 0.4)
lthigh:translate(0.0, -1.0, 0.0)

lcalf_j = gr.joint('lcalf_j', {0.0, 0.0, 90.0}, {0.0, 0.0, 0.0})
lthigh:add_child(lcalf_j)
lcalf_j:scale(2.5, 1.0/1.7, 2.5)
lcalf_j:translate(0.0, -1.5, 0.0)

lcalf = gr.sphere('lcalf')
lcalf_j:add_child(lcalf)
lcalf:set_material(red)
lcalf:scale(0.4, 1.7, 0.4)
lcalf:translate(0.0, -1.0, 0.0)

lfoot_j = gr.joint('lfoot_j', {-45.0, 0.0, 45.0}, {0.0, 0.0, 0.0})
lcalf:add_child(lfoot_j)
lfoot_j:scale(2.5, 1.0/1.7, 2.5)
lfoot_j:translate(0.0, -1.5, 0.0)

lfoot = gr.sphere('lfoot')
lfoot_j:add_child(lfoot)
lfoot:set_material(green)
lfoot:scale(0.4, 0.4, 0.8)
lfoot:translate(0.0, -0.6, 0.8)

-- Shoulders
shoulders = gr.sphere('shoulders')
torso:add_child(shoulders)
shoulders:set_material(blue)
shoulders:scale(1.0, 0.25, 1.0)
shoulders:scale(1.5, 0.5, 0.5)
shoulders:translate(0.0, 8.5, 0.0)

-- Head
neck_j = gr.joint('neck_j', {0.0, 0.0, 30.0}, {0.0, 0.0, 0.0})
shoulders:add_child(neck_j)
neck_j:scale(1.0/1.5, 2.0, 2.0)
neck_j:translate(0.0, 0.5, 0.0)

neck = gr.sphere('neck')
neck_j:add_child(neck)
neck:set_material(white)
neck:scale(0.2, 0.8, 0.2)
neck:translate(0.0, 1.0, 0.0)

head_j = gr.joint('head_j', {-20.0, 0.0, 20.0}, {-80.0, 0.0, 80.0})
neck:add_child(head_j)
head_j:scale(5.0, 1.25, 5.0)
head_j:translate(0.0, 1.0, 0.0)

head = gr.sphere('head')
head_j:add_child(head)
head:set_material(blue)
head:scale(1.0, 1.25, 1.0)
head:translate(0.0, 0.0, 0.8)

leye = gr.sphere('leye')
head:add_child(leye)
leye:set_material(red)
leye:scale(1.0, 0.8, 1.0)
leye:scale(0.1, 0.1, 0.1)
leye:translate(-3.0, 2.0, 10.0)

reye = gr.sphere('reye')
head:add_child(reye)
reye:set_material(red)
reye:scale(1.0, 0.8, 1.0)
reye:scale(0.1, 0.1, 0.1)
reye:translate(3.0, 2.0, 10.0)

-- Right arm
rupperarm_j = gr.joint('rupperarm_j', {-180.0, 0.0, 60.0}, {0.0, 0.0, 0.0})
shoulders:add_child(rupperarm_j)
rupperarm_j:scale(1.0/1.5, 2.0, 2.0)
rupperarm_j:translate(1.5, 0.0, 0.0)

rupperarm = gr.sphere('rupperarm')
rupperarm_j:add_child(rupperarm)
rupperarm:set_material(white)
rupperarm:scale(0.4, 1.7, 0.4)
rupperarm:translate(0.0, -1.0, 0.0)

rforearm_j = gr.joint('rforearm_j', {-160.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
rupperarm:add_child(rforearm_j)
rforearm_j:scale(2.5, 1.0/1.7, 2.5)
rforearm_j:translate(0.0, -1.5, 0.0)

rforearm = gr.sphere('rforearm')
rforearm_j:add_child(rforearm)
rforearm:set_material(red)
rforearm:scale(0.4, 1.7, 0.4)
rforearm:translate(0.0, -1.0, 0.0)

rhand_j = gr.joint('rhand_j', {-45.0, 0.0, 45.0}, {0.0, 0.0, 0.0})
rforearm:add_child(rhand_j)
rhand_j:scale(2.5, 1.0/1.7, 2.5)
rhand_j:rotate('y', 90.0)
rhand_j:translate(0.0, -1.5, 0.0)

rhand = gr.sphere('rhand')
rhand_j:add_child(rhand)
rhand:set_material(green)
rhand:scale(0.4, 0.6, 0.4)
rhand:translate(0.0, -1.0, 0.0)

-- Left arm
lupperarm_j = gr.joint('lupperarm_j', {-180.0, 0.0, 60.0}, {0.0, 0.0, 0.0})
shoulders:add_child(lupperarm_j)
lupperarm_j:scale(1.0/1.5, 2.0, 2.0)
lupperarm_j:translate(-1.5, 0.0, 0.0)

lupperarm = gr.sphere('lupperarm')
lupperarm_j:add_child(lupperarm)
lupperarm:set_material(white)
lupperarm:scale(0.4, 1.7, 0.4)
lupperarm:translate(0.0, -1.0, 0.0)

lforearm_j = gr.joint('lforearm_j', {-160.0, 0.0, 0.0}, {0.0, 0.0, 0.0})
lupperarm:add_child(lforearm_j)
lforearm_j:scale(2.5, 1.0/1.7, 2.5)
lforearm_j:translate(0.0, -1.5, 0.0)

lforearm = gr.sphere('lforearm')
lforearm_j:add_child(lforearm)
lforearm:set_material(red)
lforearm:scale(0.4, 1.7, 0.4)
lforearm:translate(0.0, -1.0, 0.0)

lhand_j = gr.joint('lhand_j', {-45.0, 0.0, 45.0}, {0.0, 0.0, 0.0})
lforearm:add_child(lhand_j)
lhand_j:scale(2.5, 1.0/1.7, 2.5)
lhand_j:rotate('y', 90.0)
lhand_j:translate(0.0, -1.5, 0.0)

lhand = gr.sphere('lhand')
lhand_j:add_child(lhand)
lhand:set_material(green)
lhand:scale(0.4, 0.6, 0.4)
lhand:translate(0.0, -1.0, 0.0)

s = gr.sphere('s')
rootnode:add_child(s)
s:set_material(white2)
s:translate(0.0, -20.0, 0.0)
s:scale(15.0, 10.0, 10.0)

b = gr.cube('b')
rootnode:add_child(b)
b:set_material(blue2)
b:translate(-5.0, -2.0, 2.0)
b:scale(2.0, 2.0, 2.0)
b:rotate('y', 45.0)

icosa = gr.mesh( 'icosahedron', {
	{	 1.618034	,	 1.,	 0.},
	{	-1.618034	,	 1.,	 0.},
	{	 1.618034	,	-1.,	 0.},
	{	-1.618034	,	-1.,	 0.},
	{	 1.,	 0.,	 1.618034},
	{	 1.,	 0.,	-1.618034},
	{	-1.,	 0.,	 1.618034},
	{	-1.,	 0.,	-1.618034},
	{	 0.,	 1.618034	,	 1.},
	{	 0.,	-1.618034	,	 1.},
	{	 0.,	 1.618034	,	-1.},
	{	 0.,	-1.618034	,	-1.}
	   }, {
    { 0 , 8 , 4},
    { 0 , 5 , 10},
    { 2 , 4 , 9},
    { 2 , 11 , 5},
    { 1 , 6 , 8},
    { 1 , 10 , 7},
    { 3 , 9 , 6},
    { 3 , 7 , 11},
    { 0 , 10 , 8},
    { 1 , 8 , 10},
    { 2 , 9 , 11},
    { 3 , 11 , 9},
    { 4 , 2 , 0},
    { 5 , 0 , 2},
    { 6 , 1 , 3},
    { 7 , 3 , 1},
    { 8 , 6 , 4},
    { 9 , 4 , 6},
    { 10 , 5 , 7},
    { 11 , 7 , 5}
} )
rootnode:add_child(icosa)
icosa:set_material(red)
icosa:translate(-8.0, 8.0, 0.0)
icosa:scale(2.0, 2.0, 2.0)

rootnode:translate(0.0, 2.0, -20.0)

l1_radius = 2500.0

l1_attenuation = {1.0, 2.0/l1_radius, 1.0/(l1_radius*l1_radius)}

white_light = gr.light({-100.0, 150.0, 100.0}, {0.9, 0.9, 0.9}, l1_attenuation)

gr.render(rootnode, 'puppet.png', 512, 512,
	  {0, 0, 10.0}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})
