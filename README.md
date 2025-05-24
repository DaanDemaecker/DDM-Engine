# DDM3-Lite-Engine  

## Motivation
During my Game Development course at [Digital Arts and Entertainment](https://www.digitalartsandentertainment.be) in Belgium, we made our own basic 2D engine in our Programming 4 class.
This inspired me to make my own basic 3D engine.
In my Graphics Programming class, we learned about DirectX and made our own [software rasterizer](https://github.com/DaanDemaecker/DualRasterizer), this made me look more into graphics programming and graphics API's.
After some research, I decided to use the [Vulkan](https://www.vulkan.org) API to render.
As I was new to Vulkan I started by creating a separate [renderer](https://github.com/DaanDemaecker/VulkanRenderer.git) that turned into a testing project, which can also be found on my GitHub.


# Capabilities

- ***Component-based system***  
      The player can create his own components and attach them to a game object to add custom functionality.

- ***Model Loading***  
     Loading and rendering 3D Models, supported types:
    - .obj
    - .fbx
    - .gltf

- ***Texture loading***  
    Loading image files as Textures using the [**STB**](https://github.com/nothings/stb) library

- ***Skybox Display***  
   A cubemap can be attached to any camera to display a skybox.

- ***GUI***  
ImGui has been implemented and can be used for debugging purposes or anything else the user wants to.

- ***Global light***  
Currently, there is a single global directional light that is used for shading in any way the user wants to. The light has a direction, color, and intensity that can be customized

## Future goals
Future goals for my engine include:

- Sound system using fmod


## Object Loader
For my Graphics Programming 2 course, we used Vulkan in class, for the first time for our school. We were tasked to make a project to showcase our knowledge.
As the choice of project was very free, it took me a while to choose, and considering the amount of time we got for this assignment, I decided to make a runtime model and material loader.
I used the [Dear Imgui](https://github.com/ocornut/imgui) library to make a simple interface to load the objects and Textures.
