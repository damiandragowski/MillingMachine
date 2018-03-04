#include ".\exception.h"

Exception::~Exception(void)
{
}

int SplitString(const string& input, 
       const string& delimiter, vector<string>& results
       )
{
	bool includeEmpties=true;
    int iPos = 0;
    int newPos = -1;
    int sizeS2 = (int)delimiter.size();
    int isize = (int)input.size();

    if( 
        ( isize == 0 )
        ||
        ( sizeS2 == 0 )
    )
    {
        return 0;
    }

    vector<int> positions;

    newPos = input.find (delimiter, 0);

    if( newPos < 0 )
    { 
        return 0; 
    }

    int numFound = 0;

    while( newPos >= iPos )
    {
        numFound++;
        positions.push_back(newPos);
        iPos = newPos;
        newPos = input.find (delimiter, iPos+sizeS2);
    }

    if( numFound == 0 )
    {
        return 0;
    }

    for( int i=0; i <= (int)positions.size(); ++i )
    {
        string s("");
        if( i == 0 ) 
        { 
            s = input.substr( i, positions[i] ); 
		}
        int offset = 0;
		if ( i != 0 )
			offset = positions[i-1] + sizeS2;
		else
			offset = positions[i] + sizeS2;

        if( offset < isize )
        {
            if( i == positions.size() )
            {
                s = input.substr(offset);
            }
            else if( i > 0 )
            {
                s = input.substr( positions[i-1] + sizeS2, 
                      positions[i] - positions[i-1] - sizeS2 );
            }
        }
        if( includeEmpties || ( s.size() > 0 ) )
        {
            results.push_back(s);
        }
    }
    return numFound;
}

float parseFloat(string str)
{
	float ret;
	std::stringstream ss(str);
	ss >> ret;
	if ( ss.fail() )
		throw NumException();
	return ret;
}
