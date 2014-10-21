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
			//var eventIndex = accessor.minkoEvents.length - 1;
			//console.log("GET AN EVENT FROM JS: " + type + "(event index: " + eventIndex + ")");

			/*
			if (type != "touchmove" || type == "touchmove" && accessor.minkoEvents.length < 10)
				accessor.minkoEvents.push(event);
			*/
			console.log('JS Event: ' + type + ' (' + event.currentTarget.minkoName + ')');
			
			// Workaround for API 19 to properly fire touchmove
			if (type == "touchstart" || type == "touchend")
				event.preventDefault();
			
			eventData = {};
			eventData.type = type;
			eventData.clientX = event.clientX;
			eventData.clientY = event.clientY;
			eventData.pageX = event.pageX;
			eventData.pageY = event.pageY;
			eventData.layerX = event.layerX;
			eventData.layerY = event.layerY;
			eventData.screenX = event.screenX;
			eventData.screenY = event.screenY;
			
			if (type.indexOf("touch") != -1)
			{
				eventData.changedTouches = [];
				for (var i = 0; i < event.changedTouches.length; i++)
				{
					eventData.changedTouches[i] = {};
					eventData.changedTouches[i].clientX = event.changedTouches[i].clientX;
					eventData.changedTouches[i].clientY = event.changedTouches[i].clientY;
					eventData.changedTouches[i].identifier = event.changedTouches[i].identifier;
				}
			}
			
			var jsonStringify = JSON.stringify(eventData);
			console.log(jsonStringify);
			
			MinkoNativeInterface.onEvent(event.currentTarget.minkoName, jsonStringify);
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
	
	/*
	if (!window.Minko.onmessage)
	{
		window.Minko.onmessage = function(message)
		{
			console.log('MINKO: ' + message);
		}
	}
	window.Minko.messagesToSend = [];
	*/
	
	window.Minko.sendMessage = function(message)
	{
		console.log("Send message: " + message);
		MinkoNativeInterface.onMessage(message);
	}
	
	console.log("Finished to inject JS into web page.");
})();