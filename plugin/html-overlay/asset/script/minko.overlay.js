if (!window.Minko)
	var Minko = {};
else
	var Minko = window.Minko;

window.Minko = Minko;

Minko.bridge = null;
Minko.loaded = 0;

Minko.bindJsErrors = function()
{
	Minko.window.onerror = function (message, url, linenumber)
	{
		var debugElement = Minko.document.getElementById("minkoOverlayDebug");
		
		if (debugElement)
			debugElement.innerHTML += message + " line " + linenumber + " " + url + "</br>";
		else
			console.log('[Minko HTML Overlay] Javascript error: ' + message + ' on line ' + linenumber + ' for ' + url);
	}
}

Minko.loadedHandler = function(event)
{
	if(Minko.loaded == -1)
		return;

	delete Minko.window;
	delete Minko.document;
	delete Minko.onmessage;

	Minko.loaded = 1;

	if (Minko.iframe !== undefined)
	{
		Minko.window = event.currentTarget.contentWindow;
		Minko.document = event.currentTarget.contentDocument;

		Minko.document.addEventListener('touchmove', function(event)
		{
			event.preventDefault();
		});
	}
	else
	{
		Minko.window = window;
		Minko.document = document;
	}

	if (Minko.window.Minko !== undefined)
	{
		for(var k in Minko.window.Minko)
		{
			Minko[k] = Minko.window.Minko[k];
		}
	}

	Minko.window.Minko = Minko;

	Minko.document.body.oncontextmenu = function(event)
	{ 
		event.preventDefault(); 
		return false;
	};

	if (!Minko.onmessage)
	{
		Minko.onmessage = function(message)
		{
			console.log('[Minko HTML Overlay] message received: ' + message);
		}
	}
	Minko.messagesToSend = [];

	Minko.sendMessage = function(message)
	{
		Minko.messagesToSend.push(message);
	}

	if (Minko.platform == "emscripten")
		Minko.bindRedispatchEvents();

	console.log("[Minko HTML Overlay] loaded page " + Minko.window.location.href);

	Minko.bindJsErrors();

	var ev = document.createEvent("Event");
	ev.initEvent("minkoReady", true, true);
	Minko.window.dispatchEvent(ev);
}

Minko.addListener = function(accessor, type)
{
	if(!accessor.minkoEvents)
		accessor.minkoEvents = [];
	accessor.addEventListener(type, function(event)
	{
		accessor.minkoEvents.push(event);
	});
};

Minko.getEventsCount = function(accessor)
{
	if(accessor && accessor.minkoEvents)
		return accessor.minkoEvents.length;
	else
		return 0;
};

Minko.clearEvents = function(accessor)
{
	if(accessor)
		accessor.minkoEvents = [];
};

Minko.listeners = {};

Minko.addEventListener = function(type, callback)
{
   if (!(Minko.listeners[type]))
       Minko.listeners[type] = [];

   Minko.listeners[type].push(callback);
};

Minko.removeEventListener = function(type, callback)
{
   if (!(Minko.listeners[type]))
       Minko.listeners[type] = [];

	var index = Minko.listeners[type].indexOf(callback);
	
	if (index != -1)
		Minko.listeners[type].splice(index, 1);
};

Minko.dispatchEvent = function(event)
{
   var callbacks = Minko.listeners[event.type];

   if (!callbacks)
       return;

   for(var i = 0; i < callbacks.length; ++i)
       callbacks[i](event);
};

Minko.dispatchMessage = function(message)
{
	var ev = document.createEvent("Event");
	ev.initEvent("message", true, true);
	ev.message = message;	
	Minko.dispatchEvent(ev);
	
	Minko.onmessage(message);
};

/*
** EMSCRIPTEN SPECIFIC CODE
*/
	
Minko.createIframe = function() //EMSCRIPTEN
{
	Minko.loaded = -1;

	var canvas = document.getElementById('canvas');
	var iframe = document.createElement('iframe');

	iframe.id = 'canvasiframe';
	iframe.className = 'emscripten';

	iframe.style.width = canvas.clientWidth + 'px';
	iframe.style.height = canvas.clientHeight + 'px';

	iframe.style.backgroundColor = 'transparent';
	iframe.allowTransparency = 'true';
	iframe.frameBorder = '0';

	iframe.style.position = 'relative';
	canvas.parentNode.style.position = 'relative';

	iframe.style.overflowX = 'hidden';
	iframe.style.overflowY = 'hidden';
	iframe.style.overflow = 'hidden';

	canvas.style.position = 'absolute';
	canvas.style.left = '0';
	canvas.style.right = '0';

	canvas.parentNode.appendChild(iframe);

	iframe.onload = Minko.loadedHandler;

	iframe.addEventListener('mouseover',	Minko.redispatchMouseEvent);
	iframe.addEventListener('mouseout',		Minko.redispatchMouseEvent);

	Minko.iframe = iframe;
	Minko.canvas = canvas;
}

