#include "Producer.h"

void producer::reader_thread(const std::string& comport,
                            const std::chrono::seconds duration,
                            ThreadedQueue& queue) {
    auto start_time = std::chrono::steady_clock::now();
    HANDLE h_comm = INVALID_HANDLE_VALUE;

    try {
        h_comm = CreateFileA(comport.c_str(), GENERIC_READ, 0, NULL,
                            OPEN_EXISTING, 0, NULL);

        if (h_comm == INVALID_HANDLE_VALUE) {
            throw std::invalid_argument(
                "Invalid comport handle, or failed to connect to comport.");
        } else {
            std::cout << "Serial Port Open!\n\n";
            Sleep(1000);
        }

        DCB dcb_struct_parameter = { 0 };
        dcb_struct_parameter.DCBlength = sizeof(dcb_struct_parameter);

        BOOL status = GetCommState(h_comm, &dcb_struct_parameter);
        if (!status) {
            throw std::runtime_error("Error in CommState.");
        }

        dcb_struct_parameter.BaudRate = config::BAUD_RATE;
        dcb_struct_parameter.ByteSize = config::BYTE_SIZE;
        dcb_struct_parameter.Parity = NOPARITY;
        dcb_struct_parameter.StopBits = ONESTOPBIT;

        status = SetCommState(h_comm, &dcb_struct_parameter);
        if (!status) {
            throw std::runtime_error("Error setting CommState.");
        }

        COMMTIMEOUTS timeouts = { 0 };
        timeouts.ReadIntervalTimeout = MAXDWORD;
        timeouts.ReadTotalTimeoutMultiplier = 0;
        timeouts.ReadTotalTimeoutConstant = 0;
        if (SetCommTimeouts(h_comm, &timeouts) == FALSE) {
            throw std::runtime_error("Error setting timeouts.");
        }

        PurgeComm(h_comm, PURGE_RXCLEAR | PURGE_TXCLEAR);

        DWORD bytes_read;
        uint8_t receive_data_buffer[config::SERIAL_BUFFER_SIZE] = { 0 };

        // Uses std::deque as std::queue doesn't have a ".clear()" method
        std::deque<uint8_t> slider;

        while (true) {
            if (std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::steady_clock::now() - start_time) >=
                duration) {
                break;
            }

            if (ReadFile(h_comm, receive_data_buffer,
                         sizeof(receive_data_buffer), &bytes_read, NULL)) {
                if (bytes_read > 0) {
                    for (DWORD i = 0; i < bytes_read; ++i) {
                        slider.push_back(receive_data_buffer[i]);
                        if (slider.size() > config::PACKET_SIZE) {
                            slider.pop_front();
                        }

                        if (slider.size() == config::PACKET_SIZE) {
                            if (slider.front() == config::SYNC_BYTE) {
                                Message received_message;
                                received_message.bytes[0] = slider[0];  // config::SYNC_BYTE
                                received_message.bytes[1] = slider[1];  // Data 1
                                received_message.bytes[2] = slider[2];  // Data 2
                                received_message.bytes[3] = slider[3];  // Data 3
                                received_message.bytes[4] = slider[4];  // Data 4
                                received_message.checksum = slider[5];  // Checksum

                                if (received_message.verify()) {
                                    queue.push(received_message);
                                }
                            }
                        
                        slider.pop_front();
                        }
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Uncaught error!" << std::endl;
    }

    if (h_comm != INVALID_HANDLE_VALUE) {
        CloseHandle(h_comm);
    }
    queue.set_is_finished();
}
