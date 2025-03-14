#include "api/Server.hpp"

#include <iostream>
#include <thread>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <db_file_path>" << std::endl;
        return 1;
    }

    std::string dbFilePath = argv[1];
    std::cout << "Starting API Server with database file: " << dbFilePath << std::endl;

    Server server(dbFilePath);

    std::thread apiThread([&server]() { server.start(); });
    apiThread.detach();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}
