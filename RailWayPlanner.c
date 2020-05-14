#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

//--------------------------------- PROGRAM CONSTANTS ----------------------------------------
#define NUMBER_OF_REQUIRED_ARGUMENTS 2
// include '\0'
#define MAX_LENGTH_OF_LINE 1025
#define INITIAL_TO_NEGATIVE -1
#define SIZE_INITIAL_OF_ARRAY 3
// must be a prime number so it checks if the number of parts in the array is is divisible
// and also this is the number of empty places added to the arrary after resize (array called
// allParts.
#define LINEAR_FACTOR_TO_RESIZE 3

//--------------------------------- MASSAGES ----------------------------------------

#define SUCCESS_TO_BE_WRRITEN_TO_FILE "The minimal price is:"
#define FAIL_FILE_DONT_EXIST "File doesn't exists."
#define FAIL_FILE_IS_EMPTY "File is empty"
#define FAIL_INVALID_NUM_OF_ARGS "Usage: RailWayPlanner <InputFile>"
#define FAIL_IN_LINE "Invalid input in line:"

//--------------------------------- PROGRAM OUTOUT FILE -------------------------------------

const char* ADRESS_TO_FILE_TO_WRITE = "railway_planner_output.txt";

//--------------------------------- GLOBAL VARIABLES ----------------------------------------

int gRequiredLengthOfRail = 0;
int gNumberOfConnectionsTypes = 0;
char *gKindsOfConnections = NULL;
int gErrorInLineIndex = 0;

//--------------------------------- STRUCTS -------------------------------------------------

/**
 * a struct that represent a part and stores all its data, initialized while created
 */
typedef struct part
{
    // not using _start but part of this program for future use
    char _start;
    char _end;
    int _length;
    int _price;
    int _indexOfStartInKindsOfConnection;
} part ;

/**
 * a function that creates a part, saves it on the heap and return a pointer to it
 * @param start - the type of the start connection
 * @param end - the type of the end connection
 * @param length - the length of the part
 * @param price - the price of the part
 * @return a pointer to the created part if succesfully allocated memory for it, else return
 * pointer to NULL
 */
part* createPart(char start, char end, int length, int price)
{
    //memory allocation
    part* newPart = (part*)malloc(sizeof(part));
    //memory allocation check
    if(newPart == NULL)
    {
        return NULL;
    }
    newPart -> _start = start;
    newPart -> _end = end;
    newPart -> _length = length;
    newPart -> _price = price;
    newPart ->_indexOfStartInKindsOfConnection = INITIAL_TO_NEGATIVE;
    // checks the index of the start connection in the collection of connection
    // (at the gKindOfConnection) and set its index number
    for(int i = 0; i < gNumberOfConnectionsTypes &&
    newPart->_indexOfStartInKindsOfConnection == INITIAL_TO_NEGATIVE; i++)
    {
        if(start == gKindsOfConnections[i])
        {
            newPart ->_indexOfStartInKindsOfConnection = i;
        }
    }
    return newPart;
}

/**
 *  a function that free all heap saved data
 * @param allParts - array of parts
 * @param tableOfPrices - the table built for the algorithem to caculate the cheapest price of rail
 */
void setAllProgramDataFree(part** allParts, int*** tableOfPrices)
{
    if(tableOfPrices != NULL)
    {
        if(*tableOfPrices != NULL)
        {
            for(int l = 0 ; l <= gRequiredLengthOfRail; l++)
            {
                if((*tableOfPrices)[l]!=NULL)
                {
                    free((*tableOfPrices)[l]);
                }
            }
            free(*tableOfPrices);
        }
    }
    if((*allParts) != NULL)
    {
        free(*allParts);
    }
    if(gKindsOfConnections != NULL)
    {
        free(gKindsOfConnections);
    }
}

/**
 * a function that write down to a given file (as address), it will write
 * depends on the write succed value, if true it will write the final answer, else will write the
 * relevant error
 * @param outPutFile - the path to the file
 * @param writeSucces true if called at success run
 * @param result - the cheapest cost to build the rail, if there is no way to buid trail, it
 * equals -1
 * @return 0 if succesfully open a file to write on, else return non-zero int
 */
