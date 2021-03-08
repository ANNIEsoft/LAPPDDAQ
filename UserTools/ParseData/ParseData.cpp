#include "ParseData.h"

ParseData::ParseData():Tool(){}


bool ParseData::Initialise(std::string configfile, DataModel &data){

  if(configfile!="")  m_variables.Initialise(configfile);
  //m_variables.Print();

  m_data= &data;
  m_log= m_data->Log;

  if(!m_variables.Get("verbose",m_verbose)) m_verbose=1;


  return true;
}


bool ParseData::Execute(){

  if(m_data->TCS.receiveFlag == 1)
  {
	  int boardindex, retval;
	  std::vector<unsigned short> temp_Waveform;

	  for(int i=0; i<m_data->TCS.PsecClassStore.size(); i++)
	  {
			//fill ParsedStream with vectors from data
			retval = getParsedData(m_data->TCS.PsecClassStore[i].RawWaveform);
			if(retval!=0)
			{
				std::cout << "Parsing went wrong! " << retval << std::endl;
			}
			m_data->TCS.ParsedStream[m_data->TCS.PsecClassStore[i].BoardIndex] = data;
	  }
  }
  return true;
}


bool ParseData::Finalise(){

  	return true;
}

int ParseData::getParsedData(std::vector<unsigned short> buffer)
{

	//make sure an acdc buffer has been
	//filled. if not, there is nothing to be done.
	if(buffer.size() == 0)
	{
		string err_msg = "You tried to parse ACDC data without pulling/setting an ACDC buffer";
		return -1;
	}else
	{
		std::cout << buffer.size() << std::endl;
	}

	//clear the data map prior.
	data.clear();

	int dist;
	int channel_count=0;

	//byte that indicates the metadata of
	//each psec chip is about to follow. 
	const unsigned short startword = 0xF005; 
	unsigned short endword = 0xBA11;
	unsigned short endoffile = 0x4321;

	//indices of elements in the acdcBuffer
	//that correspond to the byte ba11
	vector<int> start_indices; 
	vector<unsigned short>::iterator bit;

	//loop through the data and find locations of startwords. 
    //this can be made more efficient if you are having efficiency problems.
	for(bit = buffer.begin(); bit != buffer.end(); ++bit)
	{
		//the iterator is at an element with startword value. 
		//push the index (integer, from std::distance) to a vector. 
        if(*bit == startword)
        {
        	dist= std::distance(buffer.begin(), bit);
        	if(start_indices.size()!=0 && abs(dist-start_indices[start_indices.size()])<(6*256+15))
        	{
            	continue;        
        	}
        	start_indices.push_back(dist);
        }
	}

	if(start_indices.size() != NUM_PSEC)
	{
        string fnnn = "acdc-corrupt-psec-buffer.txt";
        cout << "Printing to file : " << fnnn << endl;
        ofstream cb(fnnn);
        for(unsigned short k: buffer)
        {
            cb << hex << k << endl;
        }
        return -2;
	}

	for(int i: start_indices)
	{
		//re-use buffer iterator from above
		//to set starting point. 
		bit = buffer.begin() + i + 1; //the 1 is to start one element after the startword
		//while we are not at endword, 
		//append elements to ac_info
		vector<unsigned short> infobytes;
		while(*bit != endword && *bit != endoffile)
		{
			infobytes.push_back((unsigned short)*bit);
			if(infobytes.size()==NUM_SAMP)
			{
				data[channel_count] = infobytes;
				infobytes.clear();
				channel_count++;
			}
			++bit;
		}	
	}

	if(data.size()!=NUM_CH)
	{
		return -3;
	}

	for(int i=0; i<NUM_CH; i++)
	{
		if(data[i].size()!=NUM_SAMP)
		{
			return -4;
		}
	}

	return 0;
}