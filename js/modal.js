$(window).load(function(){
	$('img').on('click',function()
	{
    	var sr=$(this).attr('src'); 
    	$('#mimg').attr('src',sr);
    	$('#myModal').modal('show');
	});
});