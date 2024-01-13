#pragma once

#include <iostream>
#include <GLFW/glfw3.h>

class Player;

Player *get_player(GLFWwindow *window);
void setup_player_callbacks(GLFWwindow *window, Player *player);

void glfw_error_callback(int error, const char *description);
void glfw_window_size_callback(GLFWwindow *window, int width, int height);

void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void glfw_mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void glfw_mouse_position_callback(GLFWwindow *window, double xpos, double ypos);
void glfw_mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
