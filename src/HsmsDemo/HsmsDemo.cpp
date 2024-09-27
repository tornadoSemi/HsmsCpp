
#include <iostream>
#include <windows.h>
#include "CHsmsEquipment.h"

int main()
{


    CHsmsEquipment equip;
    equip.Start();

    while (1)
    {
        Sleep(1000);
    }

    equip.Stop();
}