Minko.loadUrlEmscripten = function(url) //EMSCRIPTEN
{
	Minko.iframe.src = url;
	Minko.loaded = 0;
}

Minko.getOffsetTop = function(element) //EMSCRIPTEN
{
	var result = 0;
	while(element){
		result += element.offsetTop;
		element = element.offsetParent;
	}
	return result;
};

Minko.getOffsetLeft = function(element) //EMSCRIPTEN
{
	var result = 0;
	while(element){
		result += element.offsetLeft;
		element = element.offsetParent;
	}
	return result;
};

Minko.redispatchKeyboardEvent = function(event) //EMSCRIPTEN
{
	var eventCopy = document.createEvent('Event');

	eventCopy.initEvent(event.type, event.bubbles, event.cancelable);

	var copiedProperties = ['type', 'bubbles', 'cancelable', 'view', 
	'ctrlKey', 'altKey', 'shiftKey', 'metaKey', 'keyCode', 'charCode', 
	'which', 'key', 'detail', 'keyIdentifier'];

	for(var k in copiedProperties)
		eventCopy[copiedProperties[k]] = event[copiedProperties[k]];

	document.dispatchEvent(eventCopy);
}

Minko.redispatchMouseEvent = function(event) //EMSCRIPTEN
{
	if (event.type == 'mouseout' && event.target != event.currentTarget)
		return;

	if (event.type == 'mouseover' && event.target != event.currentTarget)
		return;

	var pageX = 1 + Minko.getOffsetLeft(Minko.iframe) + (event.pageX || event.layerX);
	var pageY = 1 + Minko.getOffsetTop(Minko.iframe) + (event.pageY || event.layerY);

	var screenX = pageX - document.body.scrollLeft;
	var screenY = pageY - document.body.scrollTop;

	var eventCopy = document.createEvent('MouseEvents');
	eventCopy.initMouseEvent(event.type, event.bubbles, event.cancelable, event.view, event.detail,
		pageX, pageY, screenX, screenY, 
		event.ctrlKey, event.altKey, event.shiftKey, event.metaKey, event.button, event.relatedTarget);

	Minko.canvas.dispatchEvent(eventCopy);
}

Minko.redispatchWheelEvent = function(event)
{
	var eventCopy = document.createEvent('Event');

	eventCopy.initEvent(event.type, event.bubbles, event.cancelable);

	var copiedProperties = ['detail', 
		'wheelDelta', 'wheelDeltaX', 'wheelDeltaY', 'wheelDeltaZ', 
		'delta', 'deltaMode', 'deltaX', 'deltaY', 'deltaZ', 
		'which', 'key', 'detail', 'keyIdentifier'];

	for(var k in copiedProperties)
		eventCopy[copiedProperties[k]] = event[copiedProperties[k]];

	Minko.canvas.dispatchEvent(eventCopy);
	event.preventDefault();
};

Minko.identifiers = [];
Minko.nextId = 1;

Minko.getTouchId = function(identifier)
{
    if (!Minko.identifiers[identifier])
        Minko.identifiers[identifier] = Minko.nextId++;
    return Minko.identifiers[identifier];
}

Minko.copyTouchList = function(touches)
{
	var result = [];
	var properties = ['identifier'];

    for (var i = 0; i < touches.length; ++i)
    {
    	var touch = touches[i];

    	var copiedTouch = {};

    	copiedTouch.identifier = Minko.getTouchId(touch.identifier);
        
		var pageX = 1 + Minko.getOffsetLeft(Minko.iframe) + (touch.pageX || touch.layerX);
		var pageY = 1 + Minko.getOffsetTop(Minko.iframe) + (touch.pageY || touch.layerY);

		var screenX = pageX - document.body.scrollLeft;
		var screenY = pageY - document.body.scrollTop;

		copiedTouch.pageX = pageX;
		copiedTouch.pageY = pageY;
		copiedTouch.screenX = screenX;
		copiedTouch.screenY = screenY;
		copiedTouch.clientX = screenX;
		copiedTouch.clientY = screenY;

    	result.push(copiedTouch);
    }

    return result;
}

