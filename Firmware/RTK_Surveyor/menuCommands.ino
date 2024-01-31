const uint8_t bufferLen = 255;
char cmdBuffer[bufferLen];
char valueBuffer[bufferLen];
const int MAX_TOKENS = 10;

void menuCommands()
{
    systemPrintln("COMMAND MODE");
    systemPrint("CMD>");
    while (1)
    {
        InputResponse response = getString(cmdBuffer, bufferLen);

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

        bool commandProcessed = false;
        if (strcmp(tokens[0], "SET") == 0)
        {
            auto field = tokens[1];
            auto value = tokens[2];
            updateSettingWithValue(field, value);
            commandProcessed=true;
        }
        else if (strcmp(tokens[0], "GET") == 0)
        {
            auto field = tokens[1];
            getSettingValue(field, valueBuffer);
            systemPrintln(valueBuffer);
            commandProcessed=true;
        }
        else if (strcmp(tokens[0], "EXIT") == 0)
        {
            return;
        }
        else if (strcmp(tokens[0], "APPLY") == 0)
        {
            systemPrintln("Apply Settings!");
            recordSystemSettings();
            ESP.restart();
            
            return;
        }
        else
        {
            systemPrintln("Invalid Command!");
            commandProcessed=true;
        }

        if (commandProcessed)
            systemPrint("CMD>");
    }
}