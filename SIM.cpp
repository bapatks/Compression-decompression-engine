/*
Kaustubh Bapat
CDA 5636 Project 2
UFID: xxxx-xxxx

*/
#include<iostream>
#include<fstream>
#include<cstring>
#include<string>
#include<vector>
#include<bitset>

using namespace std;

ifstream org_file, dec_file;

class compress
{
private:
	struct frequency_table
	{
		string instr;
		int num;
	};

	int arr[4];
	string dictionary[8];
	string compressed_binary;

	void oparser(void);
	void print_cout(void);
	string create_string(int format_specifier, int mismatch_loc, int dict_index);

	//Used for format 4
	string create_string(int format_specifier, int mismatch_loc1, int mismatch_loc2, int dict_index);

	string frmt0(string);
	string frmt1(string);
	string frmt2(string);
	string frmt34(string);
	string frmt5(string);
	string frmt6(string);

public:
	void compressor(void);
};

class decompress
{
private:
	string dictionary[8];
	string compressed_binary;
	//string decompressed_binary;
	void cparser(void);
public:
	void decompressor(void);
};



/* Member function definitions for class compress*/
void compress::oparser()
{
	string str;
	vector<frequency_table> list;
	while(org_file>>str)
	{
		bool flag = false;
		//cout<<str<<endl;
		int size = list.size();
		for(int i=0; i<size; i++)
		{
			if(str.compare(list[i].instr)==0)
			{
				list[i].num++;
				//cout<<"Entered "<<str<<" count = "<<list[i].num<<endl;
				flag = true;
			}
		}
		if(!flag)
		{
			//cout<<"NOT entered "<<str<<endl;
			frequency_table frq;
			frq.instr = str;
			frq.num = 1;
			list.push_back(frq);
		}
	}

	/*
	Since the file has been read completely once, EOF needs to be reset and 
	offset should be reset to 0
	*/
	org_file.clear();
	org_file.seekg(0, ios::beg);

	int size = list.size();
	/*
	for(int j=0; j<size; j++)
	{
		cout<<list[j].instr<<"\t"<<list[j].num<<endl;
	}
	*/
	string push_to_dict;
	for(int i=0; i<8; i++)
	{
		int new_size=list.size();
		int largest_index = 0;
		for(int j=0; j<new_size; j++)
		{
			//cout<<list[j].instr<<"\t"<<list[j].num<<endl;
			if(list[largest_index].num < list[j].num)
			{
				largest_index = j;
			}
		}
		push_to_dict = list[largest_index].instr;
		list.erase(list.begin()+largest_index); //remove the element which has largest frequency
		dictionary[i] = push_to_dict;
	}
	/*
	cout<<"********************"<<endl;
	for(int k=0; k<8; k++)
	{
		cout<<dictionary[k]<<endl;
	}
	cout<<"********************"<<endl;
	*/
}

string compress::create_string(int fs, int ml, int di)
{
	string temp;
	string int2bit;
	//cout<<"I came here";
	if(fs == 2 || fs == 3)
	{
		if(fs==2)
		{
			temp.append("010");
		}
		else if(fs==3)
		{
			temp.append("011");
		}
		
		int2bit = bitset<5>(ml).to_string();
		temp.append(int2bit);
		//temp.append(dict_index_str[di]);
		temp.append(bitset<3>(di).to_string());
	}

	return temp;

}

string compress::create_string(int fs, int ml1, int ml2, int di)
{
	string temp;
	string int2bit;
	//cout<<"I came here";
	if(fs == 4)
	{
		temp.append("100");
		int2bit = bitset<5>(ml1).to_string();
		temp.append(int2bit);
		int2bit = bitset<5>(ml2).to_string();
		temp.append(int2bit);
		//temp.append(dict_index_str[di]);
		temp.append(bitset<3>(di).to_string());
	}

	return temp;
}

