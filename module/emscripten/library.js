mergeInto(LibraryManager.library, {
    emscripten_async_wget3_data: function(url, request, param, additionalHeader, arg, free, onload, onerror, onprogress) {
        var _url = UTF8ToString(url);
        var _request = UTF8ToString(request);
        var _param = UTF8ToString(param);

        var http = new XMLHttpRequest();
        http.open(_request, _url, true);
        http.responseType = 'arraybuffer';

        var handle = Browser.getNextWgetRequestHandle();

        // LOAD
        http.onload = function http_onload(e) {
            if (http.status >= 200 && http.status < 300 || _url.substr(0,4).toLowerCase() != "http") {
                var byteArray = new Uint8Array(http.response);
                var buffer = _malloc(byteArray.length);
                HEAPU8.set(byteArray, buffer);
                if (onload) dynCall_viiii(onload, handle, arg, buffer, byteArray.length);
                if (free) _free(buffer);
            } else {
                if (onerror) dynCall_viiii(onerror, handle, arg, http.status, http.statusText);
            }
            delete Browser.wgetRequests[handle];
        };

        // ERROR
        http.onerror = function http_onerror(e) {
            if (onerror) {
                dynCall_viiii(onerror, handle, arg, http.status, http.statusText);
            }
            delete Browser.wgetRequests[handle];
        };

        // PROGRESS
        http.onprogress = function http_onprogress(e) {
            if (onprogress) dynCall_viiii(onprogress, handle, arg, e.loaded, e.lengthComputable || e.lengthComputable === undefined ? e.total : 0);
        };

        // ABORT
        http.onabort = function http_onabort(e) {
            delete Browser.wgetRequests[handle];
        };

        try {
            var setContentType = true;
            var additionalHeaderObject = JSON.parse(UTF8ToString(additionalHeader));
            for (var entry in additionalHeaderObject) {
                if (entry.toLowerCase() == 'content-type')
                    setContentType = false;
                http.setRequestHeader(entry, additionalHeaderObject[entry]);
            }
        } catch (ex) { }

        if (_request == "POST") {
            //Send the proper header information along with the request
            if (setContentType)
                http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");

            http.send(_param);
        } else {
            http.send(null);
        }

        Browser.wgetRequests[handle] = http;

        return handle;
    }
});
