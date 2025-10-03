#include <cstddef>
#include <cstdio>
#include <iostream>
#include <ostream>

/*
===ERROR TYPES===
-1 = NO FILE ENTERED
-2 = FILE NOT FOUND
*/

// Structure to hold a single task's data
typedef struct{
    char name[256];   // Task name (max 255 chars + null terminator)
    int priority;     // Priority number (user decides)
    bool isDone;      // Status: true = done, false = pending
}Task_str;

void menuUI();  // Displays the menu options

int main(int argc, char *argv[]){
    // If no file name is provided, exit with error
    if(argc < 2){
        std::cout << "file not found";
        return -1;
    }

    Task_str task;            // Temporary task holder
    Task_str *editTasks;      // Dynamic array for editing tasks
    bool isOpen = true;       // Keeps program running until user quits
    int decision, taskCount = 0, i = 0, marking;
    size_t taskSize = sizeof(Task_str);
    FILE *data;

    // Debug: shows program arguments
    std::cout << argc << std::endl;
    for(int i = 0; i < argc; i++) std::cout << argv[i] << std::endl;

    // Main program loop
    while(isOpen == true){
        menuUI();
        std::cout << "Pick an option: ";
        std::cin >> decision;
        std::cin.ignore(); // clears leftover newline from input

        switch (decision) {
            case 1: // ADD TASK
                std::cout << "Enter task: ";
                std::cin.getline(task.name, sizeof(task.name));
                std::cout << "Enter priority (whole number): ";
                std::cin >> task.priority;
                std::cin.ignore();
                task.isDone = false; // new tasks start as pending

                // Open file in append-binary mode
                data = fopen(argv[1], "ab");
                if(data == NULL){
                    std::cout << "file couldnt be accessed";
                    return -2;
                }

                // Write the new task to the file
                fwrite(&task, sizeof(Task_str), 1, data);
                std::cout << "task saved to " << argv[1] << std::endl;

                fclose(data);
                break;

            case 2: // LIST TASKS
                data = fopen(argv[1], "rb");
                if(data == NULL){
                    std::cout << "file couldnt be accessed";
                    return -2;
                }

                std::cout << "\n=======TASKS=======" << std::endl;
                taskCount = 0;

                // Read each task from file and print it
                while(fread(&task, sizeof(Task_str), 1, data) == 1) {
                    std::cout << ++taskCount << ". " 
                              << task.name 
                              << ", " << (task.isDone ? "Done" : "Pending") 
                              << ". priority: " << task.priority
                              << std::endl;
                }

                if(taskCount == 0) {
                    std::cout << "No tasks found." << std::endl;
                }

                fclose(data);
                std::cout << "==================="<< std::endl << std::endl;
                break;
            
            case 3: // MARK TASK AS DONE
                // First show task list
                data = fopen(argv[1], "rb");
                if(data == NULL){
                    std::cout << "file couldnt be accessed";
                    return -2;
                }
                
                std::cout << "\n=======TASKS=======" << std::endl;
                taskCount = 0;
                while(fread(&task, sizeof(Task_str), 1, data) == 1) {
                    std::cout << ++taskCount << ". " 
                              << task.name 
                              << ", " << (task.isDone ? "Done" : "Pending") 
                              << ". priority: " << task.priority
                              << std::endl;
                }

                if(taskCount == 0) {
                    std::cout << "No tasks found." << std::endl;
                }
                fclose(data);
                std::cout << "==================="<< std::endl << std::endl;

                // Load all tasks into memory
                data = fopen(argv[1], "rb");
                if(data == NULL){
                    std::cout << "file couldnt be accessed";
                    return -2;
                }

                editTasks = new Task_str[taskCount];
                i = 0;
                while(fread(&editTasks[i], sizeof(Task_str), 1, data) == 1) i++;
                fclose(data);

                // Ask user which task to mark done
                do{
                    std::cout << "Which you want to mark as done: ";
                    std::cin >> marking;
                    if(marking < 1 || marking > taskCount){
                        std::cout << "Invalid input" << std::endl;
                    }
                }while(marking < 1 || marking > taskCount);

                // Mark selected task
                if(editTasks[marking-1].isDone) {
                    std::cout << "Task is already marked as done!" << std::endl;
                } else {
                    editTasks[marking-1].isDone = true;
                    std::cout << "Task '" << editTasks[marking-1].name << "' marked as done!" << std::endl;
                }

                // Rewrite all tasks back to the file
                data = fopen(argv[1], "wb");
                if(data == NULL){
                    std::cout << "file couldnt be accessed";
                    return -2;
                }

                fwrite(editTasks, sizeof(Task_str), taskCount, data);
                fclose(data);

                delete [] editTasks; // free memory
                break;

            default: // EXIT PROGRAM
                isOpen = false;
                break;
        }
    }

    return 0;
}

// Shows the menu on screen
void menuUI(){
    std::cout << "======TO-DO=======" << std::endl;
    std::cout << "1) ADD TASK" << std::endl;
    std::cout << "2) LIST TASKS" << std::endl;
    std::cout << "3) MARK TASK DONE" << std::endl;
    std::cout << "ANY) EXIT" << std::endl;
    std::cout << "==================" << std::endl;
}
