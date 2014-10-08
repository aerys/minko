;(function() {
	window.onerror = function (message, url, linenumber)
	{
		var debugElement = document.getElementById("debug");
		if (debugElement)
			debugElement.innerHTML += message + " line " + linenumber + " " + url + "</br>";
		else
			console.log('Javascript error: ' + message + ' on line ' + linenumber + ' for ' + url);
	}

	console.log("Minko init");

	var m = {};

	m.getOffsetTop = function(element)
	{
		var result = 0;
		while(element){
			result += element.offsetTop;
			element = element.offsetParent;
		}
		return result;
	};

	m.getOffsetLeft = function(element)
	{
		var result = 0;
		while(element){
			result += element.offsetLeft;
			element = element.offsetParent;
		}
		return result;
	};
	
	m.addListener = function(accessor, type)
	{
		if(!accessor.minkoEvents)
			accessor.minkoEvents = [];
		
		accessor.addEventListener(type, function(event)
		{
			var eventIndex = accessor.minkoEvents.length - 1;
			console.log("GET AN EVENT FROM JS: " + type + "(event index: " + eventIndex + ")");

			if (type != "touchmove" || type == "touchmove" && accessor.minkoEvents.length < 10)
				accessor.minkoEvents.push(event);
		});
	};

	m.getEventsCount = function(accessor)
	{
		if(accessor && accessor.minkoEvents)
			return accessor.minkoEvents.length;
		else
			return 0;
	};

	m.clearEvents = function(accessor)
	{
		if(accessor)
			accessor.minkoEvents = [];
	};

	window.Minko = m;
	
	if (!window.Minko.onmessage)
	{
		window.Minko.onmessage = function(message)
		{
			console.log('MINKO: ' + message);
		}
	}
	window.Minko.messagesToSend = [];

	window.Minko.sendMessage = function(message)
	{
		window.Minko.messagesToSend.push(message);
	}
	
	console.log("Finished to inject JS into web page.");
})();