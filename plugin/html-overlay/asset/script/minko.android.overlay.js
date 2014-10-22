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
	
	m.touchstartEventFlags = [];
	
	m.addListener = function(accessor, type)
	{
		if (type == "touchstart")
			m.touchstartEventFlags[accessor] = false;
			
		accessor.addEventListener(type, function(event)
		{
			// Work around for touchstart event that is fired twice
			if (type == "touchstart") 
			{
				if (!m.touchstartEventFlags[accessor])
				{
					m.touchstartEventFlags[accessor] = true;
					setTimeout(function(){ m.touchstartEventFlags[accessor] = false; }, 100);
				}
				else
					return;
			}
				
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

	m.sendMessage = function(message)
	{
		console.log("Send message: " + message);
		MinkoNativeInterface.onMessage(message);
	}
	
	window.Minko = m;
	
	console.log("Finished to inject JS into web page.");
})();