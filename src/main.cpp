/* Standard C++ header files. */
#include <cerrno>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

/* Local header files. */
#include "Bluez_AdapterManager.h"
#include "Bluez_Dbus.h"
#include "Bluez_DeviceConnector.h"
#include "DevicesManager.h"
#include "BluFizz.h"
#include "TaskManager.h"

namespace {

std::mutex exitMutex;
std::condition_variable cv;
bool exit = false;

/**
 * @brief Handler to exit program by signals.
 * 
 * @param s Signal value.
 */
void signalHandler(int s)
{
  {
    std::lock_guard lock(exitMutex);
    exit = true;
  }
  cv.notify_one();
}

/**
 * @brief Main loop.
 */
int run()
{
  std::unique_lock lock(exitMutex);
  cv.wait(lock, [] { return exit; });
  return EXIT_SUCCESS;
}

void initRun(void)
{
  std::signal(SIGINT, signalHandler);
}

} /* anonymous namespace */

int main(void)
{
  /* Initialize the BlueTooth adapter. */
  initRun();

  std::shared_ptr<sdbus::IConnection> connection = sdbus::createSystemBusConnection();
  // to catch signals
  connection->enterEventLoopAsync();

  // Object Manager
  sdbus::ServiceName destination{B_SERVICE_NAME};
  sdbus::ObjectPath objectPath{OBJECT_MANAGER_PATH};
  const std::shared_ptr<sdbus::IProxy> objectManagerProxy =
    sdbus::createProxy(*connection, std::move(destination), std::move(objectPath));

  // Device client
  bluetooth::bluez::BluezDeviceClient deviceClient(connection, B_SERVICE_NAME, objectManagerProxy);
  bluetooth::bluez::BluezDeviceConnector deviceConnector(deviceClient);

  // Manager
  device::DevicesManager devicesManager;
  storage::Database database("/tmp/blue.sqlite3");
  blufizz::tools::TaskManager taskManager(deviceConnector, devicesManager, database);

  bluetooth::bluez::BluezAdapterManager manager(connection, objectManagerProxy, taskManager);
  // for the moment only the default adapter is used
  auto &adapter = manager.get("/org/bluez/hci0");

  adapter.enable();

  blufizz::BluFizz blufizz(adapter);
  blufizz.run();
  int result = run();

  /* Finalize the BlueTooth adapter. */

  // remove all connected devices.
  for (const auto& [address, device] : devicesManager.devices()) {
    if (device->connected()) {
      adapter.removeDevice(device->objectPath());
    }
  }

  return result;
}
