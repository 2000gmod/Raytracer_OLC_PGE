# CPU Raytracer

A real-time raytracer that runs on the CPU, using OLC's Pixel Game Engine as windowing backend and nlohmann's *json* library for parsing. Based on *Raytracing in One Weekend*.

## Configuration
The file ```rtconfig.json``` provides configuration for the program.

The following fields are available:

- **width**, **height**: The width and height in (screen) pixels for the render window.

- **pixSize**: Size of each render window pixel in screen pixels (higher means lower resolutions).

- **renderTileSize**: Upper limit for the size of each rendering tile (in render window pixels).

- **renderThreads**: How many threads to use for rendering, will default to available CPU cores if set to 0.

- **memoryStrategy**: Switches between standard memory allocation ("heap") or an experimental arena-like allocator ("arena"). **Recommended to leave at "heap" unless rendering very simple scenes!**

- **scene**: Scene file to load, must be inside the ```scenes``` subdirectory.

## Controls
- **Escape**: Exits program.
- **W, A, S, D**: Forwards, left, backwards and right movement.
- **Shift, Ctrl**: Up and down movement.
- **Arrow keys**: Camera rotation.
- **X, Z**: Increase and decrease focus distance.
- **M, N**: Increase and decrease camera aperture.
- **L, K**: Increase and decrease camera aperture blade count; If set to 2, this will simulate a perfect circular aperture.
- **F**: Set focus distance to object in center of screen.
- **F1**: Enable/disable info and screen helpers.
- **T**: Show rendering regions.

## Custom scenes

In the ```scenes``` directory, users may put custom scenes. 

Scene file format documentation coming soon, for now, use the provided example.