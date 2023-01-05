package minko.plugin.htmloverlay;

import android.webkit.SslErrorHandler;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.net.http.SslError;
import android.util.Base64;
import android.util.Log;
import android.webkit.JsResult;
import java.io.IOException;
import java.io.InputStream;

public class MinkoWebViewClient extends WebViewClient
{
    // Native functions
    public native void webViewPageLoaded();

    @Override
    public boolean shouldOverrideUrlLoading(WebView view, String url)
    {
        view.loadUrl(url);

        return true;
    }

    @Override
    public void onReceivedError (WebView view, int errorCode, String description, String failingUrl)
    {
        Log.i("minko-java", "[MinkoWebViewClient] onReceivedError: " + description);
    }

    @Override
    public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
        // FIXME: This is totally unsafe. We should ensure this handler only exists for local builds.
        handler.proceed(); // Ignore SSL certificate errors
    }

       @Override
    public void onScaleChanged(WebView view, float oldScale, float newScale)
    {
        view.zoomOut();
    }

    @Override
    public void onPageFinished(WebView view, String url)
    {
        if (!url.contains("#") && view.getProgress() == 100)
        {
            super.onPageFinished(view, url);

            // Inject JS Minko script into the webview
            injectScriptFile(view, "script/minko.overlay.js");


            // Load URL provided inside the webview
            String evalString = "javascript: Minko.init('androidWebView');";
            view.loadUrl(evalString);

            // Call native function to inform C++ code that the page is loaded
            webViewPageLoaded();

            Log.i("minko-java", "[MinkoWebViewClient] Page has FINISHED to load (url: " + url + ").");
        }
    }

    private void injectScriptFile(WebView view, String scriptFile)
    {
        InputStream input;

        try
        {
            // Get the content of the script file
            input = view.getContext().getAssets().open(scriptFile);
            byte[] buffer = new byte[input.available()];
            input.read(buffer);
            input.close();

            // String-ify the script byte-array using BASE64 encoding !!!
            String encoded = Base64.encodeToString(buffer, Base64.NO_WRAP);
            view.loadUrl("javascript:(function() {" +
                    "var parent = document.getElementsByTagName('head').item(0);" +
                    "var script = document.createElement('script');" +
                    "script.type = 'text/javascript';" +
                    // Tell the browser to BASE64-decode the string into your script !!!
                    "script.innerHTML = window.atob('" + encoded + "');" +
                    "parent.appendChild(script)" +
                    "})()"
            );
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }
}
