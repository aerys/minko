package minko.plugin.htmloverlay;

import android.graphics.Color;
import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.os.Build;
import android.util.Log;
import android.view.ViewGroup;
import android.view.View;
import android.view.Display;
import android.view.MotionEvent;
import android.view.WindowManager;
import android.widget.LinearLayout;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebSettings;
import java.util.Vector;
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;
import java.lang.Exception;
import org.libsdl.app.*;
import android.webkit.CookieManager;

public class InitWebViewTask implements Runnable
{
    private Activity _sdlActivity;
    private WebView _webView;
    private WebViewJSInterface _jsInterface;

    // Native functions
    public native void webViewInitialized();

    public InitWebViewTask(Activity sdlActivity)
    {
        _sdlActivity = sdlActivity;
        _webView = null;
    }

    @Override
    public void run()
    {
        ViewGroup layout = SDLActivity.getLayout();

        // Create the WebView from SDLActivity context
        _webView = new MinkoWebView(SDLActivity.getContext());

        // Enable the JS for the WebView
        _webView.getSettings().setJavaScriptEnabled(true);
        WebChromeClient wcc = new WebChromeClient();
        _webView.setWebChromeClient(wcc);

        // Set our own WebViewClient to override some
        // methods and inject Minko overlay JS script
        _webView.setWebViewClient(new MinkoWebViewClient());

        // Transparent background
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.JELLY_BEAN)
        {
            _webView.setBackgroundColor(Color.argb(1, 0, 0, 0));
            _webView.setLayerType(WebView.LAYER_TYPE_SOFTWARE, null);
        }
        else
        {
            _webView.setBackgroundColor(0x00ff0000);
            _webView.setLayerType(WebView.LAYER_TYPE_HARDWARE, null);
        }

        // Scale to fit the page
        _webView.getSettings().setLoadWithOverviewMode(true);
        _webView.getSettings().setUseWideViewPort(true);

        // Disable zoom
        _webView.getSettings().setSupportZoom(false);
        _webView.getSettings().setBuiltInZoomControls(true);
        _webView.getSettings().setDisplayZoomControls(false);

        // Disable zoom when double tap (only for API < 19)
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.KITKAT)
        {
            _webView.getSettings().setUseWideViewPort(false);
            defaultFixedViewport();
        }

        CookieManager.getInstance().setAcceptFileSchemeCookies(true);
        CookieManager.getInstance().setAcceptCookie(true);

        _webView.requestFocus(View.FOCUS_DOWN);

        // Add the WebView
        layout.addView(_webView);

        // Add a JavaScript interface
        _jsInterface = new WebViewJSInterface(_sdlActivity);
        _webView.addJavascriptInterface(_jsInterface, "MinkoNativeInterface");

        // Increase WebView performances
        _webView.getSettings().setRenderPriority(WebSettings.RenderPriority.HIGH);
        _webView.getSettings().setCacheMode(WebSettings.LOAD_DEFAULT);

        // Don't require user interaction to play media content
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.JELLY_BEAN_MR1)
        {
            _webView.getSettings().setMediaPlaybackRequiresUserGesture(false);
        }

        // Allow remote WebView debugging with Chrome DevTools for Android > 4.4
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT)
        {
            WebView.setWebContentsDebuggingEnabled(true);
        }

        Log.i("minko-java", "[InitWebViewTask] WebView is now instantiated: " + _webView + ".");
        webViewInitialized();
    }

    public String evalJS(String js)
    {
        EvalJSCallable evalJSCallable = new EvalJSCallable(_webView, js);
        FutureTask<String> task = new FutureTask<String>(evalJSCallable);

        // Run the task to evaluate JS
        _sdlActivity.runOnUiThread(task);

        String returnValue = "";

        try
        {
            returnValue = task.get();
        }
        catch (Exception e)
        {
            Log.i("minko-java", "[InitWebViewTask] Exception: " + e.toString());
            returnValue = e.getMessage();
        }

        return returnValue;
    }

    public void loadUrl(String url)
    {
        // It's an operation on the WebView, don't forget to perform it on UI thread!
        LoadUrlRunnable loadUrlRunnable = new LoadUrlRunnable(_webView, url);
        _sdlActivity.runOnUiThread(loadUrlRunnable);

        Log.i("minko-java", "[InitWebViewTask] WebView has loaded an URL! (" + url + ")");
    }

    public void changeResolution(int width, int height)
    {
        if (_webView == null)
            return;

        ChangeResolutionRunnable changeResolutionRunnable = new ChangeResolutionRunnable(_webView, width, height);
        _sdlActivity.runOnUiThread(changeResolutionRunnable);
    }

    public void hide(boolean value)
    {
        if (value)
            _webView.setVisibility(View.VISIBLE);
        else
            _webView.setVisibility(View.GONE);
    }

    private int getScale()
    {
        Display display = ((WindowManager) _sdlActivity.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        int width = display.getWidth();
        Double val = new Double(width) / 800d;
        val = val * 100d;
        return val.intValue();
    }

    private void defaultFixedViewport()
    {
        WebSettings settings = _webView.getSettings();
        settings.setUseWideViewPort(true);
        settings.setLoadWithOverviewMode(true);
    }
}