void compress::print_cout(void)
{
	int size = compressed_binary.size();
	int lines = size/32;
	int rem = size%32;
	string temp;
	ofstream outfile;
	outfile.open("cout.txt");
	if(!outfile)
	{
		cout<<"Could not generate output file"<<endl;
		return;
	}

	if(rem>0)
	{
		lines++;
	}
	for(int k=0; k<lines; k++)
	{
		temp = "";
		temp = compressed_binary.substr(k*32, 32);
		outfile<<temp;
		if(k==lines-1)
		{
			if(rem>0)
			{
				for(int j=0; j<32-rem; j++)
				{
					outfile<<"1";
				}
			}
		}
		outfile<<endl;
	}
	outfile<<"xxxx"<<endl;
	for(int i=0; i<8; i++)
	{
		outfile<<dictionary[i]<<endl;
	}
	outfile.close();
}

string compress::frmt1(string instr)
{
	string bit_mask, temp;
	
	int count;
	//char prev = '0';
	//bool flag = false;
	for(int j=0; j<8; j++)
	{
		count = 0;
		temp = dictionary[j];
		for(int i=0; i<32; i++)
		{
			temp.at(i) = ((instr.at(i) - '0') ^ (temp.at(i) - '0')) + '0';
			if(temp.at(i) == '1')
			{
				count++;
			}
			if(count>4)
			{
				break;
			}
		}
		if(count>4)
		{
			continue;
		}
		else if(count <= 4)
		{
			int f_pos = temp.find_first_of("1");
			int l_pos = temp.find_last_of("1"); 
			if(f_pos != std::string::npos && l_pos != std::string::npos)
			{
				if(l_pos-f_pos < 4)
				{
					bit_mask = temp.substr(f_pos, 4);
					//cout<<"NON-EMPTY format 1"<<endl;
					return "001"+(bitset<5>(f_pos).to_string())+bit_mask+(bitset<3>(j).to_string());
				}
			}
			
		}
	}
	
	return "";
}

string compress::frmt2(string instr)
{
	int mismatch_loc;
	bool flag = false;
	string cmprs = "";
	for(int i=0; i<8; i++)
	{
		//cout<<"Checking for dictionary entry"<<i<<endl;
		int changes = 0;
		for(int j=0; j<32; j++)
		{
			if(changes>1)
			{
				//cout<<"I broke"<<endl;
				break;
			}

			if(instr.at(j) != dictionary[i].at(j))
			{
				//cout<<"instr.at(j) = "<<instr.at(j)<<"\t";
				//cout<<"dictionary[i].at(j) = "<<dictionary[i].at(j)<<"\t";
				changes++;
				mismatch_loc = j;
				//cout<<"dict_index = "<<i<<"\tml ="<<mismatch_loc<<"\tchanges ="<<changes<<endl;
			}

			
			if(j==31)
			{
				flag = true;
			}
		}
		if(flag && changes==1)
		{
			//cout<<mismatch_loc<<endl;
			cmprs = create_string(2, mismatch_loc, i);
			//cout<<"NON-EMPTY format 2"<<endl;
			return cmprs;
		}
		//cout<<"discarding dictionary entry "<<i<<endl;
		//cout<<endl;
	}
	return cmprs;
}


string compress::frmt34(string instr)
{
	int mismatch_loc1, mismatch_loc2;
	bool flag = false;
	string cmprs = "";
	for(int i=0; i<8; i++)
	{
		//cout<<"Checking for dictionary entry"<<i<<endl;
		int changes = 0;
		for(int j=0; j<32; j++)
		{
			if(changes>2)
			{
				//cout<<"I broke"<<endl;
				break;
			}

			if(instr.at(j) != dictionary[i].at(j))
			{
				//cout<<"instr.at(j) = "<<instr.at(j)<<"\t";
				//cout<<"dictionary[i].at(j) = "<<dictionary[i].at(j)<<"\t";
				changes++;
				if(changes==1)
				{
					mismatch_loc1 = j;
				}
				else if(changes==2)
				{
					mismatch_loc2 = j;
				}
				//cout<<"dict_index = "<<i<<"\tml ="<<mismatch_loc<<"\tchanges ="<<changes<<endl;
			}

			
			if(j==31)
			{
				flag = true;
			}
		}
		if(flag && changes==2)
		{
			//cout<<mismatch_loc<<endl;
			if(mismatch_loc2 == mismatch_loc1 + 1)
			{
				//cout<<"This is format 3"<<endl;
				cmprs = create_string(3, mismatch_loc1, i);
				//cout<<"NON-EMPTY format 3"<<endl;
				return cmprs;
			}
			else
			{
				//cout<<"This is format 4"<<endl;
				cmprs = create_string(4, mismatch_loc1, mismatch_loc2, i);
				//cout<<"NON-EMPTY format 4"<<endl;
				return cmprs;
			}
			
		}
		//cout<<"discarding dictionary entry "<<i<<endl;
		//cout<<endl;
	}
	return cmprs;
}

