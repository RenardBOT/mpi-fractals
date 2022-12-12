int main(int argc, char const *argv[])
{
    int size = 20;
    //declare an array on the heap
    int *array1 = (int*)malloc(5 * sizeof(int));  
    int *array2 = (int*)malloc(5 * sizeof(int));
    int *array3 = (int*)malloc(5 * sizeof(int));
    int *array4 = (int*)malloc(5 * sizeof(int));

    // fill array1 with values from 0 to 4
    for (int i = 0; i < 5; i++)
    {
        array1[i] = i;
    }
    // fill array2 with values from 5 to 9
    for (int i = 0; i < 5; i++)
    {
        array2[i] = i + 5;
    }
    // fill array3 with values from 10 to 14
    for (int i = 0; i < 5; i++)
    {
        array3[i] = i + 10;
    }
    // fill array4 with values from 15 to 19
    for (int i = 0; i < 5; i++)
    {
        array4[i] = i + 15;
    }

    int * array5 = (int*)malloc(20 * sizeof(int));
    array5[0] = array1;
    array5[4] = array2;
    /* code */
    return 0;
}
/*
❝ i can barely figure out anything of substance about you...  ❞
*/
