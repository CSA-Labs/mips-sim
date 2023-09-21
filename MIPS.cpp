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
      if (WrtEnable == bitset<1>(1))
      {
        Registers[WrtReg.to_ulong()] = WrtData;
      }
      ReadData1 = Registers[RdReg1.to_ulong()];
      ReadData2 = Registers[RdReg2.to_ulong()]; 
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
      if (ALUOP == bitset<3>(ADDU) || ALUOP == bitset<3>(ADDIU))
      {
        ALUresult = oprand1.to_ulong() + oprand2.to_ulong();
      }
      else if (ALUOP == bitset<3>(SUBU))
      {
        ALUresult = oprand1.to_ulong() - oprand2.to_ulong();
      }
      else if (ALUOP == bitset<3>(AND))
      {
        ALUresult = oprand1 & oprand2;
      }
      else if (ALUOP == bitset<3>(OR))
      {
        ALUresult = oprand1 | oprand2;
      }
      else if (ALUOP == bitset<3>(NOR))
      {
        ALUresult = ~(oprand1 | oprand2);
      }
  
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
      int address = ReadAddress.to_ulong();
      if (address+3 < IMem.size())
      {
        Instruction = (IMem[address].to_ulong() << 24) | (IMem[address+1].to_ulong() << 16) | (IMem[address+2].to_ulong() << 8) | (IMem[address+3].to_ulong());
      }
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
      int address = Address.to_ulong();

      if (readmem == bitset<1>(1))
      {
          readdata = bitset<32>(0);

          if (address+3 < DMem.size())
          {
            readdata = (DMem[address].to_ulong() << 24) | (DMem[address + 1].to_ulong() << 16) | (DMem[address + 2].to_ulong() << 8) | DMem[address + 3].to_ulong();
          }
      }
      if (writemem == bitset<1>(1))
      {
        if (address+3 < DMem.size())
        {
          DMem[address + 3] = bitset<8>((WriteData.to_ulong() >> 0) & 0xFF);
          DMem[address + 2] = bitset<8>((WriteData.to_ulong() >> 8) & 0xFF);
          DMem[address + 1] = bitset<8>((WriteData.to_ulong() >> 16) & 0xFF);
          DMem[address] = bitset<8>((WriteData.to_ulong() >> 24) & 0xFF);
        }
      }
      cout<<"WriteData: " << WriteData << endl;
      cout <<"DMem: "<< DMem[address] << DMem[address + 1] << DMem[address + 2] << DMem[address + 3] << endl;

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
  bitset<32> PC(0);                                                            // Program Counter

  while (1)                                                                     // TODO: implement!
  {
    myInsMem.ReadMemory(PC);                         // Fetch Instruction from memory

    if (myInsMem.Instruction == bitset<32>(HALT))                                      // If current Instruction is HALT then terminate
    {
      break;
    }

    bitset<6> opcode = (myInsMem.Instruction >> 26).to_ulong();
    bitset<5> Rs = ((myInsMem.Instruction >> 21) & bitset<32>(0x1F)).to_ulong();
    bitset<5> Rt = ((myInsMem.Instruction >> 16) & bitset<32>(0x1F)).to_ulong();
    bitset<5> Rd = ((myInsMem.Instruction >> 11) & bitset<32>(0x1F)).to_ulong();
    bitset<5> shamt = ((myInsMem.Instruction >> 6) & bitset<32>(0x1F)).to_ulong();
    bitset<6> funct = (myInsMem.Instruction & bitset<32>(0x3F)).to_ulong();
    bitset<26> address = (myInsMem.Instruction & bitset<32>(0x3FFFFFF)).to_ulong();
    bitset<16> imm = (myInsMem.Instruction & bitset<32>(0xFFFF)).to_ulong();

    bitset<3> ALUOP;
    bool sign;
    bitset<32> signExtendedImm;

    if (opcode.to_ulong() == 0)                                             // R-Type Instruction
    {
      switch(funct.to_ulong())
      {
        case ADDU:

          ALUOP = ADDU;

          myRF.ReadWrite(Rs, Rt, bitset<5>(), bitset<32>(), bitset<1>(0));            // write to Rs

          myALU.ALUOperation(ALUOP, myRF.ReadData1, myRF.ReadData2);                   // Perform ALU Operation

          myRF.ReadWrite(bitset<5>(), bitset<5>(), Rd, myALU.ALUresult, bitset<1>(1)); // Write to Rd

          break;

        case SUBU:

          ALUOP = SUBU;

          myRF.ReadWrite(Rs, Rt, bitset<5>(), bitset<32>(), bitset<1>(0));

          myALU.ALUOperation(ALUOP, myRF.ReadData1, myRF.ReadData2);

          myRF.ReadWrite(bitset<5>(), bitset<5>(), Rd, myALU.ALUresult, bitset<1>(1));

          break;

        case AND:

          ALUOP = AND;

          myRF.ReadWrite(Rs, Rt, bitset<5>(), bitset<32>(), bitset<1>(0));

          myALU.ALUOperation(ALUOP, myRF.ReadData1, myRF.ReadData2);

          myRF.ReadWrite(bitset<5>(), bitset<5>(), Rd, myALU.ALUresult, bitset<1>(1));

          break;

        case OR:

          ALUOP = OR;

          myRF.ReadWrite(Rs, Rt, bitset<5>(), bitset<32>(), bitset<1>(0));

          myALU.ALUOperation(ALUOP, myRF.ReadData1, myRF.ReadData2);

          myRF.ReadWrite(bitset<5>(), bitset<5>(), Rd, myALU.ALUresult, bitset<1>(1));


          break;

        case NOR:

          ALUOP = NOR;

          myRF.ReadWrite(Rs, Rt, bitset<5>(), bitset<32>(), bitset<1>(0));

          myALU.ALUOperation(ALUOP, myRF.ReadData1, myRF.ReadData2);

          myRF.ReadWrite(bitset<5>(), bitset<5>(), Rd, myALU.ALUresult, bitset<1>(1));


          break;

        default:

          break;
      }
    }

    else if (opcode.to_ulong() == J)                  // J-Type Instruction
    {
      PC = (PC.to_ulong() & 0xF0000000) | (myInsMem.Instruction.to_ulong() & 0x03FFFFFF);
    }

    else                                            // I-Type Instruction
    {
      switch (opcode.to_ulong())
      {
        case ADDIU:
          // sign extend the 16 bit imm
          sign = imm[15];

          signExtendedImm = imm.to_ulong();

          if(sign)   // if negative sign extend with 1's, not useful since we are only using unsigned numbers
          {
            signExtendedImm = signExtendedImm | bitset<32>(0xFFFF0000);
          }

          myRF.ReadWrite(Rs, bitset<5>(), bitset<5>(), bitset<32>(), bitset<1>(0));

          myALU.ALUOperation(ADDIU, myRF.ReadData1, signExtendedImm);

          myRF.ReadWrite(bitset<5>(), bitset<5>(), Rt, myALU.ALUresult, bitset<1>(1)); // write to Rt

          break;

        case LW:
          // sign extend the 16 bit
          sign = imm[15];

          signExtendedImm = imm.to_ulong();

          if(sign)
          {
            signExtendedImm = signExtendedImm | bitset<32>(0xFFFF0000);
          }

          myRF.ReadWrite(Rs, bitset<5>(), bitset<5>(), bitset<32>(), bitset<1>(0)); // write to Rs

          myALU.ALUOperation(ADDU, myRF.ReadData1, signExtendedImm); // add base + offset

          myDataMem.MemoryAccess(myALU.ALUresult, bitset<32>(), bitset<1>(1), bitset<1>(0)); // read from memory

          myRF.ReadWrite(bitset<5>(), bitset<5>(), Rt, myDataMem.readdata, bitset<1>(1)); // write to Rt

          break;

        case SW:
        // sign extend the 16 bit imm then add in alu (base address from register + imm (offset)) then store to memory
          sign = imm[15];

          signExtendedImm = imm.to_ulong();

          if(sign)
          {
            signExtendedImm = signExtendedImm | bitset<32>(0xFFFF0000);
          }

          myRF.ReadWrite(Rs, Rt, bitset<5>(), bitset<32>(), bitset<1>(0)); // read from Rs, Rt
          myALU.ALUOperation(ADDU, myRF.ReadData1, signExtendedImm); // add base + offset
          myDataMem.MemoryAccess(myALU.ALUresult, myRF.ReadData2, bitset<1>(0), bitset<1>(1)); // write to memory

          break;

        case BEQ:

          if(myRF.ReadData1 == myRF.ReadData2)
          {
            PC = PC.to_ulong() + (imm.to_ulong());
          }

          break;
      }
    }
    PC = PC.to_ulong() + 4;
    myRF.OutputRF(); // dump RF; 
  }
  myDataMem.OutputDataMem(); // dump data mem

  return 0;
}