Minko.redispatchTouchEvent = function(event) //EMSCRIPTEN
{
	var eventCopy = document.createEvent('Event');

	eventCopy.initEvent(event.type, event.bubbles, event.cancelable);

	var copiedProperties = ['type', 'bubbles', 'cancelable', 'view'];

	for(var k in copiedProperties)
		eventCopy[copiedProperties[k]] = event[copiedProperties[k]];

	eventCopy.touches = Minko.copyTouchList(event.touches);
	eventCopy.targetTouches = Minko.copyTouchList(event.targetTouches);
	eventCopy.changedTouches = Minko.copyTouchList(event.changedTouches);

	Minko.canvas.dispatchEvent(eventCopy);
}

Minko.generateTouchList = function(event) //EMSCRIPTEN
{
	var result = [];

	var copiedTouch = {};

	copiedTouch.identifier = Minko.getTouchId(event.pointerId);
    
	var pageX = 1 + Minko.getOffsetLeft(Minko.iframe) + (event.pageX || event.layerX);
	var pageY = 1 + Minko.getOffsetTop(Minko.iframe) + (event.pageY || event.layerY);

	var screenX = pageX - document.body.scrollLeft;
	var screenY = pageY - document.body.scrollTop;

	copiedTouch.pageX = pageX;
	copiedTouch.pageY = pageY;
	copiedTouch.screenX = screenX;
	copiedTouch.screenY = screenY;
	copiedTouch.clientX = screenX;
	copiedTouch.clientY = screenY;

	result.push(copiedTouch);

    return result;
}

Minko.pointerTouches = [];

Minko.removePointerTouch = function(id)
{
	var pointerTouches = [];

	for(var i = 0; i < Minko.pointerTouches.length; ++i)
	{
		if (Minko.pointerTouches[i].identifier == id)
			continue;
		
		pointerTouches.push(Minko.pointerTouches[i]);
	}

	Minko.pointerTouches = pointerTouches;
}

Minko.redispatchPointerEvent = function(event) //EMSCRIPTEN
{
	var eventCopy = document.createEvent('Event');

	var type = event.type;

	if (event.pointerType == "touch")
	{
		if (type == "pointerdown")
			type = "touchstart";
		else if (type == "pointerup")
			type = "touchend";
		else if (type == "pointermove")
			type = "touchmove";
		else if (type == "pointercancel")
			type = "touchcancel";

		eventCopy.initEvent(type, event.bubbles, event.cancelable);

		var copiedProperties = ['bubbles', 'cancelable', 'view'];

		for(var k in copiedProperties)
			eventCopy[copiedProperties[k]] = event[copiedProperties[k]];

		eventCopy.changedTouches = Minko.generateTouchList(event);

		if (type == "touchend" || type == "touchmove")
		{
			Minko.removePointerTouch(eventCopy.changedTouches[0].identifier);
		}
		if (type == "touchstart" || type == "touchmove")
		{
			Minko.pointerTouches.push(eventCopy.changedTouches[0]);
		}
		
		eventCopy.touches = Minko.pointerTouches.concat();
	}
	else
	{
		if (type == "pointerdown")
			type = "mousedown";
		else if (type == "pointerup")
			type = "mouseup";
		else if (type == "pointermove")
			type = "mousemove";
		else if (type == "pointercancel")
			return;

		var pageX = 1 + Minko.getOffsetLeft(Minko.iframe) + (event.pageX || event.layerX);
		var pageY = 1 + Minko.getOffsetTop(Minko.iframe) + (event.pageY || event.layerY);

		var screenX = pageX - document.body.scrollLeft;
		var screenY = pageY - document.body.scrollTop;

		var eventCopy = document.createEvent('MouseEvents');
		eventCopy.initMouseEvent(type, event.bubbles, event.cancelable, event.view, event.detail,
			pageX, pageY, screenX, screenY, 
			event.ctrlKey, event.altKey, event.shiftKey, event.metaKey, event.button, event.relatedTarget);
	}

	Minko.canvas.dispatchEvent(eventCopy);
}

