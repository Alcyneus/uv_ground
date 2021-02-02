#if !defined(_BIT_SYNC_SORT_HPP_)
#define _BIT_SYNC_SORT_HPP_



/**
 * @brief 
 * 
 * @param arr 
 * @param n 
 */
void bubbleSort(int *array,int size)
{
	int m,i,j;
	for(i=0; i<size-1; i++)
    {
        for(j=0; j<size-1-i; j++)
        {
            if(array[j] > array[j+1])
			{
				m = array[j];
				array[j] = array[j+1];
				array[j+1] = m;
			}
        }		
    }	
}


#endif // _BIT_SYNC_SORT_HPP_
