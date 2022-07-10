#include <iostream>
#include <fstream>  //para los archivos
#include <string>   //para los string
#include <stdlib.h> //control de memoria dinamica
#include <math.h>
#include <cmath>
#include <iomanip>
using namespace std;
//-------------------------------------------Aqui definimos las variables---------------------------------------------------------------------------------------------------------------------
int *V; // arreglo que contendrá los pesos
// representación de matriz nxn
float **Matrix_Pij, **MatrixBinary_Xij;
int *ArrayWeaponTarget; // Array donde los valores son las armas asignadas y el indice es el objetivo, básicamente será nuestra representación
                        //[1,1,1,1,1] la posición es el tipo de arma
int m, n;

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// simplemente leemos el archivo, reservamos memoria y guardamos los datos del archivo en distintas estructuras
void Instancias(string file)
{
    string a;
    int cont, contF, contR;

    ifstream indatos(file);
    while (getline(indatos, a))
    {
        if (cont == 0)
        {
            m = stoi(a);
            n = stoi(a);
            V = new int[n]; // Pesos de los objetivos
            ArrayWeaponTarget = new int[m];
            Matrix_Pij = new float *[m];       // reservando memoria para las probabilidades
            MatrixBinary_Xij = new float *[m]; // Reservando memoria para las armas i asignadas al objetivo j
            for (int i = 0; i < m; i++)
            {
                Matrix_Pij[i] = new float[n];
                MatrixBinary_Xij[i] = new float[n];
            }
            cont++;
        }
        else if (cont - 1 < m)
        {
            *(V + (cont - 1)) = stoi(a);
            cont++;
        }
        else
        {
            if (contF < m)
            {
                *(*(Matrix_Pij + contR) + contF) = stof(a);
                contF++;
            }
            else if (contF == m)
            {
                contF = 0;
                contR++;
                *(*(Matrix_Pij + contR) + contF) = stof(a);
                contF++;
            }
        }
    }
    indatos.close();
}
// Realizamos la sustracción 1-Pij
void transformacion(float **Matriz)
{
    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < m; j++)
        {
            *(*(Matriz + i) + j) = 1 - *(*(Matriz + i) + j);
        }
    }
}
//----------------------------------------La matriz le asignamos las armas a ciertos objetivos---------------------------------------------
float **MatrixXij(float **Matrix, int *Array, int n)
{
    for (int j = 0; j < n; j++)
    {
        *(*(Matrix + *(Array + j)) + j) = 1;
    }
    return Matrix;
}

//--------------------------------------------------Funcion para leer arreglos------------------------------------------------------------------------
void ReadArreglo(int *array, int m)
{
    cout << "[";
    for (int i = 0; i < m; i++)
    {
        cout << *(array + i) << ",";
    }
    cout << "]" << endl;
}

//---------------------------------------------------Función para leer la matriz--------------------------------------------------------------------------------------------------------
void ReadMatrix(float **Matriz)
{
    cout << "[";
    for (int i = 0; i < m; i++)
    {
        cout << "[";
        for (int j = 0; j < n; j++)
        {
            cout << setprecision(7) << *(*(Matriz + i) + j) << ",";
        }

        cout << "]" << endl;
    }
    cout << "]" << endl;
}
//------------------------------------------------------Función de Evaluación----------------------------------------------------------------------------------
float Evaluation_function(float **MatrizProb, float **MatrixWeaponObjetive, int *Array, int i, int j)
{
    float EvaluationFunction = 0;
    float Sum = 0;
    float Prod = 1;
    // Asignamos las armas a los objetivos correspondientes
    MatrixWeaponObjetive = MatrixXij(MatrixWeaponObjetive, Array, j);

    for (int col = 0; col < j; col++)
    { // Sumatoria
        Sum = *(V + col);
        for (int row = 0; row < i; row++)
        { // Productoria
            Prod = Prod * pow(*(*(MatrizProb + col) + row), *(*(MatrixWeaponObjetive + col) + row));
        }
        EvaluationFunction = EvaluationFunction + (Sum * Prod);
        Prod = 1;
    }
    return EvaluationFunction;
}

//----------------------------------------------Asginanción de las armas a los objetivos-------------------------------
int *WTA(int *arr, int m)
{
    int index = 0;
    bool *Use = new bool[m];
    int *TipoArma = new int[m];
    for (int num = 0; num < m; num++)
    {
        *(TipoArma + num) = num;
    }
    ReadArreglo(TipoArma, m);
    for (int t = 0; t < m; t++)
    {
        *(Use + t) = false;
    }

    for (int i = 0; i < m; i++)
    {
        do
        {
            index = (rand() % m);
        } while (*(Use + index));
        *(arr + i) = *(TipoArma + index);
        *(Use + index) = true;
    }
    delete[] Use;
    delete[] TipoArma;
    return arr;
}
//-----------------------------------------------Liberamos memoria de las matrices-------------------------------------------------

