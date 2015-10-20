mergeInto(LibraryManager.library, {
    emscripten_async_wget3_data: function(url, request, param, additionalHeader, arg, free, onload, onerror, onprogress) {
        var _url = Pointer_stringify(url);
        var _request = Pointer_stringify(request);
        var _param = Pointer_stringify(param);

        var http = new XMLHttpRequest();
        http.open(_request, _url, true);
        http.responseType = 'arraybuffer';

        var handle = Browser.getNextWgetRequestHandle();

        // LOAD
        http.onload = function http_onload(e) {
            if (http.status == 200 || http.status == 206 || _url.substr(0,4).toLowerCase() != "http") {
                var byteArray = new Uint8Array(http.response);
                var buffer = _malloc(byteArray.length);
                HEAPU8.set(byteArray, buffer);
                if (onload) Runtime.dynCall('viiii', onload, [handle, arg, buffer, byteArray.length]);
                if (free) _free(buffer);
            } else {
                if (onerror) Runtime.dynCall('viiii', onerror, [handle, arg, http.status, http.statusText]);
            }
            delete Browser.wgetRequests[handle];
        };

        // ERROR
        http.onerror = function http_onerror(e) {
            if (onerror) {
                Runtime.dynCall('viiii', onerror, [handle, arg, http.status, http.statusText]);
            }
            delete Browser.wgetRequests[handle];
        };

        // PROGRESS
        http.onprogress = function http_onprogress(e) {
            if (onprogress) Runtime.dynCall('viiii', onprogress, [handle, arg, e.loaded, e.lengthComputable || e.lengthComputable === undefined ? e.total : 0]);
        };

        // ABORT
        http.onabort = function http_onabort(e) {
            delete Browser.wgetRequests[handle];
        };

        // Useful because the browser can limit the number of redirection
        try {
            if (http.channel instanceof Ci.nsIHttpChannel)
            http.channel.redirectionLimit = 0;
        } catch (ex) { }

        try {
            var additionalHeaderObject = JSON.parse(Pointer_stringify(additionalHeader));
            for (var entry in additionalHeaderObject) {
                http.setRequestHeader(entry, additionalHeaderObject[entry]);
            }
        } catch (ex) { }

        if (_request == "POST") {
            //Send the proper header information along with the request
            http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
            http.setRequestHeader("Content-length", _param.length);
            http.setRequestHeader("Connection", "close");
            http.send(_param);
        } else {
            http.send(null);
        }

        Browser.wgetRequests[handle] = http;

        return handle;
    }
});