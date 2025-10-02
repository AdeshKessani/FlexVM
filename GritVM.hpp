#ifndef GRITVM_HPP
#define GRITVM_HPP

#include "GritVMBase.hpp"

#include <iostream>

class GritVM : public GritVMInterface {
public:
    GritVM() : accumulator{0}, status{WAITING} {} //Constructor
    ~GritVM() = default; //Default destructor

  //public methods
  virtual STATUS load(const std::string filename, const std::vector<long> &initialMemory);
  virtual STATUS run();
  virtual std::vector<long> getDataMem() { return data_mem; }
  virtual STATUS reset();
  void printVM(bool printData, bool printInstruction);
private:
    //private methods
    long evaluate(Instruction inst); //executes instruction
    void advance(long jump); //advances currentInstructrion

    //private data members
    std::vector<long> data_mem; //Data Memory
    std::list<Instruction> instruction_mem; //Instruction Memory
    std::list<Instruction>::iterator currentInstruction; //Iterator in the Instruction Memory
    long accumulator; // holds the total of the operations
    STATUS status; //current status of GritVM Machine
};

#endif /* GRITVM_HPP */