#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#include "values.h"

// This program is to highlight the functionality of both the fork() and pipe() functions in c++. 
// The two files, main.cpp and values.h, work together to functionally use fork and pipe in order 
// to fulfill the prompt. 

// values.h:
//      values.h strictly contains standard implementation of a doubly linked list, with struct values of
//      value (integer value of variable) and name (name of variable).
//      functions include...
//          void insert(node **_head, std::string _name, int _value) {} - takes double pointer to head node, name, and value as parameters, and inserts
//                                                                        into selected linked list

//          void print_all(node *_head, std::ofstream &ofs) {} - takes head node and output file stream as parameters, and simply
//                                                               prints all values in selected linked list

//          void print_reverse(node *_head) {} -                 takes head node as parameter, and prints values in reverse

//          node* find_value(node** _head, std::string _name) {} - takes double pointer to head node, and name. it searches through the selected linked
//                                                                 list until the current node's name value is equal to '_name'

// main.cpp:
//      starts by initializing input and output file streams to arguments in the command line,
//      the while loop then reads the 'sv' file and retrieves all command lines and then 
//      stores them in a vector 'command_lines'. Each string in the vector is then filtered
//      and reformatted so variables are more easily attainable, which it does by 
//      removing spaces, finding indexes and ranges of each variable, and seeing which (if any)
//      operation character is found in the command line. 

//      Then, forking and piping are used to connect the reading and writing processes of the command
//      lines. The child process (id == 0) is used to read 'giver' variables and send them through
//      a pipe to the parent function, where giver is the value of the variable that is being used to 
//      manipulate another variable.
//      Since both parent and child functions contain iterative loops, then for each iteration of 
//      reading through the command lines the parent waits for the child to finish its iteration first, as
//      to work through the lines together chronologically. The parent then reads the giver variable 
//      from the pipe (which is sent as a linked list node), reads the value from that node, and then
//      alters the 'taker' node's value, which is found using the 'find_value()' function in values.h.

//      Upon finishing their respective iterative loops, they will both reach the end of main.cpp, where 
//      only the parent is allowed to print the final values, so as to not print twice.

//      Short explanations will be provided for functions as they are initialized below.

