#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

#define ADDU (0x21)
#define SUBU (0x23)
#define ADDIU (0x09)
#define AND (0x24)
#define OR  (0x25)
#define NOR (0x27)
#define BEQ (0x04)
#define J  (0x02)
#define SW (0x2B)
#define LW (0x23)
#define HALT (0xFFFFFFFF)


// Memory size.
// In reality, the memory size should be 2^32, but for this lab and space reasons,
// we keep it as this large number, but the memory is still 32-bit addressable.
#define MemSize (65536)


class RF
{
  public:
    bitset<32> ReadData1, ReadData2; 
    RF()
    { 
      Registers.resize(32);  
      Registers[0] = bitset<32> (0);  
    }

    void ReadWrite(bitset<5> RdReg1, bitset<5> RdReg2, bitset<5> WrtReg, bitset<32> WrtData, bitset<1> WrtEnable)
    {   
      /**
       * @brief Reads or writes data from/to the Register.
       *
       * This function is used to read or write data from/to the register, depending on the value of WrtEnable.
       * Put the read results to the ReadData1 and ReadData2.
       */
      // TODO: implement!               
    }

    void OutputRF()
    {
      ofstream rfout;
      rfout.open("RFresult.txt",std::ios_base::app);
      if (rfout.is_open())
      {
        rfout<<"A state of RF:"<<endl;
        for (int j = 0; j<32; j++)
        {        
          rfout << Registers[j]<<endl;
        }

      }
      else cout<<"Unable to open file";
      rfout.close();

    }     
  private:
    vector<bitset<32> >Registers;
};


class ALU
{
  public:
    bitset<32> ALUresult;
    bitset<32> ALUOperation (bitset<3> ALUOP, bitset<32> oprand1, bitset<32> oprand2)
    {   
      /**
       * @brief Implement the ALU operation here.
       *
       * ALU operation depends on the ALUOP, which are definded as ADDU, SUBU, etc. 
       */
      // TODO: implement!
      return ALUresult;
    }            
};


class INSMem
{
  public:
    bitset<32> Instruction;
    INSMem()
    {       IMem.resize(MemSize); 
      ifstream imem;
      string line;
      int i=0;
      imem.open("imem.txt");
      if (imem.is_open())
      {
        while (getline(imem,line))
        {      
          IMem[i] = bitset<8>(line);
          i++;
        }

      }
      else cout<<"Unable to open file";
      imem.close();

    }

    bitset<32> ReadMemory (bitset<32> ReadAddress) 
    {    
      // TODO: implement!
      /**
       * @brief Read Instruction Memory (IMem).
       *
       * Read the byte at the ReadAddress and the following three byte,
       * and return the read result. 
       */
      return Instruction;     
    }     

  private:
    vector<bitset<8> > IMem;

};

class DataMem    
{
  public:
    bitset<32> readdata;  
    DataMem()
    {
      DMem.resize(MemSize); 
      ifstream dmem;
      string line;
      int i=0;
      dmem.open("dmem.txt");
      if (dmem.is_open())
      {
        while (getline(dmem,line))
        {      
          DMem[i] = bitset<8>(line);
          i++;
        }
      }
      else cout<<"Unable to open file";
      dmem.close();

    }  
    bitset<32> MemoryAccess (bitset<32> Address, bitset<32> WriteData, bitset<1> readmem, bitset<1> writemem) 
    {    
      /**
       * @brief Reads/writes data from/to the Data Memory.
       *
       * This function is used to read/write data from/to the DataMem, depending on the readmem and writemem.
       * First, if writemem enabled, WriteData should be written to DMem, clear or ignore the return value readdata,
       * and note that 32-bit WriteData will occupy 4 continious Bytes in DMem. 
       * If readmem enabled, return the DMem read result as readdata.
       */
      // TODO: implement!
      return readdata;     
    }   

    void OutputDataMem()
    {
      ofstream dmemout;
      dmemout.open("dmemresult.txt");
      if (dmemout.is_open())
      {
        for (int j = 0; j< 1000; j++)
        {     
          dmemout << DMem[j]<<endl;
        }

      }
      else cout<<"Unable to open file";
      dmemout.close();

    }             

  private:
    vector<bitset<8> > DMem;

};  



int main()
{
  RF myRF;
  ALU myALU;
  INSMem myInsMem;
  DataMem myDataMem;
  bitset<32> PC(0); // Program Counter

  while (1)  // TODO: implement!
  {
    bitset<32> instruction = myInsMem.ReadMemory(PC); // Fetch instruction from memory

    if (instruction == bitset<32>(HALT)) // If current instruction is HALT then terminate
    {
      break;
    }

    bitset<6> opcode = (instruction >> 26).to_ulong();
    bitset<5> Rs = ((instruction >> 21) & bitset<32>(0x1F)).to_ulong();
    bitset<5> Rt = ((instruction >> 16) & bitset<32>(0x1F)).to_ulong();
    bitset<5> Rd = ((instruction >> 11) & bitset<32>(0x1F)).to_ulong();
    bitset<16> imm = (instruction & bitset<32>(0xFFFF)).to_ulong();

    switch (opcode.to_ulong())
    {
      case ADDU:
        myALU.ALUOperation(bitset<6>(ADDU), myRF.ReadData1, myRF.ReadData2);
        break;
      case SUBU:
        myALU.ALUOperation(bitset<6>(ADDU), myRF.ReadData1, myRF.ReadData2);
        break;
      case ADDIU:
        myALU.ALUOperation(bitset<6>(ADDU), myRF.ReadData1, imm);
        break;
      case AND:
        myALU.ALUOperation(bitset<6>(AND), myRF.ReadData1, myRF.ReadData2);
        break;
      case OR:  
        myALU.ALUOperation( bitset<6>(OR), myRF.ReadData1, myRF.ReadData2);
        break;
      case NOR:
        myALU.ALUOperation(bitset<6>(NOR), myRF.ReadData1, myRF.ReadData2);
      case BEQ:
        if(myRF.ReadData1 == myRF.ReadData2)
        {
          PC = PC.to_ulong() + (imm.to_ulong());
        }
        break; 
      case J:
        PC = (PC.to_ulong() & 0xF0000000) | (instruction & 0x03FFFFFF);
          break;
      case LW:
        myDataMem.MemoryAccess(myRF.ReadData1 + imm, bitset<32>(), bitset<1>(1), bitset<1>(0));
          break;
      case SW:
        myDataMem.MemoryAccess(myRF.ReadData1 + imm, myRF.ReadData2, bitset<1>(0), bitset<1>(1));
          break;
      default:
          break;
        }
      
      if (opcode.to_ulong() != BEQ && opcode.to_ulong() != SW)
      {
        myRF.ReadWrite(Rs, Rt, Rd, myALU.ALUresult, bitset<1>(1));
      }

    PC = PC.to_ulong() + 4;
  }

  myRF.OutputRF(); // dump RF; 
  myDataMem.OutputDataMem(); // dump data mem

  return 0;
}