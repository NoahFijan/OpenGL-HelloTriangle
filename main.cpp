#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cmath>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        createShader();
        createVertexBuffer();
        mainLoop();
    }

private:
    GLFWwindow* window;
    uint32_t VBO;
    uint32_t VAO;
    uint32_t shaderProgram;
    float rotationDeg = 0.01f;
    float vertices[9] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    };

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            throw std::runtime_error("Failed to initialize glad!");
        }

        glViewport(0, 0, WIDTH, HEIGHT);
    }

    void rotateCcwYAxis() {
        for (int i = 0; i < 8; i+=3) {
            float x, z;
            x = vertices[i]*cos(rotationDeg) + vertices[i+2]*sin(rotationDeg);
            z = vertices[i]*sin(rotationDeg)*(-1) + vertices[i+2]*cos(rotationDeg);
            vertices[i] = x;
            vertices[i+2] = z;
        }
    }

    void rotateCcwXAxis() {
        for (int i = 1; i < 8; i+=3) {
            float y, z;
            y = vertices[i]*cos(rotationDeg) - vertices[i+1]*sin(rotationDeg);
            z = vertices[i]*sin(rotationDeg) + vertices[i+1]*cos(rotationDeg);
            vertices[i] = y;
            vertices[i+1] = z;
        }
    }

    void rotateCwYAxis() {
        for (int i = 0; i < 8; i+=3) {
            float x, z;
            x = vertices[i]*cos(rotationDeg*(-1)) + vertices[i+2]*sin(rotationDeg*(-1));
            z = vertices[i]*sin(rotationDeg*(-1))*(-1) + vertices[i+2]*cos(rotationDeg*(-1));
            vertices[i] = x;
            vertices[i+2] = z;
        }
    }

    void rotateCwXAxis() {
        for (int i = 1; i < 8; i+=3) {
            float y, z;
            y = vertices[i]*cos(rotationDeg*(-1)) - vertices[i+1]*sin(rotationDeg*(-1));
            z = vertices[i]*sin(rotationDeg*(-1)) + vertices[i+1]*cos(rotationDeg*(-1));
            vertices[i] = y;
            vertices[i+1] = z;
        }
    }

    void processInput() {
        //process key input here, example:
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
        if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            rotateCcwYAxis();
        }
        if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            rotateCcwXAxis();
        }
        if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            rotateCwYAxis();
        }
        if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            rotateCwXAxis();
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    }

    void createVertexBuffer() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        glBindVertexArray(0); 
    }

    void readFile(const std::string& filename, std::string* outputString) {
        std::ifstream file(filename);

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open shaderfile!");
        }

        std::string line;

        while (getline(file, line)) {
            outputString->append(line).append("\n");
        }

        file.close();
    }

    void createShader() {
        uint32_t vertexShader;
        uint32_t fragmentShader;
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        std::string vertexShaderSource;
        std::string fragmentShaderSource;

        readFile("shaders/shader.vert", &vertexShaderSource);
        readFile("shaders/shader.frag", &fragmentShaderSource);

        const GLchar* vertSource = (const GLchar*)vertexShaderSource.c_str();
        const GLchar* fragSource = (const GLchar*)fragmentShaderSource.c_str();

        glShaderSource(vertexShader, 1, &vertSource, NULL);
        glShaderSource(fragmentShader, 1, &fragSource, NULL);

        glCompileShader(vertexShader);
        glCompileShader(fragmentShader);

        int success;
        char infoLog[512];

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        glGetProgramiv(fragmentShader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        }

        shaderProgram = glCreateProgram();

        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glClear(GL_COLOR_BUFFER_BIT);

            processInput();

            glUseProgram(shaderProgram);
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            glfwSwapBuffers(window);

            glfwPollEvents();
        }

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);

        glfwTerminate();
    }

};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}