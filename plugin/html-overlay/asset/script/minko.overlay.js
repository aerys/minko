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

	Minko.window.dispatchEvent(new Event('minkoReady'));
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

	if (event.type == 'mousewheel' || event.type == "DOMMouseScroll")
	{

		var copiedProperties = ['detail', 
		'wheelDelta', 'wheelDeltaX', 'wheelDeltaY', 'wheelDeltaZ', 
		'delta', 'deltaMode', 'deltaX', 'deltaY', 'deltaZ', 
		'which', 'key', 'detail', 'keyIdentifier'];

		for(var k in copiedProperties)
			eventCopy[copiedProperties[k]] = event[copiedProperties[k]];

		event.preventDefault();
	}

	Minko.canvas.dispatchEvent(eventCopy);
}

Minko.bindRedispatchEvents = function() //EMSCRIPTEN
{
	var a = ['mousemove', 'mouseup', 'mousedown', 'click', 'mousewheel', 'DOMMouseScroll'];

	for(var k in a)
		Minko.window.addEventListener(a[k], Minko.redispatchMouseEvent);

	a = ['keydown', 'keyup', 'keypress'];

	for(var k in a)
		Minko.window.addEventListener(a[k], Minko.redispatchKeyboardEvent);
}

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
}

Minko.connectWebViewJavascriptBridge(function(bridge) // iOS / OSX
{
    Minko.bridge = bridge;

    Minko.loadedHandler();

    bridge.init(function(message, responseCallback)
    {
        if (responseCallback)
        {
        }
    });

    bridge.send({type: 'ready', value:'true'});
});

Minko.init = function(platform)
{
	Minko.platform = platform;

	if (platform == "emscripten")
	{
		Minko.createIframe();
	}
	else if (platform == "macWebView")
	{
		Minko.loadedHandler();
	}
}