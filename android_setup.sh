#! /bin/sh

cp -vruf framework/{lib,src} android/example-sponza/jni/framework/
cp -vruf framework/{lib,src} android/example-cube/jni/framework/
cp -vruf framework/effect android/example-sponza/assets/
cp -vruf framework/effect android/example-cube/assets/
cp -vruf plugins/*/{lib,src} android/example-sponza/jni/plugin/
cp -vruf plugins/*/{lib,src} android/example-cube/jni/plugin/
cp -vruf examples/cube/src/* android/example-cube/jni/cube/
cp -vruf examples/cube/asset/* android/example-cube/assets/
cp -vruf examples/sponza/src/* android/example-sponza/jni/sponza/
cp -vruf examples/sponza/asset/* android/example-sponza/assets/
