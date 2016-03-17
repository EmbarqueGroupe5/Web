$(document).ready(function(){
	var url = "http://192.168.169.1/media/Storage/Arduino/";
	
	
	var forward = 38;
	var backward = 40;
	var left  = 37;
	var right  = 39;
	var map = {37: false, 38: false, 39: false, 40: false};
  
	$(document).keydown(function(e) {
	  if (e.keyCode in map) {
		  map[e.keyCode] = true;
		  if (map[forward] && map[left]) {
			  console.log("forward left");
			  run("ForwardLeft");
		  }
		  else if (map[forward] && map[right]) {
			  console.log("forward right");
			  run("ForwardRight");
		  }
		  else if (map[backward] && map[left]) {
			  console.log("backward left");
			  run("BackwardLeft");
		  }
		  else if (map[backward] && map[right]) {
			  console.log("backward right");
			  run("BackwardRight");
		  }
		  else if (map[forward]) {
			  console.log("forward");
			  run("Forward");
		  }
		  else if (map[backward]) {
			  console.log("backward");
			  run("Backward");
		  }
		  else if (map[left]) {
			  console.log("left");
			  run("Left");
		  }
		  else if (map[right]) {
			  console.log("right");
			  run("Right");
		  }
	  }
	}).keyup(function(e) {
	  if (e.keyCode in map) {
			console.log("stop");
			run("Stop");
		  map[e.keyCode] = false;
	  }
	});

	
	function run(direction) {
		console.log("Running to " + direction);
		direction = "?" + direction
		$.ajax({
			url: direction,
			method: "GET",
			success: function(data){
				$(".return").html(data);
			}
		});
	}

	
	$('.url').click(function(){
		var url = this.href;
		url.replace("?", "");
		
		console.log(url);
		
		run(url);
		return false;
	});
});