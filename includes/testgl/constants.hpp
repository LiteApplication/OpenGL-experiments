#pragma once

#define WINDOW_TITLE "TestGL - OpenGL Sandbox"
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800

#define GLFW_VSYNC 1 // 0 = off, 1 = on
#define CAPTURE_MOUSE true

#ifndef NDEBUG
#define GLFW_REQUEST_DEBUG_CONTEXT true
#endif

#define CHUNK_SIZE 16           // in voxels
#define VIEW_DISTANCE 15        // in chunks
#define HEIGHT_VIEW_REDUCTION 2 // 1 = no reduction, 2 = half, 3 = third, etc.

#define GEN_ALL_CHUNKS_ON_START false

#define TICKS_PER_SECOND 20
#define CHUNK_GEN_PER_TICK 128
#define CHUNK_MESH_PER_TICK 256
#define CHUNK_GPU_UPLOAD_PER_FRAME 64
