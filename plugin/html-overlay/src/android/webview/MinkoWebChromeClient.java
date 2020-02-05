package minko.plugin.htmloverlay;

import android.webkit.JsResult;
import android.webkit.WebChromeClient;
import android.webkit.PermissionRequest;
import android.webkit.WebView;
import android.util.Log;

final class MinkoWebChromeClient extends WebChromeClient
{
    @Override
    public void onPermissionRequest(PermissionRequest request) {
        // Allow HTML5 Camera API.
        request.grant(request.getResources());
    }
}