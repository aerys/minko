package minko.plugin.htmloverlay;

import android.app.Activity;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.graphics.Color;
import android.view.View;
import android.webkit.WebSettings;
import android.view.Display;
import android.view.WindowManager;
import android.content.Context;
import android.os.Build;
import java.util.Vector;
import java.util.concurrent.Callable;
import java.util.concurrent.FutureTask;
import java.lang.Exception;

import static android.util.Log.*;
import org.libsdl.app.*;

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
		//layout.setBackgroundColor(Color.RED);
		
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
        _webView.setBackgroundColor(0x00ff0000);
        _webView.setLayerType(WebView.LAYER_TYPE_SOFTWARE, null);

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
			//forceFixedViewport();
		}
		
		// Disable scroll bar
		_webView.setVerticalScrollBarEnabled(false);
		_webView.setHorizontalScrollBarEnabled(false);
		
		// Disable scroll edge gradient
		_webView.setOverScrollMode(View.OVER_SCROLL_NEVER);
		
		// Add the WebView
        layout.addView(_webView);
		
		// Add a JavaScript interface
		_jsInterface = new WebViewJSInterface();		
		_webView.addJavascriptInterface(_jsInterface, "MinkoNativeInterface");
		
		// Increase WebView performances
		_webView.getSettings().setRenderPriority(WebSettings.RenderPriority.HIGH);
        _webView.getSettings().setCacheMode(WebSettings.LOAD_NO_CACHE);
		
		d("MINKOJAVA", "WEBVIEW IS NOW INSTANCIATED: " + _webView);
		webViewInitialized();
    }
	
	/*
	public WebView getWebView()
	{
		d("MINKOJAVA", "RETURN WEBVIEW: " + _webView);
		
		return _webView;
	}
	*/
	
	public String evalJS(String js)
	{
		
		d("MINKOJAVATEST", "Try to evaluate JS: " + js);
		/*String jsResult = _jsInterface.getJSValue(_webView, js);
		d("MINKOJAVATEST", "Result: " + jsResult);
		return jsResult;
		*/
		
		//d("MINKOJAVA", "Try to evaluate JS: " + js);
		
		EvalJSCallable evalJSCallable = new EvalJSCallable(_webView, js);
		FutureTask<String> task = new FutureTask<String>(evalJSCallable);

        _sdlActivity.runOnUiThread(task);
		
		String returnValue = "";
		try 
		{
			returnValue = task.get();
		}
		catch (Exception e) 
		{
			d("MINKOJAVA", "Exception: " + e.toString());
			returnValue = e.getMessage();
		}
		
		//d("MINKOJAVA", "Return value of eval JS result: " + returnValue);
		
		return returnValue;
	}
	
	public void loadUrl(String url)
	{
		d("MINKOJAVA", "TRY TO LOAD THIS URL: " + url);
		d("MINKOJAVA", "WEBVIEW VALUE: " + _webView);
		// It's an operation on the WebView, don't forget to perform it on UI thread!
		LoadUrlRunnable loadUrlRunnable = new LoadUrlRunnable(_webView, url);
		_sdlActivity.runOnUiThread(loadUrlRunnable);
		
		d("MINKOJAVA", "WEBVIEW HAS LOADED AN URL! (" + url + ")");
	}
	
	public void changeResolution(int width, int height)
	{
		if (_webView == null)
			return;
			
		d("MINKOJAVA", "Change the resolution with these values: " + width + ", " + height);

		ChangeResolutionRunnable changeResolutionRunnable = new ChangeResolutionRunnable(_webView, width, height);
		_sdlActivity.runOnUiThread(changeResolutionRunnable);
	}
	
	private int getScale()
    {
        Display display = ((WindowManager) _sdlActivity.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
        int width = display.getWidth();
        Double val = new Double(width) / 800d;
        val = val * 100d;
        return val.intValue();
    }

    private void forceFixedViewport()
    {
        WebSettings settings = _webView.getSettings();

        settings.setLoadWithOverviewMode(false);
        // Activating viewport on Android 2.x will deactivate stretching
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.HONEYCOMB)
        {
            // Set default zoom to unzoom, no setting this will sometimes trigger zoom 100% on some phone (like double tap)
            // It seems to glitch on Android 2.x, a white screen will appear after enabling this option
            //settings.setDefaultZoom(WebSettings.ZoomDensity.FAR);
            // Force using viewport html statement, sadly it activates double tap to zoom
            settings.setUseWideViewPort(true);
        }
        // Try not to use default zoom (useful ?)
        settings.setSupportZoom(false);
        settings.setBuiltInZoomControls(false);
        // Set scale on devices that supports it
        _webView.setPadding(0, 0, 0, 0);

        //Enable DOM storage, and tell Android where to save the Database
        //settings.setDatabasePath("/data/data/" + this.getPackageName() + "/databases/");

        int percentScale = getScale();
        _webView.setInitialScale(percentScale);
    }

    private void defaultFixedViewport()
    {
        WebSettings settings = _webView.getSettings();
        settings.setUseWideViewPort(true);
        settings.setLoadWithOverviewMode(true);
    }
}