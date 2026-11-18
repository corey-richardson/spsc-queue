# Single-Producer Single-Consumer Queue Serial Logger

Uses a Single-Producer Single-Consumer (SPSC) Queue to coordinate data transfer between a `Producer` thread and a `Consumer` thread.

The `Producer` thread connects via UART to the board. The received bytes are saved into a buffer and a sliding window, capped to 6 bytes; when it reaches more than 6 bytes, the front of the double ended queue is discarded. If the front of the queue is 0x55 (sync byte) and the checksum is valid, the message gets pushed to the `ThreadedQueue` (class).

The `Checksum::verify()` method handles checksum validation.

The `ThreadedQueue` class has three methods: `push`, `pop`, `set_is_finished`. This class is used to handle one-way inter-thread transfers. Mutex's and Condition Variables are used to block access to the queue whilst a thread is using the resource, preventing simultaneous accesses and protecting atomicity.

The `Consumer` thread pops messages from the `ThreadedQueue`, and writes the message data to a CSV file, as well as the `std::cout` stream 3 times a second.

The MSB and LSB are combined by` Message::combineMsbLsb()` in `Types.h`.

> The README describes my use case values. See `Config.h` to configure your own values.

## Usage

```sh
.\SPSC_Queue.exe COMPORT DURATION_SECONDS OUTPUT_FILENAME?
```

```sh
.\SPSC_Queue.exe COM4 120
.\SPSC_Queue.exe COM4 120 test_run
```

## Data and Thread Flow

```mermaid
flowchart TD
    Main["Main Thread"] -. Spawns / Joins .-> Reader
    Main -. Spawns / Joins .-> Writer

    Device["Hardware"] -->|Data Stream| Reader["Reader Thread"]
    Reader -->|Parsed Packets| Queue["Thread-Safe Queue"]
    Queue -->|Buffered Packets| Writer["Writer Thread"]

    Writer -->|Log| File["CSV File"]
    Writer -->|Subsample| Console["Console Display"]
```
