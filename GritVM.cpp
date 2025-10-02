/* Daud Ahmad Nisar (U37366522) and Adesh Nandlal Kessani (U69434322)
This code implements a virtual machine (GritVM) that loads, executes, and manages a list of instructions, 
manipulating memory and an accumulator while supporting operations like arithmetic, jumps, and halting*/


#include "GritVM.hpp"

// Load a GVM file into the program and set data members
STATUS GritVM::load(std::string filename, const std::vector<long> &initialMemory) {
    if(status != WAITING) // Ensure the machine is in a waiting state before loading
        return status;

    std::ifstream iFF(filename); // Open the input file
    std::string gvmLine;

    if(iFF.fail()) {
        throw("CannotOpenFile"); // Throw an error if the file cannot be opened
    }
    else {
        while (!iFF.eof()) { // Read each line until reaching the end of the file
            getline(iFF, gvmLine);
            if(!gvmLine.empty()) { // Ignore empty lines
                if(gvmLine.at(0) != '#') // Ignore lines starting with '#' (comments)
                {
                    Instruction is = GVMHelper::parseInstruction(gvmLine);
                    if(is.operation == UNKNOWN_INSTRUCTION) {
                        status = ERRORED; // Mark status as ERRORED if an unknown instruction is found
                        return status;
                    }
                    instruction_mem.push_back(is); // Store valid instruction
                }
            }
        }
    }

    // If no instructions were loaded, keep status as WAITING, otherwise set to READY
    status = instruction_mem.empty() ? WAITING : READY; 

    data_mem = initialMemory; // Initialize memory with provided values
    currentInstruction = instruction_mem.begin(); // Point to the first instruction
    iFF.close(); // Close the file

    return status;
}

// Execute the loaded instructions in the GritVM machine
STATUS GritVM::run() {
    if(status == READY) { // Ensure the machine is ready to run
        status = RUNNING;
        while(status == RUNNING) { // Execute until status changes
            advance(evaluate(*currentInstruction)); // Process current instruction
        }
    }
    return status;
}

// Reset the virtual machine to its initial state
STATUS GritVM::reset() {
    accumulator = 0; // Reset accumulator
    data_mem.clear(); // Clear memory
    instruction_mem.clear(); // Clear instruction list
    status = WAITING; // Set status back to WAITING
    return status;
}

// Evaluate an instruction and determine the next step
long GritVM::evaluate(Instruction inst) {
    switch (inst.operation)
    {
        case CLEAR: {
            accumulator = 0; // Reset the accumulator
            return 1;
        }
        break;
        case AT: {
            accumulator = data_mem[inst.argument]; // Load memory value into accumulator
            return 1;
        }
        break;
        case SET: {
            data_mem[inst.argument] = accumulator; // Store accumulator value in memory
            return 1;
        }
        break;
        case INSERT: {
            auto iter = data_mem.begin();
            data_mem.insert(iter + inst.argument, accumulator); // Insert value at specified index
            return 1;
        }
        break;
        case ERASE: {
            auto iter = data_mem.begin();
            auto itErase = iter + inst.argument;
            data_mem.erase(itErase); // Remove value at specified index
            return 1;
        }
        break;
        case ADDCONST: {
            accumulator += inst.argument; // Add constant to accumulator
            return 1;
        }
        break;
        case SUBCONST: {
            accumulator -= inst.argument; // Subtract constant from accumulator
            return 1;
        }
        break;
        case MULCONST: {
            accumulator *= inst.argument; // Multiply accumulator by constant
            return 1;
        }
        break;
        case DIVCONST: {
            accumulator /= inst.argument; // Divide accumulator by constant
            return 1;
        } 
        break;
        case ADDMEM:{
            accumulator += data_mem[inst.argument]; // Add memory value to accumulator
            return 1;
        }
        break;
        case SUBMEM: {
            accumulator -= data_mem[inst.argument]; // Subtract memory value from accumulator
            return 1;
        }
        break;
        case MULMEM: {
            accumulator *= data_mem[inst.argument]; // Multiply accumulator by memory value
            return 1;
        }
        break;
        case DIVMEM: {
            accumulator /= data_mem[inst.argument]; // Divide accumulator by memory value
            currentInstruction++;
        }
        break;
        case JUMPREL: {
            if(inst.argument == 0) {
                status = ERRORED; // Prevent jumps of zero
                return 0;
            }
            return inst.argument;
        }
        break;
        case JUMPZERO: {
            if(inst.argument == 0) {
                status = ERRORED;
                return 0;
            }
            return (accumulator == 0) ? inst.argument : 1; // Jump if accumulator is zero
        }
        break;
        case JUMPNZERO: {
            if(inst.argument == 0) {
                status = ERRORED;
                return 0;
            }
            return (accumulator != 0) ? inst.argument : 1; // Jump if accumulator is nonzero
        }
        break;
        case NOOP: { // No operation
            return 1;
        }
        break;
        case HALT:  {
            status = HALTED; // Stop execution
            return 1;
        }
        break;
        case OUTPUT: {
            std::cout << accumulator; // Print accumulator value
            return 1;
        }
        break;
        case CHECKMEM: {
            if(static_cast<long>(data_mem.size()) < inst.argument) { // Check for valid memory access
                status = ERRORED;
            }
            return 1;   
        }
        break;
        case UNKNOWN_INSTRUCTION: { // Handle unknown instruction case
            status = UNKNOWN;
            return 1;
        }
        break;
        default: {
            return 1;
        }
        break;
    }
    return 0;
}

// Move the instruction pointer forward or backward
void GritVM::advance(long jump) {
    if(jump > 0) {
        while(jump != 0) {
            currentInstruction++; // Move forward
            jump--;
        }
    }
    else if(jump < 0) {
        while(jump != 0) {
            currentInstruction--; // Move backward
            jump++;
        }
    }
    else if (jump == 0) {
        // No movement if jump is zero
    }
}

// Print the current state of the GritVM machine
void GritVM::printVM(bool printData, bool printInstruction) {
    std::cout << "****** Output Dump ******" << std::endl;
    std::cout << "Status: " << GVMHelper::statusToString(status) << std::endl;
    std::cout << "Accumulator: " << accumulator << std::endl;

    // Print data memory if requested
    if (printData) {
        std::cout << "*** Data Memory ***" << std::endl;
        int index = 0;
        std::vector<long>::iterator it = data_mem.begin();
        while(it != data_mem.end()) {
            long item = (*it);
            std::cout << "Location " << index++ << ": " << item << std::endl;
            it++;
        }
    }

    // Print instruction memory if requested
    if (printInstruction) {
        std::cout << "*** Instruction Memory ***" << std::endl;
        int index = 0;
        std::list<Instruction>::iterator it = instruction_mem.begin();
        while(it != instruction_mem.end()) {
            Instruction item = (*it);
            std::cout << "Instruction " << index++ << ": " 
                      << GVMHelper::instructionToString(item.operation) 
                      << " " << item.argument << std::endl;
            it++;
        }
    }
}
