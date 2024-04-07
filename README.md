# Voxel Renderer

This is a simple voxel renderer implemented in C++, utilizing OpenGL for drawing the scene. It features a separate thread for chunk building and rendering, a basic wave shader, and random terrain generation using Perlin noise.

## Dependencies

- CMake: Make sure you have CMake installed to build the project.
- GLFW: This project utilizes GLFW for window creation and OpenGL context management.
- GLM: GLM (OpenGL Mathematics) is used for mathematics operations in OpenGL applications.

Make sure to install these dependancies as the program cannot be built without them


**Arch Linux :**
```bash
sudo pacman -S glfw glm cmake gcc
```

## Building Instructions

1. Clone this repository to your local machine.

  ```bash
  git clone https://github.com/LiteApplication/OpenGL-experiments
  ```
2. Navigate to the cloned repository directory.

  ```bash
  cd OpenGL-experiments
  ```
3. Create a build directory and navigate into it.

  ```bash
  mkdir build
  cd build
  ```
4. Run CMake to generate build files.

  ```bash
  cmake ..
  ```
5. Build the project using your preferred build system (Makefiles, Visual Studio solution, etc.).

  ```bash
  cmake --build .
  ```

6. After successful compilation, the executable will be generated in the `build` directory.

## Usage

1. Run the executable generated after building the project.

   ```bash
   ./TestGL
   ```

2. Interact with the renderer using the provided controls or interface.

## Controls

- Use `ZQSD` to navigate the scene
- `Scroll` to change the field of view (zoom in and out)
- Use `SPACE` and `SHIFT` to go up and down
- Use `CTRL` to go faster
- Use `H` to go even faster

## Credits

This voxel renderer project was created by [LiteApplication]. It is inspired by [learnopengl.com](https://learnopengl.com/).

## License

This project is licensed under the [MIT License](LICENSE). Feel free to use and modify it according to your needs.
