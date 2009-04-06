
$(document).ready(function() 
{
	// show/hide the variable groups to make the layout less cluttered
	$(".grouphdr").each(function(idx, thing){
		$(thing).mousedown(function(){
		
			$(this).next().toggle("slow");
				
			return false;
		});
	});
	
	// checkboxes
	$(".booleans").each(function(idx, thing){
		var params = this.id.split("_");
		
		$(this).data('bar', params);
		
		$(this).click(function(e)
		{
			var params = $(this).data('bar');
			
			if (this.checked)
				params[2] = 1;
			else
				params[2] = 0;
			
			submit_variables(this, params);
		});
		
	});
	

	// make the bars highlight when moused
	$(".selectbar").mouseover(function(){
		this.className = this.className + '_hl';
		return false;
	});
	
	$(".selectbar").mouseout(function(){
		this.className = this.className.replace(/_hl/,'');
		return false;
	});	
	
	// make the bars come to life (integers)
	$(".selectbar").each(function(idx, thing){
		
		/* 
			bar parameters are done using the id tag:
				0 - group
				1 - variable name
				2 - current value
				3 - min
				4 - max
				5 - step
				
				6 is set by the code, and is the floating point
				precision (which is inferred from the step size)
				
				If the numeric items have a p in them, that is
				actually a decimal point.
		*/
		
		// get the id from the element, split to params
		var params = this.id.split("_");
		
		params[6] = 0;
		for (i = 2; i < 6; i++)
			if (params[i].indexOf('p') != -1)
			{
				if (i == 5) params[6] = params[i].length - params[i].indexOf('p') - 1;
				params[i] = parseFloat(params[i].replace(/p/,'.'));
			}
			else
				params[i] = parseInt(params[i]);
				
		// add the inner divs, transform it into a nice bar
		var bar = "<div style=\"position: relative\"><div>&nbsp;</div><span style=\"display: block; position: absolute; right: 0.1em; top: 0\">" + params[2] + "</span></div>";
		$(thing).html(bar);
		
		// select the child, and attach the parameters to it
		var child = $(thing).children("div:first-child");
		child.data('bar', params);
		
		// find the inner div, set it correctly
		child.children("div:first-child").width(((params[2] - params[3])/(params[4] - params[3]))*100 + "%");
		
		
		// set the text correctly
		
		// add a mousemove handler to ensure that we 
		// can move the variable around
		
		child.data('down', false);
		
		child
		.hover(
			function() { 
				return true;
			},
			function() {
				$(this).data('down', false);
				return true;
			}
		)
		.mousedown(function(e){
			$(this).data('down', true);
			dragFn(this, e);
			return true;
		})
		.mouseup(function(){
			$(this).data('down', false);
			return false;
		})
		.mousemove(function(e){
			dragFn(this, e);
			return false;
		})
		.children()
		.keydown(function(e){
			alert("keydown");
		})
		.focus(function(e){
			alert("focus");
		});
	
	});
});

function dragFn(that, e)
{
	if ($(that).data('down') == true)
	{
		var params = $(that).data('bar');
	
		// calculate the new value
		var diff = params[4] - params[3];
		
		// the -5 here is to allow the edges to be less significant
		var x = (e.clientX - parseInt($(that).position().left)) - 5;
		if (x < 0) x = 0;
		
		// determine the value by converting the mouse position into a ratio
		params[2] = (x / ($(that).width() - 10) ) * diff;
		
		// move to an even step size
		params[2] = Math.round(params[2] / params[5]) * params[5] + params[3];
		
		// ensure its not too big
		if (params[2] > params[4])
			params[2] = params[4];
		
		// then convert it back and display it
		$(that).children("div:first-child").width(((params[2] - params[3]) / diff)*100 + "%");
		if (params[6] == 0)
			$(that).children("span").html(params[2]);
		else
			$(that).children("span").html(params[2].toFixed(params[6]));
		
		submit_variables(that, params);

	}
}

// submits the variables
function submit_variables(that, params)
{
	if ($(that).data('ajaxprocess') != true)
	{
		$(that).data('ajaxprocess', true);
		setTimeout(
			function(){
				$.ajax({
					type: "POST",
					url: "/varcontrol",
					data: 
						"group=" + params[0].substr(1) + 
						"&var=" + params[1].substr(1) + 
						"&value=" + params[2] +
						"&instance=" + current_instance,
					error: function(x, status, e) {
						// display something somewhere to indicate an error
					},
					success: function(msg) {
						// display something somewhere to indicate success?
						switch (msg)
						{
							case "FAIL":
							case "INVALID":
								break;
							case "SUCCESS":
								break;
							case "RELOAD":
								window.location.reload();
								break;
						}
					}
				});
				
				$(that).data('ajaxprocess', false);
			},
			50
		);
	}
}


