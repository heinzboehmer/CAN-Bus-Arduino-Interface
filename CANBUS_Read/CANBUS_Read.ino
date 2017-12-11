#include <SPI.h>
#include "mcp_can.h"

#define numberOfCells 4 //Specify number of cells in battery pack for hash calculations

const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);  //Set CS pin

struct canStruct {  //Struct declaration
  int canID;
  String parameter;
  int index;
  float value;
};

String messageArray[33 + 2 * numberOfCells];

struct canStruct interpretData() {
  //Declare variables and struct
  unsigned char len = 0;
  unsigned char buf[8];
  String parameter;
  String index;
  String value;
  int i = 0;

  parameter.reserve(2);
  index.reserve(3);
  value.reserve(6);

  CAN.readMsgBuf(&len, buf);  // read data,  len: data length, buf: data buf

  canStruct returnStruct;

  returnStruct.canID = CAN.getCanId();

//  for (int i = 0; i < 8; ++i) { //Split up buffer into parameter, value, and index strings
  while (i < sizeof(buf)) {
    if (i < 2) index += (char) buf[i];
    else if (i == 2) {parameter += (char) buf[i];
    Serial.println((char) buf[i]);}
    else value += (char) buf[i];
    i++;
  }

  int indexInt = strtol(index.c_str(), NULL, 16);
  float valueFloat = strtol(value.c_str(), NULL, 16);
  returnStruct.parameter = parameter;

  if (returnStruct.parameter == "v" && indexInt != 0) valueFloat /= 1000.0;
  else if (returnStruct.parameter == "t" && indexInt != 0 && valueFloat > 0x7fffffL) valueFloat -= 0x1000000L; //Two's complement conversion
  else if (returnStruct.parameter == "%" && (indexInt == 4 || indexInt == 7 || indexInt == 9 || indexInt == 11 || indexInt == 12)) valueFloat /= 1000.0;
  else if (returnStruct.parameter == "%" && indexInt == 12) valueFloat /= 10.0;

  returnStruct.index = indexInt;
  returnStruct.value = valueFloat;

  Serial.println(returnStruct.canID);
  Serial.println(returnStruct.parameter);
  Serial.println(returnStruct.index);
  Serial.println(returnStruct.value);
  Serial.println();
  
  return returnStruct;
}


