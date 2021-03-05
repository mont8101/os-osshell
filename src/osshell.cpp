#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <filesystem>
#include <unistd.h>
#include <fstream>
#include <sys/stat.h>
#include <filesystem>
#include <sys/wait.h>
#include <ctype.h>
using namespace std;

bool fileExecutableExists(std::string file_path);
void splitString(std::string text, char d, std::vector<std::string>& result);
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result);
void freeArrayOfCharArrays(char **array, size_t array_length);
void splitPathCmd(std::string split, char *path, std::vector<std::string>& cmd);

int main (int argc, char **argv)
{
    // Get list of paths to binary executables
    std::vector<std::string> os_path_list;
    char* os_path = getenv("PATH");
    splitString(os_path, ':', os_path_list);

    // Create list to store history
    std::vector<std::string> history;

    // Create variables for storing command user types
    std::string user_command;               // to store command user types in
    std::vector<std::string> command_list;  // to store `user_command` split into its variour parameters
    char **command_list_exec;               // to store `command_list` converted to an array of character arrays

    // Welcome message
    printf("Welcome to OSShell! Please enter your commands ('exit' to quit).\n");

    std::string input;
    std::string exitCheck = "exit";
    std::string historyCheck = "history";
    std::string clearCheck = "clear";

    // Repeat:
    //  Print prompt for user input: "osshell> " (no newline)
    //  Get user input for next command
    //  If command is `exit` exit loop / quit program
    //  If command is `history` print previous N commands
    //  For all other commands, check if an executable by that name is in one of the PATH directories
    //   If yes, execute it
    //   If no, print error statement: "<command_name>: Error command not found" (do include newline)
    int linecount = -1;
    while(input != exitCheck){
        std::cout << "osshell> " ;
        std::getline(std::cin, input);
        
        splitString(input, ' ', command_list); //split input into cmd list
        vectorOfStringsToArrayOfCharArrays(command_list, &command_list_exec); //converts it to char **
        std::filesystem::path path;
        
        //set current path to where youre at 
        path = std::filesystem::current_path();
        //std::cout << "path: " << path << std::endl;
        //check if command exists

        std::string line;
        const char *specificLines[128][32];
        std::ofstream myfile;
        myfile.open("src/history.txt", ios::app);
        if (myfile.is_open()){
            myfile << input;
            myfile << endl;
            linecount++;
            myfile.close();
        }
        else cout << "Unable to open file" << endl;

        if(command_list_exec[0] == historyCheck){
            if(command_list_exec[1] != NULL && command_list_exec[1] == clearCheck){
                std::cout << "IM GOING TO CLEAR" << std::endl;
                remove("src/history.txt");
            }
            else if(command_list_exec[1] != NULL){
                bool num = true;
                for(int i = 1; i < strlen(command_list_exec[1]); i++){
                    if(!isdigit(command_list_exec[1][i])){
                        num = false;
                    }
                }
                if(num){
                    //for(int i = history.size() - inputNum; i < history.size; i++){}
                    int startLine = atoi(command_list_exec[1]);
                    ifstream infile;
                    infile.open("src/history.txt", ios::in);
                    int linecounter = -1;
                    while(getline(infile, line)){
                        linecounter++;
                        specificLines[linecounter][0] = line.c_str();
                    }
                    int checkLine;
                    checkLine = linecounter - startLine;
                    startLine = linecounter - checkLine;
                    for(int i = startLine; i < linecounter; i++){
                        std::cout << startLine << ": " << specificLines[startLine][0] << std::endl;
                        startLine++;
                    }
                    //getline(i);
                }
                else{
                    std::cout << "That is not a number" << std::endl;
                }
            }
            else {
                ifstream infile;
                int linecounter = -1;
                infile.open("src/history.txt", ios::in);
                while(getline(infile, line)){
                    linecounter++;
                    std::cout << linecounter << ": " << line << std::endl;
                //history.push_back(line);
                }
                infile.close();
            }
                    //if file == 128 lines, decrement counter, copy to temp file, copy temp to history.txt except 1st line
        }


        //write function to take in ospathlist and command list to check in for loop if it exists, return first path found, return empty string if dont find for error
        else if(command_list_exec[0][0] == '.' || command_list_exec[0][0] == '/'){
            if(std::filesystem::exists(command_list[0])){ //check if it exists in current directory if its . or / 
                int pid = fork();
                if(pid == 0){
                    execv(command_list_exec[0], command_list_exec);
                    exit(1);
                }
                else{
                    int status;
                    waitpid(pid, &status, 0);
                }
            }        
            else {
                if(input != exitCheck){
                   std::cout << "error, command not found" << endl;
                }
            }
        }
        else{
            bool found = false;
            for(int i = 0; i < os_path_list.size(); i++){
                std::string pathCheck = os_path_list[i] + '/' + command_list_exec[0];
                if(std::filesystem::exists(pathCheck)){
                    found = true;
                    int pid = fork();
                    if(pid == 0){
                        execv(pathCheck.c_str(), command_list_exec);
                        exit(1);
                    }
                    else{
                        int status;
                        waitpid(pid, &status, 0);
                    }
                    i = os_path_list.size();
                }   
            }
            if(found == false){
                if(input != exitCheck){
                        std::cout << "error, command not found" << endl;
                    }
            }
        }
        
        
        //freeArrayOfCharArrays(command_list_exec, command_list.size() + 1);
        //run if child
        //if parent wait
        
        

        freeArrayOfCharArrays(command_list_exec, command_list.size()+1);

    }



    return 0;
}
/*
if(input == historyCheck){
            for(int i = 0; i < history.size(); i++){
                std::cout << i << ":" << history[i] << std::endl;
            }
        }

        if(input == historyCheck){
            myfile.open("history.txt");
            //for(int i = history.size() - inputNum; i < history.size; i++){}
                //getline(i);
            //while(getline(line, myfile, myfile)){
                //std::cout << line << std::endl;
            //}
            myfile.close();
        }
*/



