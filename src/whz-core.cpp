#include <filesystem>
#include <print>
#include "whz_server.hpp"

auto main() -> int {
  std::filesystem::path path{"/tmp/whz"};

  // SETUP Stage (low level C++)
  // Initialise the base objects that are part of the core (not in the nodes)
  // Load the configured system nodes dynamically and let them initialise themselves
  // Load the configuration file and process it (e.g. validate paths and settings)
  // Check the available resources (CPU, memory, disk space, etc.), adjust config if necessary
  // Create the LUA context and check the existence of the LUA start-script, execute startup operations if needed
  // Create the database object(s) and connect to the database(s), execute startup operations if needed
  // Connect to the other whz-cores if configured
  // Load the configured user nodes dynamically and let them initialise themselves
  // Create the server object and start listening in the server
  // Start the LUA start-script

  // SETUP Stage (high level C++ and LUA)
  // Initialise user objects, attach dynamic nodes (user + system) and load the config
  // Create the server object and start listening in the server

  // --------------------------------------------------------------------------------

  // REQUEST Processing Stage (low level C++)
  // Pull the next requests from the io_uring buffer and assign reused contexts and threads/tasks to them, they are queued
  // Execute as many requests as we have CPU cores configured and available, they are executed in parallel
  // REQ: Parse the request (HTTP header) and validate it, check the caller permissions and the rate limits
  // REQ: Validate any user input and request parameters and escape/sanitize them before further processing
  // REQ: Create a user-session object and cache it if necessary.
  // REQ: Find the page from the path and the query parameters, check the cache.
  // REQ: Assemble the page and its resources.
  // REQ: Parse the page template and process them + fill the data in it, render the page
  // REQ: Construct HTTP header and serve the page data and resources requested
  // Free the context and the thread/task

  // REQUEST Processing Stage (high level C++ and LUA)
  // REQ: Process request and receive the data of it
  // REQ: Apply the business logic and assemble the page data
  // REQ: Render the page and serve it
  // REQ: Close request.


  whz::server s{"0.0.0.0", 8080, std::move(path), 1};

  s.listen_and_serve();
  return 0;
}