int main(int argc, char *argv[]) {

    //      initialization of two linked lists, one for input variables and the other for internal variables

  node *input_vars_head = nullptr;
  node *internal_vars_head = nullptr;

    //      opening input and output file streams

  std::ifstream graph_file(argv[1]);        // argv[1]
  std::ifstream input_file(argv[2]);    // argv[2]
  std::ofstream output_file(argv[3]); // argv[3]

  output_file << "PROCESSES:" << std::endl;
  output_file << "----------" << std::endl << std::endl;

  std::string graph_string;

  //        various vectors to store information from the command lines of 'graph_file'

  std::vector<std::string> input_vars_names;
  std::vector<std::string> internal_vars_names;
  std::vector<std::string> command_lines;

  //        reading from graph_file strings and storing them in the above vectors

  while (std::getline(graph_file, graph_string)) {

    if (graph_string.find("input_var") !=
        std::string::npos) { // reading input vars

      for (int i = 10; i < graph_string.size(); ++i) {

        if ((graph_string[i] != ' ') && (graph_string[i] != ',') &&
            (graph_string[i] != ';')) {

          std::string vars_names_pushee = std::string(1, graph_string[i]);

          input_vars_names.push_back(vars_names_pushee);
          
        }
      }

    } else if (graph_string.find("internal_var") !=
               std::string::npos) { // reading internal variables

      int sub_size = graph_string.size() - 13;

      std::string int_vars_substring = graph_string.substr(13, sub_size);

      replace(int_vars_substring.begin(), int_vars_substring.end(), ',', ' ');
      replace(int_vars_substring.begin(), int_vars_substring.end(), ';', ' ');

      std::string internal_process_name;

      std::istringstream iss(int_vars_substring);

      while (iss >> internal_process_name) {

        internal_vars_names.push_back(internal_process_name);
        insert(&internal_vars_head, internal_process_name, 0);
      }

    } else if (graph_string.find("write") !=
               std::string::npos) { // terminate while loop

      break;

    } else {

      command_lines.push_back(graph_string);
    }
  }

  std::string input_string;

  int input_vars_names_count = 0;

  //        using the variables input_string and input_vars_names_count, the while loop reads through
  //        the input file for integer values to be stored in the input variables. 
  //        it first formats the string by replacing commas with spaces, and then reading from 
  //        a stringstream
  //        
  //        it then inserts values into their respective input variables, found in the linked list
  //        with head 'input_vars_head'

  while (std::getline(input_file, input_string)) {

    replace(input_string.begin(), input_string.end(), ',', ' ');

    int n;

    std::istringstream iss(input_string);

    while (iss >> n) {

      std::string input_name_temp = input_vars_names[input_vars_names_count++];
      insert(&input_vars_head, input_name_temp, n);
    }
  }

  output_file << "Input variables..." << std::endl;
  print_all(input_vars_head, output_file);
  output_file << std::endl << std::endl;

  output_file << "Internal variables..." << std::endl;
  print_all(internal_vars_head, output_file);
  output_file << std::endl << std::endl;

  // read process lines, have child read the 'giver' variable, or the variable which will have its value used to calculate another,
  // and have the parent read both the operation character and the 'taker' variable, or the variable which will be altered 
  // by combination of both the operation character and the integer value in 'giver'
  
  //pd[0] == read, pd[1] == write

  int pd[2];

  if (pipe(pd) == -1) {

    return 1;
  }

  int id = fork();

  if (id == 0) { // child

    close(pd[0]);

    for (int i = 0; i < command_lines.size(); ++i) {

      output_file << "Child Process: " << std::endl;

      std::string command_temp = command_lines[i];
      std::string giver_name;
      std::string taker_name;
      node *giver_head = nullptr;

        //      formatting the current command line by removing spaces

      command_temp.erase(
          remove_if(command_temp.begin(), command_temp.end(), isspace),
          command_temp.end());
      output_file << "Command Line:" << command_temp << std::endl;

        //      storing the index of '->' to know when to stop reading for the 'giver' variable.
        //      this is a bit extra, but is done in order to prevent issues with variation in 
        //      the input file. like, for example, if each input variable name was not a single character 'x','y','z', 
        //      but instead a string 'input1','input2','input3'

      int seperation_index = command_temp.find("->");
      int giver_starting_index = 0;

        //      reading the first character of current command line to find operation character, if any

      if ((command_temp[0] == '*') || (command_temp[0] == '+') ||
          (command_temp[0] == '-') || (command_temp[0] == '/')) {

        giver_starting_index = 1;
      }

      for (int i = giver_starting_index; i < seperation_index; ++i) {

        giver_name += command_temp[i];
      }


        //      searches through both input and internal variable linked lists to see 
        //      which one the current command line's giver belongs to. if neither, return 1

      if (find_value(&input_vars_head, giver_name) != nullptr) {

        giver_head = input_vars_head;

      } else if (find_value(&internal_vars_head, giver_name) != nullptr) {

        giver_head = internal_vars_head;

      } else {

        output_file << "Could not find parent head of " << giver_name << "."
                    << std::endl;

        return 1;
      }

        //      find_value returns a pointer to the node which shares its name with 'giver_name'

      node *giver_temp = find_value(&giver_head, giver_name);
      int giver_temp2;

        //      sending a pointer to a node through the pipe, which makes it easier for the parent as
        //      the current value of said node will always be up to date, which may not be the case 
        //      if just an integer were sent

      output_file << "Sending name " << giver_name << " with value "
                  << giver_temp->value << " through the pipe." << std::endl;
      giver_temp2 = write(pd[1], &giver_temp, sizeof(giver_temp));

      output_file << "child iteration done" << std::endl
                  << std::endl
                  << std::endl;
    }

    close(pd[1]);

  } else { // parent

    close(pd[1]);

    for (int i = 0; i < command_lines.size(); ++i) {

        //      parent waits for child to send a node through the pipe for each iteration

      wait(NULL);

      output_file << "Parent Process: " << std::endl;
      std::string command_temp = command_lines[i];
      std::string taker_name;
      node *taker_head = nullptr;

        //      formatting of command line

      command_temp.erase(
          remove_if(command_temp.begin(), command_temp.end(), isspace),
          command_temp.end());

      int seperation_index = command_temp.find("->");

      int tak_str_size = command_temp.size();

        //      preventative measure for sv2, as it had missing semicolons in some lines :(

      if (command_temp[command_temp.size() - 1] == ';') {

        tak_str_size = tak_str_size - 1;
      }

      for (int i = seperation_index + 2; i < tak_str_size; ++i) {

        taker_name += command_temp[i];
      }

        //      finding which linked list taker_name belongs to

      if (find_value(&input_vars_head, taker_name) != nullptr) {

        taker_head = input_vars_head;

      } else {

        taker_head = internal_vars_head;
      }

      int oper_temp;
      int d;
      node *giver_node_temp;

      std::string operation_print;

      node *taker = find_value(&taker_head, taker_name);

        //      reading node from pipe and storing in giver_node_temp

      d = read(pd[0], &giver_node_temp, sizeof(giver_node_temp));

      oper_temp = giver_node_temp->value;

        //      parent now performs arithmetic with given operation character and giver's value
        //      on the taker node's value
      if (command_temp[0] == '*') {

        taker->value = (taker->value) * oper_temp;
        operation_print = "*";

      } else if (command_temp[0] == '+') {

        taker->value = (taker->value) + oper_temp;
        operation_print = "+";

      } else if (command_temp[0] == '-') {

        taker->value = (taker->value) - oper_temp;
        operation_print = "-";

      } else if (command_temp[0] == '/') {

        taker->value = (taker->value) / oper_temp;
        operation_print = "/";

      } else {

        taker->value = oper_temp;
        operation_print = " ";
      }

      output_file << "Reading value " << oper_temp
                  << " from pipe and performing... " << taker_name << " "
                  << operation_print << "=" << oper_temp << "." << std::endl;

      output_file << "parent iteration done" << std::endl
                  << std::endl
                  << std::endl;
    }

    close(pd[0]);
  }

    //      final variables that are outputted, made so that only the parent can access.

  if (id > 0) {

    output_file << std::endl << std::endl;

    output_file << "FINAL VARIABLES:" << std::endl;
    output_file << "----------------" << std::endl << std::endl;
    output_file << "Input Variables: " << std::endl;

    print_all(input_vars_head, output_file);
    output_file << std::endl;

    output_file << "Internal Variables: " << std::endl;

    print_all(internal_vars_head, output_file);
    output_file << std::endl;

  }

    //      Done! Thank you for reading.

  return 0;
}
