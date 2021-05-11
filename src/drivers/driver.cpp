#include <drivers/driver.h>

using namespace byos::drivers;

Driver::Driver()
{

}

Driver::~Driver()
{

}

void Driver::Activate()
{

}

void Driver::Deactivate()
{

}

int Driver::Reset()
{
    return 0;
}

DriverManager::DriverManager()
{
    numDrivers = 0;
}

void DriverManager::AddDriver(Driver* driver)
{
    drivers[numDrivers] = driver;
    ++numDrivers;
}

void DriverManager::ActivateAll()
{
    for(int i = 0; i < numDrivers; ++i)
    {
        drivers[i]->Activate();
    }
}
