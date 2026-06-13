#pragma once


class Application
{
public:
    static Application& GetInstance() {
        static Application instance;
        return instance;
    }
    // Delete copy constructor and assignment operator
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    /**
     * Initialize the application
     */
    void Initialize();
    void Run();

private:
    Application();
    ~Application();
};