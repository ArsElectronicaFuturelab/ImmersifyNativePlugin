# ImmersifyNativePlugin - How To Use

In order to build the project the 'native plugin' has to be located in the same folder as the unity project folder (UnityImmersifyPlayer). 
If you have cloned the public unity project, the native plugin folder should be located in the same folder as 'UnityImmersifyPlayer' and 'Spin License Tools'.

The VS project automatically copies the built .dll into the unity project (.\UnityImmersifyPlayer\Assets\Immersify\Plugins\x86_64). 
Please note that unity ignores the ImmersifyUnityPlugin_d.dll (debug-build), you either have to manually rename it or use a release build.

In order to generate the VS solution first open an elevated powershell prompt and navigate to the prj folder of the native plugin. 
In there should be the folder CMake and CMakeLists.txt.
With the latest CMake installed, enter the following command into the shell:

cmake -G "Visual Studio 15 2017 Win64" -B .

Now you should be able to build the project and automatically copy the generated .dll into the unity project. Keep in mind that there should be no unity instance running, otherwise the copy process doesn't complete.