void MemoryFreeMatrix(float **Matriz, int m)
{
    for (int f = 0; f < m; f++)
    {
        delete[] Matriz[f];
    }
    delete[] Matriz;
}
//------------------------------------------------------------------Reiniciando valores de la matriz de asignación-------------------------
float **ReMatrixAT(float **Matrix, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            *(*(Matrix + i) + j) = 0;
        }
    }
    return Matrix;
}
//------------------------------------------------------Función asignada para el movimiento------------------------------------------------
int *Shift(int *ArrayWeapon, int n)
{
    //----------------------------------------------------------------------

    int LastValue;
    int *AuxArrayWeapon = new int[n];
    LastValue = *(ArrayWeapon + (n - 1)); // guardamos el ultimo valor del arreglo en la primera posición
    *(AuxArrayWeapon + 0) = LastValue;
    for (int iteration = 0; iteration < n - 1; iteration++)
    {
        *(AuxArrayWeapon + (iteration + 1)) = *(ArrayWeapon + iteration);
    }

    *ArrayWeapon = *AuxArrayWeapon;
    for (int r = 0; r < n; r++)
    {
        *(ArrayWeapon + r) = *(AuxArrayWeapon + r);
    }

    delete[] AuxArrayWeapon;

    return ArrayWeapon;
}
// agregar una variable para detenerse
// Movimiento swap
// debemos definir algunas varibales globales que iran cambiando a través del tiempo
int aux_Position1 = 0, aux_Position2 = 0;
int *Swap(int *array, int position1, int position2)
{
    aux_Position1 = *(array + position1);
    aux_Position2 = *(array + position2);

    *(array + position1) = aux_Position2;
    *(array + position2) = aux_Position1;

    return array;
}

