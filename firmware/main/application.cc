#include "application.h"
#include "board.h"

Application::Application() {
    // Constructor implementation
}

Application::~Application() {
    // Destructor implementation
}


void Application::Initialize() {
    // Initialize application resources here
    Board::GetInstance().initialize();
}

void Application::Run() {
    
    // Main application loop
}