int writeOn(const char* outPutFile, const char* whatToWrite, int result)
{
    FILE* fileToWriteOn = NULL;
    fileToWriteOn = fopen(outPutFile, "w");
    if(fileToWriteOn == NULL)
    {
        EXIT_FAILURE;
    }
    int resultOfWriting = INITIAL_TO_NEGATIVE;
    if(!strcmp(whatToWrite, SUCCESS_TO_BE_WRRITEN_TO_FILE))
    {
        resultOfWriting = fprintf(fileToWriteOn, "%s %d", SUCCESS_TO_BE_WRRITEN_TO_FILE, result);
    }
    if(!strcmp(whatToWrite, FAIL_FILE_DONT_EXIST))
    {
        resultOfWriting = fprintf(fileToWriteOn, "%s", FAIL_FILE_DONT_EXIST);
    }
    if(!strcmp(whatToWrite, FAIL_FILE_IS_EMPTY))
    {
        resultOfWriting = fprintf(fileToWriteOn, "%s", FAIL_FILE_IS_EMPTY);
    }
    if(!strcmp(whatToWrite, FAIL_INVALID_NUM_OF_ARGS))
    {
        resultOfWriting = fprintf(fileToWriteOn, "%s", FAIL_INVALID_NUM_OF_ARGS);
    }
    if(!strcmp(whatToWrite, FAIL_IN_LINE))
    {
        resultOfWriting = fprintf(fileToWriteOn, "%s%d%s", FAIL_IN_LINE, gErrorInLineIndex, ".");
    }
    if(resultOfWriting < 0)
    {
        fclose(fileToWriteOn);
        return EXIT_FAILURE;
    }
    fclose(fileToWriteOn);
    return EXIT_SUCCESS;
}
/**
 * a function that by giving a line to it checks if the string after that is after
 * number of commas contain only digits
 * @param lineToCheck - the line to be checked
 * @param readAfterCommaNumber - the number of commas we want to check after it
 * if the string contain only digits
 * @return 0 in case all the string is digits after the comma, else return non-zero
 * int
 */
