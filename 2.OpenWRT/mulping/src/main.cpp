#include <signal.h>
#include <vector>
#include <string>
#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

#include "common.h"

#ifndef INFINITE_LOOP
#define INFINITE_LOOP true
#endif

#define DEBUG false
#define MULTIPLE_THREADS false

// Linux signal handler
volatile sig_atomic_t infinite_loop = 1;

// Linux signal handler
void handle_signal(int signal)
{
    if (signal == SIGINT)
    {
        printf("Caught SIGINT signal, terminating...\n");
        infinite_loop = 0;
    }
}


// Add the timestamp
std::string get_formatted_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_tm = std::localtime(&now_time_t);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream timestamp;
    timestamp << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S") << "." << std::setw(3) << std::setfill('0') << now_ms.count();
    return timestamp.str();
}



// Ping the target device
void ping_single_thread(std::vector<struct ping_dev_info>& devices, int const pid, useconds_t wait_time)
{

    // Convert the vector to array
    struct ping_dev_info* dev_array = devices.data();
    int dev_num = devices.size();

    // Create the temporary device info array
    auto* dev_temp_array = new struct ping_dev_info[dev_num];
    memcpy(dev_temp_array, dev_array, dev_num * sizeof(struct ping_dev_info));

#if INFINITE_LOOP
    while (infinite_loop)
    {
#endif
        // Send the ICMP message to remote devices
        for (int i = 0; i < dev_num; i++)
        {
            // Print the debug message
//            printf("Idx: %d, IP: %s\n", i, inet_ntoa(dev_array[i].addr.sin_addr));

            // Set the ICMP header
            set_icmp_header(&(dev_array[i].packet), dev_array[i].seq++, pid);

            // send the ping message
            if (send_ping(&(dev_array[i])) <= 0) {
                perror("sendto");
            } else {
                printf("%s sendto %s: icmp_seq=%d\n", get_formatted_timestamp().c_str(), inet_ntoa(dev_array[i].addr.sin_addr), dev_array[i].seq);
                // printf("sendto %s: icmp_seq=%d\n", inet_ntoa(dev_array[i].addr.sin_addr), dev_array[i].seq);
            }

            // Receive the ICMP feedback
            if (recv_ping(&(dev_temp_array[i])) <= 0) {
                perror("recvfrom");
            }
        }

        // Sleep
        usleep(wait_time);

        // Print the debug message
        printf("-------------------------\n");

#if INFINITE_LOOP
    }
#endif

    // Free the memory
    delete[] dev_temp_array;
}


// Ping the target device
void ping_multi_threads(struct ping_dev_info* dev_inf, int const pid, useconds_t wait_time) {

    // Create a temporary device info
    auto* dev_temp = new struct ping_dev_info;
    memcpy(dev_temp, dev_inf, sizeof(struct ping_dev_info));

#if INFINITE_LOOP
    while (infinite_loop)
    {
#endif
        // set the ICMP header
        set_icmp_header(&(dev_inf->packet), dev_inf->seq++, pid);

        // send the ping message
        if (send_ping(dev_inf) <= 0) {
            perror("sendto");
        } else {
            printf("%s sendto %s: icmp_seq=%d\n", get_formatted_timestamp().c_str(), inet_ntoa(dev_inf->addr.sin_addr), dev_inf->seq);
            // printf("sendto %s: icmp_seq=%d\n", inet_ntoa(dev_inf->addr.sin_addr), dev_inf->seq);
        }

        // receive the ping message
        if (recv_ping(dev_temp) <= 0) {
            perror("recvfrom");
        }

        // wait several milliseconds
        usleep(wait_time);

#if INFINITE_LOOP
    }
#endif

    // Free the memory
    delete dev_temp;
}


// Function to trim leading and trailing whitespaces from a string
std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}


// Create the ICMP socket
void create_ping_dev_target(const char* addr, struct ping_dev_info* dev_info) {
    // Create the ICMP socket
    dev_info->sock_fd = create_icmp_sock();

    // Set the ICMP address struct
    set_icmp_addr_struct(&(dev_info->addr), addr);
}


// Parse the configuration file
std::vector<struct ping_dev_info> parse_config(const std::string& filename, useconds_t* sample_rate)
{
    // Read the file
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string line;
    std::vector<std::string> addrs;
    useconds_t s_rate = 0;

    // Parse the file line by line
    while (std::getline(file, line))
    {
        // Find the position of '=' character
        size_t pos = line.find('=');
        if (pos != std::string::npos)
        {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));

            // Check the key and extract the corresponding value
            if (key == "addrs")
            {
                // Extract the IP addresses
                size_t start = value.find('[');
                size_t end = value.find(']');
                std::string ip_list = value.substr(start + 1, end - start - 1);

                std::istringstream iss(ip_list);
                std::string addr;
                while (std::getline(iss, addr, ','))
                {
                    addrs.push_back(trim(addr));
                }
            }
            else if (key == "s_rate")
            {
                // Extract the sampling rate
                s_rate = std::stoi(value);
                s_rate = 1000000 / s_rate;
            }
        }
    }

    // Prepare the target device info vector
    std::vector<struct ping_dev_info> dev_info_vector;

    // Print the extracted values
    std::cout << "IP addresses:" << std::endl;
    for (const auto& addr : addrs)
    {
        std::cout << addr << std::endl;

        // Meanwhile, create the target device info for each address
        struct ping_dev_info dev_info = {};
        create_ping_dev_target(addr.c_str(), &dev_info);
        dev_info.seq = 0;
        dev_info_vector.push_back(dev_info);
    }
    std::cout << "Sampling rate: " << s_rate << std::endl;

    // Close the file
    file.close();

    // Return the extracted values
    *sample_rate = s_rate;
    return dev_info_vector;
}


// Debug environment
std::vector<struct ping_dev_info> debug_config(useconds_t* sample_rate)
{
    std::vector<std::string> addrs;
    addrs.emplace_back("8.8.8.8");
    addrs.emplace_back("8.8.4.4");

    // Prepare the target device info vector
    std::vector<struct ping_dev_info> dev_info_vector;

    // Prepare the debug devices
    for (const auto& addr : addrs)
    {
        std::cout << addr << std::endl;

        // Meanwhile, create the target device info for each address
        struct ping_dev_info dev_info = {};
        create_ping_dev_target(addr.c_str(), &dev_info);
        dev_info.seq = 0;
        dev_info_vector.push_back(dev_info);
    }

    // Return the extracted values
    *sample_rate = 1000000;
    return dev_info_vector;
}

// Main
int main(int argc, char *argv[])
{
#if DEBUG

    useconds_t  sample_hz = 0;
    std::vector<struct ping_dev_info> dev_info_vector = debug_config(&sample_hz);

#else

    // check the input parameters
    // ping <address> <sample_rate>
    if (argc != 2)
    {
        printf("Usage: ping <config_file>\n");
        return 1;
    }

    // Get the target devices and sample hz from the configuraion file
    useconds_t sample_hz = 0;
    std::vector<struct ping_dev_info> dev_info_vector = parse_config(argv[1], &sample_hz);

#endif

    // Register signal handler for SIGINT
    signal(SIGINT, handle_signal);

#if MULTIPLE_THREADS

    // Start ping for each device
     std::vector<std::thread> threads;
     for (auto& dev_info : dev_info_vector) {
         threads.emplace_back(ping_multi_threads, &dev_info, getpid(), sample_hz);
     }

    //  Wait for all threads to finish
     for (auto& thread : threads) {
         thread.join();
     }

#else

    // Start ping for each device
    ping_single_thread(dev_info_vector, getpid(), sample_hz);

#endif

    return 0;
}