/*
   file_path: path to a file
   RETURN: true/false - whether or not that file exists and is executable
*/
bool fileExecutableExists(std::string file_path)
{
    bool exists = false;
    // check if `file_path` exists
    // if so, ensure it is not a directory and that it has executable permissions

    return exists;
}

/*
   text: string to split
   d: character delimiter to split `text` on
   result: vector of strings - result will be stored here
*/
void splitString(std::string text, char d, std::vector<std::string>& result)
{
    enum states { NONE, IN_WORD, IN_STRING } state = NONE;

    int i;
    std::string token;
    result.clear();
    for (i = 0; i < text.length(); i++)
    {
        char c = text[i];
        switch (state) {
            case NONE:
                if (c != d)
                {
                    if (c == '\"')
                    {
                        state = IN_STRING;
                        token = "";
                    }
                    else
                    {
                        state = IN_WORD;
                        token = c;
                    }
                }
                break;
            case IN_WORD:
                if (c == d)
                {
                    result.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
            case IN_STRING:
                if (c == '\"')
                {
                    result.push_back(token);
                    state = NONE;
                }
                else
                {
                    token += c;
                }
                break;
        }
    }
    if (state != NONE)
    {
        result.push_back(token);
    }
}

/*
   list: vector of strings to convert to an array of character arrays
   result: pointer to an array of character arrays when the vector of strings is copied to
*/
void vectorOfStringsToArrayOfCharArrays(std::vector<std::string>& list, char ***result)
{
    int i;
    int result_length = list.size() + 1;
    *result = new char*[result_length];
    for (i = 0; i < list.size(); i++)
    {
        (*result)[i] = new char[list[i].length() + 1];
        strcpy((*result)[i], list[i].c_str());
    }
    (*result)[list.size()] = NULL;
}

/*
   array: list of strings (array of character arrays) to be freed
   array_length: number of strings in the list to free
*/
void freeArrayOfCharArrays(char **array, size_t array_length)
{
    int i;
    for (i = 0; i < array_length; i++)
    {
        if (array[i] != NULL)
        {
            delete[] array[i];
        }
    }
    delete[] array;
}