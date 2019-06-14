#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space reason, we keep it as this large number, but the memory is still 32-bit addressable.


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
            // implement the funciton by you.   
			ReadData1 = Registers[RdReg1.to_ulong()];
			ReadData2 = Registers[RdReg2.to_ulong()];
			if(WrtEnable.to_ulong()==1)
			{
				Registers[WrtReg.to_ulong()] = WrtData;
			}
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
                 // implement the ALU operations 
				 if(ALUOP.to_ulong() ==ADDU)
				{
					ALUresult = oprand1.to_ulong() + oprand2.to_ulong();
				}
				else if(ALUOP.to_ulong() == SUBU)
				{
					ALUresult = oprand1.to_ulong() + oprand2.to_ulong();
				}
				else if(ALUOP.to_ulong() == AND)
				{
					ALUresult = oprand1 & oprand2;
				}
				else if(ALUOP.to_ulong() == OR)
				{
					ALUresult = oprand1 | oprand2;
				}
				else if(ALUOP.to_ulong() == NOR)
				{
					ALUresult = ~(oprand1|oprand2);
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
               // Read the byte at the ReadAddress and the following three byte
			   string tempInst;
			   for(int j = 0; j < 4; j++)
			   {
					bitset<8> temp = IMem[(ReadAddress.to_ulong()) + j];
					tempInst = tempInst + temp.to_string();
			   }
			   Instruction = bitset<32>(tempInst);
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
               // memory access, read from Mem
			   if(readmem.to_ulong() ==1)
			   {
					string tempReadData;
					for (int j=0; j<4; j++)
					{
						bitset<8> tempData= DMem[Address.to_ulong() + j];
						tempReadData= tempReadData+ tempData.to_string();
					}
					readdata= bitset<32>(tempReadData);
			   }
			   else if(writemem.to_ulong() == 1)
			   {
					for(int i=0; i<4; i++)
					{
						for (int j=7 ;j>=0 ;j--)
						{
							DMem[Address.to_ulong()+i][j] = WriteData[31-(8*i+7-j)];
						}
					}
			   }
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

void dumpResults(bitset<32> pc, bitset<5> WrRFAdd, bitset<32> WrRFData, bitset<1> RFWrEn, bitset<32> WrMemAdd, bitset<32> WrMemData, bitset<1> WrMemEn)
{
                  ofstream fileout;
                  
                  fileout.open("Results.txt",std::ios_base::app);
                  if (fileout.is_open())
                  {
                  
                  fileout <<pc<<' '<<WrRFAdd<<' '<<WrRFData<<' '<<RFWrEn<<' '<<WrMemAdd<<' '<<WrMemData<<' '<<WrMemEn << endl;
                     
                  }
                  else cout<<"Unable to open file";
                  fileout.close();

}

bitset<6> getSixBits(bitset<32> Ins, int start, int end)
{
	int j=5;
	bitset<6> temp;
	for(int i=start; i>=end; i--)
	{
		temp[j]=Ins[i];
		j--;
	}
	return temp;
}

bitset<5> getFiveBits(bitset<32> Ins, int start, int end)
{
	int j=4;
	bitset<5> temp;
	for(int i=start; i>=end; i--)
	{
		temp[j]= Ins[i];
		j--;
	}
	return temp;
}

bitset<32> signExtend(bitset<16> Imm)
{
	int j=0;
	bitset<32> extend;
	for(j=0; j<16; j++)
	{
		extend[j]=Imm[j];
		extend[j+16]=Imm[15];
	}
	return extend;
}
   
int main()
{   
    bitset<32> pc=-4;
    RF myRF;
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;
	
	bitset<32> currentInstruction=0;
	bitset<32> lastInstruction=0;
	bitset<32> llastInstruction=0;
	bitset<32> halt (std::string("11111111111111111111111111111111"));
	bitset<32> beqAddr,jAddr=0;
	bitset<32> result=0;
	bitset<32> offset=0;
	bitset<32> readData=0;
	bitset<16> imm=0;
	bitset<6> llastopcode,lastopcode,opcode,funct=0;
	bitset<5> rs,rt,rd=0;
	bitset<3> ALUop=0;
	
	//for dump use
	bitset<5> RFWrtAddr=0;
	bitset<32> RFWrtData=0;
	bitset<32> DmemWrtAddr=0;
	bitset<32> DmemWrtData=0;
	bitset<1> RFWrtEnable=0;
	bitset<1> DmemWrtEnable=0;
	bool isEqual=false;//for beq use
	
    while (1) //each loop body corresponds to one clock cycle.
	{
        //Stage3
		
		//load/store to data memory
		
		if(opcode==0x23)		//load
		{
			readData=myDataMem.MemoryAccess(result, 0, 1, 0);
			cout<<"data from mem:"<<readData<<endl;
			DmemWrtAddr=0;
			DmemWrtData=0;
			DmemWrtEnable=0;
		}
		else if(opcode==0x2B)		//store
		{
			myDataMem.MemoryAccess(result, myRF.ReadData2, 0, 1);
			DmemWrtAddr=result;
			DmemWrtData=myRF.ReadData2;
			DmemWrtEnable=1;
		}
		
		//write back to RF
	
		if(opcode!=0x02&&opcode!=0x04&&opcode!=0x2B)		//not jump,beq,store
		{
			RFWrtEnable=1;
		}
		else 
		{
			RFWrtEnable=0;
		}
		if(pc.to_ulong()==4294967292||pc.to_ulong()==0)
		{
			RFWrtEnable=0;
		}
		if(opcode==0x00)		//R-type
		{
			myRF.ReadWrite(rs,rt,rd,result,1);
			RFWrtAddr=rd;
			
			RFWrtData=result;
		}
		else if(opcode==0x09)		//addiu
		{
			myRF.ReadWrite(rs,rt,rt,result,1);
			RFWrtAddr=rt;
			
			RFWrtData=result;
		}
		else if(opcode==0x23)		//load
		{
			myRF.ReadWrite(rs,rt,rt,readData,1);
			RFWrtAddr=rt;
			
			RFWrtData=readData;
		}	
			
		if(lastopcode==0x02)
		{
			RFWrtAddr=0;
			RFWrtData=0;
			RFWrtEnable=0;
			DmemWrtAddr=0;
			DmemWrtData=0;
			DmemWrtEnable=0;
		}
		
		
		//Stage2
	
		//instruction decode/register file read
		opcode=getSixBits(currentInstruction,31,26);
		lastopcode=getSixBits(lastInstruction,31,26);
		llastopcode=getSixBits(llastInstruction,31,26);
		if(opcode==0x00)		//R-type
		{
			rs= getFiveBits(currentInstruction,25,21);
			rt= getFiveBits(currentInstruction,20,16);
			rd= getFiveBits(currentInstruction,15,11);
			funct= getSixBits(currentInstruction,5,0);
			cout<<"R-type Reg:"<<rs<<endl;
			cout<<"R-type Reg:"<<rt<<endl;
			cout<<"R-type Reg:"<<rd<<endl;
			for(int i=0; i<3; i++)
			{
				ALUop[i]= funct[i];
			}
			myRF.ReadWrite(rs,rt,rd,0,0);	
		}
		else if(opcode==0x02)		//J-type
		{
			bitset<32> tempPC= pc.to_ulong()+4;
			for(int i=0; i<2; i++)
			{
				jAddr[i]=0;
			}
			for(int i=2; i<28; i++)
			{
				jAddr[i]=currentInstruction[i-2];
			}
			for(int i=28; i<32; i++)
			{
				jAddr[i]=tempPC[i];
			}
		}
		else		//I-type
		{
			rs=getFiveBits(currentInstruction, 25,21);
			rt=getFiveBits(currentInstruction, 20,16);
			for(int i=0; i<16; i++)
			{
				imm[i]=currentInstruction[i];
			}
			offset=signExtend(imm);
			cout<<"I-type Reg:"<<rs<<endl;
			cout<<"I-type Reg:"<<rt<<endl;
			cout<<"I-type SignExtendImm:"<<offset<<endl;
			myRF.ReadWrite(rs,rt,rt,0,0);
			cout<<"Data1:"<<myRF.ReadData1<<endl;
			cout<<"Data2:"<<myRF.ReadData2<<endl;
		}
		
		//execute
		if(lastopcode!=0x02)		//jump flush pc+4 with nop
		{
			if(opcode==0x00)		//R-type
			{
				cout<<"ALUop:"<<ALUop<<endl;
				result=myALU.ALUOperation(ALUop, myRF.ReadData1, myRF.ReadData2);
				cout<<"result:"<<result<<endl;
			}	
			else if(opcode==0x09||opcode==0x23||opcode==0x2B)		//I-type(addiu, lw, sw)
			{
				ALUop=ADDU;
				cout<<"ALUop:"<<ALUop<<endl;
				result=myALU.ALUOperation(ALUop, myRF.ReadData1, offset);
				cout<<"result:"<<result<<endl;
			}
			else if(opcode==0x04)		//I-type(beq)
			{
				if(myRF.ReadData1==myRF.ReadData2)
				{
					isEqual=true;
				}
				offset=offset<<2;		//branch addr
				beqAddr=pc.to_ulong()+4+offset.to_ulong();
			}
		}
		
		if(lastopcode!=0x02&&lastopcode!=0x04)		//not jump,beq
		{
			pc=pc.to_ulong()+4;
		}
		else if(lastopcode==0x02)		//jump
		{
			pc=jAddr;
		}
		else if(lastopcode==0x04&&isEqual)				//beq and branch
		{
			pc=beqAddr;
			isEqual=false;
		}
	
		//Stage1
		
		//instruction fetch
		if(pc.to_ulong()!=0)
		{
			llastInstruction=lastInstruction;
			lastInstruction=currentInstruction;
		}
		currentInstruction= myInsMem.ReadMemory(pc.to_ulong());
		cout<<"lastIns:"<<lastInstruction<<endl;
		cout<<"currIns:"<<currentInstruction<<endl;
		if(currentInstruction==halt)
		{
			break;
		}
		
    // At the end of each cycle, fill in the corresponding data into "dumpResults" function to output files.
    // The first dumped pc value should be 0.	
		dumpResults(pc, RFWrtAddr, RFWrtData, RFWrtEnable, DmemWrtAddr, DmemWrtData,DmemWrtEnable );  
    }
	      myRF.OutputRF(); // dump RF; 
          myDataMem.OutputDataMem(); // dump data mem
      
          return 0;
        
}
