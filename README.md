# auto-rfid-based-parking-lot-sys


The RFID-based parking system was implemented in Proteus using the AT89C51 microcontroller and tested with Keil uVision. UART communication processes RFID input from a virtual terminal, controlling gate movement via a motor driver. The microcontroller initializes LCD, motor driver, and UART at 9600 baud. It waits for a 12-character RFID input, storing and verifying it against predefined IDs. Entry and exit are determined by tracking consecutive scans, activating the motor accordingly. Valid IDs trigger gate operation, while invalid ones display an "ACCESS DENIED" message. The system was simulated without real RFID hardware or an external database.
