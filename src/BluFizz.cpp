/* Standard C++ header files. */
#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

/* Local header files. */
#include "BluFizz.h"

/**
 * @brief namespace for BluFizz.
 */
namespace blufizz {


BluFizz::BluFizz(bluetooth::bluez::BluezAdapterController &adapter)
  : m_adapter(adapter)
{

}

BluFizz::~BluFizz()
{
  m_adapter.stopScan();
}

void BluFizz::run()
{
  /*
   - start scan
   - waiting for a device in the list
   - check if device is already knowned
   - if not:
      - connect
      - get info
      - store in sql database
  */

  m_adapter.startScan();
}

} /* blufizz namespace */