string compress::frmt5(string instr)
{
	string temp = "";
	for(int i=0; i<8; i++)
	{
		if(instr.compare(dictionary[i]) == 0)
		{
			temp.append("101");
			//temp.append(dict_index_str[i]);
			temp.append(bitset<3>(i).to_string());
			//cout<<"NON-EMPTY format 5"<<endl;
			return temp;
		}
	}

	return temp;
}


void compress::compressor()
{
	string str, check0, check1, check2, check34, check5, check6;
	string prev_str="";
	oparser();
	
	//int line_num=0;
	int count = -1;
	int shortest, index;
	//str = "11100001111111111111111111110111";


	while(org_file>>str)
	{
		//line_num++;
		index = -1;
		//cout<<"string is"<<str<<"\t prev_str is"<<prev_str<<endl;
		if(str.compare(prev_str) == 0)
		{
			count++;
			//cout<<line_num<<":"<<str<<endl;
		}
		else if(str.compare(prev_str) != 0)
		{
			check1 = frmt1(str);
			check2 = frmt2(str);
			check34 = frmt34(str);
			check5 = frmt5(str);

			arr[0] = check1.size();
			arr[1] = check2.size();
			arr[2] = check34.size();
			arr[3] = check5.size();

			shortest = 0;
			//cout<<"["<<arr[0]<<","<<arr[1]<<","<<arr[2]<<","<<arr[3]<<"]"<<endl;
			for(int i=0; i<4; i++)
			{
				if(shortest==0 && arr[i]!=0)
				{
					shortest = arr[i];
					index = i;
				}
				if(arr[i] != 0 && arr[i] < shortest)
				{
					shortest = arr[i];
					index = i;
				}
			}
			
			//cout<<"shortest is "<<shortest<<endl;
			//cout<<"index is "<<index<<endl;

			if(count>=0)
			{
				check0 = "000" + bitset<2>(count).to_string();
				//cout<<": PS:"<<prev_str<<"\tformat 0 compressed to "<<check0<<endl;
				compressed_binary.append(check0);
				count = -1;
			}
			

			if(index != -1)
			{
				if(index == 0)
				{
					//cout<<line_num<<":"<<str<<"\tformat 1 compressed to "<<check1<<endl;
					compressed_binary.append(check1);
					//prev_str = str;
					//continue;
				}
				else if(index == 1)
				{
					//cout<<line_num<<":"<<str<<"\tformat 2 compressed to "<<check2<<endl;
					compressed_binary.append(check2);
					//prev_str = str;
					//continue;
				}
				else if(index == 2)
				{
					//cout<<line_num<<":"<<str<<"\tformat 34 compressed to "<<check34<<endl;
					compressed_binary.append(check34);
					//prev_str = str;
					//continue;
				}
				else if(index == 3)
				{
					//cout<<line_num<<":"<<str<<"\tformat 5 compressed to "<<check5<<endl;
					compressed_binary.append(check5);
					//prev_str = str;
					//continue;
				}
				
			}
	
			else if(index == -1)
			{
				check6 = "110"+str;
				//cout<<line_num<<":"<<str<<"\tformat 6 compressed to "<<check6<<endl;
				compressed_binary.append(check6);
			}
		}

		prev_str = str;
	
	}

	print_cout();
	org_file.close();

}

/* END */


/* Member function definitions for class decompress*/

void decompress::cparser()
{
	string str;
	bool flag=false;
	int dict_index=0;
	//Extract dictionary entries
	while(dec_file>>str)
	{
		if (str.compare("xxxx")==0)
		{
			flag = true;
		}
		if(!flag)
		{
			//cout<<str<<endl;
			compressed_binary.append(str);
		}
		if(flag)
		{
			if(str.compare("xxxx") != 0)
			{
				if(dict_index<8)
				{
					dictionary[dict_index] = str;
					dict_index++;
				}
			}
		}
	}
	/*
	for(int j=0; j<8; j++)
	{
		cout<<dictionary[j]<<endl;
	}*/

	//int size = decompressed_binary.size();
	//cout<<"size = "<<size<<endl;
	/*
	Since the file has been read completely, close the file
	*/
	dec_file.close();
}


