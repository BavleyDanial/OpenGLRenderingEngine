#pragma once
#include <vector>
#include <cstdint>

static std::vector<float> cube {
    // front
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,       // 0
     0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,       // 1
     0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f,       // 2
    -0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f,       // 3

    // top
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,     // 4
     0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,     // 5
     0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,     // 6
    -0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,     // 7

    // left
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,     // 8
    -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,     // 9
    -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,     // 10
    -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,     // 11

    // right
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,     // 12
    0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,     // 13
    0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,     // 14
    0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,     // 15

    // back
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,     // 16
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,     // 17
     0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,     // 18
    -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,     // 19

    // bottom
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,     // 20
     0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,     // 21
     0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,     // 22
    -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,     // 23
};

static std::vector<uint32_t> cube_indices {
    0, 1, 2, // first triangle front
    2, 3, 0, // second triangle front

    6, 5, 4, // first triangle top
    4, 7, 6, // second triangle top

    10, 9, 8,  // first triangle left
    8, 11, 10, // second triangle left

    12, 13, 14, // first triangle right
    14, 15, 12, // second triangle right

    18, 17, 16, // first triangle back
    16, 19, 18, // second triangle back

    20, 21, 22, // first triangle bottom
    22, 23, 20, // second triangle bottom
};
