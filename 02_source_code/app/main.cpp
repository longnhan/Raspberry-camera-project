#include "main.h"

Button test(SHUTTER_BUTTON, GPIO_INPUT, EN_ACTIVE_HIGH);

void initializeModules() 
{

}

int main() 
{
    // Initialize all modules
    initializeModules();

    std::cout << "Application started!" << std::endl;
    
    while (true)
    {

    }

    return 0;
}
