
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
	
	$(".selectbar").hover(
		function(){
			$(this).addClass("selectbar_hovered");
		},
		function(){
			$(this).removeClass("selectbar_hovered");
		}
	);

	
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
		var barhtml = "";
		
		barhtml += "<div class=\"lt\">-</div>";
		barhtml += "<div class=\"gt\">+</div>";
		
		barhtml += "<div class=\"bar\"><div>&nbsp;</div><span>" + params[2] + "</span></div>";
		
		
		
		$(thing).html(barhtml);
		
		// select the child, and attach the parameters to it
		var bar = $(thing).children(".bar");
		bar.data('bar', params);
		
		modify_bar(bar, false);
		
		// find the inner div, set it's width correctly
		
		
		bar.children("div:first-child").width(((params[2] - params[3])/(params[4] - params[3]))*100 + "%");
		
		bar.data('down', false);
		
		bar.hover(
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
		});
		
		// setup the adjustment tabs on each end
		$(thing).children(".lt,.gt")
		.hover(
			function() {
				$(this).addClass("ltgt_hovered");
			},
			function() {
				// cancels the step timer, just in case
				$(this).data('mv-enabled', false);
				$(this).removeClass("ltgt_hovered");
			}
		)
		.mousedown(function(e){
			
			// pin these variables down so the timeout function can use them
			var data = this;
			var that = $(this).siblings('.bar');
			var stepsz = $(this).hasClass('gt') ? 1 : -1;
			$(this).data('mv-enabled', true);
			
			step_bar(that, stepsz);
				
			// setup a timer that allows us to repeat the step until the 
			// mouse is released
			var fn = function(){ 
				if ($(data).data('mv-enabled'))
				{
					step_bar(that, stepsz);
					setTimeout(fn, 100);
				}
			}
			
			setTimeout(fn, 600);
			return false;
		})
		.mouseup(function(e){
			// cancel the timer
			$(this).data('mv-enabled', false);
			return false;
		});
	});
});

function dragFn(that, e)
{
	if ($(that).data('down') == true)
	{
		var params = $(that).data('bar');
		var diff = params[4] - params[3];
		
		// the -5 here is to allow the edges to be less significant
		var x = (e.clientX - parseInt($(that).position().left)) - 5;
		if (x < 0) x = 0;
		
		// determine the value by converting the mouse position into a ratio
		params[2] = (x / ($(that).width() - 10) ) * diff;
		
		// move to an even step size
		params[2] = Math.round(params[2] / params[5]) * params[5] + params[3];
					
		modify_bar(that, true);
	}
}

function step_bar(that, val)
{
	var params = $(that).data('bar');
	params[2] += params[5] * val;
			
	modify_bar(that, true);
}

function modify_bar(that, dosubmit)
{
	var params = $(that).data('bar');
	var diff = params[4] - params[3];
	
	// ensure its not too big
	if (params[2] > params[4])
		params[2] = params[4];
	else if (params[2] < params[3])
		params[2] = params[3];
	
	// then convert it back and display it
	$(that).children("div:first-child").width(((params[2] - params[3]) / diff)*100 + "%");
	if (params[6] == 0)
		$(that).children("span").html(params[2]);
	else
		$(that).children("span").html(params[2].toFixed(params[6]));

	if (dosubmit)
		submit_variables(that, params);
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


