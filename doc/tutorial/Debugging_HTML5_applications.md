Before running any application in HTML5, you should always debug it in native and ensure everything works fine before trying anything in HTML5.

However, when running in HTML5, your application might behave differently than in native. Some code might work differently or not at all, and knowing what is happening can be complicated.

First of all, you should read the [Code guidelines and limitations](https://github.com/kripken/emscripten/wiki/CodeGuidelinesAndLimitations) and [Browser limitations](https://github.com/kripken/emscripten/wiki/Browser-limitations) pages from the Emscripten wiki.

General tricks
--------------

Here are some advice on debugging your applications in HTML5.

### HTML5 specific code

In your application, if you need some code to be executed solely when running in HTML5, you can use the `EMSCRIPTEN` macro 
```cpp
#if defined(EMSCRIPTEN) //my code for HTML5 #endif

#if !defined(EMSCRIPTEN) //my code for any target but HTML5 #endif 
```


### Outputting messages in the console

In debug, you can display messages in the textarea below the canvas. Just output the message to the standard output to have them displayed there. Be sure to end your message with an end of line character so that it is displayed. 
```cpp
std::cout << "my message" << std::endl 
```
 This is a good way to know if your application goes inside a particular function.

### Executing javascript code

Emscripten provides a way to execute javascript code via the `emscripten_run_script` function 
```cpp
emscripten_run_script("alert('foo');console.log('bar');"); 
```
 You can visit the [Interacting with code](https://github.com/kripken/emscripten/wiki/Interacting-with-code) page on the Emscripten wiki for more detailed techniques to interact with Javascript code.

### Watch out for return !

In native, reaching a `return` instruction in the `main` would result in the program to close. In HTML5, it is not as visible. When the `main` returns, the page is not closed, but your application will no longer be executed. If your application doesn't run normally and you can't find an obvious reason, try displaying a message right before any `main` `return` instruction, that way you will see instantly if the program returns.

```cpp
void main(int argc, char* argv) {

   //some code
   std::cout << "application returned" << std::endl;
   return 0;

} 
```


Lower level techniques
----------------------

If you're running into a more complex problem, you might need to have a better insight at what is going on. For optimization reasons, the generated Javascript code is greatly minified and impossible to read. If you need to have a better look at which functions are called, and what function caused an exception, you can tweak some parameters in emscripten.

In `%EMSCRIPTEN_HOME%/src/settings.js`, there are a lot of parameters you can tweak to have more or less optimizations on the generated code, and more or less information displayed in the console. Every parameter comes with a quite detailed description of its use and effect. Here are some useful parameters :

-   `EXCEPTION_DEBUG`: when set to 1, exceptions will be outputed in the console, with a more or less readable callstack
-   `LABEL_DEBUG`: when set to 1, prints function names as you enter them
-   `RUNNING_JS_OPTS`: when set to 0, the Javascript code will not be optimized and will keep comprehensive function names
-   `EXECUTION_TIMEOUT`: throw an exception after X seconds - useful to debug infinite loops

You will need to recompile your application for those paramters to take effect. Keep in mind that less optimized javascript will result in a bigger file and might lead to more problems with your browser.

If you want more thorought techniques for debugging your HTML5 applications, you can visit the the [Debugging](https://github.com/kripken/emscripten/wiki/Debugging) dedicated page on the Emscripten wiki. Keep in mind though that these techniques will require you to alter the compilation process, so use them only if you know what you are doing.

