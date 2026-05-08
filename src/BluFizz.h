#pragma once

/* Standard C++ header files. */
#include <iostream>
#include <stdexcept>
#include <string>

/* Local header files. */
#include "Bluez_AdapterController.h"

/**
 * @brief namespace for BlueTooth related definitions.
 */
namespace blufizz {

class BluFizz
{
public:
  BluFizz(bluetooth::bluez::BluezAdapterController &adapter);
  ~BluFizz();

  /**
   * @brief Main task.
   */
  void run();

private:

  /**
   * @brief Adapter to access the BlueZ stack.
   */
  bluetooth::bluez::BluezAdapterController &m_adapter;
};

} /* blufizz namespace */
