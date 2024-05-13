# Process-Synchronization-with-fork-and-pipe-
This program uses the C++ fork() method to create multiple processes that synchronize their work through the pipe() method in order to return a final output.

The input file provides each process with various instructions to perform certain functions on the input variables.

Below are the comments found in main.cpp:

This program is to highlight the functionality of both the fork() and pipe() functions in c++. 
The two files, main.cpp and values.h, work together to functionally use fork and pipe in order 
to fulfill the prompt. 

values.h:

  values.h strictly contains standard implementation of a doubly linked list, with struct values of value (integer value of variable) and name (name of variable).
  
  functions include...
    
    void insert(node **_head, std::string _name, int _value) {} - takes double pointer to head node, name, and value as parameters, and inserts into selected linked list

    void print_all(node *_head, std::ofstream &ofs) {} - takes head node and output file stream as parameters, and simply prints all values in selected linked list

    void print_reverse(node *_head) {} - takes head node as parameter, and prints values in reverse

    node* find_value(node** _head, std::string _name) {} - takes double pointer to head node, and name. it searches through the selected linked list until the current node's name value is equal to '_name'

main.cpp:
  
  starts by initializing input and output file streams to arguments in the command line,
  the while loop then reads the 'sv' file and retrieves all command lines and then 
  stores them in a vector 'command_lines'. Each string in the vector is then filtered
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

