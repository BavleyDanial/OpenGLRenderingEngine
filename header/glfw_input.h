#pragma once

#include <GLFW/glfw3.h>

namespace OGLR {

    class Input {
    public:
        inline static bool KeyPressed(uint32_t key) { return mPressedKeys[key]; }
        inline static bool KeyHeld(uint32_t key) { return glfwGetKey(mCurrentWindow, key) == GLFW_PRESS; }
        inline static bool KeyReleased(uint32_t key) { return mReleasedKeys[key]; }

        inline static bool MouseButtonPressed(uint32_t button) { return mPressedMouseButtons[button]; }
        inline static bool MouseButtonHeld(uint32_t button) { return glfwGetMouseButton(mCurrentWindow, button); }
        inline static bool MouseButtonReleased(uint32_t button) { return mReleasedMouseButtons[button]; }

    protected:
        static void SetCurrentWindow(GLFWwindow* window) { mCurrentWindow = window; }
        static void OnUpdate() {
            glfwPollEvents();
            for (int key = 0; key < NUM_KEYS; key++) {
                int currentKeyState = glfwGetKey(mCurrentWindow, key);

                mPressedKeys[key] =  currentKeyState == GLFW_PRESS && mPreviousKeyState[key] == GLFW_RELEASE;
                mReleasedKeys[key] = currentKeyState == GLFW_RELEASE && mPreviousKeyState[key] == GLFW_PRESS;

                mPreviousKeyState[key] = currentKeyState;
            }

            for (int button = 0; button < NUM_MOUSE_BUTTONS; button++) {
                int currentButtonState = glfwGetMouseButton(mCurrentWindow, button);

                mPressedMouseButtons[button] =  currentButtonState == GLFW_PRESS && mPreviousMouseButtonState[button] == GLFW_RELEASE;
                mReleasedMouseButtons[button] = currentButtonState == GLFW_RELEASE && mPreviousMouseButtonState[button] == GLFW_PRESS;

                mPreviousMouseButtonState[button] = currentButtonState;
            }
        }

        inline static GLFWwindow* mCurrentWindow = nullptr;
    protected:
        inline static const int NUM_KEYS = 348;
        inline static int mPreviousKeyState[NUM_KEYS] = {0};
        inline static bool mPressedKeys[NUM_KEYS] = {false};
        inline static bool mReleasedKeys[NUM_KEYS] = {false};
        inline static const int NUM_MOUSE_BUTTONS= 8;
        inline static int mPreviousMouseButtonState[NUM_MOUSE_BUTTONS] = {0};
        inline static bool mPressedMouseButtons[NUM_MOUSE_BUTTONS] = {false};
        inline static bool mReleasedMouseButtons[NUM_MOUSE_BUTTONS] = {false};
    private:
        friend class Window;
    };

}
