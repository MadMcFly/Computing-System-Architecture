#include<iostream>
#include<string>
#include<string.h>
#include<vector>
#include<bitset>
#include<fstream>
#include<stdlib.h>
#include<cmath>
#include<sstream>
#include<mutex>
#include<thread>

std::mutex g_lock;

using namespace std;

#define SN 00 // Strong Not Taken
#define WN 01 // Weak Not Taken
#define WT 10 // Weak Taken
#define ST 11 // Strong Taken

string getIndex(bitset<32> addr,int index_bits)
{
	string saddr=addr.to_string();
	string index;
	index=saddr.substr(32-index_bits,32);
	return index;
}

int main(int argc, char*argv[])
{ 
	std::thread t1(threadFunc);
    std::thread t2(threadFunc);
    std::thread t3(threadFunc);

    t1.join();
    t2.join();
    t3.join();

	 
	 return 0;
}

void threadFunc() {
	ifstream config;
     string line;
     
	 int i=0;
	 int index_bits;
	 int BHR_bits;
     config.open(argv[1]);
	 //config.open("config.txt");
     if (config.is_open())
     {
         while (getline(config,line))
        {   
            if(i==0)
			{
				index_bits=atoi(line.c_str());
			}
			else if(i==1)
			{
				BHR_bits=atoi(line.c_str());
			}
			i++;
        }
                     
     }
     else cout<<"Unable to open file";
	 
     config.close();
	 //read from config.txt to get m and k no. of bits
	 
	 int PHT_rownum=pow(2,index_bits);
	 int PHT_colnum=(int)pow(2,BHR_bits);
	 //find no. of row and col of PHT(pattern history table)
	 
	
	 vector<unsigned int> BHR(BHR_bits,1);

	 //initialize BHR(branch history register) with 1:taken
	 
	 vector<unsigned long> PHT_counter(PHT_colnum*PHT_rownum,11);
	 //initialize PHT(pattern history table) with ST: strongly predict taken	
	 
	 ifstream trace;
	 ofstream traceout;
	 
	 string outname;
     outname = string(argv[2]) + this_thread::get_id() + ".out";
	 trace.open(argv[2]);
	 
	 traceout.open(outname.c_str(),std::ios_base::app);
	 // traceout.open("trace.txt.out",std::ios_base::app);
	 
	 string hexaddr;		//addr in hex(directly read from trace)
	 string branchstate;	//whether the branch is taken or not
	 int bs;
	 long longaddr;			//addr in int(to get addr in bitset)
	 bitset<32> addr;		//addr in bitset
	 string indexbits;
	 bitset<1>out;
	 int indexaddr;			//indexaddr in int(to locate in a vector PHT segment)
	 int realaddr;			//to locate in the whole vector PHT
	 int temp;
	 int predict;			//1:predict taken;0:predict not taken
	 int miss=0;
	 
	 if(trace.is_open()&&traceout.is_open())
	 {
		while(getline(trace,line))
		{
			lock_guard<std::mutex> locker(g_lock);
			//or use g_lock.lock() and unlock()

			hexaddr=line.substr(0,8);
			branchstate=line.substr(9,9);
			bs=atoi(branchstate.c_str());
			cout<<"bs="<<bs<<endl;
			//get branch addr in trace and branchstate(taken or not taken) 
			
			std::stringstream saddr;
			saddr<<hexaddr;
            saddr>>std::hex>>longaddr;
			addr=bitset<32>(longaddr);
			//get the 32 bits addr
			saddr.clear();
			
			indexbits=getIndex(addr,index_bits);
			//get the m bits index
			
			cout<<"string:"<<indexbits<<endl;
			
			bitset<20>indexbitset(indexbits);
			
			indexaddr=indexbitset.to_ulong();
			cout<<"int:"<<indexaddr<<endl;
			//get index addr in int(to locate in the vector: PHT_counter)
			
			temp=0;
			int j=BHR_bits-1;
			if(j>=0)
			{
				for(i=0;i<BHR_bits;i++)
				{
					temp=temp+BHR[i]*pow(2,j);
					j--;
				}
				cout<<temp<<endl;
				realaddr=temp*PHT_rownum+indexaddr;
			}
			else if(j<0)
			{
				temp=0;
				realaddr=temp*PHT_rownum+indexaddr;
			}
			//find the final addr in PHT_counter
			
			if(PHT_counter[realaddr]==11||PHT_counter[realaddr]==10)
			{
				predict=1;
			}
			else if(PHT_counter[realaddr]==00||PHT_counter[realaddr]==01)
			{
				predict=0;
			}
			
			traceout<<predict<<endl;
			
			
			if(bs==1)
			{
				if(PHT_counter[realaddr]==11||PHT_counter[realaddr]==10)
				{
					PHT_counter[realaddr]=11;
				}
				if(PHT_counter[realaddr]==01)
				{
					PHT_counter[realaddr]=11;
				}
				if(PHT_counter[realaddr]==00)
				{
					PHT_counter[realaddr]=01;
				}
				cout<<BHR_bits<<endl;
				if(BHR_bits!=0)
				{	
					for(j=BHR_bits-1;j>0;j--)
					{
						BHR[j]=BHR[j-1];
					}
					BHR[0]=1;
				}
				
				cout<<BHR_bits<<endl;
			}
			else if(bs==0)
			{
				if(PHT_counter[realaddr]==01||PHT_counter[realaddr]==00)
				{
					PHT_counter[realaddr]=00;
				}
				else if(PHT_counter[realaddr]==11)
				{
					PHT_counter[realaddr]=10;
				}
				else if(PHT_counter[realaddr]==10)
				{
					PHT_counter[realaddr]=00;
				}
				if(BHR_bits!=0)
				{
					for(j=BHR_bits-1;j>0;j--)
					{
						BHR[j]=BHR[j-1];
					}
					BHR[0]=0;
				}
			}
			if(predict!=bs)
			{
				miss++;
			}
		}
	 }
	 cout<<"m="<<index_bits<<endl;
	 cout<<"k="<<BHR_bits<<endl;
	 cout<<"row="<<PHT_rownum<<endl;	//4096
	 cout<<"col="<<PHT_colnum<<endl;	//4
	 cout<<"miss:"<<miss<<endl;
	 traceout.close();
}