void decompress::decompressor()
{
	int pos = 0;
	int index;
	string read_str;
	string prev_dcmprs;
	ofstream outfile;
	outfile.open("dout.txt");
	if(!outfile)
	{
		cout<<"Could not generate output file"<<endl;
		return;
	}


	cparser();

	//cout<<"read the file"<<endl;
	
	int size = compressed_binary.size();
	while(pos<size)
	{
		read_str = compressed_binary.substr(pos,3);
		//cout<<read_str<<endl;
		if(read_str.compare("000")==0)
		{
			read_str = compressed_binary.substr(pos+3, 2);
			index = stoi(read_str, nullptr, 2);
			//cout<<"Num in format 0 is "<<index<<endl;
			for(int j=0; j<index+1; j++)
			{
				//decompressed_binary.append(prev_dcmprs);
				outfile<<prev_dcmprs<<endl;
			}
			pos = pos + 3 + 2;
		}

		else if(read_str.compare("001")==0)
		{
			read_str = compressed_binary.substr(pos+3, 5);
			int loc = stoi(read_str, nullptr, 2);
			string bitmask = compressed_binary.substr(pos+3+5, 4);

			read_str = compressed_binary.substr(pos+3+5+4, 3);
			index = stoi(read_str, nullptr, 2);

			string pattern = dictionary[index].substr(loc, 4);

			//cout<<"pattern b4 is "<<pattern<<endl;
			//cout<<"bitmask is "<<bitmask<<endl;
			for(int m=0; m<4; m++)
			{
				//subract by '0' to subtract by ascii value of 0 and get actual bit
				//add '0' to add ascii value of '0' to get back proper XOR'd character 
				pattern.at(m) = ((pattern.at(m) - '0') ^ (bitmask.at(m) - '0')) + '0';
			}
			//cout<<"pattern later is "<<pattern<<endl;
			prev_dcmprs = dictionary[index];
			//cout<<"initially "<<prev_dcmprs<<endl;
			prev_dcmprs.replace(loc, 4, pattern);
			//cout<<"changed to "<<prev_dcmprs<<endl;

			//decompressed_binary.append(prev_dcmprs);
			outfile<<prev_dcmprs<<endl;
			pos = pos + 3 + 5 + 4 + 3;
		}

		else if(read_str.compare("010")==0)
		{
			read_str = compressed_binary.substr(pos+3, 5);
			int mismatch_loc = stoi(read_str, nullptr, 2);
			read_str = compressed_binary.substr(pos+3+5, 3);
			index = stoi(read_str, nullptr, 2);
			char mismatch = dictionary[index].at(mismatch_loc);
			/*
			Copy dictionary entry into prev_dcmprs and then 
			replace character at position 'mismatch_loc' to opposite of mismatch
			*/
			prev_dcmprs = dictionary[index];
			if(mismatch == '1')
			{
				//cout<<"prev_dcmprs = "<<prev_dcmprs<<endl;
				prev_dcmprs.at(mismatch_loc) = '0';
				//cout<<"prev_dcmprs = "<<prev_dcmprs<<endl;
			}
			else if(mismatch == '0')
			{
				//cout<<"prev_dcmprs = "<<prev_dcmprs<<endl;
				prev_dcmprs.at(mismatch_loc) = '1';
				//cout<<"prev_dcmprs = "<<prev_dcmprs<<endl;
			}
			//decompressed_binary.append(prev_dcmprs);
			outfile<<prev_dcmprs<<endl;
			pos = pos + 3 + 5 + 3;
		}

		else if(read_str.compare("011")==0)
		{
			read_str = compressed_binary.substr(pos+3, 5);
			int mismatch_loc = stoi(read_str, nullptr, 2);
			read_str = compressed_binary.substr(pos+3+5, 3);
			index = stoi(read_str, nullptr, 2);
			char mismatch;
			/*
			Copy dictionary entry into prev_dcmprs and then 
			replace character at position 'mismatch_loc' to opposite of mismatch
			*/
			prev_dcmprs = dictionary[index];
			for(int j=0; j<2; j++)
			{
				mismatch = dictionary[index].at(mismatch_loc);
				
				if(mismatch == '1')
				{
					//cout<<"prev_dcmprs = "<<prev_dcmprs<<endl;
					prev_dcmprs.at(mismatch_loc) = '0';
					//cout<<"prev_dcmprs = "<<prev_dcmprs<<endl;
				}
				else if(mismatch == '0')
				{
					//cout<<"prev_dcmprs = "<<prev_dcmprs<<endl;
					prev_dcmprs.at(mismatch_loc) = '1';
					//cout<<"prev_dcmprs = "<<prev_dcmprs<<endl;
				}
				mismatch_loc++;
			}
			//decompressed_binary.append(prev_dcmprs);
			outfile<<prev_dcmprs<<endl;
			pos = pos + 3 + 5 + 3;
		}

		else if(read_str.compare("100")==0)
		{
			read_str = compressed_binary.substr(pos+3, 5);
			int mismatch_loc1 = stoi(read_str, nullptr, 2);
			read_str = compressed_binary.substr(pos+3+5, 5);
			int mismatch_loc2 = stoi(read_str, nullptr, 2);
			read_str = compressed_binary.substr(pos+3+5+5, 3);
			index = stoi(read_str, nullptr, 2);
			char mismatch = dictionary[index].at(mismatch_loc1);
			/*
			Copy dictionary entry into prev_dcmprs and then 
			replace character at position 'mismatch_loc' to opposite of mismatch
			*/
			prev_dcmprs = dictionary[index];
			if(mismatch == '1')
			{
				prev_dcmprs.at(mismatch_loc1) = '0';
			}
			else if(mismatch == '0')
			{
				prev_dcmprs.at(mismatch_loc1) = '1';
			}

			mismatch = dictionary[index].at(mismatch_loc2);
			if(mismatch == '1')
			{
				prev_dcmprs.at(mismatch_loc2) = '0';
			}
			else if(mismatch == '0')
			{
				prev_dcmprs.at(mismatch_loc2) = '1';
			}
			//decompressed_binary.append(prev_dcmprs);
			outfile<<prev_dcmprs<<endl;
			pos = pos + 3 + 5 + 5 + 3;
		}

		else if(read_str.compare("101")==0)
		{
			read_str = compressed_binary.substr(pos+3, 3);
			index = stoi(read_str, nullptr, 2);
			//cout<<"num value is "<<num<<endl;
			prev_dcmprs = dictionary[index];
			//decompressed_binary.append(dictionary[index]);
			outfile<<prev_dcmprs<<endl;
			pos = pos + 3 + 3;
		}

		else if(read_str.compare("110") == 0)
		{
			read_str = compressed_binary.substr(pos+3, 32);
			prev_dcmprs = read_str;
			//decompressed_binary.append(read_str);
			outfile<<prev_dcmprs<<endl;
			pos = pos + 3 + 32;
		}

		else if(read_str.compare("111") == 0)
		{
			break;
		}
	}

	//cout<<"decompressed to "<<decompressed_binary<<endl;
	outfile.close();
}


/* END */

int main(int argc, char* argv[])
{
	int operation; //0 to compress input file, 1 to decompress it
	if(argc != 2)
	{
		cout<<"Invalid input format"<<endl<<"Format: ./SIM <operation>\n";
	}
	else
	{
		if(strcmp(argv[1],"1")==0)
		{
			org_file.open("original.txt");
			if(!org_file)
			{
				cout<<"File not found"<<endl; 
			}
			else
			{
				operation = 0;
			}
		}
		else if(strcmp(argv[1],"2")==0)
		{
			dec_file.open("compressed.txt");
			if(!dec_file)
			{
				cout<<"File not found"<<endl; 
			}
			else
			{
				operation = 1;
			}
		}
		else
		{
			cout<<"Invalid input operation"<<endl;
		}
	}

	if(operation==0)
	{
		compress c1;
		c1.compressor();
	}
	else if(operation == 1)
	{
		decompress d1;
		d1.decompressor();
	}

	return 0;
}