void lookUpTablePrint(String parameter, int indexInt, int valueInt, unsigned int canID) {

  if (parameter == "v") {
    if (indexInt == 0) {
      Serial.print("Total Number of Cells: ");
      Serial.print(valueInt);
    }

    else {
      Serial.print("Cell ");
      Serial.print(indexInt);
      Serial.print(" at ");
      Serial.print(valueInt  / 1000.0);
      Serial.print(" V");
    }
  }

  else if (parameter == "t") {
    if (indexInt == 0) {
      Serial.print("Total Number of Cells: ");
      Serial.print(valueInt);
    }

    else {
      if (valueInt > 0x7fffffL) valueInt -= 0x1000000L; //Two's complement conversion
      Serial.print("Cell ");
      Serial.print(indexInt);
      Serial.print(" at ");
      Serial.print(valueInt);
      Serial.print(" ºC");
    }
  }

  else if (parameter == "%") {
    switch(indexInt) {
      case 0:
        Serial.print("Total Number of Summary Fields: ");
        Serial.print(valueInt);
        break;

      case 1:
        Serial.print("Official Cell Count: ");
        Serial.print(valueInt);
        break;

      case 2:
        Serial.print("Observed Cell Count: ");
        Serial.print(valueInt);
        break;

      case 3:
        Serial.print("Average Cell Temperature: ");
        Serial.print(valueInt);
        Serial.print(" ºC");
        break;

      case 4:
        Serial.print("Average Cell Voltage: ");
        Serial.print(valueInt  / 1000.0);
        Serial.print(" V");
        break;

      case 5:
        Serial.print("Max Cell Temperature: ");
        Serial.print(valueInt);
        Serial.print(" ºC");
        break;

      case 6:
        Serial.print("Max Cell Temperature Index: ");
        Serial.print(valueInt);
        break;

      case 7:
        Serial.print("Max Cell Voltage: ");
        Serial.print(valueInt  / 1000.0);
        Serial.print(" V");
        break;

      case 8:
        Serial.print("Max Cell Voltage Index: ");
        Serial.print(valueInt);
        break;

      case 9:
        Serial.print("Min Cell Voltage: ");
        Serial.print(valueInt  / 1000.0);
        Serial.print(" V");
        break;

      case 10:
        Serial.print("Min Cell Voltage Index: ");
        Serial.print(valueInt);
        break;

      case 11:
        Serial.print("Pack Charge: ");
        Serial.print(valueInt / 1000.0);
        Serial.print(" V");
        break;

      case 12:
        Serial.print("Pack Current: ");
        Serial.print(valueInt / 10.0);
        Serial.print(" A");
        break;

      case 13:
        Serial.print("Pack Capacity: ");
        Serial.print(valueInt);
        Serial.print("%");
        break;

      case 14:
        Serial.print("Pack Alert: ");
        Serial.print(valueInt);
        break;

      case 15:
        Serial.print("Timestamp: ");
        Serial.print(valueInt);
        break;

      case 16:
      break;

      case 17:
        Serial.print("Min Cell Temperature: ");
        Serial.print(valueInt);
        break;

      case 18:
        Serial.print("Min Cell Temperature Index: ");
        Serial.print(valueInt);
        break;

      default:
        Serial.print("Not valid. (Can ID: ");
        Serial.print(canID);
        Serial.print(" Index: ");
        Serial.print(indexInt);
        Serial.print(" Parameter: ");
        Serial.print(parameter);
        Serial.print(" Value: ");
        Serial.print(valueInt);
        Serial.print(")");
    }
  }

  else if (parameter == "s") {
    switch(indexInt) {
      case 0:
        Serial.print("Total Number of Alert Fields: ");
        Serial.print(valueInt);
        break;

      case 1:
        if (valueInt) {
          Serial.print("Pack Normal");
          break;
      }

      case 2:
        if (valueInt) {
          Serial.print("Cell Temperature Too High");
          break;
      }

      case 3:
        if (valueInt) {
          Serial.print("Cell Voltage Too High");
          break;
      }

      case 4:
        if (valueInt) {
          Serial.print("Pack Voltage High");
          break;
      }

      case 5:
        if (valueInt) {
          Serial.print("Cell Voltage Too Low");
          break;
      }

      case 6:
        if (valueInt) {
          Serial.print("Pack Voltage Low");
          break;
      }

      case 7:
        if (valueInt) {
          Serial.print("Pack Current Too High");
          break;
      }

      case 8:
        if (valueInt) {
          Serial.print("Pack Current High");
          break;
      }

      case 9:
        if (valueInt) {
          Serial.print("Pack to Chassis Connection Detected");
          break;
      }

      case 10:
        if (valueInt) {
          Serial.print("Pack to Cell Communication Error");
          break;
      }

      case 11:
        if (valueInt) {
          Serial.print("System Error");
          break;
      }

      default:
        Serial.print("Not valid. (Can ID: ");
        Serial.print(canID);
        Serial.print(" Index: ");
        Serial.print(indexInt);
        Serial.print(" Parameter: ");
        Serial.print(parameter);
        Serial.print(" Value: ");
        Serial.print(valueInt);
        Serial.print(")");
    }
  }

  else {
    Serial.print("Not valid. (Can ID: ");
    Serial.print(canID);
    Serial.print(" Index: ");
    Serial.print(indexInt);
    Serial.print(" Parameter: ");
    Serial.print(parameter);
    Serial.print(" Value: ");
    Serial.print(valueInt);
    Serial.print(")");
  }

  Serial.println();
}


void printAll(int index, String parameter, float value, int canID) {
  Serial.println("----------------------------------------------------------");
  Serial.print("Get data from ID: ");
  Serial.println(canID, HEX);

  Serial.print("Index: ");
  Serial.print(index);
  Serial.print("\t");
  Serial.print("Parameter: ");
  Serial.print(parameter);
  Serial.print("\t");
  Serial.print("Data: ");
  Serial.print(value);

  Serial.println("\n----------------------------------------------------------\n");
}


void initializeMessageArrayLookUpTable(String initializationArray[]) {
  int i = 0;

  while (i <= numberOfCells) {
    if (i == 0) initializationArray[i] = "Total Number of Cells: ";
    else initializationArray[i] = "Cell voltage (V): ";
    i++;
  }

  while (i <= 2 * numberOfCells + 1 && i > numberOfCells) {
    if (i == 0) initializationArray[i] = "Total Number of Cells: ";
    else initializationArray[i] = "Cell temperature (ºC): ";
    i++;
  }

  initializationArray[i] = "Total Number of Summary Fields: ";
  initializationArray[i + 1] = "Official Cell Count: ";
  initializationArray[i + 2] = "Observed Cell Count: ";
  initializationArray[i + 3] = "Average Cell Temperature (ºC): ";
  initializationArray[i + 4] = "Average Cell Voltage (v): ";
  initializationArray[i + 5] = "Max Cell Temperature (ºC): ";
  initializationArray[i + 6] = "Max Cell Temperature Index: ";
  initializationArray[i + 7] = "Max Cell Voltage (V): ";
  initializationArray[i + 8] = "Max Cell Voltage Index: ";
  initializationArray[i + 9] = "Min Cell Voltage (V): ";
  initializationArray[i + 10] = "Min Cell Voltage Index: ";
  initializationArray[i + 11] = "Pack Charge (V): ";
  initializationArray[i + 12] = "Pack Current (A): ";
  initializationArray[i + 13] = "Pack Capacity (%): ";
  initializationArray[i + 14] = "Pack Alert: ";
  initializationArray[i + 15] = "Timestamp (s): ";
  initializationArray[i + 16] = "Reserved";
  initializationArray[i + 17] = "Min Cell Temperature (ºC): ";
  initializationArray[i + 18] = "Min Cell Temperature Index: ";
  initializationArray[i + 19] = "Total Number of Alert Fields: ";
  initializationArray[i + 20] = "Pack Normal";
  initializationArray[i + 21] = "Cell Temperature Too High";
  initializationArray[i + 22] = "Cell Voltage Too High";
  initializationArray[i + 23] = "Pack Voltage High";
  initializationArray[i + 24] = "Cell Voltage Too Low";
  initializationArray[i + 25] = "Pack Voltage Low";
  initializationArray[i + 26] = "Pack Current Too High";
  initializationArray[i + 27] = "Pack Current High";
  initializationArray[i + 28] = "Pack to Chassis Connection Detected";
  initializationArray[i + 29] = "Pack to Cell Communication Error";
  initializationArray[i + 30] = "System Error";
}


