const uint8_t bufferLen = 1024;
char cmdBuffer[bufferLen];
char valueBuffer[bufferLen];
const int MAX_TOKENS = 10;

void menuCommands()
{
    systemPrintln("COMMAND MODE");

    while (1)
    {
        InputResponse response = getString(cmdBuffer, bufferLen, true);

        if (response != INPUT_RESPONSE_VALID)
            continue;

        char *tokens[MAX_TOKENS];
        const char *delimiter = "|";
        int tokenCount = 0;
        tokens[tokenCount] = strtok(cmdBuffer, delimiter);

        while (tokens[tokenCount] != nullptr && tokenCount < MAX_TOKENS)
        {
            tokenCount++;
            tokens[tokenCount] = strtok(nullptr, delimiter);
        }
        if (tokenCount == 0)
            continue;

        if (strcmp(tokens[0], "SET") == 0)
        {
            auto field = tokens[1];
            if (tokens[2] == nullptr)
            {
                updateSettingWithValue(field, "");
            }
            else
            {
                auto value = tokens[2];
                Serial.printf("SET %s:%s\r\n", field, value);
                updateSettingWithValue(field, value);
            }
            systemPrintln("OK");
        }
        else if (strcmp(tokens[0], "GET") == 0)
        {
            auto field = tokens[1];
            getSettingValue(field, valueBuffer);
            systemPrint(">");
            systemPrintln(valueBuffer);
        }
        else if (strcmp(tokens[0], "CMD") == 0)
        {
            systemPrintln("OK");
        }
        else if (strcmp(tokens[0], "EXIT") == 0)
        {
            systemPrintln("OK");
            printEndpoint = PRINT_ENDPOINT_SERIAL;
            btPrintEcho = false;
            return;
        }
        else if (strcmp(tokens[0], "APPLY") == 0)
        {
            systemPrintln("OK");
            recordSystemSettings();
            ESP.restart();
            return;
        }
        else
        {
            systemPrintln("ERROR");
        }
    }

    btPrintEcho = false;
}