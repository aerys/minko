package minko.plugin.htmloverlay;

import android.webkit.JsResult;
import android.webkit.WebChromeClient;
import android.webkit.WebView;

import static android.util.Log.*;

final class MinkoWebChromeClient extends WebChromeClient
{
    @Override
    public boolean onJsAlert(WebView view, String url, String message, JsResult result)
    {
        d("MINKOCHROMECLIENT", message);
        result.confirm();
        return true;
    }
}