bool isValid(int hashIndex, int canID) {
  if (canID != 0x4D || hashIndex == -1) return false;
  else return true;
}


int hashFunction(int index, String parameter) {
  if (parameter == "v" && index <= numberOfCells) return index;

  else if (parameter == "t" && index <= numberOfCells) return numberOfCells + 1 + index;

  else if (parameter == "%" && index < 19) return 2 * numberOfCells + 2 + index;

  else if (parameter == "s" && index < 12) return 2 * numberOfCells + 21 + index;

  else return -1;
}


void setup() {
  Serial.begin(115200);

  while (CAN_OK != CAN.begin(CAN_500KBPS)) {
    Serial.println("CAN BUS Shield initialization fail");
    Serial.println(" Initializing CAN BUS Shield again");
    delay(100);
  }

  Serial.println("CAN BUS Shield initialization ok!");
  Serial.println("Enter request formatted as: <Parameter>,<Index>,");

  initializeMessageArrayLookUpTable(messageArray);
}

void loop() {
  int hashIndex;
  canStruct loopStruct;
  canStruct loopStructArray[33 + 2 * numberOfCells]; //This should be declared as global, but for some reason serial data gets corrupted when it is

  //Check if data is coming
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    //Read current canbus data
    loopStruct = interpretData();
    
    //Generate hash index
    hashIndex = hashFunction(loopStruct.index, loopStruct.parameter);
    
    //Check validity of data
    if (isValid(hashIndex, loopStruct.canID)) {
      //Store data in proper location in array
      loopStructArray[hashIndex] = loopStruct;
    }

    // printAll(loopStruct.parameter, loopStruct.index, loopStruct.value, loopStruct.canID); //Print everything that is being received for debugging purposes
    // lookUpTablePrint(loopStruct.parameter, loopStruct.index, loopStruct.value, loopStruct.canID); //Print everything that is being received for debugging purposes in an ordered way
  }

  //Check if serial data is available
  if (Serial.available()) {
    char input;
    bool parameterArrayComplete = false;
    String inputParameter;
    String inputIndex;
    
    inputParameter.reserve(2);
    inputIndex.reserve(3);

    while (Serial.available()) {
      input = (char) Serial.read();

      if (!parameterArrayComplete) {
        if (input == ',') { //Checks if byte recieved is a comma
          parameterArrayComplete = true;
          continue;
        }

        else if(input != '\n' && input != '\r') {
          inputParameter += input;
        }
      }

      else if (input != ',' && input != '\n' && input != '\r') { //Checks if byte recieved is a comma, carriage return or newline
        inputIndex += input;
      }
    }

    int inputIndexInt = strtol(inputIndex.c_str(), NULL, 10);
    hashIndex = hashFunction(inputIndexInt, inputParameter);    

//     Serial.println(inputIndexInt); //Print index and parameter values received for debugging purposes
//     Serial.println(inputParameter);

    if (isValid(hashIndex, 0x4D)) {   
      if (inputParameter == "s" && inputIndexInt != 0 && loopStructArray[hashIndex].value > 0) {
        Serial.println(messageArray[hashIndex]);
      }

      else if (inputParameter == "s" && inputIndexInt != 0 && loopStructArray[hashIndex].value == 0) {
        Serial.println("No alert.");
      }

      else {
        Serial.print(messageArray[hashIndex]);
        Serial.println(loopStructArray[hashIndex].value);
      }

      Serial.println();
      Serial.println("Enter request formatted as: <Parameter>,<Index>");
    }

    else {
      Serial.println("Values entered not valid.");
      Serial.println();
      Serial.println("Enter request formatted as: <Parameter>,<Index>");
    }
  }
}