Minko.bindRedispatchEvents = function() //EMSCRIPTEN
{
	var touchEventsSupported = 'ontouchstart' in window;
	var pointerEventsSupported = 'PointerEvent' in window;

	if (!touchEventsSupported && !pointerEventsSupported)
	{
		var a = ['mousemove', 'mouseup', 'mousedown', 'click'];

		for(var k in a)
			Minko.window.addEventListener(a[k], Minko.redispatchMouseEvent);
	}

	a = ['wheel', 'mousewheel', 'DOMMouseScroll'];

	for(var k in a)
		Minko.window.addEventListener(a[k], Minko.redispatchWheelEvent);

	if (pointerEventsSupported)
	{
		a = ['pointerdown', 'pointerup', 'pointermove', 'pointercancel']

		for(var k in a)
			Minko.window.addEventListener(a[k], Minko.redispatchPointerEvent);
	}
	else if (touchEventsSupported)
	{
		a = ['touchstart', 'touchend', 'touchmove', 'touchcancel']

		for(var k in a)
			Minko.window.addEventListener(a[k], Minko.redispatchTouchEvent);
	}

	a = ['keydown', 'keyup', 'keypress'];

	for(var k in a)
		Minko.window.addEventListener(a[k], Minko.redispatchKeyboardEvent);
}

Minko.changeViewportWidth = function(width)
{
	var metaElement = document.getElementById("metaViewport");

	if (!metaElement)
	{
		metaElement = document.createElement("meta");
		metaElement.id = "metaViewport";
		metaElement.setAttribute("name", "viewport");
		document.head.appendChild(metaElement);
	}

	metaElement.setAttribute("content", "width=" + width);
};

/*
** iOS/OSX
*/
Minko.connectWebViewJavascriptBridge = function(callback) // iOS / OSX
{ 
    if (window.WebViewJavascriptBridge)
    {
        callback(WebViewJavascriptBridge);
    }
    else
    {
        document.addEventListener('WebViewJavascriptBridgeReady', function()
        {
            callback(WebViewJavascriptBridge);
        }, false);
    }
};

/*
** Android
*/

Minko.touchIds = [];

Minko.androidEventHandler = function(event)
{	
	console.log('JS Event: ' + event.type + ' (' + event.currentTarget.minkoName + ')');
	
	// Workaround for API 19 to properly fire touchmove
	if (event.type == "touchstart" || event.type == "touchend")
		event.preventDefault();
	
	eventData = {};
	eventData.type = event.type;
	eventData.clientX = event.clientX;
	eventData.clientY = event.clientY;
	eventData.pageX = event.pageX;
	eventData.pageY = event.pageY;
	eventData.screenX = event.screenX;
	eventData.screenY = event.screenY;
	
	if (event.type.indexOf("touch") != -1)
	{
		eventData.changedTouches = [];
		for (var i = 0; i < event.changedTouches.length; i++)
		{
			var identifier = event.changedTouches[i].identifier;
			
			if (event.type == 'touchstart')
			{
				if (Minko.touchIds.indexOf(identifier) != -1)
					continue;
				Minko.touchIds.push(identifier);
			}
			else if (event.type == 'touchend')
			{
				if (Minko.touchIds.indexOf(identifier) != -1)
					Minko.touchIds.splice(Minko.touchIds.indexOf(identifier), 1);
				else
					continue;
			}
			
			eventData.changedTouches[i] = {};
			eventData.changedTouches[i].clientX = event.changedTouches[i].clientX;
			eventData.changedTouches[i].clientY = event.changedTouches[i].clientY;
			eventData.changedTouches[i].identifier = event.changedTouches[i].identifier;
		}
	}
	
	var jsonStringify = JSON.stringify(eventData);
	console.log(jsonStringify);
	
	MinkoNativeInterface.onEvent(event.currentTarget.minkoName, jsonStringify);
}

Minko.addListenerAndroid = function(accessor, type)
{
	accessor.addEventListener(type, Minko.androidEventHandler);
};

Minko.init = function(platform)
{
	Minko.platform = platform;

	if (platform == "emscripten")
	{
		Minko.createIframe();
	}
	else if (platform == "apple")
	{
		Minko.connectWebViewJavascriptBridge(function(bridge) // iOS / OSX
		{
			Minko.bridge = bridge;

			Minko.loadedHandler();

			bridge.init(function(message, responseCallback)
			{
			});

			bridge.send({type: 'ready', value:'true'});
		});
		
	}
	else if (platform == "androidWebView")
	{
		console.log("Init android!");
		Minko.loadedHandler();
		Minko.addListener = Minko.addListenerAndroid;
		
		Minko.sendMessage = function(message)
		{
			console.log("Send message: " + message);
			MinkoNativeInterface.onMessage(message);
		}
	}
}
