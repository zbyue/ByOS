#ifndef __BYOS__DRIVERS__DRIVER_H
#define __BYOS__DRIVERS__DRIVER_H

namespace byos
{
    namespace drivers
    {
        class Driver
        {
        public:
            Driver();
            ~Driver();

            virtual void Activate();
            virtual int Reset();
            virtual void Deactivate();

        };

        class DriverManager
        {
        private:
            Driver* drivers[256];
            int numDrivers;

        public:
            DriverManager();
            void AddDriver(Driver*);
            void ActivateAll();
        };
    }
}

#endif
