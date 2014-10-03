package minko.plugin.htmloverlay;

import android.app.Activity;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.graphics.Color;
import android.view.View;

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
		layout.setBackgroundColor(Color.RED);
		
		// Create the WebView from SDLActivity context
		_webView = new WebView(SDLActivity.getContext());
		
		// Enable the JS for the WebView
        _webView.getSettings().setJavaScriptEnabled(true);
        WebChromeClient wcc = new WebChromeClient();
        _webView.setWebChromeClient(wcc);
		
		// Set our own WebViewClient to override some 
		// methods and inject Minko overlay JS script
		_webView.setWebViewClient(new MinkoWebViewClient());
		
        // Transparent background
        _webView.setBackgroundColor(0xaaff0000);
        _webView.setLayerType(WebView.LAYER_TYPE_SOFTWARE, null);

		// Scale to fit the page
        _webView.getSettings().setLoadWithOverviewMode(true);
        _webView.getSettings().setUseWideViewPort(true);

		// Disable zoom
		_webView.getSettings().setSupportZoom(false);
		_webView.getSettings().setBuiltInZoomControls(true);
		_webView.getSettings().setDisplayZoomControls(false);
		
		// Disable zoom when double tap <= only on simulator?
		//_webView.getSettings().setUseWideViewPort(false);
		
		// Disable scroll bar
		_webView.setVerticalScrollBarEnabled(false);
		_webView.setHorizontalScrollBarEnabled(false);
		
		// Disable scroll edge gradient
		_webView.setOverScrollMode(View.OVER_SCROLL_NEVER);
		
		// Add the WebView
        layout.addView(_webView);
		
		// Add a JavaScript interface
		_webView.addJavascriptInterface(new WebViewJSInterface(), "MinkoNativeInterface");
		
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
}