# Automatic Toll Gate System

## Objective
This project aims to build an **Automatic Toll Collection System** with the option of recharging RFID cards when there is insufficient balance to pay the toll fee.

## Potential Components
- **Arduino UNO**
- **Servo Motor**
- **RFID RC522**
- **IR Obstacle Sensors**
- **Connecting Wires**
- **Breadboard**
- **4x4 Keypad**
- **16x2 LCD Display**

## Description
When a vehicle is detected by the first IR obstacle sensor, the servo motor closes the toll gate, which was initially open. The second IR obstacle sensor will not open the toll gate unless the driver pays the toll charge using the RFID card. 

If the balance on the card is insufficient, an option is available to recharge the card using the 4x4 keypad. When the driver successfully pays the toll fee, the second IR obstacle sensor detects the vehicle, and the toll gate opens to let the vehicle pass. All actions are displayed on the **16x2 LCD Display**.

## User Manual
### Step 1: Vehicle Detection
- A vehicle is detected by the first IR obstacle sensor, and the toll gate closes.
- The **LCD Display** shows: `Vehicle detected`.

### Step 2: Payment Process
- The LCD prompts: `Put your card to the reader`.
- The RFID card is scanned to pay the toll fee:
  - If the toll is successfully paid, the LCD shows: `Successfully paid your bill`.
  - If the card has **insufficient balance**, the LCD shows:
    ```
    Your balance is insufficient
    Your remaining balance: TK <amount>
    ```
  - If the card is **not registered**, the LCD displays:
    ```
    Unknown vehicle
    Access denied
    ```

- Options for the driver:
  - **Press A:** Recharge the RFID card using the keypad.
  - **Press B:** Register a new card and then pay the toll fee.

### Step 3: Vehicle Exit
- After the toll fee is paid:
  - The second IR obstacle sensor detects the vehicle.
  - The toll gate opens to allow the vehicle to pass.