int checkSpecificAfterCommaLine(char* lineToCheck, int readAfterCommaNumber)
{
    int commaIndex = 0;
    int charIndex = 0;
    for( ; charIndex < (int)strlen(lineToCheck); charIndex++)
    {
        if(lineToCheck[charIndex] == ',')
        {
            commaIndex++;
        }
        if(commaIndex >= readAfterCommaNumber)
        {
            ++charIndex;
            break;
        }

    }
    if(commaIndex!=readAfterCommaNumber)
    {
        return EXIT_FAILURE;
    }
    for(; charIndex < (int)strlen(lineToCheck); charIndex++)
    {
        if(!isdigit(lineToCheck[charIndex]))
        {
            if(lineToCheck[charIndex] != '\r' && lineToCheck[charIndex] != '\n')
            {
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;

}

/**
 * a function that check if a given number is negative, might be wrriten inside the code
 * but is an independent function to make code more neat
 * @param givenNumber - the number to be checked
 * @return 0 if the number is negative, else return non-zero number
 */
int checkIfNegativeNumber(int givenNumber)
{
    if(givenNumber < 0)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/**
 * a function that checks if a given string contain only digits
 * @param givenString - the string to be checked
 * @return 0 if only digits, else return non-zero integer
 */
int digitsOnly(char* givenString)
{
    while (*givenString)
    {
        if (isdigit(*givenString++) == 0)
        {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

/**
 * a function that parse first and second line in the file format, if the first/second
 * line is valid it returns its value
 * @param recivedLine - the line to be checked
 * @return if the line is valid returns its int value, else return -1
 */
int parseTopLines(char* recivedLine)
{
    int recieved = -1;
    char parseNoEndLine[MAX_LENGTH_OF_LINE];
    //cutting end of line to check the string content without it
    sscanf(recivedLine, "%s\r\n", parseNoEndLine);
    if(digitsOnly(parseNoEndLine))
    {
        return INITIAL_TO_NEGATIVE;
    }
    // now we know the string is only digits and we converting it to int
    if(sscanf(recivedLine, "%d", &recieved) != 1)
    {
        return INITIAL_TO_NEGATIVE;
    }
    if(!checkIfNegativeNumber(recieved))
    {
        return INITIAL_TO_NEGATIVE;
    }
    return recieved;
}


/**
 * a function that parse first line in the file format, if the first
 * line is valid it set the relevant global variable gRequiredLengthOfRail to be its value.
 * @param recivedLine - the line to be checked
 * @return 0 if first line is valid, else return non-zero int
 */
int parseFirstLine(char* recivedLine)
{
    int parsingResult = parseTopLines(recivedLine);
    // means the first line content is invalid
    if(parsingResult<0)
    {
        return EXIT_FAILURE;
    }
    gRequiredLengthOfRail = parsingResult;
    return EXIT_SUCCESS;
}

/**
 * a function that parse second line in the file format, if the second
 * line is valid it set the relevant global variable gNumberOfConnectionsTypes to be its value.
 * @param recivedLine - the line to be checked
 * @return 0 if second line is valid, else return non-zero int
 */
int parseSecondLine(char* recivedLine)
{
    int parsingResult = parseTopLines(recivedLine);
    // means the second line content is invalid
    if(parsingResult<0)
    {
        return EXIT_FAILURE;
    }
    gNumberOfConnectionsTypes = parsingResult;
    return EXIT_SUCCESS;
}

/**
 * a function that parse third line in the file format, if the third
 * line is valid it set the relevant global variable gKindsOfConnections to be its value,
 * set the connection types to an array of chars.
 * @param recivedLine - the line to be checked
 * @return 0 if third line is valid, else return non-zero int
 */
int parseThirdLine(char *recivedLine)
{

    char* token = strtok(recivedLine, ",");
    int indexOfConnection0 = 0;
    while(token != NULL)
    {
        // means more than one char between commas
        if(strlen(token) != 1)
        {
            return EXIT_FAILURE;
        }
        // add the relevant connection type to the global array of connections, in the index
        // of this connection type in the recived line
        gKindsOfConnections[indexOfConnection0] = *token;
        indexOfConnection0++;
        // updating token
        token = strtok(NULL, ",\r\n");
    }
    return EXIT_SUCCESS;
}

/**
 * a function that check if a given connection part aka one of the edges
 * of a part is known, if its in the collection of the known connection types
 * @param givenConnection - the connection to be checked
 * @return 0 if the given connection is in the array of connection (known to us), else
 * return non-zero int
 */
int edgeOfPartIsKnow(char givenConnection)
{
    for(int index = 0; index < gNumberOfConnectionsTypes; index++)
    {
        if(givenConnection == gKindsOfConnections[index])
        {
            return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
}

/**
 * a function that checks if the number of the inserted parts to the array of parts is 0 mudulo
 * the LINEAR_FACTOR_TO_RESIZE (its important to be prime) if yes it means needed to resize to
 * a bigger array of parts
 * @param numberOfPartsInIt -the number of parts in the array of parts
 * @return 0 if the array is needed to be resize to a bigger array (the number of elements is a
 * multiplication of LINEAR_FACTOR_TO_RESIZE)
 */
int checkIfAllPartsSizeBigEnough(int numberOfPartsInIt)
{
    if((numberOfPartsInIt % LINEAR_FACTOR_TO_RESIZE) == 0)
    {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/**
 *a function that resize (to a bigger size) the array of parts to its current size
 * and adds to this size specific number that is set at LINEAR_FACTOR_TO_RESIZE
 * @param allParts - the array of all parts
 * @param indexOfLastElement -the index of the last element in the array, to this number
 * will be added one to get the number of elements on the array.
 * @return 0 if the resizing succeeded, else return non-zero int
 */
int resizeArrayOfParts(part** allParts, int indexOfLastElement)
{
    // adding to the index of the last element one to modify it to be the amount of
    // parts in the array and set it to be currentNumberOfParts
    int currentNumberOfParts = ++indexOfLastElement;
    (*allParts) = (part*)realloc(*allParts, (currentNumberOfParts + LINEAR_FACTOR_TO_RESIZE) *
    sizeof(part));
    if((*allParts) == NULL)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

/**
 * a function that parse part line in the file format, if the part
 * line is valid, it creates a new part and add it to the array of parts, in case the array
 * of parts is big enough it resize it, all of the tasks above are using help functions
 * @param recievdLine - the part line parse
 * @param allParts - the array of parts that is adding to it a partt if this line is valid
 * @param numberOfParts - the current number of parts in the array
 * @return 0 - if success all missions above, else return non-zero int
 */
int parsePartLine(char *recievdLine , part** allParts, int* numberOfParts)
{
    char startConnection;
    char endConnection;
    int length;
    int price;
    // checking if the last part of the line is in a valid format, its special because it might
    // contain special characters as \r or \n, this part of the line is recognized by being
    // after 3 commas
    int priceInvalidCharacters = checkSpecificAfterCommaLine(recievdLine, 3);
    if(priceInvalidCharacters)
    {
        return EXIT_FAILURE;
    }
    int succefulScanning = sscanf(recievdLine, "%c,%c,%d,%d", &startConnection,
            &endConnection, &length, &price);
    // checking that we successfully read 4 fields at the line in a specific format
    if(succefulScanning != 4)
    {
        return EXIT_FAILURE;
    }
    // checking if the values at the price and the length fields are greater than 0
    if((length <= 0 || price <= 0))
    {
        return EXIT_FAILURE;
    }
    //enter the "if" if at least of one the connection at the edges of the part is unfamiliar
    if(edgeOfPartIsKnow(startConnection) || edgeOfPartIsKnow(endConnection))
    {
        return EXIT_FAILURE;
    }
    part* newValidPart = NULL;
    // after checking all the fields of the line are valid, creating the part
    newValidPart = createPart(startConnection, endConnection, length, price);
    // check successful allocation of the part in the heap
    if(newValidPart == NULL)
    {
        return EXIT_FAILURE;
    }
    // addind the part to the array of parts
    (*allParts)[*numberOfParts] = *newValidPart;
    // updating the number of parts
    (*numberOfParts)++;
    // checking before any free order that we dont free a Null pointer and than free
    // the created pointer to this part after setting the array of all parts to point
    // to this place at the heap
    if(newValidPart != NULL)
    {
        free(newValidPart);
    }
    // checking if after adding a part to the array of parts, if the array of
    // parts using memory on the heap need to grow
    if(!checkIfAllPartsSizeBigEnough(*numberOfParts))
    {
        // checking if resizing of the array of parts went successfuly
        int failedToResize = resizeArrayOfParts(allParts, *numberOfParts);
        if(failedToResize)
        {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/**
 * a function that checks and stores all the data of the input file using many halp function
 * @param recievedFile the file recieved.
 * @return 0 in case of valid lines format and content, else return non-zero int
 */
int oneFunctionToParseThemAll(FILE *recievedFile, part** allparts, int* numberOfparts)
{
    char currentLine[MAX_LENGTH_OF_LINE] = {0};
    //checking file not empty by checking the first line
    if(fgets(currentLine, MAX_LENGTH_OF_LINE, recievedFile) == NULL)
    {
        if(writeOn(ADRESS_TO_FILE_TO_WRITE, FAIL_FILE_IS_EMPTY, EXIT_FAILURE))
        {
            return EXIT_FAILURE;
        }
        return EXIT_FAILURE;
    }
    // checks if the first line is valid if not return EXIT_FAILURE
    gErrorInLineIndex++;
    if(parseFirstLine(currentLine))
    {
        writeOn(ADRESS_TO_FILE_TO_WRITE, FAIL_IN_LINE, EXIT_FAILURE);
        return EXIT_FAILURE;
    }
    gErrorInLineIndex++;
    // check second line is not null while getting its content
    if(fgets(currentLine, MAX_LENGTH_OF_LINE, recievedFile) == NULL)
    {
        return EXIT_FAILURE;
    }
    if(parseSecondLine(currentLine))
    {
        writeOn(ADRESS_TO_FILE_TO_WRITE, FAIL_IN_LINE, EXIT_FAILURE);
        return EXIT_FAILURE;
    }
    // setting the amount of needed memoty for the pointer that points to the
    // kinds of connection array, knowing the neccesery amount of memory only
    // after reading successfully second line
    gKindsOfConnections = (char*)malloc(gNumberOfConnectionsTypes * sizeof(char));
    // checking memory allocation went successfully
    if(gKindsOfConnections == NULL)
    {
        return EXIT_FAILURE;
    }
    // updating the line that might be invalid
    gErrorInLineIndex++;
    //check third line is not empty
    if(fgets(currentLine, MAX_LENGTH_OF_LINE, recievedFile) == NULL)
    {
        return EXIT_FAILURE;
    }
    // after getting the third line and knowing its not empty parsing it
    if(parseThirdLine(currentLine))
    {
        writeOn(ADRESS_TO_FILE_TO_WRITE, FAIL_IN_LINE, EXIT_FAILURE);
        return EXIT_FAILURE;
    }
    // updating the line that might be invalid
    gErrorInLineIndex++;
    // while not reaching empty line and succesfully reading the next one,
    // we parse each part line
    while(fgets(currentLine, MAX_LENGTH_OF_LINE, recievedFile) != NULL)
    {
        // the part line parsing and checking its validity
        if(parsePartLine(currentLine, allparts, numberOfparts))
        {
            writeOn(ADRESS_TO_FILE_TO_WRITE, FAIL_IN_LINE, EXIT_FAILURE);
            return EXIT_FAILURE;
        }
        // updating the line that might be invalid after each successful line reading
        gErrorInLineIndex++;
    }
    return EXIT_SUCCESS;
}

/**
 * a function that gets the best (lowest) price for a given length and ending connection type
 * by going over all parts that answer this requirments and by using the table of prices for
 * any length that is smaller than the one we are looking for now
 * @param length - the length we are looking to get the best price for
 * @param endindIndex - the ending connection type that we are looking to get the best price for
 * @param priceTable - the table that holds all the data for previous checked lengths (for a
 * lower length holds the best price for all the ending connection types
 * @param allParts - the array of all the parts
 * @param numberOfParts - the number of parts in the array of parts
 * @return the best price for a given length and ending connection type, if havnt found any
 * return infinity aka INT_MAX
 */
int getMinPrice(int length, int endindIndex, int*** priceTable, part** allParts, int numberOfParts)
{
    //this if: in case the required length is 0
    if (length == 0)
    {
        return 0;
    }
    //defualt price set to infinty AKA MAX_INT
    int bestPriceForThisLength = INT_MAX;
    char wantedEnding = gKindsOfConnections[endindIndex];
    // for each part checking if its might be a better price depending on
    // the given length we check and the ending connection type
    for(int partIndex = 0; partIndex < numberOfParts; partIndex++)
    {
        //making code clearer and less using of index every line by setting local
        // variables that hols the relevant data to work with
        int specificPartLength = (*allParts)[partIndex]._length;
        int specificPartPrice = (*allParts)[partIndex]._price;
        char specificEndingType = (*allParts)[partIndex]._end;
        int specificIndexOfStartInArrayOfConnections = (*allParts)[partIndex]
                ._indexOfStartInKindsOfConnection;
        // next checks divided for a several if condition to make
        //the code readable
        if (specificEndingType == wantedEnding)
        {
            //this if: in case specific part is at exact length and ending connection type as
            // required
            if (specificPartLength == length)
            {
                if (specificPartPrice < bestPriceForThisLength)
                {
                    bestPriceForThisLength = specificPartPrice;
                }
            }
            else if (specificPartLength < length)
            {
                int priceForOneStepBefore = (*priceTable)
                [length - specificPartLength][specificIndexOfStartInArrayOfConnections];
                //using the fact that code is read from left to right we first check
                // if the one step before price is infinity, and if it is infinity we are
                // not reading the continue of the condition there for we solve the
                // int overflow problem (that means adding infinity to number).
                // checking if the price of one step before is infinity ( means recent length
                // that ends with the begginig of this part cant be reached) and if the
                // specific part price + the price to get to the last length is less than
                // our current candidate for best price for current length
                if (priceForOneStepBefore!=INT_MAX && ((specificPartPrice +
                    priceForOneStepBefore) < bestPriceForThisLength))
                {
                    bestPriceForThisLength = specificPartPrice + priceForOneStepBefore;
                }
            }
        }
    }
    return bestPriceForThisLength;
}

/**
 * a function that checks for the ovarall length (the required length in the first line in the file)
 * the minimum price for it out of all the prices for the overall length with different ending
 * connection type
 * @param finishedTable - the table that holds all the prices for all the lengthes and the ending
 * connection types
 * @return the lowest price to build a rail in the required length, if its not possible
 * to build a rail in the required length returns -1
 */
int getTheOverAllCheapestPrice( int*** finishedTable)
{
    // initial the result to infinity to check the minimum (that is used to comparing)
    int result = INT_MAX;
    // checking for every ending connection type its price and updating the result if we found
    // a better price
    for(int i = 0; i < gNumberOfConnectionsTypes; i++)
    {
        if((*finishedTable)[gRequiredLengthOfRail][i] < result)
        {
            result = (*finishedTable)[gRequiredLengthOfRail][i];
        }
    }
    // in case there is no a smaller price than infinity (no such length of rail is buildable)
    if(result == INT_MAX)
    {
        result = INITIAL_TO_NEGATIVE;
    }
    return result;
}

int main(int argc, char* argv[])
{
    //check number of argumants
    if (argc != NUMBER_OF_REQUIRED_ARGUMENTS)
    {
        //try to write to file the error
        if (writeOn(ADRESS_TO_FILE_TO_WRITE, FAIL_INVALID_NUM_OF_ARGS, EXIT_FAILURE))
        {
            return EXIT_FAILURE;
        }
        return EXIT_FAILURE;
    }
    FILE *recievedFile = fopen(argv[1], "r");
    //check if succesfully open the source file to read from
    if (recievedFile == NULL)
    {
        //try to write to file the error
        if (writeOn(ADRESS_TO_FILE_TO_WRITE, FAIL_FILE_DONT_EXIST, EXIT_FAILURE))
        {
            return EXIT_FAILURE;
        }
        return EXIT_FAILURE;
    }
    part* allParts = (part* )calloc(SIZE_INITIAL_OF_ARRAY, sizeof(part));
    if(allParts == NULL)
    {
        EXIT_FAILURE;
    }
    int numberOfParts = 0;
    if(oneFunctionToParseThemAll(recievedFile, &allParts, &numberOfParts))
    {
        fclose(recievedFile);
        setAllProgramDataFree(&allParts, NULL);
        return EXIT_FAILURE;
    }
    fclose(recievedFile);
    //allocating empty table and initial it to the size of given length +1 (including 0)
    int** tableOfPrices = (int**)calloc(gRequiredLengthOfRail + 1, sizeof(int*));
    if(tableOfPrices == NULL)
    {
        EXIT_FAILURE;
    }
    for(int l = 0 ; l <= gRequiredLengthOfRail; l++)
    {
        // allocation and initilizing each part column
        tableOfPrices[l] = (int*)calloc(gNumberOfConnectionsTypes, sizeof(int));
        if(tableOfPrices[l] == NULL)
        {
            EXIT_FAILURE;
        }
    }
    for(int l = 0 ; l <= gRequiredLengthOfRail; l++)
    {
        for (int k = 0; k < gNumberOfConnectionsTypes; k++)
        {
            tableOfPrices[l][k] = getMinPrice(l, k, &tableOfPrices, &allParts, numberOfParts);
        }
    }
    //get the best price to build a rail in the required length
    int result = getTheOverAllCheapestPrice(&tableOfPrices);
    setAllProgramDataFree(&allParts, &tableOfPrices);
    //that means no error during all the run and we try to write the answer to the output file
    int resultOfWritingToFile = writeOn(ADRESS_TO_FILE_TO_WRITE, SUCCESS_TO_BE_WRRITEN_TO_FILE,
            result);
    if(resultOfWritingToFile)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