//--------------------------------------------------Simulated Annealing--------------------------------------------------------------------
float SA(float **MatrixPro, float **MatrixTargets, int *ArrayW_T, int i, int j)
{
    // realizamos comparación de soluciones
    float CurrentSolution, TheBestSolution = 0; //,
    float FinalSolution = 0;
    int Iterations = 7;
    int position1 = 0, position2 = position1 + 1;
    /*//*(ArrayW_T+0)=8;
    //*(ArrayW_T+1)=9;
    //*(ArrayW_T+2)=7;
    //*(ArrayW_T+3)=6;
  //*(ArrayW_T+4)=4;
   //*(ArrayW_T+5)=2;
    //*(ArrayW_T+6)=5;
    //*(ArrayW_T+7)=3;
   //*(ArrayW_T+8)=0;
    //*(ArrayW_T+9)=1;
    // ReadArreglo(ArrayW_T, i);
    // Arreglo de entrada
    // cout << "Arreglo de entrada " << endl;*/
    //cout << "Arreglo de entrada: ";
    //ReadArreglo(ArrayW_T, i);
    // Parametros para la probabilidad
    cout<<"Arreglo Inicial: ";
    ReadArreglo(ArrayW_T,i);
    float P = 0, P_Random = 0;
    float euler = exp(1.0);
    float T = 10;
    int cont = 0; //->este contador se utilizará para manejar el número de movimientos si es necesario.
    for (int iter = 0; iter < Iterations; iter++)
    { 
        cout<<"-------------------------------------------Iteracion numero-------------------------------------------------------------- "<<iter<<endl;
        CurrentSolution = Evaluation_function(MatrixPro, MatrixTargets, ArrayW_T, i, j);
        cout<<"Solución Obtenida "<<CurrentSolution<<endl;
        if (iter == 0)
        {
            FinalSolution = CurrentSolution;
        }
        else if (iter != 0)
        {
            if (CurrentSolution < FinalSolution)
            {
                FinalSolution = CurrentSolution;
                //*(ArrayW_T+position1)=aux_Position1;
                //*(ArrayW_T+position2)=aux_Position2;
                position1=0;
                position2=position1+1;
            }
            else if (CurrentSolution > FinalSolution)
            {   
                TheBestSolution = FinalSolution; // no la estamos utilizando para nada
                cont=0;
                while (cont < i)
                { 
                    if (CurrentSolution > FinalSolution)
                    {
                        float Delta = FinalSolution - CurrentSolution;
                        P = pow(euler, Delta);
                        P_Random = (float)rand() / RAND_MAX;
                        if (P_Random < P)
                        {
                            T = T * 0.9;
                            cont = i;
                            FinalSolution = CurrentSolution;
                            position1 = 0;
                            position2 = 1;
                        }
                        else if (P < P_Random)
                        {
                            if (position2 != (i - 1))
                            { 
                                
                                *(ArrayW_T + position1) = aux_Position1;
                                *(ArrayW_T + position2) = aux_Position2;
                                position1+=1;
                                position2+=1;
                                ArrayW_T = Swap(ArrayW_T, position1, position2);
                                cout<<"Ocurre Swap: ";
                                ReadArreglo(ArrayW_T,i);
                                CurrentSolution = Evaluation_function(MatrixPro, MatrixTargets, ArrayW_T, i, j);
                                cout<<"Solucion obtenida: "<<CurrentSolution<<endl;
                                if (CurrentSolution < FinalSolution)
                                {
                                    FinalSolution=CurrentSolution;
                                    position1=0;
                                    position2=position1+1;
                                    cont=i;
                                }else{
                                    
                                    *(ArrayW_T + position1) = aux_Position1;
                                    *(ArrayW_T + position2) = aux_Position2;
                                    

                                    if(position2!=(i-1)){
                                        ArrayW_T= Swap(ArrayW_T,position1,position2);
                                        cout<<"Ocurre Swap: ";
                                        ReadArreglo(ArrayW_T,i);
                                        position1 += 1;
                                        position2 += 1;
                                    }else if(position2==(i-1)){
                                        aux_Position1= *(ArrayW_T+(0));
                                        aux_Position2= *(ArrayW_T+(i-1));
                                        *(ArrayW_T+0)=aux_Position2;
                                        *(ArrayW_T+(i-1))=aux_Position1;
                                        cout<<"Ocurre Swap: ";
                                        ReadArreglo(ArrayW_T,i);
                                        position1=0;
                                        position2=position1+1;
                                    }
                                    CurrentSolution = Evaluation_function(MatrixPro, MatrixTargets, ArrayW_T, i, j);
                                    cout<<"Solucion obtenida: "<<CurrentSolution<<endl;
                                    cont++;
                                }
                            }
                            else if (position2 == (i - 1))
                            {
                                aux_Position1 = *(ArrayW_T + 0);
                                aux_Position2 = *(ArrayW_T + (i - 1));
                                *(ArrayW_T + 0) = aux_Position2;
                                *(ArrayW_T + (i - 1)) = aux_Position1;
                                cont=i;
                            }
                        }
                    }
                    else{
                        FinalSolution=CurrentSolution;
                        cont=i;
                        position1=0;
                        position2=position1+1;
                    }
                }
            }
        }
        // ArrayW_T = Shift(ArrayW_T, i);
        ArrayW_T = Swap(ArrayW_T, position1, position2);
        cout<<"Ocurre Swap: ";
        ReadArreglo(ArrayW_T,i);
        
        MatrixTargets = ReMatrixAT(MatrixTargets, i);
    }
    return FinalSolution;
}

int main(int argc, char *argv[])
{
    Instancias(*(argv + 1));
    // Transformando matriz
    //--------------------------------------------------Imprimiendo matriz de probabilidades-------------------------------------------
    // cout << "Matriz de Probabilidades Pij: " << endl;
    // ReadMatrix(Matrix_Pij);
    transformacion(Matrix_Pij);
    // cout << "Nuevo valor de la Matriz después de la sustracción (1-Pij)" << endl;
    // ReadMatrix(Matrix_Pij);
    ReadArreglo(ArrayWeaponTarget, m);
    ArrayWeaponTarget = WTA(ArrayWeaponTarget, m);
    //-----------------------------------------------------------------Imprimiendo los arreglos--------------------------------------------------------------------------------------
    // cout << "Matriz de pesos: ";
    // ReadArreglo(V, m);
    // cout << "Armas i asignadas en la posición j: ";
    // ReadArreglo(ArrayWeaponTarget, m);
    std::cout << "Iniciando el SA: " << SA(Matrix_Pij, MatrixBinary_Xij, ArrayWeaponTarget, m, n) << endl;
    delete[] ArrayWeaponTarget;
    //-------------------------------------------------------------------Liberación de memoria----------------------------------------------------------------------------------------------------------------
    MemoryFreeMatrix(Matrix_Pij, m);
    MemoryFreeMatrix(MatrixBinary_Xij, m);
    delete[] V;
    return 0;
}